    out << R"URKUNITY(// URK_UNITY_SHORTCUTS_BEGIN
namespace Debug {
inline void Log(std::string_view message) {
    detail::InvokeStatic<void>(DebugType, "Log", message);
}
inline void LogWarning(std::string_view message) {
    detail::InvokeStatic<void>(DebugType, "LogWarning", message);
}
inline void LogError(std::string_view message) {
    detail::InvokeStatic<void>(DebugType, "LogError", message);
}
}
namespace Screen {
inline int width() {
    return detail::InvokeStatic<int>(ScreenType, "get_width");
}
inline int height() {
    return detail::InvokeStatic<int>(ScreenType, "get_height");
}
inline float dpi() {
    return detail::InvokeStatic<float>(ScreenType, "get_dpi");
}
inline bool fullScreen() {
    return detail::InvokeStatic<bool>(ScreenType, "get_fullScreen");
}
inline void set_fullScreen(bool value) {
    detail::InvokeStatic<void>(ScreenType, "set_fullScreen", value);
}
}
namespace ObjectFilter {
inline constexpr int kHideFlagsHierarchyMask = 1;
inline constexpr int kHideFlagsImplicitRuntimeMask = 4 | 16 | 32;
inline bool has(std::uint32_t flags, ObjectFilterFlags flag) {
    return (flags & static_cast<std::uint32_t>(flag)) != 0;
}
inline bool include_scene(Scene scene, std::uint32_t flags = static_cast<std::uint32_t>(ObjectFilterFlags::None)) {
    if (!scene || !scene.IsValid() || !scene.isLoaded())
        return false;
    if (!has(flags, ObjectFilterFlags::IncludeDontDestroyOnLoad) && scene.isDontDestroyOnLoad())
        return false;
    return true;
}
inline bool include_game_object(GameObject object,
                                std::uint32_t flags = static_cast<std::uint32_t>(ObjectFilterFlags::None)) {
    if (!object || !object.alive())
        return false;
    if (!has(flags, ObjectFilterFlags::IncludeInactive) && !object.activeInHierarchy())
        return false;
    const int hide_flags = object.hideFlags();
    if (!has(flags, ObjectFilterFlags::IncludeHidden) &&
        (hide_flags & (kHideFlagsHierarchyMask | kHideFlagsImplicitRuntimeMask)) != 0) {
        return false;
    }
    return include_scene(object.scene(), flags);
}
inline std::vector<GameObject> canonicalize_game_objects(
    const std::vector<GameObject> &objects, std::uint32_t flags = static_cast<std::uint32_t>(ObjectFilterFlags::None)) {
    std::vector<GameObject> filtered;
    filtered.reserve(objects.size());
    std::unordered_set<void *> seen;
    for (const GameObject &object : objects) {
        void *handle = object.handle();
        if (!handle || !seen.insert(handle).second)
            continue;
        if (include_game_object(object, flags))
            filtered.push_back(object);
    }
    return filtered;
}
}
namespace SceneManager {
inline TypeRef type() {
    return {"", "UnityEngine.SceneManagement", "SceneManager"};
}
inline Scene GetActiveScene() {
    return Scene{detail::InvokeStatic<void *>(type(), "GetActiveScene")};
}
inline int sceneCount() {
    return detail::InvokeStatic<int>(type(), "get_sceneCount");
}
inline Scene GetSceneAt(int index) {
    return Scene{detail::InvokeStatic<void *>(type(), "GetSceneAt", index)};
}
inline std::vector<Scene> GetLoadedScenes() {
    std::vector<Scene> scenes;
    const int count = sceneCount();
    if (count <= 0)
        return scenes;
    scenes.reserve(static_cast<std::size_t>(count));
    for (int i = 0; i < count; ++i) {
        Scene scene = GetSceneAt(i);
        if (scene && scene.IsValid())
            scenes.push_back(scene);
    }
    return scenes;
}
inline std::vector<GameObject> GetLoadedSceneRootsFiltered(
    std::uint32_t filterFlags = static_cast<std::uint32_t>(ObjectFilterFlags::None)) {
    std::vector<GameObject> roots;
    for (const Scene &scene : GetLoadedScenes()) {
        if (!ObjectFilter::include_scene(scene, filterFlags))
            continue;
        std::vector<GameObject> sceneRoots = scene.GetRootGameObjects();
        roots.insert(roots.end(), sceneRoots.begin(), sceneRoots.end());
    }
    return ObjectFilter::canonicalize_game_objects(roots, filterFlags);
}
inline std::vector<GameObject> GetLoadedSceneRoots() {
    return GetLoadedSceneRootsFiltered();
}
inline std::vector<GameObject> FindSceneGameObjectsFiltered(
    std::uint32_t filterFlags = static_cast<std::uint32_t>(ObjectFilterFlags::None)) {
    const bool includeInactive = ObjectFilter::has(filterFlags, ObjectFilterFlags::IncludeInactive);
    std::vector<GameObject> objects;
    if (!includeInactive)
        objects = Object::FindObjectsByType<GameObject>("", "UnityEngine", "GameObject", FindObjectsSortMode::None);
    else
        objects = Object::FindObjectsOfTypeAll<GameObject>("", "UnityEngine", "GameObject");
    return ObjectFilter::canonicalize_game_objects(objects, filterFlags);
}
inline std::vector<GameObject> FindSceneGameObjects(bool includeInactive = true) {
    return FindSceneGameObjectsFiltered(includeInactive ? static_cast<std::uint32_t>(ObjectFilterFlags::IncludeInactive)
                                                        : static_cast<std::uint32_t>(ObjectFilterFlags::None));
}
inline void LoadScene(std::string_view name) {
    ResolvedMethod::resolve_exact("UnityEngine.SceneManagement.SceneManager", "LoadScene", {"System.String"})
        .invoke<void>(name);
}
inline void LoadScene(int buildIndex) {
    ResolvedMethod::resolve_exact("UnityEngine.SceneManagement.SceneManager", "LoadScene", {"System.Int32"})
        .invoke<void>(buildIndex);
}
inline int sceneCountInBuildSettings() {
    return detail::InvokeStatic<int>(type(), "get_sceneCountInBuildSettings");
}
}
namespace Time {
inline float time() {
    return detail::InvokeStatic<float>(TimeType, "get_time");
}
inline float deltaTime() {
    return detail::InvokeStatic<float>(TimeType, "get_deltaTime");
}
inline float unscaledDeltaTime() {
    return detail::InvokeStatic<float>(TimeType, "get_unscaledDeltaTime");
}
inline float timeScale() {
    return detail::InvokeStatic<float>(TimeType, "get_timeScale");
}
inline void set_timeScale(float value) {
    detail::InvokeStatic<void>(TimeType, "set_timeScale", value);
}
inline double timeAsDouble() {
    return detail::InvokeStatic<double>(TimeType, "get_timeAsDouble");
}
inline float fixedDeltaTime() {
    return detail::InvokeStatic<float>(TimeType, "get_fixedDeltaTime");
}
inline float fixedTime() {
    return detail::InvokeStatic<float>(TimeType, "get_fixedTime");
}
inline float unscaledTime() {
    return detail::InvokeStatic<float>(TimeType, "get_unscaledTime");
}
inline int frameCount() {
    return detail::InvokeStatic<int>(TimeType, "get_frameCount");
}
inline int renderedFrameCount() {
    return detail::InvokeStatic<int>(TimeType, "get_renderedFrameCount");
}
inline float realtimeSinceStartup() {
    return detail::InvokeStatic<float>(TimeType, "get_realtimeSinceStartup");
}
inline float smoothDeltaTime() {
    return detail::InvokeStatic<float>(TimeType, "get_smoothDeltaTime");
}
inline float maximumDeltaTime() {
    return detail::InvokeStatic<float>(TimeType, "get_maximumDeltaTime");
}
inline float timeSinceLevelLoad() {
    return detail::InvokeStatic<float>(TimeType, "get_timeSinceLevelLoad");
}
inline int captureFramerate() {
    return detail::InvokeStatic<int>(TimeType, "get_captureFramerate");
}
}
constexpr TypeRef InputType{"", "UnityEngine", "Input"};
constexpr TypeRef ApplicationType{"", "UnityEngine", "Application"};
constexpr TypeRef PlayerPrefsType{"", "UnityEngine", "PlayerPrefs"};
constexpr TypeRef PhysicsType{"", "UnityEngine", "Physics"};
constexpr TypeRef CursorType{"", "UnityEngine", "Cursor"};
namespace Input {
inline bool available() {
    return URK::has_input();
}
inline bool GetKey(int keyCode) {
    return URK::input_get_key(keyCode);
}
inline bool GetKeyDown(int keyCode) {
    return URK::input_get_key_down(keyCode);
}
inline bool GetKeyUp(int keyCode) {
    return URK::input_get_key_up(keyCode);
}
inline bool GetMouseButton(int button) {
    return URK::input_get_mouse_button(button);
}
inline bool GetMouseButtonDown(int button) {
    return URK::input_get_mouse_button_down(button);
}
inline bool GetMouseButtonUp(int button) {
    return URK::input_get_mouse_button_up(button);
}
inline bool GetKey(KeyCode keyCode) {
    return GetKey(static_cast<int>(keyCode));
}
inline bool GetKeyDown(KeyCode keyCode) {
    return GetKeyDown(static_cast<int>(keyCode));
}
inline bool GetKeyUp(KeyCode keyCode) {
    return GetKeyUp(static_cast<int>(keyCode));
}
inline bool GetMouseButton(MouseButton button) {
    return GetMouseButton(static_cast<int>(button));
}
inline bool GetMouseButtonDown(MouseButton button) {
    return GetMouseButtonDown(static_cast<int>(button));
}
inline bool GetMouseButtonUp(MouseButton button) {
    return GetMouseButtonUp(static_cast<int>(button));
}
inline Vector3 mousePosition() {
    return detail::InvokeStatic<Vector3>(InputType, "get_mousePosition");
}
inline Vector2 mouseScrollDelta() {
    return detail::InvokeStatic<Vector2>(InputType, "get_mouseScrollDelta");
}
inline bool anyKey() {
    return detail::InvokeStatic<bool>(InputType, "get_anyKey");
}
inline bool anyKeyDown() {
    return detail::InvokeStatic<bool>(InputType, "get_anyKeyDown");
}
inline float GetAxis(std::string_view name) {
    return detail::InvokeStatic<float>(InputType, "GetAxis", name);
}
inline float GetAxisRaw(std::string_view name) {
    return detail::InvokeStatic<float>(InputType, "GetAxisRaw", name);
}
inline bool GetButton(std::string_view name) {
    return detail::InvokeStatic<bool>(InputType, "GetButton", name);
}
inline bool GetButtonDown(std::string_view name) {
    return detail::InvokeStatic<bool>(InputType, "GetButtonDown", name);
}
inline bool GetButtonUp(std::string_view name) {
    return detail::InvokeStatic<bool>(InputType, "GetButtonUp", name);
}
inline int touchCount() {
    return detail::InvokeStatic<int>(InputType, "get_touchCount");
}
}
namespace Application {
inline std::string dataPath() {
    return detail::managed_string_to_utf8(detail::InvokeStatic<void *>(ApplicationType, "get_dataPath"));
}
inline std::string persistentDataPath() {
    return detail::managed_string_to_utf8(detail::InvokeStatic<void *>(ApplicationType, "get_persistentDataPath"));
}
inline std::string streamingAssetsPath() {
    return detail::managed_string_to_utf8(detail::InvokeStatic<void *>(ApplicationType, "get_streamingAssetsPath"));
}
inline bool isPlaying() {
    return detail::InvokeStatic<bool>(ApplicationType, "get_isPlaying");
}
inline bool isEditor() {
    return detail::InvokeStatic<bool>(ApplicationType, "get_isEditor");
}
inline bool isBatchMode() {
    return detail::InvokeStatic<bool>(ApplicationType, "get_isBatchMode");
}
inline int platform() {
    return detail::InvokeStatic<int>(ApplicationType, "get_platform");
}
inline std::string version() {
    return detail::managed_string_to_utf8(detail::InvokeStatic<void *>(ApplicationType, "get_version"));
}
inline std::string unityVersion() {
    return detail::managed_string_to_utf8(detail::InvokeStatic<void *>(ApplicationType, "get_unityVersion"));
}
inline std::string productName() {
    return detail::managed_string_to_utf8(detail::InvokeStatic<void *>(ApplicationType, "get_productName"));
}
inline std::string companyName() {
    return detail::managed_string_to_utf8(detail::InvokeStatic<void *>(ApplicationType, "get_companyName"));
}
inline int targetFrameRate() {
    return detail::InvokeStatic<int>(ApplicationType, "get_targetFrameRate");
}
inline void set_targetFrameRate(int value) {
    detail::InvokeStatic<void>(ApplicationType, "set_targetFrameRate", value);
}
inline void Quit() {
    detail::InvokeStatic<void>(ApplicationType, "Quit");
}
inline void Quit(int exitCode) {
    detail::InvokeStatic<void>(ApplicationType, "Quit", exitCode);
}
inline void OpenURL(std::string_view url) {
    detail::InvokeStatic<void>(ApplicationType, "OpenURL", url);
}
}
namespace PlayerPrefs {
inline void SetInt(std::string_view key, int value) {
    detail::InvokeStatic<void>(PlayerPrefsType, "SetInt", key, value);
}
inline int GetInt(std::string_view key, int defaultValue = 0) {
    return detail::InvokeStatic<int>(PlayerPrefsType, "GetInt", key, defaultValue);
}
inline void SetFloat(std::string_view key, float value) {
    detail::InvokeStatic<void>(PlayerPrefsType, "SetFloat", key, value);
}
inline float GetFloat(std::string_view key, float defaultValue = 0.0f) {
    return detail::InvokeStatic<float>(PlayerPrefsType, "GetFloat", key, defaultValue);
}
inline void SetString(std::string_view key, std::string_view value) {
    detail::InvokeStatic<void>(PlayerPrefsType, "SetString", key, value);
}
inline std::string GetString(std::string_view key, std::string_view defaultValue = "") {
    return detail::managed_string_to_utf8(
        detail::InvokeStatic<void *>(PlayerPrefsType, "GetString", key, defaultValue));
}
inline bool HasKey(std::string_view key) {
    return detail::InvokeStatic<bool>(PlayerPrefsType, "HasKey", key);
}
inline void DeleteKey(std::string_view key) {
    detail::InvokeStatic<void>(PlayerPrefsType, "DeleteKey", key);
}
inline void DeleteAll() {
    detail::InvokeStatic<void>(PlayerPrefsType, "DeleteAll");
}
inline void Save() {
    detail::InvokeStatic<void>(PlayerPrefsType, "Save");
}
}
namespace Cursor {
inline bool visible() {
    return detail::InvokeStatic<bool>(CursorType, "get_visible");
}
inline void set_visible(bool value) {
    detail::InvokeStatic<void>(CursorType, "set_visible", value);
}
inline int lockState() {
    return detail::InvokeStatic<int>(CursorType, "get_lockState");
}
inline void set_lockState(int value) {
    detail::InvokeStatic<void>(CursorType, "set_lockState", value);
}
}
namespace Physics {
// UnityEngine.Physics keeps Ray- and Vector3-based overloads with the same
// arity (Raycast(Ray, float, int) vs Raycast(Vector3, Vector3, float) and the
// SphereCast equivalents), so every overload here pins its signature explicitly
// with ResolvedMethod::resolve_exact; an argc-only lookup could silently bind
// the wrong method.
inline bool Raycast(Vector3 origin, Vector3 direction) {
    return ResolvedMethod::resolve_exact("UnityEngine.Physics", "Raycast",
                                         {"UnityEngine.Vector3", "UnityEngine.Vector3"})
        .invoke<bool>(origin, direction);
}
inline bool Raycast(Vector3 origin, Vector3 direction, float maxDistance) {
    return ResolvedMethod::resolve_exact("UnityEngine.Physics", "Raycast",
                                         {"UnityEngine.Vector3", "UnityEngine.Vector3", "System.Single"})
        .invoke<bool>(origin, direction, maxDistance);
}
inline bool Raycast(Vector3 origin, Vector3 direction, float maxDistance, int layerMask) {
    return ResolvedMethod::resolve_exact("UnityEngine.Physics", "Raycast",
                                         {"UnityEngine.Vector3", "UnityEngine.Vector3", "System.Single",
                                          "System.Int32"})
        .invoke<bool>(origin, direction, maxDistance, layerMask);
}
inline bool Raycast(Vector3 origin, Vector3 direction, float maxDistance, int layerMask, int queryTriggerInteraction) {
    return ResolvedMethod::resolve_exact("UnityEngine.Physics", "Raycast",
                                         {"UnityEngine.Vector3", "UnityEngine.Vector3", "System.Single",
                                          "System.Int32", "UnityEngine.QueryTriggerInteraction"})
        .invoke<bool>(origin, direction, maxDistance, layerMask, queryTriggerInteraction);
}
inline bool SphereCast(Vector3 origin, float radius, Vector3 direction, float maxDistance) {
    return ResolvedMethod::resolve_exact("UnityEngine.Physics", "SphereCast",
                                         {"UnityEngine.Vector3", "System.Single", "UnityEngine.Vector3",
                                          "System.Single"})
        .invoke<bool>(origin, radius, direction, maxDistance);
}
inline bool SphereCast(Vector3 origin, float radius, Vector3 direction, float maxDistance, int layerMask) {
    return ResolvedMethod::resolve_exact("UnityEngine.Physics", "SphereCast",
                                         {"UnityEngine.Vector3", "System.Single", "UnityEngine.Vector3",
                                          "System.Single", "System.Int32"})
        .invoke<bool>(origin, radius, direction, maxDistance, layerMask);
}
inline bool SphereCast(Vector3 origin, float radius, Vector3 direction, float maxDistance, int layerMask,
                       int queryTriggerInteraction) {
    return ResolvedMethod::resolve_exact("UnityEngine.Physics", "SphereCast",
                                         {"UnityEngine.Vector3", "System.Single", "UnityEngine.Vector3",
                                          "System.Single", "System.Int32", "UnityEngine.QueryTriggerInteraction"})
        .invoke<bool>(origin, radius, direction, maxDistance, layerMask, queryTriggerInteraction);
}
// OverlapSphere has no Ray counterpart, so the three-parameter form is
// unambiguous by arity and uses the classic array call.
inline std::vector<Collider> OverlapSphere(Vector3 center, float radius, int layerMask = -5) {
    return detail::StaticArrayCall<Collider>(PhysicsType, "OverlapSphere", center, radius, layerMask);
}
}
namespace Resources {
inline Object Load(std::string_view path) {
    return detail::InvokeStatic<Object>(ResourcesType, "Load", path);
}
}
inline Vector2 screen_size(Camera camera = {}) {
    int width_value = Screen::width();
    int height_value = Screen::height();
    if ((width_value <= 0 || height_value <= 0) && static_cast<bool>(camera)) {
        const int pixel_width = camera.pixelWidth();
        const int pixel_height = camera.pixelHeight();
        if (pixel_width > 0)
            width_value = pixel_width;
        if (pixel_height > 0)
            height_value = pixel_height;
    }
    return {static_cast<float>(width_value > 0 ? width_value : 0),
            static_cast<float>(height_value > 0 ? height_value : 0)};
}
inline Vector2 screen_center(Camera camera = {}) {
    const Vector2 size = screen_size(camera);
    return {size.x * 0.5f, size.y * 0.5f};
}
inline bool screen_contains(Vector2 point, float padding = 0.0f, Camera camera = {}) {
    const Vector2 size = screen_size(camera);
    return size.x > 0.0f && size.y > 0.0f && point.x >= padding && point.y >= padding &&
           point.x <= (size.x - padding) && point.y <= (size.y - padding);
}
inline Vector2 clamp_to_screen(Vector2 point, float padding = 0.0f, Camera camera = {}) {
    const Vector2 size = screen_size(camera);
    const float max_x = size.x > 0.0f ? std::max(padding, size.x - padding) : padding;
    const float max_y = size.y > 0.0f ? std::max(padding, size.y - padding) : padding;
    return {std::clamp(point.x, padding, max_x), std::clamp(point.y, padding, max_y)};
}
inline Vector2 direction_to_screen_edge(Vector2 direction, float padding = 24.0f, Camera camera = {}) {
    const Vector2 size = screen_size(camera);
    const Vector2 center = screen_center(camera);
    if (size.x <= 0.0f || size.y <= 0.0f)
        return center;

    Vector2 dir = direction.normalized();
    if (dir.nearly_zero())
        dir = {0.0f, -1.0f};

    const float half_w = std::max(1.0f, size.x * 0.5f - padding);
    const float half_h = std::max(1.0f, size.y * 0.5f - padding);
    float tx = std::numeric_limits<float>::infinity();
    float ty = std::numeric_limits<float>::infinity();
    if (std::fabs(dir.x) > 0.000001f)
        tx = half_w / std::fabs(dir.x);
    if (std::fabs(dir.y) > 0.000001f)
        ty = half_h / std::fabs(dir.y);

    const float scale = std::min(tx, ty);
    return {center.x + dir.x * scale, center.y + dir.y * scale};
}
inline ProjectionResult project_world(Camera camera, Vector3 world, float edge_padding = 24.0f) {
    ProjectionResult result{};
    result.world = world;
    result.screen_center = screen_center(camera);
    if (!static_cast<bool>(camera))
        return result;

    const Vector2 size = screen_size(camera);
    if (size.x <= 0.0f || size.y <= 0.0f)
        return result;

    const Transform camera_transform = camera.transform();
    Vector3 camera_position{};
    Vector3 camera_forward{0.0f, 0.0f, 1.0f};
    Vector3 camera_right{1.0f, 0.0f, 0.0f};
    Vector3 camera_up{0.0f, 1.0f, 0.0f};
    bool have_basis = false;
    if (static_cast<bool>(camera_transform)) {
        camera_position = camera_transform.position();
        camera_forward = camera_transform.forward().normalized();
        camera_right = camera_transform.right().normalized();
        camera_up = camera_transform.up().normalized();
        have_basis = true;
    }

    const Vector3 offset = world - camera_position;
    result.distance = offset.magnitude();
    if (have_basis && result.distance > 0.000001f)
        result.facing = Vector3::dot(offset / result.distance, camera_forward);

    result.screen3 = camera.WorldToScreenPoint(world);
    result.viewport = camera.WorldToViewportPoint(world);
    result.depth = result.screen3.z;
    result.in_front = result.depth > 0.01f;
    result.screen = {result.screen3.x, size.y - result.screen3.y};
    result.on_screen = result.in_front && result.viewport.x >= 0.0f && result.viewport.x <= 1.0f &&
                       result.viewport.y >= 0.0f && result.viewport.y <= 1.0f;

    Vector2 direction = result.screen - result.screen_center;
    if (!result.on_screen && have_basis && result.distance > 0.000001f) {
        const Vector3 offset_dir = offset / result.distance;
        direction = {Vector3::dot(offset_dir, camera_right), -Vector3::dot(offset_dir, camera_up)};
        if (!result.in_front)
            direction *= -1.0f;
    } else if (!result.in_front) {
        direction *= -1.0f;
    }

    if (direction.nearly_zero())
        direction = {0.0f, -1.0f};

    result.direction = direction.normalized();
    result.clamped_screen = direction_to_screen_edge(result.direction, edge_padding, camera);
    result.valid = true;
    return result;
}
inline ProjectionResult project_world(Vector3 world, float edge_padding = 24.0f) {
    return project_world(Camera::main(), world, edge_padding);
}
inline ProjectionResult project_transform(Camera camera, Transform transform, float edge_padding = 24.0f) {
    return static_cast<bool>(transform) ? project_world(camera, transform.position(), edge_padding)
                                        : ProjectionResult{};
}
inline ProjectionResult project_transform(Transform transform, float edge_padding = 24.0f) {
    return project_transform(Camera::main(), transform, edge_padding);
}
inline bool world_to_overlay(Camera camera, Vector3 world, Vector2 *out) {
    if (!out)
        return false;
    const ProjectionResult projection = project_world(camera, world, 0.0f);
    if (!projection.on_screen)
        return false;
    *out = projection.screen;
    return true;
}
inline bool world_to_overlay(Vector3 world, Vector2 *out) {
    return world_to_overlay(Camera::main(), world, out);
}
inline bool world_visible(Camera camera, Vector3 world, float min_facing = 0.01f) {
    const ProjectionResult projection = project_world(camera, world, 0.0f);
    return projection.on_screen && projection.facing >= min_facing;
}
inline bool world_visible(Vector3 world, float min_facing = 0.01f) {
    return world_visible(Camera::main(), world, min_facing);
}
)URKUNITY";

