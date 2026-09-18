#include "loader.h"
#include "config.h"
#include "loader/loader_selection.h"
#include "loader/loader_lifecycle.h"
#include "loader/loader_paths.h"
#include "loader/process_qualification.h"
#include "loader/runtime_backend.h"
#include "logger.h"
#include "mod_sdk.h"

#include "il2cpp_api.h"

#include <windows.h>

#include <cstddef>
#include <filesystem>
#include <string>

static_assert(offsetof(URK_ModContext, MainThreadRegister) > offsetof(URK_ModContext, mono),
              "URK_ModContext field order changed unexpectedly.");
static_assert(offsetof(URK_ModContext, size) > offsetof(URK_ModContext, MainThreadUnregister),
              "URK_ModContext field order changed unexpectedly.");
static_assert(offsetof(URK_ModContext, gameAssemblyModuleBase) > offsetof(URK_ModContext, runtimeBackendModuleBase),
              "URK_ModContext field order changed unexpectedly.");
static_assert(offsetof(URK_ModContext, network) > offsetof(URK_ModContext, gameAssemblyModuleBase),
              "URK_ModContext network API must remain appended.");

static Config g_cfg;

namespace {
constexpr unsigned kProcessQualificationTimeoutMs = 30000;

void LogConfigSummary(const RuntimeBackendDescriptor &backend) {
    Log("=== URKit started (safeMode=%s, runtime=%s) ===", g_cfg.safeMode ? "on" : "off", backend.name);
    Log("[config] executableDir='%s', configPath='%s', selectedMods=%zu, initDelayMs=%d, console=%s.",
        Loader_ExeDir().c_str(), g_cfg.configPath.c_str(), g_cfg.modPaths.size(), g_cfg.initDelayMs,
        g_cfg.showConsole ? "on" : "off");
}

void DebugSkip(const std::string &reason) {
    const std::string message = "[URKit][loader] bootstrap skipped: " + reason + "\n";
    OutputDebugStringA(message.c_str());
}

bool DumpFile(const std::string &directory, const char *name,
              bool (*dump)(const char *)) {
    const std::string path = directory + name;
    const bool ok = dump(path.c_str());
    Log("[dumps] %s to %s.", ok ? "wrote" : "FAILED to write", path.c_str());
    return ok;
}

void DumpEverything() {
    const std::string directory = Loader_UrKitDir() + "Dumps\\";
    std::error_code error;
    std::filesystem::create_directories(directory, error);
    if (error) {
        Log("[dumps][ERROR] Failed to create Dumps directory %s: %s",
            directory.c_str(), error.message().c_str());
        return;
    }
    Log("[dumps] AutoDump=1: dumping symbols to %s", directory.c_str());
    DumpFile(directory, "DeobfuscationMap.json.dump", &Il2Cpp_DumpSymbolNames);
    DumpFile(directory, "DeobfuscationMap.json.readable", &Il2Cpp_DumpReadableSymbolNames);
    DumpFile(directory, "DeobfuscationMap.beebyte.csv", &Il2Cpp_DumpDeobfuscationMapCsv);
}
} // namespace

LoaderRunStatus Loader_Run(LoaderStartMode mode) {
    const ProcessQualification qualification =
        ProcessQualification_WaitForUnity(kProcessQualificationTimeoutMs);
    if (!qualification.isUnityProcess) {
        DebugSkip(qualification.reason);
        return LoaderRunStatus::Skipped;
    }

    if (mode == LoaderStartMode::Injected) {
        LoaderSelection selection;
        if (!Loader_SelectPaths(&selection)) {
            OutputDebugStringA("[URKit][loader] injected session cancelled before runtime startup.\n");
            return LoaderRunStatus::Skipped;
        }

        g_cfg = Config_Load(selection.configPath);
        g_cfg.modPaths = selection.modPaths;
    } else {
        g_cfg = Config_Load();
    }

    const std::string logDirectory = mode == LoaderStartMode::Injected
                                          ? std::filesystem::path(g_cfg.configPath).parent_path().string()
                                          : std::string();
    Log_Init(g_cfg.showConsole, g_cfg.logFullTimestamp, g_cfg.logPidTid, logDirectory);
    Log_Banner();
    const RuntimeBackendDescriptor &backend = RuntimeBackend_Select(g_cfg);

    for (const std::string &warning : g_cfg.warnings)
        Log("[config][WARNING] %s", warning.c_str());
    Log("[process] qualification=%s unityPlayer=%s gameAssembly=%s mono=%s.", qualification.reason.c_str(),
        qualification.unityPlayerLoaded ? "loaded" : "not-loaded",
        qualification.il2cppLoaded ? "loaded" : "not-loaded", qualification.monoLoaded ? "loaded" : "not-loaded");
    LogConfigSummary(backend);

    const bool backendReady = RuntimeBackend_Run(backend, g_cfg);

    if (backendReady) {
        Log("=== Loader initialization succeeded ===");
        if (g_cfg.autoDump)
            DumpEverything();
        return LoaderRunStatus::Succeeded;
    }
    Log("=== Loader initialization failed; no native mods were started ===");
    return LoaderRunStatus::Failed;
}
