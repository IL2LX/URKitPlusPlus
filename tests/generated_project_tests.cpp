// Generates a Mono and an IL2CPP mod project straight from the generator
// library, then compiles a probe translation unit against the result.
//
// The SDK templates are raw string literals, so nothing in the URKit build
// itself type-checks them; a syntax error or a broken API reaches users only
// when they build a generated project. The probe instantiates the templates that
// callers actually reach, because a header-only parse would not look inside an
// uninstantiated template.
//
// No Unity game is required: the generator only records the game directory in
// the manifest.

#include "src/sdk/il2cpp_sdk_generator.h"
#include "src/sdk/mono_sdk_generator.h"

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

namespace fs = std::filesystem;

namespace {

int g_failures = 0;

void Check(bool condition, const std::string &what) {
    std::printf("%-72s %s\n", what.c_str(), condition ? "ok" : "FAILED");
    if (!condition)
        ++g_failures;
}

void Write(const fs::path &path, std::string_view text) {
    fs::create_directories(path.parent_path());
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    out.write(text.data(), static_cast<std::streamsize>(text.size()));
}

bool CompilerIsMsvc() {
    std::string compiler = fs::path(URK_TEST_CXX_COMPILER).filename().string();
    for (char &character : compiler)
        character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
    return compiler.find("clang") == std::string::npos && compiler.find("cl") != std::string::npos;
}

std::string Quote(const std::string &value) {
    return "\"" + value + "\"";
}

// Compiles without linking; only diagnostics matter.
bool SyntaxCheck(const fs::path &projectRoot, const fs::path &source, const fs::path &workingDirectory) {
    const std::string root = projectRoot.string();
    std::string command = Quote(URK_TEST_CXX_COMPILER);
    if (CompilerIsMsvc()) {
        command += " /nologo /std:c++20 /Zs /EHsc /permissive- /DWIN32_LEAN_AND_MEAN /DNOMINMAX";
        command += " /I" + Quote(root) + " /I" + Quote((projectRoot / "mod").string());
        command += " /TP " + Quote(source.string());
    } else {
        command += " -std=c++20 -fsyntax-only -DWIN32_LEAN_AND_MEAN -DNOMINMAX";
        command += " -I" + Quote(root) + " -I" + Quote((projectRoot / "mod").string());
        command += " -x c++ " + Quote(source.string());
    }
    const fs::path previous = fs::current_path();
    fs::current_path(workingDirectory);
    const int status = std::system(("\"" + command + "\"").c_str());
    fs::current_path(previous);
    return status == 0;
}

// Reaches the version-sensitive template paths: both spellings of the object
// finders, unqualified type resolution, component access, and the IL2CPP helper
// surface. Nothing here runs; instantiation is the point.
constexpr std::string_view kProbeSource = R"PROBE(
#include "sdk/unity/unity.h"

#include <string>
#include <vector>

namespace {

void probe_object_finders() {
    // Both names must instantiate: Unity renamed this API in 2022.2, and the
    // generated SDK falls back across the rename in both directions.
    std::vector<URK::Unity::GameObject> byOldName = URK::Unity::Object::FindObjectsOfType<URK::Unity::GameObject>();
    std::vector<URK::Unity::GameObject> byNewName = URK::Unity::Object::FindObjectsByType<URK::Unity::GameObject>(
        URK::Unity::FindObjectsSortMode::None);
    std::vector<URK::Unity::GameObject> all = URK::Unity::Object::FindObjectsOfTypeAll<URK::Unity::GameObject>();
    (void)byOldName;
    (void)byNewName;
    (void)all;
}

void probe_unqualified_type_resolution() {
    // An empty image means "scan every loaded assembly" on both backends.
    constexpr URK::Unity::TypeRef unqualified{"", "MyGame", "PlayerController"};
    constexpr URK::Unity::TypeRef qualified{"Assembly-CSharp.dll", "MyGame", "PlayerController"};
    (void)unqualified.resolve_class();
    (void)unqualified.resolve_type_object();
    (void)qualified.resolve_class();
}

void probe_components(URK::Unity::GameObject object) {
    URK::Unity::Transform transform = object.GetComponent<URK::Unity::Transform>();
    (void)transform.position();
    transform.set_position(URK::Unity::Vector3{0.0f, 1.0f, 2.0f});
    (void)transform.localRotation();
    transform.set_localRotation(URK::Unity::Quaternion{0.0f, 0.0f, 0.0f, 1.0f});
    transform.SetPositionAndRotation(URK::Unity::Vector3{0.0f, 0.0f, 0.0f},
                                     URK::Unity::Quaternion{0.0f, 0.0f, 0.0f, 1.0f});
    transform.SetLocalPositionAndRotation(URK::Unity::Vector3{0.0f, 0.0f, 0.0f},
                                          URK::Unity::Quaternion{0.0f, 0.0f, 0.0f, 1.0f});
    transform.Translate(URK::Unity::Vector3{1.0f, 0.0f, 0.0f}, URK::Unity::Space::World);
    transform.Rotate(URK::Unity::Vector3{0.0f, 90.0f, 0.0f}, URK::Unity::Space::World);
    transform.RotateAround(URK::Unity::Vector3{0.0f, 0.0f, 0.0f}, URK::Unity::Vector3{0.0f, 1.0f, 0.0f}, 45.0f);
    transform.LookAt(URK::Unity::Vector3{0.0f, 0.0f, 10.0f});
    transform.LookAt(transform.parent());
    (void)transform.IsChildOf(transform.parent());
    transform.DetachChildren();
    (void)transform.TransformDirection(URK::Unity::Vector3{0.0f, 0.0f, 1.0f});
    (void)transform.InverseTransformDirection(URK::Unity::Vector3{0.0f, 0.0f, 1.0f});
    (void)transform.TransformPoint(URK::Unity::Vector3{0.0f, 0.0f, 1.0f});
    (void)transform.InverseTransformPoint(URK::Unity::Vector3{0.0f, 0.0f, 1.0f});
    (void)object.GetComponentInChildren<URK::Unity::Camera>(true);
    (void)object.GetComponents<URK::Unity::Object>();
    (void)object.HasComponent<URK::Unity::Renderer>();
    (void)object.name();
    (void)object.activeInHierarchy();
    object.set_tag("Player");
    (void)object.layer();
    object.set_layer(0);
    (void)object.CompareTag("Player");
    (void)URK::Unity::GameObject::CreatePrimitive(URK::Unity::PrimitiveType::Cube);
    object.SendMessage("OnInteract");
    object.SendMessage("OnInteract", URK::Unity::SendMessageOptions::DontRequireReceiver);
    object.SendMessageUpwards("OnInteract");
    object.BroadcastMessage("OnInteract");

    URK::Unity::Camera camera = object.GetComponent<URK::Unity::Camera>();
    (void)camera.orthographic();
    camera.set_orthographic(false);
    (void)camera.orthographicSize();
    camera.set_orthographicSize(5.0f);
    (void)camera.cullingMask();
    camera.set_cullingMask(-5);
    (void)camera.clearFlags();
    (void)camera.depth();
    camera.set_depth(0.0f);
    (void)camera.rect();
    camera.set_rect(URK::Unity::Rect{0.0f, 0.0f, 1.0f, 1.0f});
    (void)camera.pixelRect();
    camera.ResetProjectionMatrix();
    (void)URK::Unity::Camera::allCamerasCount();

    URK::Unity::Button button = object.GetComponent<URK::Unity::Button>();
    URK::Unity::UnityEvent onClick = button.onClick();
    const std::uint64_t onClickToken = onClick.AddListener([]() {});
    onClick.RemoveListener(onClickToken);
    onClick.RemoveListener([]() {});
    onClick.Invoke();
    onClick.ClearListeners();
    button.Click();

    URK::Unity::Toggle toggle = object.GetComponent<URK::Unity::Toggle>();
    URK::Unity::UnityEvent1<bool> toggleChanged = toggle.onValueChanged();
    toggleChanged.AddListener([](bool) {});
    toggleChanged.Invoke(true);

    URK::Unity::Slider slider = object.GetComponent<URK::Unity::Slider>();
    URK::Unity::UnityEvent1<float> sliderChanged = slider.onValueChanged();
    sliderChanged.AddListener([](float) {});
    sliderChanged.Invoke(0.5f);

    URK::Unity::Scrollbar scrollbar = object.GetComponent<URK::Unity::Scrollbar>();
    URK::Unity::UnityEvent1<float> scrollbarChanged = scrollbar.onValueChanged();
    (void)scrollbarChanged;

    URK::Unity::Dropdown dropdown = object.GetComponent<URK::Unity::Dropdown>();
    URK::Unity::UnityEvent1<int> dropdownChanged = dropdown.onValueChanged();
    (void)dropdownChanged;

    URK::Unity::InputField inputField = object.GetComponent<URK::Unity::InputField>();
    URK::Unity::UnityEvent1<std::string_view> inputChanged = inputField.onValueChanged();
    URK::Unity::UnityEvent1<std::string_view> inputEdited = inputField.onEndEdit();
    (void)inputChanged;
    (void)inputEdited;

    URK::Unity::TmpInputField tmpInput = object.GetComponent<URK::Unity::TmpInputField>();
    URK::Unity::UnityEvent1<std::string_view> tmpChanged = tmpInput.onValueChanged();
    URK::Unity::UnityEvent1<std::string_view> tmpEdited = tmpInput.onEndEdit();
    tmpChanged.AddListener([](std::string_view) {});
    (void)tmpEdited;
}

void probe_scene_traversal() {
    (void)URK::Unity::SceneManager::GetActiveScene();
    (void)URK::Unity::SceneManager::GetLoadedSceneRoots();
    (void)URK::Unity::SceneManager::FindSceneGameObjects(false);
    (void)URK::Unity::SceneManager::FindSceneGameObjects(true);
    URK::Unity::SceneManager::LoadScene("MainMenu_Variation1");
    URK::Unity::SceneManager::LoadScene(1);
    (void)URK::Unity::SceneManager::sceneCountInBuildSettings();
}

void probe_stripped_member_detection() {
    // Managed stripping removes UnityEngine members the game never calls, so
    // presence has to be answerable without invoking anything.
    (void)URK::Unity::has_method(URK::Unity::GameObjectType, "get_scene", 0);
    (void)URK::Unity::has_method(URK::Unity::TransformType, "SetParent", 1);
    (void)URK::Unity::has_property(URK::Unity::GameObjectType, "tag");
    (void)URK::Unity::GameObject::scene_available();
}

void probe_reflection(URK::Unity::Object target) {
    (void)target.GetField<int>("health");
    target.SetField<float>("speed", 1.0f);
    (void)target.GetProperty<bool>("enabled");
    (void)target.Call<int>("Damage", 5);
    (void)target.Call<void*>("Describe", "text");
}

void probe_statics() {
    (void)URK::Unity::Time::deltaTime();
    URK::Unity::Time::set_timeScale(1.0f);
    (void)URK::Unity::Time::timeAsDouble();
    (void)URK::Unity::Time::fixedDeltaTime();
    (void)URK::Unity::Time::unscaledTime();
    (void)URK::Unity::Time::frameCount();
    (void)URK::Unity::Time::realtimeSinceStartup();
    (void)URK::Unity::Time::smoothDeltaTime();
    (void)URK::Unity::Time::maximumDeltaTime();
    (void)URK::Unity::Time::timeSinceLevelLoad();
    (void)URK::Unity::Time::renderedFrameCount();
    (void)URK::Unity::Time::captureFramerate();
    (void)URK::Unity::Screen::width();
    (void)URK::Unity::Screen::fullScreen();
    URK::Unity::Screen::set_fullScreen(false);
    (void)URK::Unity::Input::GetKey(URK::Unity::KeyCode::A);
    (void)URK::Unity::Input::GetAxis("Horizontal");
    (void)URK::Unity::Input::GetButton("Jump");
    (void)URK::Unity::Input::mousePosition();
    (void)URK::Unity::Input::mouseScrollDelta();
    (void)URK::Unity::Input::anyKey();
    (void)URK::Unity::Input::touchCount();
    (void)URK::Unity::Application::dataPath();
    (void)URK::Unity::Application::persistentDataPath();
    (void)URK::Unity::Application::streamingAssetsPath();
    (void)URK::Unity::Application::isPlaying();
    (void)URK::Unity::Application::isEditor();
    (void)URK::Unity::Application::platform();
    (void)URK::Unity::Application::version();
    (void)URK::Unity::Application::unityVersion();
    (void)URK::Unity::Application::productName();
    (void)URK::Unity::Application::companyName();
    (void)URK::Unity::Application::targetFrameRate();
    URK::Unity::Application::set_targetFrameRate(60);
    URK::Unity::Application::Quit();
    URK::Unity::Application::Quit(0);
    URK::Unity::Application::OpenURL("https://example.com");
    URK::Unity::PlayerPrefs::SetInt("score", 10);
    URK::Unity::PlayerPrefs::SetFloat("volume", 0.5f);
    URK::Unity::PlayerPrefs::SetString("name", "Biscuit");
    (void)URK::Unity::PlayerPrefs::GetInt("score");
    (void)URK::Unity::PlayerPrefs::GetInt("score", 5);
    (void)URK::Unity::PlayerPrefs::GetFloat("volume");
    (void)URK::Unity::PlayerPrefs::GetString("name");
    (void)URK::Unity::PlayerPrefs::HasKey("score");
    URK::Unity::PlayerPrefs::DeleteKey("score");
    URK::Unity::PlayerPrefs::DeleteAll();
    URK::Unity::PlayerPrefs::Save();
    (void)URK::Unity::Cursor::visible();
    URK::Unity::Cursor::set_visible(true);
    (void)URK::Unity::Cursor::lockState();
    URK::Unity::Cursor::set_lockState(0);
    (void)URK::Unity::Physics::Raycast(URK::Unity::Vector3{0.0f, 0.0f, 0.0f},
                                       URK::Unity::Vector3{0.0f, 0.0f, 1.0f});
    (void)URK::Unity::Physics::Raycast(URK::Unity::Vector3{0.0f, 0.0f, 0.0f},
                                       URK::Unity::Vector3{0.0f, 0.0f, 1.0f}, 100.0f);
    (void)URK::Unity::Physics::Raycast(URK::Unity::Vector3{0.0f, 0.0f, 0.0f},
                                       URK::Unity::Vector3{0.0f, 0.0f, 1.0f}, 100.0f, -5, 2);
    (void)URK::Unity::Physics::SphereCast(URK::Unity::Vector3{0.0f, 0.0f, 0.0f}, 0.5f,
                                          URK::Unity::Vector3{0.0f, 0.0f, 1.0f}, 100.0f);
    (void)URK::Unity::Physics::SphereCast(URK::Unity::Vector3{0.0f, 0.0f, 0.0f}, 0.5f,
                                          URK::Unity::Vector3{0.0f, 0.0f, 1.0f}, 100.0f, -5, 2);
    (void)URK::Unity::Physics::OverlapSphere(URK::Unity::Vector3{0.0f, 0.0f, 0.0f}, 1.0f);
    (void)URK::Unity::Resources::Load("Items/Sword");
}

void probe_fq_method_handler() {
    // One-liner static calls resolved by full class name and cached arity.
    (void)URK::Unity::InvokeStaticFq<double>("VRC.SDKBase.Networking", "GetServerTimeInSeconds");
    (void)URK::Unity::InvokeStaticFq<float>("UnityEngine.Time", "get_deltaTime");
    (void)URK::Unity::InvokeStaticFq<std::vector<URK::Unity::GameObject>>("UnityEngine.Object",
                                                                          "FindObjectsOfType");

    // Resolve once, invoke repeatedly; argc = -1 skips the arity mismatch check.
    const URK::Unity::ResolvedMethod deltaTime =
        URK::Unity::ResolvedMethod::resolve("UnityEngine.Time", "get_deltaTime");
    (void)deltaTime.invoke<float>();
    (void)deltaTime.call<float>(static_cast<void *>(nullptr));

    const URK::Unity::ResolvedMethod resolveAny =
        URK::Unity::ResolvedMethod::resolve("UnityEngine.Debug", "Log");
    (void)resolveAny.invoke<void>("hello");

    URK::Unity::Object target;
    const URK::Unity::ResolvedMethod describe =
        URK::Unity::ResolvedMethod::resolve("System.Object", "ToString");
    (void)describe.call<std::string>(target);

    const URK::Unity::ResolvedMethod exact = URK::Unity::ResolvedMethod::resolve_exact(
        "UnityEngine.Debug", "Log", {"System.Object"});
    (void)exact.invoke<void>("hello");
    (void)exact.class_handle();
    (void)exact.method_handle();
    (void)exact.argc();
}

void probe_method_handler_and_resolver() {
    // MethodHandler mirrors the IL2CPP-SDK static API over ResolvedMethod.
    const URK::Unity::ResolvedMethod delta =
        URK::Unity::MethodHandler::resolve("UnityEngine.Time", "get_deltaTime");
    (void)URK::Unity::MethodHandler::invoke_raw(delta, nullptr);
    (void)URK::Unity::MethodHandler::invoke<float>(delta, nullptr);
    (void)URK::Unity::MethodHandler::invoke<void>(delta, nullptr);

    // ClassResolver fluent DSL. Compile-time verification only; resolution of
    // UnityEngine.UI.Button members against live metadata is a runtime concern
    // exercised by mods.
    URK::Unity::ClassResolver resolver = URK::Unity::ClassResolver::by_name("UnityEngine.UI.Button");
    (void)resolver.klass();
    (void)resolver.raw();

    int onClickOffset = 0;
    void *onClickPtr = nullptr;
    std::string onClickName;
    resolver.field().byName("m_OnClick").required().toOffset(onClickOffset).toPtr(onClickPtr).toName(onClickName)
        .label("Button.m_OnClick");

    int interactableOffset = 0;
    std::string interactableName;
    resolver.field().byName("m_Interactable").toOffset(interactableOffset).toName(interactableName)
        .label("Selectable.m_Interactable");

    void *onClickGetter = nullptr;
    resolver.property().byName("onClick").required().toGetter(onClickGetter).label("Button.onClick");

    int selectedColorOffset = 0;
    resolver.counter().byTypeName("UnityEngine.UI.ColorBlock").expectExact(1);
    resolver.collector().byTypeName("UnityEngine.UI.ColorBlock").requireCount(1).bindOffset(0, selectedColorOffset);
    (void)selectedColorOffset;

    std::vector<void *> pointerClickMethods;
    resolver.method().byName("OnPointerClick").collectAll().toPtrList(pointerClickMethods).label("Button.OnPointerClick");

    resolver.resolvePartial();
    (void)resolver.missReport();

    URK::Unity::ClassResolver objectResolver = URK::Unity::ClassResolver::by_name("UnityEngine.Object");
    void *destroyRaw = nullptr;
    objectResolver.method().byName("Destroy").withParams(1).required().toMethodRaw(destroyRaw).label("Object.Destroy");
    (void)destroyRaw;
}

void probe_sprite_loading() {
    const URK::Unity::Texture2D created = URK::Unity::Texture2D::Create(2, 2);
    (void)created;
    (void)URK::Unity::Texture2D::Create();
    (void)created.LoadImage(reinterpret_cast<void *>(0x1));
    (void)created.LoadImage(reinterpret_cast<void *>(0x1), true);
    URK::Unity::Texture2D fromFile = URK::Unity::Texture2D::LoadFromFile("C:\\test.png");
    (void)fromFile;

    const URK::Unity::Sprite sprite = URK::Unity::Sprite::LoadFromFile("C:\\test.png");
    (void)sprite;
    (void)sprite.GetTexture();
    (void)sprite.GetRect();
    (void)sprite.GetPivot();
    (void)sprite.GetBorder();
    (void)sprite.GetPixelsPerUnit();
    (void)sprite.GetPacked();
    (void)URK::Unity::Sprite::Create(created.handle(), URK::Unity::Rect{0.0f, 0.0f, 2.0f, 2.0f},
                                     URK::Unity::Vector2{0.5f, 0.5f});
    (void)URK::Unity::Sprite::Create(created.handle(), URK::Unity::Rect{0.0f, 0.0f, 2.0f, 2.0f},
                                     URK::Unity::Vector2{0.5f, 0.5f}, 100.0f, 0,
                                     URK::Unity::Sprite::MeshType::FullRect);
}

void keep_referenced(URK::Unity::GameObject object) {
    probe_object_finders();
    probe_unqualified_type_resolution();
    probe_components(object);
    probe_scene_traversal();
    probe_reflection(object);
    probe_statics();
    probe_fq_method_handler();
    probe_method_handler_and_resolver();
    probe_sprite_loading();
}

} // namespace

extern "C" void urk_generated_project_probe(void *handle) {
    keep_referenced(URK::Unity::GameObject{handle});
}
)PROBE";

