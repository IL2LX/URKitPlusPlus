std::string ConfigModule(const ModuleProjectOptions &options) {
    std::ostringstream out;
    out << "#pragma once\n\n"
        << "namespace ModConfig {\n"
        << "inline constexpr const char* project_name = \"" << EscapeString(options.projectName) << "\";\n"
        << "// Stable namespace for this mod's deployed resources. Do not change it after release.\n"
        << "inline constexpr const char* mod_id = \"" << EscapeString(options.modId) << "\";\n"
        << "inline constexpr const char* display_name = \"" << EscapeString(options.projectName) << "\";\n"
        << "inline constexpr const char* author = \"\";\n"
        << "inline constexpr const char* version = \"0.1.0\";\n"
        << "inline constexpr const char* url = \"\";\n"
        << "inline constexpr const char* social = \"\";\n"
        << "inline constexpr const char* description = \"" << EscapeString(options.description) << "\";\n"
        << "inline constexpr bool is_il2cpp_backend = "
        << (options.backendNamespace == "URK::il2cpp" ? "true" : "false") << ";\n";
    for (const std::string &line : options.configExtraLines)
        out << line << "\n";
    out << "inline bool show_menu = true;\n"
        << "// English is used as the fixed language when localization support is not generated.\n"
        << "inline bool enable_localization = " << (options.enableLocalization ? "true" : "false") << ";\n"
        << "inline constexpr const char* default_language = \"en\";\n"
        << "// Lets ImGui windows move to another monitor.\n"
        << "inline bool enable_detached_viewports = false;\n"
        << "inline bool enable_unity_log_hook = true;\n"
        << "// Win32 virtual-key code used by the generated ImGui WndProc toggle.\n"
        << "// Default: VK_TAB (0x09). Change this value to customize the menu key.\n"
        << "inline int menu_toggle_key = 0x09;\n";
    out << "} // namespace ModConfig\n";
    return out.str();
}

std::string BackendRuntimeHeader(const ModuleProjectOptions &options) {
    return options.backendNamespace == "URK::mono" ? "sdk/mono/mono_runtime.h" : "sdk/il2cpp/il2cpp_runtime.h";
}

std::string BackendHelperHeader(const ModuleProjectOptions &options) {
    return options.backendNamespace == "URK::mono" ? "sdk/mono/mono_helpers.h" : "sdk/il2cpp/il2cpp_helpers.h";
}

std::string BackendApiField(const ModuleProjectOptions &options) {
    return options.backendNamespace == "URK::mono" ? "mono" : "il2cpp";
}

std::string RuntimeBootstrapModule(const ModuleProjectOptions &options) {
    std::ostringstream out;
    out << "#pragma once\n\n"
        << "#include \"runtime_api.h\"\n"
        << "#include \"" << BackendRuntimeHeader(options) << "\"\n\n"
        << "namespace URK {\n"
        << "inline bool initialize_backend(const ModContext* context) {\n"
        << "  return " << options.backendNamespace << "::init(context);\n"
        << "}\n"
        << "} // namespace URK\n";
    return out.str();
}

std::string ModLogHeader() {
    return R"URK(#pragma once

#include "sdk/mod_sdk.h"

namespace ModLog {
void initialize(const URK_ModContext *ctx);
const URK_ModContext *context();
void info(const char *fmt, ...);
void warn(const char *fmt, ...);
void error(const char *fmt, ...);
void success(const char *fmt, ...);
void shutdown();
} // namespace ModLog
)URK";
}

std::string ModLogSource() {
    return R"URK(#include "mod_log.h"

#include "config/mod_config.h"

#include <cstdarg>
#include <cstdio>

namespace {
const URK_ModContext *g_ctx = nullptr;

void write_log(const char *level, const char *fmt, va_list args) {
    if (!g_ctx || !g_ctx->Log)
        return;

    char message[1600]{};
    if (fmt && fmt[0])
        std::vsnprintf(message, sizeof(message), fmt, args);

    if (level && level[0])
        g_ctx->Log("[%s][%s] %s", ModConfig::display_name, level, message);
    else
        g_ctx->Log("[%s] %s", ModConfig::display_name, message);
}
} // namespace

namespace ModLog {
void initialize(const URK_ModContext *ctx) {
    g_ctx = ctx;
}
const URK_ModContext *context() {
    return g_ctx;
}

void info(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    write_log("info", fmt, args);
    va_end(args);
}

void warn(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    write_log("warn", fmt, args);
    va_end(args);
}

void error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    write_log("error", fmt, args);
    va_end(args);
}

void success(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    write_log("success", fmt, args);
    va_end(args);
}

void shutdown() {
    g_ctx = nullptr;
}
} // namespace ModLog
)URK";
}

