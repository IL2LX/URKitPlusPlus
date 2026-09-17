#include "runtime_backend.h"

#include "il2cpp_api.h"
#include "il2cpp_export_names.h"
#include "il2cpp_symbol_map.h"
#include "loader_paths.h"
#include "logger.h"
#include "mod_context.h"
#include "native_mod_loader.h"
#include "runtime_events.h"
#include "runtime_state.h"

#include <windows.h>

#include <cstddef>
#include <filesystem>
#include <string>

namespace {
constexpr int kRuntimeTimeoutMs = 30000;
constexpr int kIl2CppPreDomainSettleMs = 1500;

RuntimeState g_il2cppState{"IL2CPP"};
Il2CppApi g_il2cppApi{};
Il2CppSymbolMap g_symbolMap{};

std::string ExportMapPath() {
    const std::string directory = Loader_UrKitDir(); 
    if (directory.empty())
        return {};
    return (std::filesystem::path(directory) / "UnityPlayerExports.txt").string();
}

std::string SymbolMapPath() {
    const std::string directory = Loader_UrKitDir();
    if (directory.empty())
        return {};
    return (std::filesystem::path(directory) / "DeobfuscationMap.json").string();
}

bool RunIl2Cpp(Config &config) {
    g_il2cppState.Reset();
    g_il2cppApi = {};
    RuntimeEvents_Reset();
    Log(">> IL2CPP backend");
    Il2CppExportNameMap exportNameMap;
    const Il2CppExportNameMap *exportNames = nullptr;
    {
        const std::string mapPath = ExportMapPath();
        if (!mapPath.empty()) {
            Log("[IL2CPP] Attempting to load an export-name map from %s.", mapPath.c_str());
            if (Il2Cpp_LoadExportNameMap(mapPath.c_str(), exportNameMap)) {
                Log("[IL2CPP] Loaded %zu export-name mappings from %s (skipped "
                    "%zu malformed lines).",
                    exportNameMap.realToObfuscated.size(), mapPath.c_str(), exportNameMap.invalidLines);
                exportNames = &exportNameMap;
            } else {
                Log("[IL2CPP][WARNING] No usable export map at %s; GameAssembly "
                    "export names will be resolved by exact name.",
                    mapPath.c_str());
            }
        }
    }

    if (!Il2Cpp_BindExports(g_il2cppApi, kRuntimeTimeoutMs, exportNames, config.logUnityExports)) {
        return false;
    }

    g_symbolMap = {};
    const std::string symbolPath = SymbolMapPath();
    if (!symbolPath.empty() && Il2Cpp_LoadSymbolMap(symbolPath.c_str(), g_symbolMap)) {
        g_il2cppApi.symbolMap = &g_symbolMap;
        Log("[IL2CPP] Loaded %zu obfuscated class records from %s (skipped %zu "
            "malformed entries); class/member names with a mapping are now "
            "deobfuscated.",
            g_symbolMap.classes.size(), symbolPath.c_str(), g_symbolMap.invalidEntries);
    } else if (!symbolPath.empty()) {
        Log("[IL2CPP] No usable symbol map at %s; obfuscated class/member names are "
            "reported unchanged.",
            symbolPath.c_str());
    }

    g_il2cppState.Transition(RuntimeReadiness::ModuleSeen, nullptr, "GameAssembly");
    g_il2cppState.Transition(RuntimeReadiness::ExportsResolved, nullptr, "GameAssembly exports");

    if (config.initDelayMs) {
        Log("[IL2CPP] Applying InitDelayMs=%d before domain/metadata readiness " "probes.", config.initDelayMs);
        Sleep(config.initDelayMs);
    }

    if (!Il2Cpp_WaitForMetadataReady(g_il2cppApi, kRuntimeTimeoutMs, kIl2CppPreDomainSettleMs)) {
        return false;
    }

    if (config.accessorLeaks) {
        if (Il2Cpp_BuildBeebyteLeakPairs(g_symbolMap)) {
            Log("[IL2CPP] Recovered %zu Beebyte accessor-leak pairs from the live "
                "metadata; matching obfuscated names are deobfuscated.",
                g_symbolMap.globalPairs.size());
        } else {
            Log("[IL2CPP][WARNING] Beebyte accessor-leak scan failed; obfuscated "
                "names are reported unchanged.");
        }
    }

    if (config.structuralNames) {
        if (Il2Cpp_BuildStructuralNames(g_symbolMap)) {
            Log("[IL2CPP] Structural names generated for obfuscated classes/members "
                "without a mapping (deobfuscation map entries take priority).");
        } else {
            Log("[IL2CPP][WARNING] Structural rename failed; obfuscated names are "
                "reported unchanged.");
        }
    }

    if (!g_il2cppApi.symbolMap && (!g_symbolMap.classes.empty() || !g_symbolMap.globalPairs.empty())) {
        g_il2cppApi.symbolMap = &g_symbolMap;
        Log("[IL2CPP] No DeobfuscationMap.json was loaded, but %zu on-the-fly names "
            "and %zu leak pairs were recovered at launch; they are active.",
            g_symbolMap.classes.size(), g_symbolMap.globalPairs.size());
    }

    if (config.dumpSymbols) {
        const std::string dumpPath = SymbolMapPath() + ".dump";
        if (Il2Cpp_DumpSymbolNames(dumpPath.c_str())) {
            Log("[IL2CPP] Symbol dump written to %s; disable DumpSymbols=1 in "
                "URKit_config.ini once the map exists.",
                dumpPath.c_str());
            return true;
        }
        Log("[IL2CPP][WARNING] Symbol dump requested but failed to write.");
    }

    if (config.safeMode) {
        Log("[safe-mode][IL2CPP] runtime diagnostics complete; native mods and runtime event hooks disabled.");
        return true;
    }

    const NativeModLoadPlan modPlan = NativeMods_Discover(config);
    if (!NativeMods_RequiresRuntimeEvents(config.safeMode, modPlan.paths.size())) {
        return true;
    }

    const uint64_t runtimeEventCapabilities = RuntimeEvents_ConfigureIl2Cpp(g_il2cppApi);
    g_il2cppState.Transition(RuntimeReadiness::ModsAllowed, g_il2cppApi.Domain(), "IL2CPP domain");
    Log("[mods] pid=%lu tid=%lu backend=IL2CPP domain=%p gameAssembly=%p "
        "unityPlayer=%p entry",
        GetCurrentProcessId(), GetCurrentThreadId(), g_il2cppApi.Domain(),
        reinterpret_cast<void *>(g_il2cppApi.gameAssemblyBase), reinterpret_cast<void *>(g_il2cppApi.unityPlayerBase));
    NativeMods_Load(modPlan, ModContext_BuildIl2Cpp(config, g_il2cppApi, runtimeEventCapabilities));
    RuntimeEvents_AfterModsLoaded();
    return true;
}

const RuntimeBackendDescriptor kIl2CppBackend{"IL2CPP", true, RunIl2Cpp};
} // namespace

const RuntimeBackendDescriptor &RuntimeBackend_Il2Cpp() {
    return kIl2CppBackend;
}