constexpr std::string_view kIl2CppHelperProbeSource = R"PROBE(
#include "sdk/il2cpp/il2cpp_helpers.h"

extern "C" int urk_il2cpp_helper_probe(void *target) {
    // Guards a resolved icall target before it is hooked.
    return URK::il2cpp::helpers::is_valid_icall_target(target) ? 1 : 0;
}
)PROBE";

constexpr std::string_view kVrcSdkBaseProbeSource = R"PROBE(
#include "sdk/VRChat/VRC/SDKBase/Networking.h"
#include "sdk/VRChat/VRC/Core/APIUser.h"

#include <string>
#include <vector>

namespace {

void probe_vrc_sdkbase() {
    VRC::SDKBase::VRCPlayerApi local = VRC::SDKBase::Networking::LocalPlayer();
    (void)local;
    (void)VRC::SDKBase::Networking::Master();
    (void)VRC::SDKBase::Networking::InstanceOwner();
    (void)VRC::SDKBase::Networking::IsMaster();
    (void)VRC::SDKBase::Networking::IsNetworkSettled();
    (void)VRC::SDKBase::Networking::ServerTimeMs();
    (void)VRC::SDKBase::Networking::GetOwner(URK::Unity::GameObject{nullptr});
    (void)VRC::SDKBase::Networking::IsOwner(URK::Unity::GameObject{nullptr});
    (void)VRC::SDKBase::Networking::IsObjectReady(URK::Unity::GameObject{nullptr});
    (void)VRC::SDKBase::Networking::UniqueName(URK::Unity::GameObject{nullptr});
    VRC::SDKBase::Networking::SetOwner(local, URK::Unity::GameObject{nullptr});
    (void)VRC::SDKBase::Networking::PlayerObjects(local);
    (void)VRC::SDKBase::kNetworking;
    (void)VRC::Core::APIUser::unity_type();
    (void)VRC::Core::APIUser::GetCurrentUser();
    (void)VRC::Core::APIUser{nullptr}.GetBioLinks();
    (void)VRC::Core::APIUser{nullptr}.GetCurrentAvatarTags();
    (void)VRC::Core::APIUser{nullptr}.GetFriendIDs();
    (void)VRC::Core::APIUser{nullptr}.GetTags();
    (void)VRC::Core::APIUser{nullptr}.GetStatusHistory();
    (void)VRC::SDKBase::VRCPlayerApi::unity_type();
    (void)Unity::last_error();
    Unity::clear_error();
}

void keep_referenced() {
    probe_vrc_sdkbase();
}

} // namespace