std::string ModulesHeaderModule() {
    return R"URKMODH(#pragma once

// The URKit module system is a set of feature objects, each carrying a name,
// description and category. The URKit runtime drives registered modules:
//   ModRuntime::update()        -> Modules::System::Tick(...) every frame;
//                                  calls OnUpdate() + the OnUpdate01/05 cadences.
//   render hook (outside menu)  -> Modules::System::Render()     -> OnRender().
//   mod menu window             -> Modules::System::MenuRender() -> InMenuRender().
//   scene events                -> Modules::System::SceneLoad / SceneChanged.
//   ModRuntime::start()/stop()  -> Modules::RegisterModules() / System::UnregisterAll().

#include <cstddef>

namespace Modules {

enum class Category : int {
    Visuals = 0,
    Movements,
};

const char *category_name(Category category);

// Base class for every feature. Derive a class per feature and register it
// from Modules::RegisterModules() (see modules.cpp for the shape).
class Module {
public:
    Module(const char *name, const char *description = "", Category category = Category::Visuals);
    virtual ~Module() = default;

    // metadata
    virtual const char *name() const;
    virtual const char *description() const;
    virtual Category category() const;

    // state
    bool enabled() const;
    void SetEnabled(bool enabled); // fires OnEnable()/OnDisable() on change

    // lifecycle hooks (all optional)
    virtual void OnEnable() {}
    virtual void OnDisable() {}
    virtual void OnUpdate(float deltaTime) {}     // every frame while enabled
    virtual void OnUpdate01(float deltaTime) {}   // ~10 Hz while enabled
    virtual void OnUpdate05(float deltaTime) {}   // ~2 Hz while enabled
    virtual void OnRender() {}                    // every frame while enabled, outside the menu
    virtual void InMenuRender() {}                // inside the mod menu window while enabled
    virtual void OnSceneLoad(const char *sceneName, int buildIndex) {}    // always called
    virtual void OnSceneChanged(const char *fromScene, const char *toScene) {} // always called
    virtual void OnQuickMenuInit() {}             // once, when Canvas_QuickMenu(Clone) appears
    virtual void OnMainMenuInit() {}              // once, when Canvas_MainMenu(Clone) appears

    // drives the cadence hooks; called by System::Tick, do not override
    void TickInternal(float deltaTime);

protected:
    const char *name_ = "";
    const char *description_ = "";
    Category category_ = Category::Visuals;
    bool enabled_ = true;

    // cadence accumulators; managed by the system
    float clock01_ = 0.0f;
    float clock05_ = 0.0f;
};

// Registry + dispatch. Not meant to be implemented or modified by the user.
namespace System {
void Register(Module *module);   // registers; OnEnable() if the module starts enabled
void Unregister(Module *module);
void UnregisterAll();
void Toggle(Module *module);
void EnableAll();
void DisableAll();

template <typename T>
T &Register() {
    static T instance;
    Register(&instance);
    return instance;
}

int count();
int count(Category category);
std::size_t enabled_count();
int enabled_count(Category category);
Module *module(int index); // 0 .. count()-1, in registration order

// update + render hooks only run for enabled modules. Scene events are
// delivered to every registered module so state survives being toggled off.
void Tick(float deltaTime);
void Render();
void MenuRender();
void SceneLoad(const char *sceneName, int buildIndex);
void SceneChanged(const char *fromScene, const char *toScene);
void QuickMenuInit();
void MainMenuInit();
} // namespace System

// Implement this in modules.cpp: register your module instances here.
void RegisterModules();

} // namespace Modules
)URKMODH";
}

