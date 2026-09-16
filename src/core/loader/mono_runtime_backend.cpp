#include "runtime_backend.h"

#include "logger.h"
#include "mod_context.h"
#include "mono_api.h"
#include "mono_runtime_bootstrap.h"
#include "native_mod_loader.h"
#include "runtime_events.h"
#include "runtime_state.h"

#include <windows.h>

#include <memory>

namespace {
constexpr int kRuntimeTimeoutMs = 30000;

RuntimeState g_monoState{"Mono"};
MonoApi g_monoApi{};

bool RunMono(Config &config) {
    MonoApi &mono = g_monoApi;

    g_monoState.Reset();
    RuntimeEvents_Reset();
    Log(">> MONO backend");
    if (!Mono_Resolve(mono, kRuntimeTimeoutMs)) {
        return false;
    }

    g_monoState.Transition(RuntimeReadiness::ModuleSeen, nullptr, "module loader");
    g_monoState.Transition(RuntimeReadiness::ExportsResolved, nullptr, "export resolver");

    const MonoRuntimeBootstrapResult bootstrap = MonoRuntimeBootstrap_Attach(mono, g_monoState, kRuntimeTimeoutMs);
    MonoDomain *monoDomain = bootstrap.domain;
    MonoThread *attachedThread = bootstrap.attachedThread;
    if (!monoDomain || !attachedThread) {
        return false;
    }
    auto detachLoaderThread = [](MonoThread *thread) { Mono_DetachThread(g_monoApi, thread); };
    std::unique_ptr<MonoThread, decltype(detachLoaderThread)> loaderThreadAttachment(attachedThread,
                                                                                    detachLoaderThread);

    if (config.initDelayMs) {
        Sleep(config.initDelayMs);
    }

    if (!RuntimeEvents_WaitForMonoUnityReady(mono, kRuntimeTimeoutMs)) {
        return false;
    }
    g_monoState.Transition(RuntimeReadiness::AssembliesStable, monoDomain, g_monoState.DomainSource());

    if (config.safeMode) {
        Log("[safe-mode][Mono] runtime attach diagnostics complete; native mods and runtime event hooks disabled.");
        return true;
    }

    const NativeModLoadPlan modPlan = NativeMods_Discover(config);
    if (!NativeMods_RequiresRuntimeEvents(config.safeMode, modPlan.paths.size())) {
        return true;
    }

    const uint64_t runtimeEventCapabilities = RuntimeEvents_ConfigureMono(mono);
    g_monoState.Transition(RuntimeReadiness::ModsAllowed, monoDomain, g_monoState.DomainSource());
    Log("[mods] pid=%lu tid=%lu backend=Mono attached=yes domain=%p entry", GetCurrentProcessId(), GetCurrentThreadId(),
        monoDomain);
    NativeMods_Load(modPlan, ModContext_BuildMono(config, mono, mono.base, runtimeEventCapabilities));
    RuntimeEvents_AfterModsLoaded();
    return true;
}

const RuntimeBackendDescriptor kMonoBackend{
    "Mono",
    true,
    RunMono,
};
} // namespace

const RuntimeBackendDescriptor &RuntimeBackend_Mono() {
    return kMonoBackend;
}