extern "C" void urk_generated_vrc_probe() {
    keep_referenced();
}
)PROBE";

constexpr std::string_view kModulesProbeSource = R"PROBE(
#include "mod/modules/modules.cpp"

#include <cstring>
#include <cstdio>

namespace {

struct ProbeModule : Modules::Module {
    ProbeModule() : Modules::Module("Probe Module", "probe description", Modules::Category::Movements) {}

    int updates = 0;
    int update01s = 0;
    int update05s = 0;
    int renders = 0;
    int menuRenders = 0;
    int onEnables = 0;
    int onDisables = 0;
    int sceneLoads = 0;
    int sceneChanges = 0;

    void OnEnable() override { ++onEnables; }
    void OnDisable() override { ++onDisables; }
    void OnUpdate(float) override { ++updates; }
    void OnUpdate01(float) override { ++update01s; }
    void OnUpdate05(float) override { ++update05s; }
    void OnRender() override { ++renders; }
    void InMenuRender() override { ++menuRenders; }
    void OnSceneLoad(const char *, int) override { ++sceneLoads; }
    void OnSceneChanged(const char *, const char *) override { ++sceneChanges; }
};

ProbeModule g_probe;

void probe_categorized_modules() {
    // metadata
    std::strlen(g_probe.name());
    std::strlen(g_probe.description());
    std::strlen(Modules::category_name(g_probe.category()));
    std::strlen(Modules::category_name(Modules::Category::Visuals));
    std::strlen(Modules::category_name(Modules::Category::Movements));

    // registry
    Modules::System::Register(&g_probe);
    Modules::System::Register(&g_probe); // dedupe
    const int before = Modules::System::count();
    (void)Modules::System::module(0);
    (void)Modules::System::module(50); // out of range -> nullptr
    (void)Modules::System::count(Modules::Category::Movements);
    (void)Modules::System::count(Modules::Category::Visuals);
    (void)Modules::System::enabled_count();
    (void)Modules::System::enabled_count(Modules::Category::Movements);

    // disabled: no update/render, but scene events are still delivered
    Modules::System::Toggle(&g_probe); // enabled -> disabled, fires OnDisable
    Modules::System::Tick(0.1f);
    Modules::System::Render();
    Modules::System::MenuRender();
    Modules::System::SceneLoad("VRChat_Home", 1);
    Modules::System::SceneChanged("VRChat_Home", "SomeWorld");
    if (g_probe.updates != 0 || g_probe.renders != 0 || g_probe.menuRenders != 0 ||
        g_probe.sceneLoads != 1 || g_probe.sceneChanges != 1) {
        std::printf("FAILED: disabled module leaked update/render or skipped scene event\n");
    }

    // re-enable: update and renders resume
    Modules::System::Toggle(&g_probe); // disabled -> enabled, fires OnEnable
    Modules::System::EnableAll();      // no-op (stays enabled)
    Modules::System::DisableAll();     // enabled -> disabled, fires OnDisable
    Modules::System::EnableAll();      // disabled -> enabled, fires OnEnable
    Modules::System::Tick(0.1f);
    Modules::System::Render();
    Modules::System::MenuRender();
    Modules::System::SceneLoad("VRChat_Home", 2);
    Modules::System::SceneChanged("VRChat_Home", "OtherWorld");

    Modules::System::Unregister(&g_probe);
    Modules::System::Unregister(&g_probe); // safe no-op
    const int after = Modules::System::count();

    const bool ok = before == 1 && after == 0 && g_probe.updates == 1 &&
                    g_probe.update01s == 1 && g_probe.update05s == 0 &&
                    g_probe.renders == 1 && g_probe.menuRenders == 1 &&
                    g_probe.sceneLoads == 2 && g_probe.sceneChanges == 2 &&
                    g_probe.onEnables == 3 && g_probe.onDisables == 3;
    if (!ok) {
        std::printf("FAILED: module system probe invariants (updates=%d u01=%d u05=%d "
                    "renders=%d menu=%d loads=%d changes=%d en=%d dis=%d)\n",
                    g_probe.updates, g_probe.update01s, g_probe.update05s, g_probe.renders,
                    g_probe.menuRenders, g_probe.sceneLoads, g_probe.sceneChanges,
                    g_probe.onEnables, g_probe.onDisables);
    }
}

void keep_referenced() {
    probe_categorized_modules();
}

} // namespace

