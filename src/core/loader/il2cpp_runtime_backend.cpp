#include "runtime_backend.h"

#include "il2cpp_api.h"
#include "il2cpp_export_names.h"
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

std::string ExportMapPath() {
    const std::string directory = Loader_UrKitDir();
    if (directory.empty())
        return {};
    return (std::filesystem::path(directory) / "exports.txt").string();
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

    if (!Il2Cpp_BindExports(g_il2cppApi, kRuntimeTimeoutMs, exportNames)) {
        return false;
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