std::string ModulesSourceModule() {
    return R"URKMODS(#include "modules/modules.h"
#include "modules/Visuals/Visuals.h"

#include <vector>

namespace Modules {

const char *category_name(Category category) {
    switch (category) {
        case Category::Visuals:   return "Visuals";
        case Category::Movements: return "Movements";
        default:                  return "?";
    }
}

Module::Module(const char *name, const char *description, Category category)
    : name_(name ? name : ""), description_(description ? description : ""), category_(category) {}

const char *Module::name() const { return name_; }
const char *Module::description() const { return description_; }
Category Module::category() const { return category_; }

bool Module::enabled() const { return enabled_; }

void Module::SetEnabled(bool enabled) {
    if (enabled == enabled_) return;
    enabled_ = enabled;
    if (enabled_)
        OnEnable();
    else
        OnDisable();
}

void Module::TickInternal(float deltaTime) {
    OnUpdate(deltaTime);
    clock01_ += deltaTime;
    if (clock01_ >= 0.1f) {
        clock01_ = 0.0f;
        OnUpdate01(0.1f);
    }
    clock05_ += deltaTime;
    if (clock05_ >= 0.5f) {
        clock05_ = 0.0f;
        OnUpdate05(0.5f);
    }
}

namespace System {

namespace {
std::vector<Module *> &registry() {
    static std::vector<Module *> modules;
    return modules;
}
} // namespace

void Register(Module *module) {
    if (!module) return;
    for (Module *m : registry())
        if (m == module) return;
    registry().push_back(module);
    if (module->enabled()) module->OnEnable();
}

void Unregister(Module *module) {
    auto &list = registry();
    for (auto it = list.begin(); it != list.end(); ++it) {
        if (*it == module) {
            if (module->enabled()) module->OnDisable();
            list.erase(it);
            return;
        }
    }
}

void UnregisterAll() {
    auto &list = registry();
    for (Module *m : list)
        if (m->enabled()) m->OnDisable();
    list.clear();
}

void Toggle(Module *module) {
    if (module) module->SetEnabled(!module->enabled());
}

void EnableAll() {
    for (Module *m : registry()) m->SetEnabled(true);
}

void DisableAll() {
    for (Module *m : registry()) m->SetEnabled(false);
}

int count() { return static_cast<int>(registry().size()); }

int count(Category category) {
    int n = 0;
    for (Module *m : registry())
        if (m->category() == category) ++n;
    return n;
}

std::size_t enabled_count() {
    std::size_t n = 0;
    for (Module *m : registry())
        if (m->enabled()) ++n;
    return n;
}

int enabled_count(Category category) {
    int n = 0;
    for (Module *m : registry())
        if (m->enabled() && m->category() == category) ++n;
    return n;
}

Module *module(int index) {
    const auto &list = registry();
    if (index < 0 || static_cast<std::size_t>(index) >= list.size()) return nullptr;
    return list[static_cast<std::size_t>(index)];
}

void Tick(float deltaTime) {
    for (Module *m : registry())
        if (m->enabled()) m->TickInternal(deltaTime);
}

void Render() {
    for (Module *m : registry())
        if (m->enabled()) m->OnRender();
}

void MenuRender() {
    for (Module *m : registry())
        if (m->enabled()) m->InMenuRender();
}

void SceneLoad(const char *sceneName, int buildIndex) {
    for (Module *m : registry()) // always, even while disabled
        m->OnSceneLoad(sceneName, buildIndex);
}

void SceneChanged(const char *fromScene, const char *toScene) {
    for (Module *m : registry()) // always, even while disabled
        m->OnSceneChanged(fromScene, toScene);
}

void QuickMenuInit() {
    for (Module *m : registry()) // always, even while disabled
        m->OnQuickMenuInit();
}

void MainMenuInit() {
    for (Module *m : registry()) // always, even while disabled
        m->OnMainMenuInit();
}

} // namespace System

#if 0 // ── copy this shape for your own features ─────────────────────────
namespace {
class TeleportCheatModule : public Modules::Module {
public:
    TeleportCheatModule()
        : Modules::Module("Teleport Cheat", "Teleport to anyone instantly",
                          Modules::Category::Movements) {}

    // Only override the hooks you need.
    void OnUpdate(float) override {}
    void OnUpdate01(float) override {}
    void OnRender() override {}
    void InMenuRender() override {}
};
TeleportCheatModule g_teleportCheat;
} // namespace
#endif

void RegisterModules() {
    // Register each of your modules here; each one gets a toggle in the menu.
    // Pass the module TYPE (not an instance): the system owns one instance per
    // type, so no global variable is needed.
    Modules::System::Register<Modules::VisualsModule>();
}

} // namespace Modules
)URKMODS";
}