extern "C" void urk_generated_modules_probe() {
    keep_referenced();
}
)PROBE";

constexpr std::string_view kRuntimeConfigProbeSource = R"PROBE(
#include "mod/config/RuntimeConfig.h"

#include <string>

namespace {

void probe_runtime_config() {
    (void)RuntimeConfig::OpenOrCreateConfig("Probe");
    (void)RuntimeConfig::SaveConfig();
    (void)RuntimeConfig::Has("Movement", "Probe", "speed");
    RuntimeConfig::SetBool("Movement", "Probe", "was_enabled_before", true);
    RuntimeConfig::SetInt("Movement", "Probe", "max_players", 8);
    RuntimeConfig::SetFloat("Movement", "Probe", "speed", 1.5f);
    RuntimeConfig::SetDouble("Movement", "Probe", "height", 2.0);
    RuntimeConfig::SetString("Movement", "Probe", "nickname", "urk");
    const RuntimeConfig::Color color{1.0f, 0.5f, 0.25f, 1.0f};
    RuntimeConfig::SetColor("Movement", "Probe", "tint", color);

    const bool was_enabled = RuntimeConfig::GetBool("Movement", "Probe", "was_enabled_before", false);
    const int max_players = RuntimeConfig::GetInt("Movement", "Probe", "max_players", 1);
    const float speed = RuntimeConfig::GetFloat("Movement", "Probe", "speed", 0.0f);
    const double height = RuntimeConfig::GetDouble("Movement", "Probe", "height", 0.0);
    const std::string nickname = RuntimeConfig::GetString("Movement", "Probe", "nickname", "");
    (void)was_enabled;
    (void)max_players;
    (void)speed;
    (void)height;
    (void)nickname;

    RuntimeConfig::Color read{};
    (void)RuntimeConfig::GetColor("Movement", "Probe", "tint", read, RuntimeConfig::Color{});

    (void)RuntimeConfig::Remove("Movement", "Probe", "nickname");
}

void keep_referenced() {
    probe_runtime_config();
}

} // namespace

extern "C" void urk_generated_runtime_config_probe() {
    keep_referenced();
}
)PROBE";



struct GeneratedProject {
    fs::path root;
    std::string label;
};

bool GenerateBoth(const fs::path &workspace, std::vector<GeneratedProject> *projects) {
    const fs::path gameDirectory = workspace / "game";
    fs::create_directories(gameDirectory);
    std::string error;

    const fs::path monoSdk = workspace / "staged" / "mono";
    if (!MonoSdkGenerator::Generate(monoSdk.string(), "", &error)) {
        std::printf("FAILED: Mono SDK staging: %s\n", error.c_str());
        return false;
    }
    const fs::path monoProject = workspace / "mono" / "project";
    if (!MonoSdkGenerator::GenerateModProject(monoProject.string(), monoSdk.string(), URK_TEST_SDK_DIR, "SmokeMono",
                                              gameDirectory.string(), "Mods", true, &error)) {
        std::printf("FAILED: Mono project generation: %s\n", error.c_str());
        return false;
    }
    projects->push_back({monoProject, "mono"});

    const fs::path il2cppSdk = workspace / "staged" / "il2cpp";
    if (!Il2CppSdkGenerator::Generate(il2cppSdk.string(), "", &error)) {
        std::printf("FAILED: IL2CPP SDK staging: %s\n", error.c_str());
        return false;
    }
    const fs::path il2cppProject = workspace / "il2cpp" / "project";
    if (!Il2CppSdkGenerator::GenerateModProject(il2cppProject.string(), il2cppSdk.string(), URK_TEST_SDK_DIR,
                                                "SmokeIl2Cpp", gameDirectory.string(), "Mods", true, &error)) {
        std::printf("FAILED: IL2CPP project generation: %s\n", error.c_str());
        return false;
    }
    projects->push_back({il2cppProject, "il2cpp"});
    return true;
}