std::string VisualsHeaderModule() {
    return R"URKVISMODH(#pragma once

#include "modules/modules.h"

namespace Modules {

// A module skeleton; copy it to add more features. Each registered module gets
// a toggle in the menu. While disabled, OnUpdate/OnRender/InMenuRender are not
// called, but OnSceneLoad/OnSceneChanged still are so state survives toggling.
class VisualsModule : public Modules::Module {
public:
    VisualsModule();

    // fired by the menu toggle
    void OnEnable() override;
    void OnDisable() override;

    // only called while the module is enabled
    void OnUpdate(float deltaTime) override;
    void OnUpdate01(float deltaTime) override;
    void OnUpdate05(float deltaTime) override;
    void OnRender() override;
    void InMenuRender() override;

    // always called
    void OnSceneLoad(const char *sceneName, int buildIndex) override;
    void OnSceneChanged(const char *fromScene, const char *toScene) override;

    // menu availability; called once for every module, enabled or not
    void OnQuickMenuInit() override;
    void OnMainMenuInit() override;
};

} // namespace Modules
)URKVISMODH";
}

std::string VisualsSourceModule() {
    return R"URKVISMODS(#include "modules/Visuals/Visuals.h"

namespace Modules {

VisualsModule::VisualsModule()
    : Modules::Module("Visuals", "Visual effects and rendering tweaks", Modules::Category::Visuals) {
}

void VisualsModule::OnEnable() {
}

void VisualsModule::OnDisable() {
}

void VisualsModule::OnUpdate(float deltaTime) {
    (void)deltaTime;
}

void VisualsModule::OnUpdate01(float deltaTime) {
    (void)deltaTime;
}

void VisualsModule::OnUpdate05(float deltaTime) {
    (void)deltaTime;
}

void VisualsModule::OnRender() {
}

void VisualsModule::InMenuRender() {
}

void VisualsModule::OnSceneLoad(const char *sceneName, int buildIndex) {
    (void)sceneName;
    (void)buildIndex;
}

void VisualsModule::OnSceneChanged(const char *fromScene, const char *toScene) {
    (void)fromScene;
    (void)toScene;
}

void VisualsModule::OnQuickMenuInit() {
}

void VisualsModule::OnMainMenuInit() {
}

} // namespace Modules
)URKVISMODS";
}

std::string ModHooksHeader() {
    return R"URK(#pragma once

#include "sdk/mod_sdk.h"

namespace ModHooks {
bool install(const URK_ModContext *ctx);
void uninstall();
} // namespace ModHooks
)URK";
}

std::string ModHooksSource(const ModuleProjectOptions &options) {
    std::ostringstream out;
    out << "#include \"mod_hooks.h\"\n\n"
        << "#include \"config/mod_config.h\"\n"
        << "#include \"support/mod_log.h\"\n\n"
        << "#include \"sdk/runtime_api.h\"\n"
        << "#include \"sdk/hook_api.h\"\n"
        << "#include \"render_imgui_hook.h\"\n"
        << "#include \"unity_log_hook.h\"\n"
        << "#include \"" << BackendRuntimeHeader(options) << "\"\n"
        << "#include \"" << BackendHelperHeader(options) << "\"\n";
    out << R"URK(
namespace {
URK::hooks::HookSet g_hooks;
} // namespace

namespace ModHooks {
bool install(const URK_ModContext *ctx) {
    URK::set_context(ctx);
)URK"
        << "  " << options.backendNamespace << "::init(ctx);\n"
        << R"URK(
if (!URK::hooks::available()) {
    ModLog::warn("hook API is unavailable; no hooks were installed");
    return true;
}

// Register validated targets with g_hooks so uninstall() can detach them.
if (ModConfig::enable_unity_log_hook && !ModUnityLogHook::install(ctx))
    ModLog::warn("Unity log hooks were requested but not installed");
if (!ModRenderHook::install(ctx))
    ModLog::warn("ImGui render hook could not be installed; continuing without menu");

ModLog::info("hook registry ready; ImGui render hook initialization requested");
return true;
}

void uninstall() {
    ModUnityLogHook::uninstall();
    if (!ModRenderHook::uninstall())
        ModLog::error("ImGui render hook shutdown was incomplete; see hook diagnostics");
    g_hooks.detach_all();
}
} // namespace ModHooks
)URK";
    return out.str();
}