std::string ReadText(const fs::path &path) {
    std::ifstream in(path, std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

// A setter whose C++ parameter is the generic `Object` wrapper cannot be
// dispatched by inferred type: inference yields "UnityEngine.Object" while the
// property's declared C# type is something narrower (AudioClip, Font, ...), so
// the exact-overload lookup never matches and the call silently never happens.
// Such setters must name the declared type through CallExact.
void CheckObjectSettersAreExact(const GeneratedProject &project) {
    const std::string text = ReadText(project.root / "sdk" / "unity" / "unity_components.h");
    Check(!text.empty(), project.label + ": unity_components.h is readable");

    std::size_t offset = 0;
    int checked = 0;
    bool clean = true;
    const std::string_view needle = "(Object value) const {";
    while ((offset = text.find(needle, offset)) != std::string::npos) {
        const std::size_t lineStart = text.rfind('\n', offset) + 1;
        const std::size_t bodyEnd = text.find('}', offset);
        const std::string declaration = text.substr(lineStart, offset - lineStart);
        if (declaration.find("void set_") == std::string::npos) {
            offset += needle.size();
            continue;
        }
        ++checked;
        const std::string body = text.substr(offset, bodyEnd - offset);
        if (body.find("CallExact<void>") == std::string::npos) {
            std::printf("  generic-Object setter without CallExact: %s\n", declaration.c_str());
            clean = false;
        }
        offset += needle.size();
    }
    Check(checked > 0, project.label + ": generic-Object property setters are present");
    Check(clean, project.label + ": generic-Object property setters dispatch via CallExact");
}

void CheckLayout(const GeneratedProject &project) {
    const char *const required[] = {
        "CMakeLists.txt",
        "CMakePresets.json",
        ".urk/project.ini",
        ".urk/generated-files.ini",
        "sdk/mod_sdk.h",
        "sdk/runtime_api.h",
        "mod/config/RuntimeConfig.h",
        "sdk/unity/unity.h",
        "sdk/unity/unity_types.h",
        "sdk/unity/unity_invoke.h",
        "sdk/unity/unity_components.h",
        "sdk/unity/unity_shortcuts.h",
        "sdk/VRChat/VRC/SDKBase/VRCPlayerAPI.h",
        "sdk/VRChat/VRC/SDKBase/Networking.h",
        "mod/config/mod_config.h",
        "mod/modules/modules.h",
        "mod/modules/modules.cpp",
        "mod/modules/Visuals/Visuals.h",
        "mod/modules/Visuals/Visuals.cpp",
        "mod/hooks/render_imgui_hook.cpp",
        "mod/hooks/win32_viewport_policy.cpp",
    };
    for (const char *relative : required) {
        std::error_code code;
        const fs::path path = project.root / relative;
        const bool present = fs::is_regular_file(path, code) && fs::file_size(path, code) > 0 && !code;
        Check(present, project.label + ": " + relative + " is generated and non-empty");
    }
}

} // namespace

int main(int argc, char **argv) {
    fs::path workspace;
    bool keep = false;
    for (int index = 1; index < argc; ++index) {
        const std::string_view argument = argv[index];
        if (argument == "--keep" && index + 1 < argc) {
            workspace = fs::path(argv[++index]);
            keep = true;
        }
    }
    std::error_code cleanup;
    if (workspace.empty())
        workspace = fs::temp_directory_path() / "urk_generated_project_tests";
    fs::remove_all(workspace, cleanup);
    fs::create_directories(workspace);

    std::vector<GeneratedProject> projects;
    if (!GenerateBoth(workspace, &projects))
        return 1;

    for (const GeneratedProject &project : projects) {
        CheckLayout(project);
        CheckObjectSettersAreExact(project);

        const fs::path probe = project.root / "urk_probe_unity.cpp";
        Write(probe, kProbeSource);
        Check(SyntaxCheck(project.root, probe, project.root),
              project.label + ": generated Unity SDK compiles against real call sites");
        fs::remove(probe, cleanup);

        const fs::path vrcProbe = project.root / "urk_probe_vrc.cpp";
        Write(vrcProbe, kVrcSdkBaseProbeSource);
        Check(SyntaxCheck(project.root, vrcProbe, project.root),
              project.label + ": generated VRChat SDKBase header compiles against real call sites");
        fs::remove(vrcProbe, cleanup);

        const fs::path moduleProbe = project.root / "urk_probe_modules.cpp";
        Write(moduleProbe, kModulesProbeSource);
        Check(SyntaxCheck(project.root, moduleProbe, project.root),
              project.label + ": generated global Modules source compiles");
        fs::remove(moduleProbe, cleanup);

        const fs::path configProbe = project.root / "urk_probe_runtime_config.cpp";
        Write(configProbe, kRuntimeConfigProbeSource);
        Check(SyntaxCheck(project.root, configProbe, project.root),
              project.label + ": generated RuntimeConfig header compiles against real call sites");
        fs::remove(configProbe, cleanup);

        if (project.label == "il2cpp") {
            const fs::path helperProbe = project.root / "urk_probe_il2cpp_helpers.cpp";
            Write(helperProbe, kIl2CppHelperProbeSource);
            Check(SyntaxCheck(project.root, helperProbe, project.root),
                  project.label + ": generated IL2CPP helpers compile");
            fs::remove(helperProbe, cleanup);
        }
    }

    if (!keep)
        fs::remove_all(workspace, cleanup);
    else
        std::printf("\nGenerated projects kept in %s\n", workspace.string().c_str());

    if (g_failures) {
        std::printf("\n%d FAILURE(S)\n", g_failures);
        return 1;
    }
    std::printf("\nALL PASS (0 failures)\n");
    return 0;
}