std::string NetworkInitHeader() {
    return R"URK(#pragma once

#include "sdk/mod_sdk.h"

namespace ModNetwork {
bool init(const URK_ModContext *ctx);
void shutdown();
} // namespace ModNetwork
)URK";
}

std::string NetworkInitSource() {
    return R"URK(#include "mod_network.h"

#include "sdk/runtime_api.h"

namespace ModNetwork {
bool init(const URK_ModContext *ctx) {
    URK::set_context(ctx);
    return true;
}

void shutdown() {
}
} // namespace ModNetwork
)URK";
}

std::string GameRuntimeHeader() {
    return R"URK(#pragma once

#include "sdk/mod_async.h"
#include "sdk/mod_sdk.h"

namespace ModRuntime {
bool start(const URK_ModContext *ctx);
void update();
void on_scene_loaded(const URK_SceneInfo *scene);
void on_scene_changed(const URK_SceneInfo *previousScene, const URK_SceneInfo *currentScene);
void on_object_destroy_requested(const URK_ObjectDestroyRequest *request);
void stop();
} // namespace ModRuntime
)URK";
}

std::string GameRuntimeSource(const ModuleProjectOptions &options) {
    std::ostringstream out;
    out << "#include \"mod_runtime.h\"\n\n"
        << "#include \"support/mod_log.h\"\n\n"
        << "#include \"sdk/runtime_api.h\"\n"
        << "#include \"sdk/runtime_bootstrap.h\"\n"
        << "#include \"sdk/unity/unity.h\"\n"
        << "#include \"sdk/VRChat/Menus.h\"\n"
        << "#include \"modules/modules.h\"\n\n"
        << "namespace ModRuntime {\n"
        << "bool start(const URK_ModContext* ctx) {\n"
        << "  URK::set_context(ctx);\n"
        << "  if (!URK::initialize_backend(ctx)) {\n"
        << "    ModLog::error(\"" << EscapeString(options.backendDisplayName)
        << " runtime API initialization failed\");\n"
        << "    return false;\n"
        << "  }\n\n"
        << "  ModLog::info(\"runtime ready: backend=" << EscapeString(options.backendDisplayName)
        << " main_thread=%s scene_events=%s\", "
           "URK::has_main_thread() ? \"yes\" : \"no\", "
           "URK::has_scene_events() ? \"yes\" : \"no\");\n"
        << "  Modules::RegisterModules();\n"
        << "  return true;\n"
        << "}\n\n"
        << "void update() {\n"
        << "  VRC::poll_menus();\n"
        << "  Modules::System::Tick(Unity::Time::deltaTime());\n"
        << "}\n\n"
        << "void on_scene_loaded(const URK_SceneInfo* scene) {\n"
        << "  if (!scene || scene->size < sizeof(URK_SceneInfo)) return;\n"
        << "  ModLog::info(\"scene loaded: name=%s buildIndex=%d handle=%d\", "
           "scene->name, scene->buildIndex, scene->handle);\n"
        << "  Modules::System::SceneLoad(scene->name, scene->buildIndex);\n"
        << "}\n\n"
        << "void on_scene_changed(const URK_SceneInfo* previousScene, "
           "const URK_SceneInfo* currentScene) {\n"
        << "  if (!previousScene || !currentScene || "
           "previousScene->size < sizeof(URK_SceneInfo) || "
           "currentScene->size < sizeof(URK_SceneInfo)) return;\n"
        << "  ModLog::info(\"scene changed: %s -> %s\", previousScene->name, "
           "currentScene->name);\n"
        << "  Modules::System::SceneChanged(previousScene->name, currentScene->name);\n"
        << "}\n\n";
    out << "void on_object_destroy_requested(const URK_ObjectDestroyRequest* request) {\n"
        << "  if (!request || request->size < sizeof(URK_ObjectDestroyRequest)) return;\n"
        << "  ModLog::info(\"object destroy requested: name=%s type=%s instanceId=%d delay=%.3f immediate=%s\",\n"
        << "               request->name, request->typeName, request->instanceId, request->delaySeconds,\n"
        << "               (request->flags & URK_OBJECT_DESTROY_REQUEST_IMMEDIATE) ? \"yes\" : \"no\");\n"
        << "}\n\n";
    out << "void stop() {\n"
        << "  Modules::System::UnregisterAll();\n"
        << "  ModLog::info(\"runtime stopped\");\n"
        << "}\n"
        << "} // namespace ModRuntime\n";
    return out.str();
}


