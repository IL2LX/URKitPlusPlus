    out << R"URKUNITY(// URK_UNITY_INVOKE_BEGIN
template <> struct detail::is_wrapper<Object> : std::true_type {};
template <> struct detail::is_wrapper<TypeObject> : std::true_type {};
template <> struct detail::is_wrapper<Component> : std::true_type {};
template <> struct detail::is_wrapper<Behaviour> : std::true_type {};
template <> struct detail::is_wrapper<MonoBehaviour> : std::true_type {};
template <> struct detail::is_wrapper<ScriptableObject> : std::true_type {};
template <> struct detail::is_wrapper<GameObject> : std::true_type {};
template <> struct detail::is_wrapper<Transform> : std::true_type {};
template <> struct detail::is_wrapper<Camera> : std::true_type {};
template <> struct detail::is_wrapper<Light> : std::true_type {};
template <> struct detail::is_wrapper<Renderer> : std::true_type {};
template <> struct detail::is_wrapper<SkinnedMeshRenderer> : std::true_type {};
template <> struct detail::is_wrapper<Collider> : std::true_type {};
template <> struct detail::is_wrapper<RectTransform> : std::true_type {};
template <> struct detail::is_wrapper<Rigidbody> : std::true_type {};
template <> struct detail::is_wrapper<Rigidbody2D> : std::true_type {};
template <> struct detail::is_wrapper<AudioSource> : std::true_type {};
template <> struct detail::is_wrapper<Animator> : std::true_type {};
template <> struct detail::is_wrapper<Canvas> : std::true_type {};
template <> struct detail::is_wrapper<Graphic> : std::true_type {};
template <> struct detail::is_wrapper<Image> : std::true_type {};
template <> struct detail::is_wrapper<Text> : std::true_type {};
template <> struct detail::is_wrapper<Button> : std::true_type {};
template <> struct detail::is_wrapper<Mesh> : std::true_type {};
template <> struct detail::is_wrapper<Material> : std::true_type {};
template <> struct detail::is_wrapper<Texture> : std::true_type {};
template <> struct detail::is_wrapper<Sprite> : std::true_type {};
template <> struct detail::is_wrapper<AssetBundle> : std::true_type {};
template <> struct detail::is_wrapper<Scene> : std::true_type {};
template <> struct detail::is_wrapper<CanvasGroup> : std::true_type {};
template <> struct detail::is_wrapper<CanvasScaler> : std::true_type {};
template <> struct detail::is_wrapper<CanvasRenderer> : std::true_type {};
template <> struct detail::is_wrapper<GraphicRaycaster> : std::true_type {};
template <> struct detail::is_wrapper<Selectable> : std::true_type {};
template <> struct detail::is_wrapper<RawImage> : std::true_type {};
template <> struct detail::is_wrapper<TextMeshProUGUI> : std::true_type {};
template <> struct detail::is_wrapper<TmpInputField> : std::true_type {};
template <> struct detail::is_wrapper<TmpDropdown> : std::true_type {};
template <> struct detail::is_wrapper<Toggle> : std::true_type {};
template <> struct detail::is_wrapper<Slider> : std::true_type {};
template <> struct detail::is_wrapper<Scrollbar> : std::true_type {};
template <> struct detail::is_wrapper<Dropdown> : std::true_type {};
template <> struct detail::is_wrapper<InputField> : std::true_type {};
template <> struct detail::is_wrapper<Mask> : std::true_type {};
template <> struct detail::is_wrapper<RectMask2D> : std::true_type {};
template <> struct detail::is_wrapper<ScrollRect> : std::true_type {};
template <> struct detail::is_wrapper<LayoutElement> : std::true_type {};
template <> struct detail::is_wrapper<HorizontalLayoutGroup> : std::true_type {};
template <> struct detail::is_wrapper<VerticalLayoutGroup> : std::true_type {};
template <> struct detail::is_wrapper<GridLayoutGroup> : std::true_type {};
template <> struct detail::is_wrapper<ContentSizeFitter> : std::true_type {};
template <> struct detail::is_wrapper<AspectRatioFitter> : std::true_type {};
template <> struct detail::is_wrapper<EventSystem> : std::true_type {};
template <> struct detail::is_wrapper<BaseInputModule> : std::true_type {};
template <> struct detail::is_wrapper<StandaloneInputModule> : std::true_type {};
template <> struct detail::is_wrapper<InputSystemUIInputModule> : std::true_type {};
template <> struct detail::is_wrapper<MeshRenderer> : std::true_type {};
template <> struct detail::is_wrapper<MeshFilter> : std::true_type {};
template <> struct detail::is_wrapper<MeshCollider> : std::true_type {};
template <> struct detail::is_wrapper<Texture2D> : std::true_type {};
template <> struct detail::is_wrapper<Shader> : std::true_type {};
inline GameObject Component::gameObject() const {
    return Call<GameObject>("get_gameObject");
}
inline Transform Component::transform() const {
    return Call<Transform>("get_transform");
}
template <class T> inline T Component::GetComponent() const {
    return gameObject().GetComponent<T>();
}
template <class T> inline T Component::GetComponent(const char *name) const {
    return gameObject().GetComponent<T>(name);
}
inline Object Component::GetComponent(std::string_view image, std::string_view namespc,
                                      std::string_view className) const {
    return gameObject().GetComponent(image, namespc, className);
}
template <class T> inline T Component::GetComponentInChildren(bool includeInactive) const {
    return gameObject().GetComponentInChildren<T>(includeInactive);
}
inline Object Component::GetComponentInChildren(std::string_view image, std::string_view namespc,
                                                std::string_view className, bool includeInactive) const {
    return gameObject().GetComponentInChildren(image, namespc, className, includeInactive);
}
template <class T> inline T Component::GetComponentInParent(bool includeInactive) const {
    return gameObject().GetComponentInParent<T>(includeInactive);
}
inline Object Component::GetComponentInParent(std::string_view image, std::string_view namespc,
                                              std::string_view className, bool includeInactive) const {
    return gameObject().GetComponentInParent(image, namespc, className, includeInactive);
}
template <class T> inline std::vector<T> Component::GetComponents() const {
    return gameObject().GetComponents<T>();
}
template <class T> inline std::vector<T> Component::GetComponentsInChildren(bool includeInactive) const {
    return gameObject().GetComponentsInChildren<T>(includeInactive);
}
template <class T> inline std::vector<T> Component::GetComponentsInParent(bool includeInactive) const {
    return gameObject().GetComponentsInParent<T>(includeInactive);
}
template <class T> inline detail::RootedObjectArray<T> Component::GetComponentsRooted() const {
    return gameObject().GetComponentsRooted<T>();
}
template <class T>
inline detail::RootedObjectArray<T> Component::GetComponentsInChildrenRooted(bool includeInactive) const {
    return gameObject().GetComponentsInChildrenRooted<T>(includeInactive);
}
template <class T>
inline detail::RootedObjectArray<T> Component::GetComponentsInParentRooted(bool includeInactive) const {
    return gameObject().GetComponentsInParentRooted<T>(includeInactive);
}
template <class T> inline T Component::AddComponent() const {
    return gameObject().AddComponent<T>();
}
inline Object Component::AddComponent(std::string_view image, std::string_view namespc,
                                      std::string_view className) const {
    return gameObject().AddComponent(image, namespc, className);
}
template <class T> inline bool Component::HasComponent() const {
    return gameObject().HasComponent<T>();
}
inline bool Component::HasComponent(std::string_view image, std::string_view namespc,
                                    std::string_view className) const {
    return gameObject().HasComponent(image, namespc, className);
}
template <class T> inline T Component::GetOrAddComponent() const {
    return gameObject().GetOrAddComponent<T>();
}
inline Object Component::GetOrAddComponent(std::string_view image, std::string_view namespc,
                                           std::string_view className) const {
    return gameObject().GetOrAddComponent(image, namespc, className);
}
inline std::string Object::runtime_class_name() const {
    return detail::class_display_name(detail::Backend::object_get_class(handle_));
}
inline std::string Object::ToString() const {
    return detail::managed_string_to_utf8(Call<void *>("ToString"));
}
inline std::string Object::name() const {
    return detail::managed_string_to_utf8(Call<void *>("get_name"));
}
inline std::string GameObject::tag() const {
    return detail::managed_string_to_utf8(Call<void *>("get_tag"));
}

namespace detail {
// runtime_invoke expects value-type params as pointers to local storage, but
// managed reference/object/string params as the managed pointer itself.
template <class T> struct Arg {
    T storage;
    void *ptr;
    bool valid;
    explicit Arg(T value) noexcept(std::is_nothrow_move_constructible_v<T>)
        : storage(std::move(value)), ptr(&storage), valid(true) {
    }
    Arg(const Arg &other) noexcept(std::is_nothrow_copy_constructible_v<T>)
        : storage(other.storage), ptr(&storage), valid(other.valid) {
    }
    Arg(Arg &&other) noexcept(std::is_nothrow_move_constructible_v<T>)
        : storage(std::move(other.storage)), ptr(&storage), valid(other.valid) {
    }
    Arg &operator=(const Arg &other) noexcept(std::is_nothrow_copy_assignable_v<T>) {
        if (this != &other) {
            storage = other.storage;
            ptr = &storage;
            valid = other.valid;
        }
        return *this;
    }
    Arg &operator=(Arg &&other) noexcept(std::is_nothrow_move_assignable_v<T>) {
        if (this != &other) {
            storage = std::move(other.storage);
            ptr = &storage;
            valid = other.valid;
        }
        return *this;
    }
};
template <class T>
    requires is_wrapper_v<T>
struct Arg<T> {
    void *storage;
    void *ptr;
    bool valid;
    Arg(T v) : storage(v.handle()), ptr(storage), valid(true) {
    }
};
template <> struct Arg<void *> {
    void *storage;
    void *ptr;
    bool valid;
    Arg(void *v) : storage(v), ptr(storage), valid(true) {
    }
};
template <> struct Arg<const char *> {
    void *storage;
    void *ptr;
    bool valid;
    Arg(const char *v)
        : storage(Backend::new_string(v ? std::string_view(v) : std::string_view{})), ptr(storage),
          valid(storage != nullptr) {
    }
};
template <> struct Arg<char *> {
    void *storage;
    void *ptr;
    bool valid;
    Arg(char *v)
        : storage(Backend::new_string(v ? std::string_view(v) : std::string_view{})), ptr(storage),
          valid(storage != nullptr) {
    }
};
template <std::size_t N> struct Arg<char[N]> {
    void *storage;
    void *ptr;
    bool valid;
    Arg(const char (&v)[N])
        : storage(Backend::new_string(std::string_view(v, N > 0 && v[N - 1] == '\0' ? N - 1 : N))), ptr(storage),
          valid(storage != nullptr) {
    }
};
template <std::size_t N> struct Arg<const char[N]> {
    void *storage;
    void *ptr;
    bool valid;
    Arg(const char (&v)[N])
        : storage(Backend::new_string(std::string_view(v, N > 0 && v[N - 1] == '\0' ? N - 1 : N))), ptr(storage),
          valid(storage != nullptr) {
    }
};
template <> struct Arg<std::string> {
    void *storage;
    void *ptr;
    bool valid;
    Arg(const std::string &v) : storage(Backend::new_string(v)), ptr(storage), valid(storage != nullptr) {
    }
};
template <> struct Arg<std::string_view> {
    void *storage;
    void *ptr;
    bool valid;
    Arg(std::string_view v) : storage(Backend::new_string(v)), ptr(storage), valid(storage != nullptr) {
    }
};
void *make_reflection_array(void *elementType, const std::vector<void *> &values);
template <class T> void *reflection_argument(Arg<T> &argument);
template <class Tuple> void *invoke_reflection_method(void *reflectionMethod, void *target, Tuple &arguments);
template <class Tuple>
void *invoke_value_type_method(const void *method, const void *declaringClass, void *target, Tuple &arguments);
template <class T> inline std::optional<const char *> parameter_type_name() {
    if constexpr (std::is_array_v<T> && std::is_same_v<std::remove_cv_t<std::remove_extent_t<T>>, char>)
        return "System.String";
    else
        return std::nullopt;
}
template <> inline std::optional<const char *> parameter_type_name<bool>() {
    return "System.Boolean";
}
template <> inline std::optional<const char *> parameter_type_name<int>() {
    return "System.Int32";
}
template <> inline std::optional<const char *> parameter_type_name<unsigned int>() {
    return "System.UInt32";
}
template <> inline std::optional<const char *> parameter_type_name<short>() {
    return "System.Int16";
}
template <> inline std::optional<const char *> parameter_type_name<unsigned short>() {
    return "System.UInt16";
}
template <> inline std::optional<const char *> parameter_type_name<long long>() {
    return "System.Int64";
}
template <> inline std::optional<const char *> parameter_type_name<unsigned long long>() {
    return "System.UInt64";
}
template <> inline std::optional<const char *> parameter_type_name<float>() {
    return "System.Single";
}
template <> inline std::optional<const char *> parameter_type_name<double>() {
    return "System.Double";
}
template <> inline std::optional<const char *> parameter_type_name<void *>() {
    return "System.Object";
}
template <> inline std::optional<const char *> parameter_type_name<std::string_view>() {
    return "System.String";
}
template <> inline std::optional<const char *> parameter_type_name<std::string>() {
    return "System.String";
}
template <> inline std::optional<const char *> parameter_type_name<const char *>() {
    return "System.String";
}
template <> inline std::optional<const char *> parameter_type_name<char *>() {
    return "System.String";
}
template <> inline std::optional<const char *> parameter_type_name<Vector2>() {
    return "UnityEngine.Vector2";
}
template <> inline std::optional<const char *> parameter_type_name<Vector3>() {
    return "UnityEngine.Vector3";
}
template <> inline std::optional<const char *> parameter_type_name<Quaternion>() {
    return "UnityEngine.Quaternion";
}
template <> inline std::optional<const char *> parameter_type_name<Color>() {
    return "UnityEngine.Color";
}
template <> inline std::optional<const char *> parameter_type_name<Rect>() {
    return "UnityEngine.Rect";
}
template <> inline std::optional<const char *> parameter_type_name<Bounds>() {
    return "UnityEngine.Bounds";
}
template <> inline std::optional<const char *> parameter_type_name<Ray>() {
    return "UnityEngine.Ray";
}
template <> inline std::optional<const char *> parameter_type_name<TypeObject>() {
    return "System.Type";
}
template <> inline std::optional<const char *> parameter_type_name<FindObjectsSortMode>() {
    return "UnityEngine.FindObjectsSortMode";
}
template <> inline std::optional<const char *> parameter_type_name<Object>() {
    return "UnityEngine.Object";
}
template <> inline std::optional<const char *> parameter_type_name<Component>() {
    return "UnityEngine.Component";
}
template <> inline std::optional<const char *> parameter_type_name<Behaviour>() {
    return "UnityEngine.Behaviour";
}
template <> inline std::optional<const char *> parameter_type_name<MonoBehaviour>() {
    return "UnityEngine.MonoBehaviour";
}
template <> inline std::optional<const char *> parameter_type_name<ScriptableObject>() {
    return "UnityEngine.ScriptableObject";
}
template <> inline std::optional<const char *> parameter_type_name<GameObject>() {
    return "UnityEngine.GameObject";
}
template <> inline std::optional<const char *> parameter_type_name<Transform>() {
    return "UnityEngine.Transform";
}
template <> inline std::optional<const char *> parameter_type_name<Camera>() {
    return "UnityEngine.Camera";
}
template <> inline std::optional<const char *> parameter_type_name<Light>() {
    return "UnityEngine.Light";
}
template <> inline std::optional<const char *> parameter_type_name<SkinnedMeshRenderer>() {
    return "UnityEngine.SkinnedMeshRenderer";
}
template <> inline std::optional<const char *> parameter_type_name<RectTransform>() {
    return "UnityEngine.RectTransform";
}
template <> inline std::optional<const char *> parameter_type_name<Rigidbody>() {
    return "UnityEngine.Rigidbody";
}
template <> inline std::optional<const char *> parameter_type_name<Rigidbody2D>() {
    return "UnityEngine.Rigidbody2D";
}
template <> inline std::optional<const char *> parameter_type_name<AudioSource>() {
    return "UnityEngine.AudioSource";
}
template <> inline std::optional<const char *> parameter_type_name<Animator>() {
    return "UnityEngine.Animator";
}
template <> inline std::optional<const char *> parameter_type_name<Canvas>() {
    return "UnityEngine.Canvas";
}
template <> inline std::optional<const char *> parameter_type_name<CanvasRenderer>() {
    return "UnityEngine.CanvasRenderer";
}
template <> inline std::optional<const char *> parameter_type_name<Graphic>() {
    return "UnityEngine.UI.Graphic";
}
template <> inline std::optional<const char *> parameter_type_name<GraphicRaycaster>() {
    return "UnityEngine.UI.GraphicRaycaster";
}
template <> inline std::optional<const char *> parameter_type_name<Selectable>() {
    return "UnityEngine.UI.Selectable";
}
template <> inline std::optional<const char *> parameter_type_name<Image>() {
    return "UnityEngine.UI.Image";
}
template <> inline std::optional<const char *> parameter_type_name<Text>() {
    return "UnityEngine.UI.Text";
}
template <> inline std::optional<const char *> parameter_type_name<Button>() {
    return "UnityEngine.UI.Button";
}
template <> inline std::optional<const char *> parameter_type_name<Mesh>() {
    return "UnityEngine.Mesh";
}
template <> inline std::optional<const char *> parameter_type_name<Material>() {
    return "UnityEngine.Material";
}
template <> inline std::optional<const char *> parameter_type_name<Texture>() {
    return "UnityEngine.Texture";
}
template <> inline std::optional<const char *> parameter_type_name<Sprite>() {
    return "UnityEngine.Sprite";
}
template <> inline std::optional<const char *> parameter_type_name<AssetBundle>() {
    return "UnityEngine.AssetBundle";
}
template <> inline std::optional<const char *> parameter_type_name<Renderer>() {
    return "UnityEngine.Renderer";
}
template <> inline std::optional<const char *> parameter_type_name<Collider>() {
    return "UnityEngine.Collider";
}
template <> inline std::optional<const char *> parameter_type_name<CanvasGroup>() {
    return "UnityEngine.CanvasGroup";
}
template <> inline std::optional<const char *> parameter_type_name<CanvasScaler>() {
    return "UnityEngine.UI.CanvasScaler";
}
template <> inline std::optional<const char *> parameter_type_name<RawImage>() {
    return "UnityEngine.UI.RawImage";
}
template <> inline std::optional<const char *> parameter_type_name<TextMeshProUGUI>() {
    return "TMPro.TextMeshProUGUI";
}
template <> inline std::optional<const char *> parameter_type_name<TmpInputField>() {
    return "TMPro.TMP_InputField";
}
template <> inline std::optional<const char *> parameter_type_name<TmpDropdown>() {
    return "TMPro.TMP_Dropdown";
}
template <> inline std::optional<const char *> parameter_type_name<Toggle>() {
    return "UnityEngine.UI.Toggle";
}
template <> inline std::optional<const char *> parameter_type_name<Slider>() {
    return "UnityEngine.UI.Slider";
}
template <> inline std::optional<const char *> parameter_type_name<Scrollbar>() {
    return "UnityEngine.UI.Scrollbar";
}
template <> inline std::optional<const char *> parameter_type_name<Dropdown>() {
    return "UnityEngine.UI.Dropdown";
}
template <> inline std::optional<const char *> parameter_type_name<InputField>() {
    return "UnityEngine.UI.InputField";
}
template <> inline std::optional<const char *> parameter_type_name<Mask>() {
    return "UnityEngine.UI.Mask";
}
template <> inline std::optional<const char *> parameter_type_name<RectMask2D>() {
    return "UnityEngine.UI.RectMask2D";
}
template <> inline std::optional<const char *> parameter_type_name<ScrollRect>() {
    return "UnityEngine.UI.ScrollRect";
}
template <> inline std::optional<const char *> parameter_type_name<LayoutElement>() {
    return "UnityEngine.UI.LayoutElement";
}
template <> inline std::optional<const char *> parameter_type_name<HorizontalLayoutGroup>() {
    return "UnityEngine.UI.HorizontalLayoutGroup";
}
template <> inline std::optional<const char *> parameter_type_name<VerticalLayoutGroup>() {
    return "UnityEngine.UI.VerticalLayoutGroup";
}
template <> inline std::optional<const char *> parameter_type_name<GridLayoutGroup>() {
    return "UnityEngine.UI.GridLayoutGroup";
}
template <> inline std::optional<const char *> parameter_type_name<ContentSizeFitter>() {
    return "UnityEngine.UI.ContentSizeFitter";
}
template <> inline std::optional<const char *> parameter_type_name<AspectRatioFitter>() {
    return "UnityEngine.UI.AspectRatioFitter";
}
template <> inline std::optional<const char *> parameter_type_name<EventSystem>() {
    return "UnityEngine.EventSystems.EventSystem";
}
template <> inline std::optional<const char *> parameter_type_name<BaseInputModule>() {
    return "UnityEngine.EventSystems.BaseInputModule";
}
template <> inline std::optional<const char *> parameter_type_name<StandaloneInputModule>() {
    return "UnityEngine.EventSystems.StandaloneInputModule";
}
template <> inline std::optional<const char *> parameter_type_name<InputSystemUIInputModule>() {
    return "UnityEngine.InputSystem.UI.InputSystemUIInputModule";
}
template <> inline std::optional<const char *> parameter_type_name<MeshRenderer>() {
    return "UnityEngine.MeshRenderer";
}
template <> inline std::optional<const char *> parameter_type_name<MeshFilter>() {
    return "UnityEngine.MeshFilter";
}
template <> inline std::optional<const char *> parameter_type_name<MeshCollider>() {
    return "UnityEngine.MeshCollider";
}
template <> inline std::optional<const char *> parameter_type_name<Texture2D>() {
    return "UnityEngine.Texture2D";
}
template <> inline std::optional<const char *> parameter_type_name<Shader>() {
    return "UnityEngine.Shader";
}
template <class... Args> std::vector<const char *> inferred_parameter_types() {
    std::vector<const char *> names;
    names.reserve(sizeof...(Args));
    bool all = true;
    (
        [&] {
            auto n = parameter_type_name<std::remove_cvref_t<Args>>();
            if (n)
                names.push_back(*n);
            else
                all = false;
        }(),
        ...);
    if (!all)
        names.clear();
    return names;
}
template <class Ret> Ret from_result(void *r) {
    if constexpr (std::is_void_v<Ret>)
        return;
    else if constexpr (std::is_same_v<std::remove_cvref_t<Ret>, std::string>)
        return managed_string_to_utf8(r);
    else if constexpr (is_wrapper_v<Ret>)
        return Ret{r};
    else if constexpr (std::is_pointer_v<Ret>)
        return static_cast<Ret>(r);
    else {
        if (!r) {
            if (!fallback_error())
                set_error("Unity conversion failed: managed return object is null");
            return Ret{};
        }
        void *p = Backend::object_unbox(r);
        if (!p) {
            if (!fallback_error())
                set_error("Unity conversion failed: object_unbox failed for value type "
                          "return");
            append_backend_error();
            return Ret{};
        }
        return *static_cast<Ret *>(p);
    }
}
}

template <class Ret, class... Args> Ret Object::Call(std::string_view methodName, Args &&...args) const {
    detail::clear_error();
    if (!handle_) {
        detail::set_error("Unity Object::Call failed: target object is null");
        return detail::from_result<Ret>(nullptr);
    }
    const void *k = detail::Backend::object_get_class(handle_);
    if (!k) {
        detail::set_error("Unity Object::Call failed: object_get_class failed");
        detail::append_backend_error();
        return detail::from_result<Ret>(nullptr);
    }
    auto inferred = detail::inferred_parameter_types<Args...>();
    const bool exact = !inferred.empty() || sizeof...(Args) == 0;
    const void *m = exact ? detail::Backend::find_method_exact(k, methodName, inferred)
                          : detail::Backend::find_method(k, methodName, sizeof...(Args));
    if (!m) {
        const std::string lookupDetail = detail::fallback_error() ? detail::fallback_error() : "";
        detail::set_error(std::string("Unity Object::Call failed: method not found or ambiguous: ") +
                          (exact ? detail::signature_text(methodName, inferred)
                                 : std::string(methodName) + "/" + std::to_string(sizeof...(Args))) +
                          (lookupDetail.empty() ? std::string{} : std::string("; detail: ") + lookupDetail));
        detail::append_backend_error();
        return detail::from_result<Ret>(nullptr);
    }
    auto pack = std::tuple<detail::Arg<std::remove_cvref_t<Args>>...>(
        detail::Arg<std::remove_cvref_t<Args>>(std::forward<Args>(args))...);
    std::array<void *, sizeof...(Args)> argv{};
    std::size_t i = 0;
    bool argsValid = true;
    std::apply([&](auto &...a) { ((argsValid = argsValid && a.valid, argv[i++] = a.ptr), ...); }, pack);
    if (!argsValid) {
        detail::set_error(std::string("Unity Object::Call failed: managed string "
                                      "argument allocation failed in ") +
                          std::string(methodName));
        detail::append_backend_error();
        return detail::from_result<Ret>(nullptr);
    }
    if (detail::Backend::class_is_valuetype(k)) {
        void *result = detail::invoke_value_type_method(m, k, handle_, pack);
        return detail::from_result<Ret>(result);
    }
    void *result = nullptr;
    void *ex = nullptr;
    if (!detail::Backend::runtime_invoke(m, handle_, argv.empty() ? nullptr : argv.data(), &result, &ex) || ex) {
        detail::set_error(std::string("Unity Object::Call failed: runtime_invoke exception in ") +
                          std::string(methodName));
        detail::append_backend_error();
        return detail::from_result<Ret>(nullptr);
    }
    return detail::from_result<Ret>(result);
}

template <class Ret, class... Args>
Ret detail::InvokeStatic(TypeRef type, std::string_view methodName, Args &&...args) {
    clear_error();
    const void *k = type.resolve_class();
    if (!k) {
        set_error(std::string("Unity static call failed: class not found for ") + std::string(type.namespc) + "." +
                  std::string(type.name));
        append_backend_error();
        return from_result<Ret>(nullptr);
    }
    auto inferred = inferred_parameter_types<Args...>();
    const bool exact = !inferred.empty() || sizeof...(Args) == 0;
    const void *m = exact ? Backend::find_method_exact(k, methodName, inferred)
                          : Backend::find_method(k, methodName, sizeof...(Args));
    if (!m) {
        const std::string lookupDetail = fallback_error() ? fallback_error() : "";
        set_error(std::string("Unity static call failed: Unity method not found or ambiguous: ") +
                  std::string(type.namespc) + "." + std::string(type.name) + "." +
                  (exact ? signature_text(methodName, inferred)
                         : std::string(methodName) + "/" + std::to_string(sizeof...(Args))) +
                  (lookupDetail.empty() ? std::string{} : std::string("; detail: ") + lookupDetail));
        append_backend_error();
        return from_result<Ret>(nullptr);
    }
    auto pack =
        std::tuple<Arg<std::remove_cvref_t<Args>>...>(Arg<std::remove_cvref_t<Args>>(std::forward<Args>(args))...);
    std::array<void *, sizeof...(Args)> argv{};
    std::size_t i = 0;
    bool argsValid = true;
    std::apply([&](auto &...a) { ((argsValid = argsValid && a.valid, argv[i++] = a.ptr), ...); }, pack);
    if (!argsValid) {
        set_error(std::string("Unity static call failed: managed string argument "
                              "allocation failed in ") +
                  std::string(methodName));
        append_backend_error();
        return from_result<Ret>(nullptr);
    }
    void *result = nullptr;
    void *ex = nullptr;
    if (!Backend::runtime_invoke(m, nullptr, argv.empty() ? nullptr : argv.data(), &result, &ex) || ex) {
        set_error(std::string("Unity static call failed: runtime_invoke exception in ") + std::string(methodName));
        append_backend_error();
        return from_result<Ret>(nullptr);
    }
    return from_result<Ret>(result);
}

template <class Ret, class... Args>
Ret detail::InvokeStaticExact(TypeRef type, std::string_view methodName,
                              const std::vector<const char *> &parameterTypeNames, Args &&...args) {
    clear_error();
    const void *k = type.resolve_class();
    if (!k) {
        set_error(std::string("Unity static call failed: class not found for ") + std::string(type.namespc) + "." +
                  std::string(type.name));
        append_backend_error();
        return from_result<Ret>(nullptr);
    }
    if (parameterTypeNames.size() != sizeof...(Args)) {
        set_error(std::string("Unity static call failed: declared parameter type count does not match the argument "
                              "count in ") +
                  std::string(methodName));
        return from_result<Ret>(nullptr);
    }
    for (const char *declared : parameterTypeNames) {
        if (!declared || !*declared) {
            set_error(std::string("Unity static call failed: parameter type names must be non-empty in ") +
                      std::string(methodName));
            return from_result<Ret>(nullptr);
        }
    }
    const void *m = Backend::find_method_exact(k, methodName, parameterTypeNames);
    if (!m) {
        const std::string lookupDetail = fallback_error() ? fallback_error() : "";
        set_error(std::string("Unity static call failed: Unity method not found or ambiguous: ") +
                  std::string(type.namespc) + "." + std::string(type.name) + "." +
                  signature_text(methodName, parameterTypeNames) +
                  (lookupDetail.empty() ? std::string{} : "; detail: " + lookupDetail));
        append_backend_error();
        return from_result<Ret>(nullptr);
    }
    auto pack =
        std::tuple<Arg<std::remove_cvref_t<Args>>...>(Arg<std::remove_cvref_t<Args>>(std::forward<Args>(args))...);
    std::array<void *, sizeof...(Args)> argv{};
    std::size_t i = 0;
    bool argsValid = true;
    std::apply([&](auto &...a) { ((argsValid = argsValid && a.valid, argv[i++] = a.ptr), ...); }, pack);
    if (!argsValid) {
        set_error(std::string("Unity static call failed: managed string argument allocation failed in ") +
                  std::string(methodName));
        append_backend_error();
        return from_result<Ret>(nullptr);
    }
    void *result = nullptr;
    void *ex = nullptr;
    if (!Backend::runtime_invoke(m, nullptr, argv.empty() ? nullptr : argv.data(), &result, &ex) || ex) {
        set_error(std::string("Unity static call failed: runtime_invoke exception in ") + std::string(methodName));
        append_backend_error();
        return from_result<Ret>(nullptr);
    }
    return from_result<Ret>(result);
}

template <class T, class... Args>
std::vector<T> detail::StaticArrayCall(TypeRef type, std::string_view methodName, Args &&...args) {    void *array = InvokeStatic<void *>(type, methodName, std::forward<Args>(args)...);
    return RootedObjectArray<T>::from_managed_array(array, "Unity static array call").copy_items();
}

template <class T, class... ExtraArgs>
std::vector<T> detail::FindObjectsUsing(TypeRef owner, std::string_view methodName, std::string_view image,
                                        std::string_view namespc, std::string_view className,
                                        ExtraArgs &&...extraArgs) {
    auto rooted = FindObjectsUsingRooted<T>(owner, methodName, image, namespc, className,
                                            std::forward<ExtraArgs>(extraArgs)...);
    std::vector<T> out = std::move(rooted).copy_items();
    if (out.empty() && !fallback_error())
        set_error(std::string("Unity object finding failed: no non-null instances found: ") + std::string(image) +
                  ":" + std::string(namespc) + "." + std::string(className));
    return out;
}

template <class T, class... ExtraArgs>
detail::RootedObjectArray<T>
detail::FindObjectsUsingRooted(TypeRef owner, std::string_view methodName, std::string_view image,
                               std::string_view namespc, std::string_view className, ExtraArgs &&...extraArgs) {
    clear_error();
    TypeRef target{image, namespc, className};
    void *type = target.resolve_type_object();
    if (!type) {
        set_error(std::string("Unity object finding failed: class not found: ") + std::string(image) + ":" +
                  std::string(namespc) + "." + std::string(className));
        append_backend_error();
        return {};
    }
    void *array = InvokeStatic<void *>(owner, methodName, TypeObject{type}, std::forward<ExtraArgs>(extraArgs)...);
    // FindObjectsOfType is the pre-2022.2 spelling, FindObjectsByType the newer one.
    // Neither exists in every Unity version, so fall back across the rename both ways.
    if (!array && methodName == "FindObjectsOfType" && sizeof...(ExtraArgs) == 0) {
        clear_error();
        array = InvokeStatic<void *>(owner, "FindObjectsByType", TypeObject{type}, FindObjectsSortMode::None);
    } else if (!array && methodName == "FindObjectsByType") {
        clear_error();
        array = InvokeStatic<void *>(owner, "FindObjectsOfType", TypeObject{type});
    }
    return RootedObjectArray<T>::from_managed_array(array, "Unity rooted object finding");
}

template <class T> T Object::GetField(std::string_view fieldName) const {
    detail::clear_error();
    detail::FieldOut<std::remove_cvref_t<T>> out{};
    if (!handle_) {
        detail::set_error("Unity Object::GetField failed: target object is null");
        return out.get();
    }
    const void *k = detail::Backend::object_get_class(handle_);
    if (!k) {
        detail::set_error("Unity Object::GetField failed: object_get_class failed");
        detail::append_backend_error();
        return out.get();
    }
    const void *f = detail::Backend::find_field(k, fieldName);
    if (!f) {
        detail::set_error(std::string("Unity Object::GetField failed: field not found: ") + std::string(fieldName));
        detail::append_backend_error();
        return out.get();
    }
    if (!detail::Backend::field_get_value(handle_, f, out.ptr())) {
        detail::set_error(std::string("Unity Object::GetField failed: field read failed: ") + std::string(fieldName));
        detail::append_backend_error();
    }
    return out.get();
}
template <class T> void Object::SetField(std::string_view fieldName, T value) const {
    detail::clear_error();
    if (!handle_) {
        detail::set_error("Unity Object::SetField failed: target object is null");
        return;
    }
    const void *k = detail::Backend::object_get_class(handle_);
    if (!k) {
        detail::set_error("Unity Object::SetField failed: object_get_class failed");
        detail::append_backend_error();
        return;
    }
    const void *f = detail::Backend::find_field(k, fieldName);
    if (!f) {
        detail::set_error(std::string("Unity Object::SetField failed: field not found: ") + std::string(fieldName));
        detail::append_backend_error();
        return;
    }
    detail::FieldArg<std::remove_cvref_t<T>> arg(value);
    if (!detail::Backend::field_set_value(handle_, f, arg.ptr)) {
        detail::set_error(std::string("Unity Object::SetField failed: field write failed: ") + std::string(fieldName));
        detail::append_backend_error();
    }
}
template <class T> T Object::StaticGetField(TypeRef type, std::string_view fieldName) {
    detail::clear_error();
    detail::FieldOut<std::remove_cvref_t<T>> out{};
    const void *k = type.resolve_class();
    if (!k) {
        detail::set_error(std::string("Unity Object::StaticGetField failed: class not found for ") +
                          std::string(type.namespc) + "." + std::string(type.name));
        detail::append_backend_error();
        return out.get();
    }
    const void *f = detail::Backend::find_field(k, fieldName);
    if (!f) {
        detail::set_error(std::string("Unity Object::StaticGetField failed: field not found: ") +
                          std::string(fieldName));
        detail::append_backend_error();
        return out.get();
    }
    if (!detail::Backend::field_static_get_value(k, f, out.ptr())) {
        detail::set_error(std::string("Unity Object::StaticGetField failed: field read failed: ") +
                          std::string(fieldName));
        detail::append_backend_error();
    }
    return out.get();
}
template <class T> void Object::StaticSetField(TypeRef type, std::string_view fieldName, T value) {
    detail::clear_error();
    const void *k = type.resolve_class();
    if (!k) {
        detail::set_error(std::string("Unity Object::StaticSetField failed: class not found for ") +
                          std::string(type.namespc) + "." + std::string(type.name));
        detail::append_backend_error();
        return;
    }
    const void *f = detail::Backend::find_field(k, fieldName);
    if (!f) {
        detail::set_error(std::string("Unity Object::StaticSetField failed: field not found: ") +
                          std::string(fieldName));
        detail::append_backend_error();
        return;
    }
    detail::FieldArg<std::remove_cvref_t<T>> arg(value);
    if (!detail::Backend::field_static_set_value(k, f, arg.ptr)) {
        detail::set_error(std::string("Unity Object::StaticSetField failed: field write failed: ") +
                          std::string(fieldName));
        detail::append_backend_error();
    }
}
template <class Ret>
Ret Object::CallExact(std::string_view methodName, const std::vector<const char *> &parameterTypeNames,
                      void **rawArgs) const {
    detail::clear_error();
    if (!handle_) {
        detail::set_error("Unity Object::CallExact failed: target object is null");
        return detail::from_result<Ret>(nullptr);
    }
    const void *k = detail::Backend::object_get_class(handle_);
    if (!k) {
        detail::set_error("Unity Object::CallExact failed: object_get_class failed");
        detail::append_backend_error();
        return detail::from_result<Ret>(nullptr);
    }
    const void *m = detail::Backend::find_method_exact(k, methodName, parameterTypeNames);
    if (!m) {
        const std::string lookupDetail = detail::fallback_error() ? detail::fallback_error() : "";
        detail::set_error(std::string("Unity Object::CallExact failed: exact method not found: ") +
                          detail::signature_text(methodName, parameterTypeNames) +
                          (lookupDetail.empty() ? std::string{} : std::string("; detail: ") + lookupDetail));
        detail::append_backend_error();
        return detail::from_result<Ret>(nullptr);
    }
    void *result = nullptr;
    void *ex = nullptr;
    if (!detail::Backend::runtime_invoke(m, handle_, rawArgs, &result, &ex) || ex) {
        detail::set_error(std::string("Unity Object::CallExact failed: runtime_invoke exception in ") +
                          detail::signature_text(methodName, parameterTypeNames));
        detail::append_backend_error();
        return detail::from_result<Ret>(nullptr);
    }
    return detail::from_result<Ret>(result);
}
template <class Ret, class... Args>
Ret Object::CallExact(std::string_view methodName, const std::vector<const char *> &parameterTypeNames,
                      Args &&...args) const {
    detail::clear_error();
    if (parameterTypeNames.size() != sizeof...(Args)) {
        detail::set_error(std::string("Unity Object::CallExact failed: argument count does not match ") +
                          detail::signature_text(methodName, parameterTypeNames) + "; received=" +
                          std::to_string(sizeof...(Args)));
        return detail::from_result<Ret>(nullptr);
    }
    auto pack = std::tuple<detail::Arg<std::remove_cvref_t<Args>>...>(
        detail::Arg<std::remove_cvref_t<Args>>(std::forward<Args>(args))...);
    std::array<void *, sizeof...(Args)> argv{};
    std::size_t i = 0;
    bool argsValid = true;
    std::apply([&](auto &...a) { ((argsValid = argsValid && a.valid, argv[i++] = a.ptr), ...); }, pack);
    if (!argsValid) {
        detail::set_error(std::string("Unity Object::CallExact failed: managed "
                                      "string argument allocation failed in ") +
                          std::string(methodName));
        detail::append_backend_error();
        return detail::from_result<Ret>(nullptr);
    }
    if (handle_) {
        const void *k = detail::Backend::object_get_class(handle_);
        if (k && detail::Backend::class_is_valuetype(k)) {
            const void *m = detail::Backend::find_method_exact(k, methodName, parameterTypeNames);
            if (m) {
                void *result = detail::invoke_value_type_method(m, k, handle_, pack);
                return detail::from_result<Ret>(result);
            }
        }
    }
    return CallExact<Ret>(methodName, parameterTypeNames, argv.empty() ? nullptr : argv.data());
}
template <class... Args>
bool Object::TryCallExact(std::string_view methodName, const std::vector<const char *> &parameterTypeNames,
                          Args &&...args) const {
    CallExact<void>(methodName, parameterTypeNames, std::forward<Args>(args)...);
    return detail::fallback_error() == nullptr;
}
namespace detail {
inline TypeRef reflection_type_ref(std::string_view typeName) {
    const std::size_t separator = typeName.rfind('.');
    if (separator == std::string_view::npos)
        return {"", "", typeName};
    return {"", typeName.substr(0, separator), typeName.substr(separator + 1)};
}
template <class T> void *reflection_argument(Arg<T> &argument) {
    using Value = std::remove_cvref_t<T>;
    constexpr bool charArray =
        std::is_array_v<Value> && std::is_same_v<std::remove_cv_t<std::remove_extent_t<Value>>, char>;
    if constexpr (is_wrapper_v<Value> || std::is_pointer_v<Value> || std::is_same_v<Value, std::string> ||
                  std::is_same_v<Value, std::string_view> || charArray) {
        return argument.ptr;
    } else {
        const auto typeName = parameter_type_name<Value>();
        if (!typeName) {
            set_error("Unity Object::InvokeGeneric failed: no managed type mapping "
                      "exists for a value-type argument");
            return nullptr;
        }
        const TypeRef type = reflection_type_ref(*typeName);
        const void *klass = type.resolve_class();
        if (!klass) {
            set_error(std::string("Unity Object::InvokeGeneric failed: value-type "
                                  "class not found: ") +
                      *typeName);
            append_backend_error();
            return nullptr;
        }
        void *boxed = Backend::value_box(klass, argument.ptr);
        if (!boxed) {
            set_error(std::string("Unity Object::InvokeGeneric failed: value-type "
                                  "boxing failed: ") +
                      *typeName);
            append_backend_error();
        }
        return boxed;
    }
}
template <class Tuple>
void *invoke_reflection_method(void *reflectionMethod, void *target, Tuple &arguments) {
    if (!reflectionMethod) {
        set_error("Unity reflection invocation failed: MethodInfo is null");
        return nullptr;
    }

    std::vector<void *> values;
    values.reserve(std::tuple_size_v<std::remove_reference_t<Tuple>>);
    std::apply(
        [&](auto &...argument) {
            (values.push_back(reflection_argument(argument)), ...);
        },
        arguments);
    if (fallback_error())
        return nullptr;

    void *argumentArray = nullptr;
    if constexpr (std::tuple_size_v<std::remove_reference_t<Tuple>> != 0) {
        void *objectType = TypeRef{"mscorlib", "System", "Object"}.resolve_type_object();
        argumentArray = make_reflection_array(objectType, values);
        if (!argumentArray)
            return nullptr;
    }

    Object methodObject{reflectionMethod};
    return methodObject.CallExact<void *>("Invoke", {"System.Object", "System.Object[]"}, target, argumentArray);
}
template <class Tuple>
void *invoke_value_type_method(const void *method, const void *declaringClass, void *target, Tuple &arguments) {
    if (!method || !declaringClass || !target) {
        set_error("Unity value-type invocation failed: method, declaring class, or target is null");
        return nullptr;
    }
    void *reflectionMethod = Backend::method_get_object(method, declaringClass);
    if (!reflectionMethod) {
        set_error("Unity value-type invocation failed: MethodInfo conversion failed");
        append_backend_error();
        return nullptr;
    }
    return invoke_reflection_method(reflectionMethod, target, arguments);
}
inline void *make_reflection_array(void *elementType, const std::vector<void *> &values) {
    if (!elementType) {
        set_error("Unity reflection array creation failed: element type is null");
        return nullptr;
    }
    TypeRef arrayTypeRef{"mscorlib", "System", "Array"};
    // Array.CreateInstance expects the element Type. Passing MakeArrayType()
    // here creates a jagged array (for example Type[][] instead of Type[]),
    // so reference writes either fail or corrupt the reflection invocation.
    void *array =
        InvokeStatic<void *>(arrayTypeRef, "CreateInstance", TypeObject{elementType}, static_cast<int>(values.size()));
    if (!array) {
        if (!fallback_error())
            set_error("Unity reflection array creation failed: Array.CreateInstance "
                      "returned null");
        return nullptr;
    }
    if (!Backend::has_array_length() || !Backend::has_array_ref_at() || !Backend::has_array_set_ref()) {
        set_error("Unity reflection array creation failed: required array APIs are unavailable");
        append_backend_error();
        return nullptr;
    }
    if (Backend::array_length(array) != values.size()) {
        set_error("Unity reflection array creation failed: Array.CreateInstance returned an unexpected length");
        return nullptr;
    }
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (!Backend::array_set_ref(array, i, values[i])) {
            set_error("Unity reflection array creation failed: array_set_ref "
                      "rejected an element");
            append_backend_error();
            return nullptr;
        }
        if (Backend::array_ref_at(array, i) != values[i]) {
            set_error("Unity reflection array creation failed: written array element did not round-trip");
            return nullptr;
        }
    }
    return array;
}

struct ResolvedGenericMethod {
    const void *method = nullptr;
    const void *declaringClass = nullptr;
    void *reflectionMethod = nullptr;
    void *genericParameterArray = nullptr;

    explicit operator bool() const noexcept {
        return method && declaringClass && reflectionMethod && genericParameterArray;
    }
};

inline ResolvedGenericMethod find_generic_method(const void *klass, std::string_view methodName,
                                                 std::size_t runtimeArgumentCount,
                                                 std::size_t genericArgumentCount) {
    if (!klass) {
        set_error("Unity generic method lookup failed: class is null");
        return {};
    }

    const std::string requestedName(methodName);
    const void *current = klass;
    while (current) {
        ResolvedGenericMethod match{};
        std::size_t matches = 0;
        void *iterator = nullptr;
        while (const void *candidate = Backend::class_get_methods(current, &iterator)) {
            const char *candidateName = Backend::method_get_name(candidate);
            if (!candidateName || requestedName != candidateName ||
                Backend::method_get_param_count(candidate) != runtimeArgumentCount ||
                !Backend::method_is_generic(candidate)) {
                continue;
            }

            void *methodInfo = Backend::method_get_object(candidate, current);
            if (!methodInfo)
                continue;

            // A runtime can surface an inflated MethodInfo while enumerating a
            // generic definition. Normalize it before validating generic arity.
            Object reflectionMethod{methodInfo};
            void *genericDefinition = reflectionMethod.CallExact<void *>("GetGenericMethodDefinition", {});
            if (!genericDefinition)
                continue;

            reflectionMethod = Object{genericDefinition};
            void *genericParameters = reflectionMethod.CallExact<void *>("GetGenericArguments", {});
            if (!genericParameters || !Backend::has_array_length() || !Backend::has_array_ref_at() ||
                !Backend::has_array_set_ref() || Backend::array_length(genericParameters) != genericArgumentCount) {
                continue;
            }

            match = {candidate, current, genericDefinition, genericParameters};
            ++matches;
        }

        if (matches > 1) {
            set_error(std::string("Unity generic method lookup failed: ambiguous overload: ") + requestedName);
            return {};
        }
        if (match)
            return match;
        current = Backend::class_get_parent(current);
    }

    set_error(std::string("Unity generic method lookup failed: no overload with ") +
              std::to_string(genericArgumentCount) + " generic argument(s): " + requestedName);
    return {};
}
}
inline GameObject GameObject::CreateUi(std::string_view name) {
    detail::clear_error();
    const void *gameObjectClass = GameObjectType.resolve_class();
    if (!gameObjectClass) {
        detail::set_error("Unity GameObject::CreateUi failed: GameObject class not found");
        detail::append_backend_error();
        return {};
    }
    void *rectTransformType = RectTransformType.resolve_type_object();
    void *systemType = TypeRef{"mscorlib", "System", "Type"}.resolve_type_object();
    if (!rectTransformType || !systemType) {
        detail::set_error("Unity GameObject::CreateUi failed: RectTransform or "
                          "System.Type metadata is unavailable");
        detail::append_backend_error();
        return {};
    }
    void *componentTypes = detail::make_reflection_array(systemType, {rectTransformType});
    if (!componentTypes)
        return {};
    const std::vector<const char *> signature{"System.String", "System.Type[]"};
    const void *constructor = detail::Backend::find_method_exact(gameObjectClass, ".ctor", signature);
    if (!constructor) {
        detail::set_error("Unity GameObject::CreateUi failed: GameObject(String, "
                          "Type[]) constructor is unavailable");
        detail::append_backend_error();
        return {};
    }
    void *object = detail::Backend::object_new(gameObjectClass);
    void *managedName = detail::Backend::new_string(name);
    if (!object || !managedName) {
        detail::set_error("Unity GameObject::CreateUi failed: GameObject or "
                          "managed name allocation failed");
        detail::append_backend_error();
        return {};
    }
    void *arguments[] = {managedName, componentTypes};
    void *exception = nullptr;
    if (!detail::Backend::runtime_invoke(constructor, object, arguments, nullptr, &exception) || exception) {
        detail::set_error("Unity GameObject::CreateUi failed: GameObject(String, "
                          "Type[]) constructor threw or could not be invoked");
        detail::append_backend_error();
        return {};
    }
    return GameObject{object};
}
template <class T>
void Object::SetReferenceArrayProperty(std::string_view propertyName, const std::vector<T> &values) const {
    static_assert(detail::is_wrapper_v<T>);
    detail::clear_error();
    if (!handle_) {
        detail::set_error("Unity Object::SetReferenceArrayProperty failed: target "
                          "object is null");
        return;
    }
    const TypeRef elementType = T::unity_type();
    void *elementTypeObject = elementType.resolve_type_object();
    if (!elementTypeObject) {
        detail::set_error(std::string("Unity Object::SetReferenceArrayProperty "
                                      "failed: element type not found: ") +
                          std::string(elementType.image) + ":" + std::string(elementType.namespc) + "." +
                          std::string(elementType.name));
        detail::append_backend_error();
        return;
    }
    std::vector<void *> handles;
    handles.reserve(values.size());
    for (const T &value : values)
        handles.push_back(value.handle());
    void *array = detail::make_reflection_array(elementTypeObject, handles);
    if (!array)
        return;
    const std::string parameterType = std::string(elementType.namespc) + "." + std::string(elementType.name) + "[]";
    void *args[] = {array};
    CallExact<void>(std::string("set_") + std::string(propertyName), {parameterType.c_str()}, args);
}
template <class Ret, class... Args>
Ret Object::InvokeGeneric(std::string_view methodName, const std::vector<TypeObject> &genericTypes,
                          Args &&...args) const {
    detail::clear_error();
    if (!handle_) {
        detail::set_error("Unity Object::InvokeGeneric failed: target object is null");
        return detail::from_result<Ret>(nullptr);
    }
    const void *klass = detail::Backend::object_get_class(handle_);
    if (!klass) {
        detail::set_error("Unity Object::InvokeGeneric failed: object_get_class failed");
        detail::append_backend_error();
        return detail::from_result<Ret>(nullptr);
    }
    if (genericTypes.empty()) {
        detail::set_error("Unity Object::InvokeGeneric failed: at least one "
                          "generic type is required");
        return detail::from_result<Ret>(nullptr);
    }

    const detail::ResolvedGenericMethod resolved =
        detail::find_generic_method(klass, methodName, sizeof...(Args), genericTypes.size());
    if (!resolved) {
        const std::string lookupDetail = detail::fallback_error() ? detail::fallback_error() : "unknown lookup failure";
        detail::set_error(std::string("Unity Object::InvokeGeneric failed: generic method not found: ") +
                          std::string(methodName) + "; detail: " + lookupDetail);
        detail::append_backend_error();
        return detail::from_result<Ret>(nullptr);
    }

    // GetGenericArguments() provides a correctly typed, fresh Type[] for the
    // selected definition. Reuse it instead of synthesizing a reflection array.
    void *typeArray = resolved.genericParameterArray;
    if (!detail::Backend::has_array_length() || !detail::Backend::has_array_ref_at() ||
        !detail::Backend::has_array_set_ref() || detail::Backend::array_length(typeArray) != genericTypes.size()) {
        detail::set_error("Unity Object::InvokeGeneric failed: generic parameter array is invalid");
        detail::append_backend_error();
        return detail::from_result<Ret>(nullptr);
    }
    for (std::size_t i = 0; i < genericTypes.size(); ++i) {
        void *typeObject = genericTypes[i].handle();
        if (!typeObject || !detail::Backend::array_set_ref(typeArray, i, typeObject) ||
            detail::Backend::array_ref_at(typeArray, i) != typeObject) {
            detail::set_error("Unity Object::InvokeGeneric failed: could not prepare generic type argument array");
            detail::append_backend_error();
            return detail::from_result<Ret>(nullptr);
        }
    }

    Object reflectionMethod{resolved.reflectionMethod};
    void *inflated = reflectionMethod.CallExact<void *>("MakeGenericMethod", {"System.Type[]"}, typeArray);
    if (!inflated)
        return detail::from_result<Ret>(nullptr);
    auto pack = std::tuple<detail::Arg<std::remove_cvref_t<Args>>...>(
        detail::Arg<std::remove_cvref_t<Args>>(std::forward<Args>(args))...);
    void *result = detail::invoke_reflection_method(inflated, handle_, pack);
    return detail::from_result<Ret>(result);
}
template <class T>
std::vector<T> Object::CallArrayExact(std::string_view methodName, const std::vector<const char *> &parameterTypeNames,
                                      void **rawArgs) const {
    std::vector<T> out;
    detail::clear_error();
    if (!handle_) {
        detail::set_error("Unity Object::CallArrayExact failed: target object is null");
        return out;
    }
    const void *k = detail::Backend::object_get_class(handle_);
    if (!k) {
        detail::set_error("Unity Object::CallArrayExact failed: object_get_class failed");
        detail::append_backend_error();
        return out;
    }
    const void *m = detail::Backend::find_method_exact(k, methodName, parameterTypeNames);
    if (!m) {
        const std::string lookupDetail = detail::fallback_error() ? detail::fallback_error() : "";
        detail::set_error(std::string("Unity Object::CallArrayExact failed: exact method not found: ") +
                          detail::signature_text(methodName, parameterTypeNames) +
                          (lookupDetail.empty() ? std::string{} : std::string("; detail: ") + lookupDetail));
        detail::append_backend_error();
        return out;
    }
    void *array = nullptr;
    void *ex = nullptr;
    if (!detail::Backend::runtime_invoke(m, handle_, rawArgs, &array, &ex) || ex) {
        detail::set_error(std::string("Unity Object::CallArrayExact failed: "
                                      "runtime_invoke exception in ") +
                          detail::signature_text(methodName, parameterTypeNames));
        detail::append_backend_error();
        return out;
    }
    if (!array) {
        detail::set_error(std::string("Unity Object::CallArrayExact failed: returned array was null: ") +
                          std::string(methodName));
        detail::append_backend_error();
        return out;
    }
    if (!detail::Backend::has_array_length()) {
        detail::set_error("Unity Object::CallArrayExact failed: backend "
                          "array_length API is unavailable");
        detail::append_backend_error();
        return out;
    }
    const std::size_t count = detail::Backend::array_length(array);
    if (count == 0)
        return out;
    if (!detail::Backend::has_array_ref_at()) {
        detail::set_error("Unity Object::CallArrayExact failed: backend "
                          "array_ref_at API is unavailable");
        detail::append_backend_error();
        return out;
    }
    out.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        void *item = detail::Backend::array_ref_at(array, i);
        if (item)
            out.emplace_back(item);
    }
    return out;
}
template <class T, class... Args>
std::vector<T> Object::CallArrayExact(std::string_view methodName, const std::vector<const char *> &parameterTypeNames,
                                      Args &&...args) const {
    detail::clear_error();
    if (parameterTypeNames.size() != sizeof...(Args)) {
        detail::set_error(std::string("Unity Object::CallArrayExact failed: argument count does not match ") +
                          detail::signature_text(methodName, parameterTypeNames) + "; received=" +
                          std::to_string(sizeof...(Args)));
        return {};
    }
    auto pack = std::tuple<detail::Arg<std::remove_cvref_t<Args>>...>(
        detail::Arg<std::remove_cvref_t<Args>>(std::forward<Args>(args))...);
    std::array<void *, sizeof...(Args)> argv{};
    std::size_t i = 0;
    bool argsValid = true;
    std::apply([&](auto &...a) { ((argsValid = argsValid && a.valid, argv[i++] = a.ptr), ...); }, pack);
    if (!argsValid) {
        detail::set_error(std::string("Unity Object::CallArrayExact failed: managed string "
                                      "argument allocation failed in ") +
                          std::string(methodName));
        detail::append_backend_error();
        return {};
    }
    return CallArrayExact<T>(methodName, parameterTypeNames, argv.empty() ? nullptr : argv.data());
}
template <class T, class... Args>
detail::RootedObjectArray<T>
Object::CallArrayExactRooted(std::string_view methodName, const std::vector<const char *> &parameterTypeNames,
                             Args &&...args) const {
    detail::clear_error();
    if (!handle_) {
        detail::set_error("Unity Object::CallArrayExactRooted failed: target object is null");
        return {};
    }
    if (parameterTypeNames.size() != sizeof...(Args)) {
        detail::set_error(std::string("Unity Object::CallArrayExactRooted failed: argument count does not match ") +
                          detail::signature_text(methodName, parameterTypeNames) + "; received=" +
                          std::to_string(sizeof...(Args)));
        return {};
    }
    const void *klass = detail::Backend::object_get_class(handle_);
    if (!klass) {
        detail::set_error("Unity Object::CallArrayExactRooted failed: object_get_class failed");
        detail::append_backend_error();
        return {};
    }
    const void *method = detail::Backend::find_method_exact(klass, methodName, parameterTypeNames);
    if (!method) {
        const std::string lookupDetail = detail::fallback_error() ? detail::fallback_error() : "";
        detail::set_error(std::string("Unity Object::CallArrayExactRooted failed: exact method not found: ") +
                          detail::signature_text(methodName, parameterTypeNames) +
                          (lookupDetail.empty() ? std::string{} : std::string("; detail: ") + lookupDetail));
        detail::append_backend_error();
        return {};
    }

    auto pack = std::tuple<detail::Arg<std::remove_cvref_t<Args>>...>(
        detail::Arg<std::remove_cvref_t<Args>>(std::forward<Args>(args))...);
    std::array<void *, sizeof...(Args)> argv{};
    std::size_t index = 0;
    bool argsValid = true;
    std::apply([&](auto &...argument) { ((argsValid = argsValid && argument.valid, argv[index++] = argument.ptr), ...); },
               pack);
    if (!argsValid) {
        detail::set_error(std::string("Unity Object::CallArrayExactRooted failed: managed argument allocation failed in ") +
                          std::string(methodName));
        detail::append_backend_error();
        return {};
    }

    void *array = nullptr;
    void *exception = nullptr;
    if (!detail::Backend::runtime_invoke(method, handle_, argv.empty() ? nullptr : argv.data(), &array, &exception) ||
        exception) {
        detail::set_error(std::string("Unity Object::CallArrayExactRooted failed: runtime_invoke exception in ") +
                          detail::signature_text(methodName, parameterTypeNames));
        detail::append_backend_error();
        return {};
    }
    return detail::RootedObjectArray<T>::from_managed_array(array, "Unity Object::CallArrayExactRooted");
}

template <class T>
detail::RootedObjectArray<T> detail::QueryComponentsRooted(const Object &target, TypeRef componentType,
                                                           bool recursive, bool includeInactive, bool reverse) {
    clear_error();
    void *typeObject = componentType.resolve_type_object();
    if (!typeObject) {
        set_error(std::string("Unity component query failed: component class not found: ") +
                  std::string(componentType.image) + ":" + std::string(componentType.namespc) + "." +
                  std::string(componentType.name));
        append_backend_error();
        return {};
    }

    // GetComponentsInternal is Unity's native-backed implementation. Prefer it
    // because stripped managed wrappers can retain metadata without a callable
    // body in IL2CPP players.
    auto result = target.CallArrayExactRooted<T>(
        "GetComponentsInternal",
        {"System.Type", "System.Boolean", "System.Boolean", "System.Boolean", "System.Boolean", "System.Object"},
        TypeObject{typeObject}, false, recursive, includeInactive, reverse, Object{});
    if (result)
        return result;

    const std::string internalError =
        fallback_error() ? fallback_error() : "GetComponentsInternal returned no rooted array";
    clear_error();

    std::string publicMethod;
    if (!recursive) {
        publicMethod = "GetComponents";
        result = target.CallArrayExactRooted<T>(publicMethod, {"System.Type"}, TypeObject{typeObject});
    } else {
        publicMethod = reverse ? "GetComponentsInParent" : "GetComponentsInChildren";
        if (includeInactive) {
            result = target.CallArrayExactRooted<T>(publicMethod, {"System.Type", "System.Boolean"},
                                                    TypeObject{typeObject}, true);
        } else {
            // Preserve compatibility with older Unity versions that only expose
            // the one-parameter overload, then try the newer explicit overload.
            result = target.CallArrayExactRooted<T>(publicMethod, {"System.Type"}, TypeObject{typeObject});
            if (!result) {
                const std::string oneParameterError =
                    fallback_error() ? fallback_error() : "one-parameter public component query failed";
                clear_error();
                result = target.CallArrayExactRooted<T>(publicMethod, {"System.Type", "System.Boolean"},
                                                        TypeObject{typeObject}, false);
                if (!result) {
                    const std::string explicitBooleanError =
                        fallback_error() ? fallback_error() : "two-parameter public component query failed";
                    set_error("Unity component query failed through native and public paths; native: " +
                              internalError + "; public one-parameter: " + oneParameterError +
                              "; public explicit-boolean: " + explicitBooleanError);
                    return {};
                }
            }
        }
    }
    if (result)
        return result;

    const std::string publicError = fallback_error() ? fallback_error() : "public component query returned no rooted array";
    set_error("Unity component query failed through native and public paths; native: " + internalError +
              "; public " + publicMethod + ": " + publicError);
    return {};
}
inline std::vector<std::string> Object::CallStringArrayExact(
    std::string_view methodName, const std::vector<const char *> &parameterTypeNames) const {
    std::vector<std::string> out;
    detail::clear_error();
    if (!handle_) {
        detail::set_error("Unity Object::CallStringArrayExact failed: target object is null");
        return out;
    }
    const void *k = detail::Backend::object_get_class(handle_);
    if (!k) {
        detail::set_error("Unity Object::CallStringArrayExact failed: object_get_class failed");
        detail::append_backend_error();
        return out;
    }
    const void *m = detail::Backend::find_method_exact(k, methodName, parameterTypeNames);
    if (!m) {
        const std::string lookupDetail = detail::fallback_error() ? detail::fallback_error() : "";
        detail::set_error(std::string("Unity Object::CallStringArrayExact failed: "
                                      "exact method not found: ") +
                          detail::signature_text(methodName, parameterTypeNames) +
                          (lookupDetail.empty() ? std::string{} : std::string("; detail: ") + lookupDetail));
        detail::append_backend_error();
        return out;
    }
    void *array = nullptr;
    void *ex = nullptr;
    if (!detail::Backend::runtime_invoke(m, handle_, nullptr, &array, &ex) || ex) {
        detail::set_error(std::string("Unity Object::CallStringArrayExact failed: "
                                      "runtime_invoke exception in ") +
                          detail::signature_text(methodName, parameterTypeNames));
        detail::append_backend_error();
        return out;
    }
    if (!array) {
        detail::set_error(std::string("Unity Object::CallStringArrayExact failed: "
                                      "returned array was null: ") +
                          std::string(methodName));
        detail::append_backend_error();
        return out;
    }
    if (!detail::Backend::has_array_length()) {
        detail::set_error("Unity Object::CallStringArrayExact failed: backend "
                          "array_length API is unavailable");
        detail::append_backend_error();
        return out;
    }
    const std::size_t count = detail::Backend::array_length(array);
    if (count == 0)
        return out;
    if (!detail::Backend::has_array_ref_at()) {
        detail::set_error("Unity Object::CallStringArrayExact failed: backend "
                          "array_ref_at API is unavailable");
        detail::append_backend_error();
        return out;
    }
    out.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        void *item = detail::Backend::array_ref_at(array, i);
        if (!item) {
            detail::set_error(std::string("Unity Object::CallStringArrayExact failed: string array "
                                          "contains a null element in ") +
                              std::string(methodName));
            detail::append_backend_error();
            out.clear();
            return out;
        }
        out.emplace_back(detail::managed_string_to_utf8(item));
        if (detail::fallback_error()) {
            out.clear();
            return out;
        }
    }
    return out;
}

namespace detail {
// "VRC.SDKBase.Networking" -> {"VRC.SDKBase", "Networking"}. The empty image
// slot makes TypeRef scan every loaded assembly on both backends, so lookup is
// backend-neutral and cached.
inline std::pair<std::string, std::string> split_fqn(std::string_view fqn) {
    const std::size_t dot = fqn.rfind('.');
    if (dot == std::string_view::npos)
        return {std::string{}, std::string(fqn)};
    return {std::string(fqn.substr(0, dot)), std::string(fqn.substr(dot + 1))};
}

// Resolve by name with no arity filter. Both backends reject argc < 0 in
// find_method (IL2CPP outright, Mono on ambiguity), so this walks the class
// and its bases directly. It answers only when exactly one method matches and
// otherwise fails with an ambiguity error, mirroring find_method's contract.
inline const void *find_method_any_arity(const void *klass, std::string_view methodName, int &outArgc) {
    outArgc = -1;
    if (!klass) {
        set_error("Unity method lookup failed: class is null");
        return nullptr;
    }
    const std::string requested(methodName);
    const void *current = klass;
    const void *match = nullptr;
    std::size_t candidates = 0;
    while (current) {
        void *iterator = nullptr;
        while (const void *candidate = Backend::class_get_methods(current, &iterator)) {
            const char *name = Backend::method_get_name(candidate);
            if (!name || requested != name)
                continue;
            if (!match) {
                match = candidate;
                outArgc = static_cast<int>(Backend::method_get_param_count(candidate));
            }
            ++candidates;
        }
        current = Backend::class_get_parent(current);
    }
    if (candidates > 1) {
        set_error("Unity method lookup failed: ambiguous method: " + requested);
        return nullptr;
    }
    return match;
}
} // namespace detail

// A cached, reusable method handle, mirroring the IL2CPP-SDK MethodHandler.
// resolve() splits a full class name, resolves the class through TypeRef's
// cached scan (empty image = all assemblies on either backend), and caches the
// Method handle each backend keeps for exact (class, name, arity) hits. Hold
// the ResolvedMethod in a function-local static or a module field and invoke()
// per frame without re-walking metadata.
class ResolvedMethod {
  public:
    ResolvedMethod() noexcept = default;

    static ResolvedMethod resolve(std::string_view className, std::string_view methodName, int argc = -1) {
        detail::clear_error();
        const std::pair<std::string, std::string> parts = detail::split_fqn(className);
        const TypeRef type{std::string_view{}, parts.first, parts.second};
        const void *k = type.resolve_class();
        if (!k) {
            detail::set_error(std::string("Unity ResolvedMethod::resolve failed: class not found: ") +
                              std::string(className));
            detail::append_backend_error();
            return {};
        }
        const void *m = nullptr;
        int resolvedArgc = argc;
        if (argc >= 0) {
            m = detail::Backend::find_method(k, methodName, argc);
        } else {
            m = detail::find_method_any_arity(k, methodName, resolvedArgc);
        }
        if (!m) {
            detail::set_error(std::string("Unity ResolvedMethod::resolve failed: method not found or ambiguous: ") +
                              std::string(className) + "." + std::string(methodName) +
                              (argc >= 0 ? "/" + std::to_string(argc) : std::string("/any")));
            detail::append_backend_error();
            return {};
        }
        ResolvedMethod out;
        out.klass_ = k;
        out.method_ = m;
        out.argc_ = resolvedArgc;
        out.methodName_ = std::string(methodName);
        return out;
    }

    static ResolvedMethod resolve_exact(std::string_view className, std::string_view methodName,
                                        const std::vector<const char *> &parameterTypeNames) {
        detail::clear_error();
        const std::pair<std::string, std::string> parts = detail::split_fqn(className);
        const TypeRef type{std::string_view{}, parts.first, parts.second};
        const void *k = type.resolve_class();
        if (!k) {
            detail::set_error(std::string("Unity ResolvedMethod::resolve_exact failed: class not found: ") +
                              std::string(className));
            detail::append_backend_error();
            return {};
        }
        const void *m = detail::Backend::find_method_exact(k, methodName, parameterTypeNames);
        if (!m) {
            detail::set_error(std::string("Unity ResolvedMethod::resolve_exact failed: exact method not found: ") +
                              std::string(className) + "." + detail::signature_text(methodName, parameterTypeNames));
            detail::append_backend_error();
            return {};
        }
        ResolvedMethod out;
        out.klass_ = k;
        out.method_ = m;
        out.argc_ = static_cast<int>(parameterTypeNames.size());
        out.methodName_ = std::string(methodName);
        return out;
    }

    explicit operator bool() const noexcept {
        return klass_ && method_;
    }
    const void *class_handle() const noexcept {
        return klass_;
    }
    const void *method_handle() const noexcept {
        return method_;
    }
    int argc() const noexcept {
        return argc_;
    }

    // Static invoke. resolve() with argc = -1 disables the arity check; the
    // method is then dispatched with whatever argument count the call site
    // supplies and the runtime rejects a mismatch.
    template <class Ret = void, class... Args> Ret invoke(Args &&...args) const {
        return invoke_impl<Ret>(nullptr, std::forward<Args>(args)...);
    }
    // Instance invoke. The managed target is passed separately from the method
    // arguments so a single Object argument cannot collide with the static
    // overload's pack.
    template <class Ret = void, class... Args> Ret call(void *target, Args &&...args) const {
        return invoke_impl<Ret>(target, std::forward<Args>(args)...);
    }
    template <class Ret = void, class... Args> Ret call(const Object &target, Args &&...args) const {
        return invoke_impl<Ret>(target.handle(), std::forward<Args>(args)...);
    }

  private:
    template <class Ret, class... Args> Ret invoke_impl(void *target, Args &&...args) const {
        detail::clear_error();
        if (!klass_ || !method_) {
            detail::set_error("Unity ResolvedMethod::invoke failed: method has not been resolved");
            return detail::from_result<Ret>(nullptr);
        }
        if (argc_ >= 0 && static_cast<std::size_t>(argc_) != sizeof...(Args)) {
            detail::set_error(std::string("Unity ResolvedMethod::invoke failed: argument count mismatch for ") +
                              methodName_ + ": resolved with " + std::to_string(argc_) + ", invoked with " +
                              std::to_string(sizeof...(Args)));
            return detail::from_result<Ret>(nullptr);
        }
        auto pack = std::tuple<detail::Arg<std::remove_cvref_t<Args>>...>(
            detail::Arg<std::remove_cvref_t<Args>>(std::forward<Args>(args))...);
        std::array<void *, sizeof...(Args)> argv{};
        std::size_t index = 0;
        bool argsValid = true;
        std::apply([&](auto &...a) { ((argsValid = argsValid && a.valid, argv[index++] = a.ptr), ...); }, pack);
        if (!argsValid) {
            detail::set_error(std::string("Unity ResolvedMethod::invoke failed: managed string "
                                          "argument allocation failed in ") +
                              methodName_);
            detail::append_backend_error();
            return detail::from_result<Ret>(nullptr);
        }
        if (target) {
            const void *k = detail::Backend::object_get_class(target);
            if (k && detail::Backend::class_is_valuetype(k)) {
                void *result = detail::invoke_value_type_method(method_, klass_, target, pack);
                return detail::from_result<Ret>(result);
            }
        }
        void *result = nullptr;
        void *ex = nullptr;
        if (!detail::Backend::runtime_invoke(method_, target, argv.empty() ? nullptr : argv.data(), &result, &ex) ||
            ex) {
            detail::set_error(std::string("Unity ResolvedMethod::invoke failed: runtime_invoke exception in ") +
                              methodName_);
            detail::append_backend_error();
            return detail::from_result<Ret>(nullptr);
        }
        return detail::from_result<Ret>(result);
    }

    const void *klass_ = nullptr;
    const void *method_ = nullptr;
    int argc_ = -1;
    std::string methodName_;
};

// One-liner static call by full class name, e.g.
// InvokeStaticFq<double>("VRC.SDKBase.Networking", "GetServerTimeInSeconds").
// Resolves with the arity of the supplied arguments, matching find_method's
// ambiguity contract.
template <class Ret = void, class... Args>
Ret InvokeStaticFq(std::string_view className, std::string_view methodName, Args &&...args) {
    const ResolvedMethod resolved = ResolvedMethod::resolve(className, methodName, static_cast<int>(sizeof...(Args)));
    return resolved.invoke<Ret>(std::forward<Args>(args)...);
}

// Static call returning a managed array by full class name. Copies the rooted
// elements, so the caller owns no GC lifetime.
template <class T, class... Args>
std::vector<T> StaticArrayCallFq(std::string_view className, std::string_view methodName, Args &&...args) {
    void *array = InvokeStaticFq<void *>(className, methodName, std::forward<Args>(args)...);
    return detail::RootedObjectArray<T>::from_managed_array(array, "Unity FQN static array call").copy_items();
}

// ===========================================================================
// MethodHandler
//
// IL2CPP-SDK compat surface. The SDK's MethodHandler::resolve() returns a
// Method handle; the equivalent here is ResolvedMethod (same lookup: full
// class name + method name + optional arity, cached). invoke_raw/invoke() add
// the SDK's raw-parameter and typed-unboxing convenience on top of it.
// ===========================================================================
using detail::Backend;
using detail::type_name_matches;
class MethodHandler {
  public:
    MethodHandler() = delete;

    static ResolvedMethod resolve(std::string_view className, std::string_view methodName, int argc = -1) {
        return ResolvedMethod::resolve(className, methodName, argc);
    }

    static void *invoke_raw(const ResolvedMethod &method, void *obj, void **params = nullptr) {
        if (!method) {
            detail::set_error("Unity MethodHandler::invoke_raw failed: method has not been resolved");
            return nullptr;
        }
        void *result = nullptr;
        void *ex = nullptr;
        if (!detail::Backend::runtime_invoke(method.method_handle(), obj, params, &result, &ex) || ex) {
            detail::set_error("Unity MethodHandler::invoke_raw failed: runtime_invoke exception");
            detail::append_backend_error();
            return nullptr;
        }
        return result;
    }

    template <typename TReturn = void>
    static TReturn invoke(const ResolvedMethod &method, void *obj, void **params = nullptr) {
        if constexpr (std::is_void_v<TReturn>) {
            invoke_raw(method, obj, params);
        } else {
            void *result = invoke_raw(method, obj, params);
            if (!result)
                return TReturn{};
            if constexpr (std::is_pointer_v<TReturn>)
                return reinterpret_cast<TReturn>(result);
            else {
                void *unboxed = detail::Backend::object_unbox(result);
                return *reinterpret_cast<TReturn *>(unboxed ? unboxed : result);
            }
        }
    }
};

// ===========================================================================
// ClassResolver -- the IL2CPP-SDK reflection DSL port.
//
// Fluent queries over one class handle that validate against live metadata and
// capture offsets / raw handles / native method pointers. Backend-neutral: all
// metadata access goes through detail::Backend with capability gating (e.g.
// toPtr() yields nullptr on backends that expose no method pointer). The SDK's
// Deobfuscation integration and SEH guards are intentionally not brought over;
// deobfuscate() is accepted for API parity but registers nothing.
// ===========================================================================
using ResolverTraceFn = void (*)(const char *msg);
inline ResolverTraceFn g_resolverTrace = nullptr;

inline void SetResolverTrace(ResolverTraceFn fn) {
    g_resolverTrace = fn;
}

inline void ResolverTrace(const char *msg) {
    if (g_resolverTrace)
        g_resolverTrace(msg);
}

namespace reflection_detail {

constexpr std::uint32_t kStaticMemberFlag = 0x0010u;

inline std::string type_name_string(const void *type) {
    if (!type)
        return {};
    char buffer[512]{};
    return Backend::type_get_name(type, buffer, sizeof(buffer)) ? std::string(buffer) : std::string{};
}

inline std::string short_name(std::string_view name) {
    const std::size_t pos = name.find_last_of(".+/");
    return pos == std::string_view::npos ? std::string(name) : std::string(name.substr(pos + 1));
}

// Full normalized equality or short-name equality, mirroring the SDK's
// TypeNameEquals.
inline bool equal_requested(std::string_view actual, const char *requested) {
    if (!requested || !*requested)
        return false;
    if (type_name_matches(actual, requested))
        return true;
    return type_name_matches(short_name(actual), requested);
}

inline bool type_matches_class(std::string_view typeName, const void *klass) {
    if (!klass)
        return false;
    const char *nm = Backend::class_get_name(klass);
    if (nm && equal_requested(typeName, nm))
        return true;
    const char *ns = Backend::class_get_namespace(klass);
    if (ns && ns[0] && nm) {
        const std::string full = std::string(ns) + "." + nm;
        if (equal_requested(typeName, full.c_str()))
            return true;
    }
    return false;
}

inline const void *method_param_type(const void *method, int index) {
    return method && index >= 0 ? Backend::method_get_param_type(method, static_cast<std::uint32_t>(index)) : nullptr;
}

struct FieldDescriptor {
    const void *klass = nullptr;
    const void *field = nullptr;
    bool is_static = false;
    std::string type_name;
};

} // namespace reflection_detail

class FieldQuery {
    friend class ClassResolver;

    const void *m_targetType = nullptr;
    bool m_hasTypeFilter = false;
    std::string m_typeName;
    std::string m_name;
    std::optional<bool> m_static;
    bool m_selfType = false;
    bool m_required = false;
    int *m_offsetDest = nullptr;
    void **m_ptrDest = nullptr;
    void **m_fieldRawDest = nullptr;
    std::string *m_nameDest = nullptr;
    std::string m_label;
    const void *m_matched = nullptr;

  public:
    FieldQuery &label(std::string l) {
        m_label = std::move(l);
        return *this;
    }

    std::string describe() const {
        if (!m_label.empty())
            return m_label;
        if (!m_name.empty())
            return m_name;
        if (!m_typeName.empty())
            return "field:" + m_typeName;
        if (m_selfType)
            return "field:self";
        if (m_hasTypeFilter) {
            const char *n = m_targetType ? Backend::class_get_name(m_targetType) : nullptr;
            return std::string("field:") + (n ? n : "<null-type>");
        }
        return "field:?";
    }

    FieldQuery &byType(const void *t) {
        m_targetType = t;
        m_hasTypeFilter = true;
        return *this;
    }
    FieldQuery &byTypeName(std::string_view n) {
        m_typeName = n;
        return *this;
    }
    FieldQuery &byName(std::string_view n) {
        m_name = n;
        return *this;
    }
    FieldQuery &isStatic() {
        m_static = true;
        return *this;
    }
    FieldQuery &notStatic() {
        m_static = false;
        return *this;
    }
    FieldQuery &isSelf() {
        m_selfType = true;
        return *this;
    }
    FieldQuery &required() {
        m_required = true;
        return *this;
    }
    FieldQuery &toOffset(int &dest) {
        m_offsetDest = &dest;
        return *this;
    }
    FieldQuery &toPtr(void *&dest) {
        m_ptrDest = &dest;
        return *this;
    }
    FieldQuery &toFieldRaw(void *&dest) {
        m_fieldRawDest = &dest;
        return *this;
    }
    FieldQuery &toName(std::string &dest) {
        m_nameDest = &dest;
        return *this;
    }
    FieldQuery &deobfuscate(std::string, std::string) {
        return *this;
    }

    bool matches(const void *klass, const void *field, bool isStatic, std::string_view typeName) const {
        if (m_matched)
            return false;
        if (!m_name.empty()) {
            const char *n = Backend::field_get_name(field);
            if (!n || m_name != n)
                return false;
        }
        if (m_static.has_value() && m_static.value() != isStatic)
            return false;
        if (m_hasTypeFilter && !reflection_detail::type_matches_class(typeName, m_targetType))
            return false;
        if (m_selfType && !reflection_detail::type_matches_class(typeName, klass))
            return false;
        if (!m_typeName.empty() && !reflection_detail::equal_requested(typeName, m_typeName.c_str()))
            return false;
        return true;
    }

    bool valid() const {
        return !m_required || m_matched != nullptr;
    }
    bool matched() const {
        return m_matched != nullptr;
    }

    void capture(const void *field) {
        m_matched = field;
    }

    void apply() const {
        if (!m_matched)
            return;
        if (m_offsetDest)
            *m_offsetDest = Backend::field_get_offset(m_matched);
        if (m_ptrDest)
            *m_ptrDest = const_cast<void *>(m_matched);
        if (m_fieldRawDest)
            *m_fieldRawDest = const_cast<void *>(m_matched);
        if (m_nameDest) {
            const char *n = Backend::field_get_name(m_matched);
            *m_nameDest = n ? n : "";
        }
    }

    void reset() {
        m_matched = nullptr;
    }
};

class MethodQuery {
    friend class ClassResolver;

    std::string m_name;
    int m_paramCount = -1;
    std::vector<std::pair<int, const void *>> m_paramTypes;
    const void *m_returnType = nullptr;
    bool m_hasReturnFilter = false;
    bool m_collectAll = false;
    bool m_required = false;
    std::string m_label;
    void **m_ptrDest = nullptr;
    void **m_methodRawDest = nullptr;
    std::vector<void *> *m_ptrListDest = nullptr;
    std::vector<const void *> m_matched;

  public:
    MethodQuery &label(std::string l) {
        m_label = std::move(l);
        return *this;
    }

    std::string describe() const {
        if (!m_label.empty())
            return m_label;
        if (!m_name.empty())
            return m_name;
        return "method:?";
    }

    MethodQuery &byName(std::string_view n) {
        m_name = n;
        return *this;
    }
    MethodQuery &withParams(int count) {
        m_paramCount = count;
        return *this;
    }
    MethodQuery &paramType(int index, const void *type) {
        m_paramTypes.emplace_back(index, type);
        return *this;
    }
    MethodQuery &returnType(const void *type) {
        m_returnType = type;
        m_hasReturnFilter = true;
        return *this;
    }
    MethodQuery &collectAll() {
        m_collectAll = true;
        return *this;
    }
    MethodQuery &required() {
        m_required = true;
        return *this;
    }
    MethodQuery &toPtr(void *&dest) {
        m_ptrDest = &dest;
        return *this;
    }
    MethodQuery &toMethodRaw(void *&dest) {
        m_methodRawDest = &dest;
        return *this;
    }
    MethodQuery &toPtrList(std::vector<void *> &dest) {
        m_ptrListDest = &dest;
        m_collectAll = true;
        return *this;
    }
    MethodQuery &deobfuscate(std::string, std::string) {
        return *this;
    }

    bool matches(const void *method) const {
        if (!m_collectAll && !m_matched.empty())
            return false;
        if (!m_name.empty()) {
            const char *n = Backend::method_get_name(method);
            if (!n || m_name != n)
                return false;
        }
        if (m_paramCount >= 0 && static_cast<int>(Backend::method_get_param_count(method)) != m_paramCount)
            return false;
        for (const auto &entry : m_paramTypes) {
            const void *pt = reflection_detail::method_param_type(method, entry.first);
            const std::string pn = reflection_detail::type_name_string(pt);
            if (pn.empty() || !reflection_detail::type_matches_class(pn, entry.second))
                return false;
        }
        if (m_hasReturnFilter) {
            const void *rt = Backend::method_get_return_type(method);
            const std::string rn = reflection_detail::type_name_string(rt);
            if (rn.empty() || !reflection_detail::type_matches_class(rn, m_returnType))
                return false;
        }
        return true;
    }

    bool valid() const {
        return !m_required || !m_matched.empty();
    }
    bool matched() const {
        return !m_matched.empty();
    }

    void capture(const void *method) {
        m_matched.push_back(method);
    }

    void apply() const {
        if (!m_matched.empty()) {
            if (m_ptrDest)
                *m_ptrDest = Backend::method_pointer(m_matched[0]);
            if (m_methodRawDest)
                *m_methodRawDest = const_cast<void *>(m_matched[0]);
        }
        if (m_ptrListDest) {
            m_ptrListDest->clear();
            for (const void *m : m_matched)
                m_ptrListDest->push_back(Backend::method_pointer(m));
        }
    }

    void reset() {
        m_matched.clear();
    }
};

class PropertyQuery {
    friend class ClassResolver;

    const void *m_targetType = nullptr;
    bool m_hasTypeFilter = false;
    std::string m_typeName;
    std::string m_name;
    int m_index = 0;
    bool m_requireSetter = false;
    bool m_requireGetter = true;
    std::optional<bool> m_static;
    bool m_required = false;
    std::string m_label;
    void **m_getterPtrDest = nullptr;
    void **m_setterPtrDest = nullptr;
    void **m_getterRawDest = nullptr;
    void **m_setterRawDest = nullptr;
    std::string *m_nameDest = nullptr;
    int m_seen = 0;
    const void *m_matched = nullptr;

  public:
    PropertyQuery &label(std::string l) {
        m_label = std::move(l);
        return *this;
    }

    std::string describe() const {
        if (!m_label.empty())
            return m_label;
        if (!m_name.empty())
            return m_name;
        if (!m_typeName.empty())
            return "prop:" + m_typeName + "[" + std::to_string(m_index) + "]";
        return "prop:?";
    }

    PropertyQuery &byType(const void *t) {
        m_targetType = t;
        m_hasTypeFilter = true;
        return *this;
    }
    PropertyQuery &byTypeName(std::string_view n) {
        m_typeName = n;
        return *this;
    }
    PropertyQuery &byName(std::string_view n) {
        m_name = n;
        return *this;
    }
    PropertyQuery &atIndex(int i) {
        m_index = i;
        return *this;
    }
    PropertyQuery &isStatic() {
        m_static = true;
        return *this;
    }
    PropertyQuery &notStatic() {
        m_static = false;
        return *this;
    }
    PropertyQuery &hasSetter() {
        m_requireSetter = true;
        return *this;
    }
    PropertyQuery &allowNoGetter() {
        m_requireGetter = false;
        return *this;
    }
    PropertyQuery &required() {
        m_required = true;
        return *this;
    }
    PropertyQuery &toGetter(void *&dest) {
        m_getterPtrDest = &dest;
        return *this;
    }
    PropertyQuery &toSetter(void *&dest) {
        m_setterPtrDest = &dest;
        return *this;
    }
    PropertyQuery &toGetterRaw(void *&dest) {
        m_getterRawDest = &dest;
        return *this;
    }
    PropertyQuery &toSetterRaw(void *&dest) {
        m_setterRawDest = &dest;
        return *this;
    }
    PropertyQuery &toName(std::string &dest) {
        m_nameDest = &dest;
        return *this;
    }
    PropertyQuery &deobfuscate(std::string, std::string) {
        return *this;
    }

    bool matches(const void *property) const {
        if (m_matched)
            return false;
        const void *getter = Backend::property_get_get_method(property);
        const void *setter = Backend::property_get_set_method(property);
        if (m_requireGetter && !getter)
            return false;
        if (m_requireSetter && !setter)
            return false;
        if (m_static.has_value()) {
            const void *accessor = getter ? getter : setter;
            if (!accessor || Backend::method_is_static(accessor) != m_static.value())
                return false;
        }
        if (!m_name.empty()) {
            const char *n = Backend::property_get_name(property);
            if (!n || m_name != n)
                return false;
        }
        const std::string typeName =
            getter ? reflection_detail::type_name_string(Backend::method_get_return_type(getter))
                   : reflection_detail::type_name_string(reflection_detail::method_param_type(setter, 0));
        if (m_hasTypeFilter && !reflection_detail::type_matches_class(typeName, m_targetType))
            return false;
        if (!m_typeName.empty() && !reflection_detail::equal_requested(typeName, m_typeName.c_str()))
            return false;
        return true;
    }

    void capture(const void *property) {
        if (m_seen++ == m_index)
            m_matched = property;
    }

    bool valid() const {
        return !m_required || m_matched != nullptr;
    }
    bool matched() const {
        return m_matched != nullptr;
    }

    void apply() const {
        if (!m_matched)
            return;
        const void *getter = Backend::property_get_get_method(m_matched);
        const void *setter = Backend::property_get_set_method(m_matched);
        if (m_getterPtrDest && getter)
            *m_getterPtrDest = Backend::method_pointer(getter);
        if (m_setterPtrDest && setter)
            *m_setterPtrDest = Backend::method_pointer(setter);
        if (m_getterRawDest && getter)
            *m_getterRawDest = const_cast<void *>(getter);
        if (m_setterRawDest && setter)
            *m_setterRawDest = const_cast<void *>(setter);
        if (m_nameDest) {
            const char *n = Backend::property_get_name(m_matched);
            *m_nameDest = n ? n : "";
        }
    }

    void reset() {
        m_matched = nullptr;
        m_seen = 0;
    }
};

class FieldCounter {
    friend class ClassResolver;

    const void *m_targetType = nullptr;
    bool m_hasTypeFilter = false;
    std::string m_typeName;
    std::optional<bool> m_static;
    int m_count = 0;
    int m_expectedMin = -1;
    int m_expectedMax = -1;
    int m_expectedExact = -1;

  public:
    FieldCounter &byType(const void *t) {
        m_targetType = t;
        m_hasTypeFilter = true;
        return *this;
    }
    FieldCounter &byTypeName(std::string_view n) {
        m_typeName = n;
        return *this;
    }
    FieldCounter &isStatic() {
        m_static = true;
        return *this;
    }
    FieldCounter &notStatic() {
        m_static = false;
        return *this;
    }
    FieldCounter &expectExact(int n) {
        m_expectedExact = n;
        return *this;
    }
    FieldCounter &expectMin(int n) {
        m_expectedMin = n;
        return *this;
    }
    FieldCounter &expectMax(int n) {
        m_expectedMax = n;
        return *this;
    }
    FieldCounter &expectRange(int min, int max) {
        m_expectedMin = min;
        m_expectedMax = max;
        return *this;
    }

    bool matches(const reflection_detail::FieldDescriptor &f) const {
        if (m_static.has_value() && m_static.value() != f.is_static)
            return false;
        if (m_hasTypeFilter && !reflection_detail::type_matches_class(f.type_name, m_targetType))
            return false;
        if (!m_typeName.empty() && !reflection_detail::equal_requested(f.type_name, m_typeName.c_str()))
            return false;
        return true;
    }

    void increment() {
        ++m_count;
    }

    bool valid() const {
        if (m_expectedExact >= 0 && m_count != m_expectedExact)
            return false;
        if (m_expectedMin >= 0 && m_count < m_expectedMin)
            return false;
        if (m_expectedMax >= 0 && m_count > m_expectedMax)
            return false;
        return true;
    }
    int count() const {
        return m_count;
    }
    void reset() {
        m_count = 0;
    }

    std::string describe() const {
        const std::string what =
            !m_typeName.empty() ? m_typeName
                                : (m_targetType ? std::string(Backend::class_get_name(m_targetType)) : std::string("?"));
        return "count:" + what + "=" + std::to_string(m_count);
    }
};

class IndexedFieldCollector {
    friend class ClassResolver;

    const void *m_targetType = nullptr;
    bool m_hasTypeFilter = false;
    std::string m_typeName;
    std::optional<bool> m_static;
    int m_requiredCount = 0;

    struct Binding {
        int index = 0;
        int *offsetDest = nullptr;
        void **ptrDest = nullptr;
        void **fieldRawDest = nullptr;
        std::string *nameDest = nullptr;
        Binding(int i, int *offset, void **ptr, void **raw, std::string *name)
            : index(i), offsetDest(offset), ptrDest(ptr), fieldRawDest(raw), nameDest(name) {}
    };
    std::vector<Binding> m_bindings;
    std::vector<const void *> m_captured;

  public:
    IndexedFieldCollector &byType(const void *t) {
        m_targetType = t;
        m_hasTypeFilter = true;
        return *this;
    }
    IndexedFieldCollector &byTypeName(std::string_view n) {
        m_typeName = n;
        return *this;
    }
    IndexedFieldCollector &isStatic() {
        m_static = true;
        return *this;
    }
    IndexedFieldCollector &notStatic() {
        m_static = false;
        return *this;
    }
    IndexedFieldCollector &requireCount(int n) {
        m_requiredCount = n;
        return *this;
    }
    IndexedFieldCollector &requireMinCount(int n) {
        m_requiredCount = n;
        return *this;
    }

    IndexedFieldCollector &bindOffset(int index, int &offsetDest) {
        m_bindings.emplace_back(index, &offsetDest, nullptr, nullptr, nullptr);
        return *this;
    }
    IndexedFieldCollector &bindPtr(int index, void *&ptrDest) {
        m_bindings.emplace_back(index, nullptr, &ptrDest, nullptr, nullptr);
        return *this;
    }
    IndexedFieldCollector &bindFieldRaw(int index, void *&dest) {
        m_bindings.emplace_back(index, nullptr, nullptr, &dest, nullptr);
        return *this;
    }
    IndexedFieldCollector &bind(int index, int &offsetDest, void *&ptrDest) {
        m_bindings.emplace_back(index, &offsetDest, &ptrDest, nullptr, nullptr);
        return *this;
    }
    IndexedFieldCollector &bind(int index, int &offsetDest, std::string &nameDest) {
        m_bindings.emplace_back(index, &offsetDest, nullptr, nullptr, &nameDest);
        return *this;
    }
    IndexedFieldCollector &bind(int index, void *&ptrDest, std::string &nameDest) {
        m_bindings.emplace_back(index, nullptr, &ptrDest, nullptr, &nameDest);
        return *this;
    }

    bool matches(const reflection_detail::FieldDescriptor &f) const {
        if (m_static.has_value() && m_static.value() != f.is_static)
            return false;
        if (m_hasTypeFilter && !reflection_detail::type_matches_class(f.type_name, m_targetType))
            return false;
        if (!m_typeName.empty() && !reflection_detail::equal_requested(f.type_name, m_typeName.c_str()))
            return false;
        return true;
    }

    void capture(const void *field) {
        m_captured.push_back(field);
    }

    bool valid() const {
        return m_requiredCount <= 0 || static_cast<int>(m_captured.size()) >= m_requiredCount;
    }
    int collected() const {
        return static_cast<int>(m_captured.size());
    }

    std::string describe() const {
        const std::string what =
            !m_typeName.empty() ? m_typeName
                                : (m_targetType ? std::string(Backend::class_get_name(m_targetType)) : std::string("?"));
        return "collect:" + what + " got " + std::to_string(m_captured.size()) + "/" +
               std::to_string(m_requiredCount);
    }

    void apply() const {
        for (const Binding &b : m_bindings) {
            if (b.index < 0 || b.index >= static_cast<int>(m_captured.size()))
                continue;
            const void *field = m_captured[b.index];
            if (b.offsetDest)
                *b.offsetDest = Backend::field_get_offset(field);
            if (b.ptrDest)
                *b.ptrDest = const_cast<void *>(field);
            if (b.fieldRawDest)
                *b.fieldRawDest = const_cast<void *>(field);
            if (b.nameDest) {
                const char *n = Backend::field_get_name(field);
                *b.nameDest = n ? n : "";
            }
        }
    }

    void reset() {
        m_captured.clear();
    }
};

class ClassResolver {
    const void *m_klass = nullptr;
    std::vector<FieldQuery> m_fieldQueries;
    std::vector<MethodQuery> m_methodQueries;
    std::vector<PropertyQuery> m_propertyQueries;
    std::vector<FieldCounter> m_fieldCounters;
    std::vector<IndexedFieldCollector> m_indexedCollectors;
    std::vector<std::string> m_misses;
    bool m_includeInherited = false;

  public:
    explicit ClassResolver(const void *klass) : m_klass(klass) {}

    static ClassResolver by_name(std::string_view fullName) {
        const std::pair<std::string, std::string> parts = detail::split_fqn(fullName);
        const TypeRef type{std::string_view{}, parts.first, parts.second};
        return ClassResolver(type.resolve_class());
    }

    FieldQuery &field() {
        return m_fieldQueries.emplace_back();
    }
    MethodQuery &method() {
        return m_methodQueries.emplace_back();
    }
    PropertyQuery &property() {
        return m_propertyQueries.emplace_back();
    }
    FieldCounter &counter() {
        return m_fieldCounters.emplace_back();
    }
    IndexedFieldCollector &collector() {
        return m_indexedCollectors.emplace_back();
    }

    // get_fields() returns declared fields only, so inherited members are
    // invisible without walking parents.
    ClassResolver &includeInherited(bool enable = true) {
        m_includeInherited = enable;
        return *this;
    }
    ClassResolver &deobfuscate(std::string) {
        return *this;
    }

    void reset() {
        for (auto &q : m_fieldQueries)
            q.reset();
        for (auto &q : m_methodQueries)
            q.reset();
        for (auto &q : m_propertyQueries)
            q.reset();
        for (auto &c : m_fieldCounters)
            c.reset();
        for (auto &c : m_indexedCollectors)
            c.reset();
        m_misses.clear();
    }

    bool validate(bool collectMisses = false) {
        if (!m_klass)
            return false;
        reset();

        if (g_resolverTrace) {
            char buf[256]{};
            const char *ns = Backend::class_get_namespace(m_klass);
            const char *nm = Backend::class_get_name(m_klass);
            snprintf(buf, sizeof(buf), "validate klass=%p raw='%s%s%s' fQ=%zu mQ=%zu ctr=%zu coll=%zu", m_klass,
                     ns ? ns : "", ns && ns[0] ? "." : "", nm ? nm : "<null>", m_fieldQueries.size(),
                     m_methodQueries.size(), m_fieldCounters.size(), m_indexedCollectors.size());
            g_resolverTrace(buf);
        }

        bool needsStatic = false;
        for (const auto &q : m_fieldQueries)
            needsStatic |= q.m_static.has_value();
        for (const auto &c : m_fieldCounters)
            needsStatic |= c.m_static.has_value();
        for (const auto &c : m_indexedCollectors)
            needsStatic |= c.m_static.has_value();

        int depth = 0;
        for (const void *cur = m_klass; cur && depth < 64;
             cur = m_includeInherited ? Backend::class_get_parent(cur) : nullptr, ++depth) {
            void *it = nullptr;
            while (const void *field = Backend::class_get_fields(cur, &it)) {
                reflection_detail::FieldDescriptor f;
                f.klass = cur;
                f.field = field;
                f.is_static = Backend::field_is_static(field);
                f.type_name = reflection_detail::type_name_string(Backend::field_get_type(field));
                for (auto &counter : m_fieldCounters)
                    if (counter.matches(f))
                        counter.increment();
                for (auto &coll : m_indexedCollectors)
                    if (coll.matches(f))
                        coll.capture(field);
                for (auto &query : m_fieldQueries)
                    if (query.matches(cur, field, f.is_static, f.type_name))
                        query.capture(field);
            }
        }

        bool ok = true;
        auto check = [&](bool valid, auto &&describe) -> bool {
            if (valid)
                return true;
            ok = false;
            if (collectMisses)
                m_misses.push_back(describe());
            return false;
        };

        for (const auto &c : m_fieldCounters)
            if (!check(c.valid(), [&] { return c.describe(); }) && !collectMisses)
                return false;
        for (const auto &c : m_indexedCollectors)
            if (!check(c.valid(), [&] { return c.describe(); }) && !collectMisses)
                return false;
        for (const auto &q : m_fieldQueries)
            if (!check(q.valid(), [&] { return q.describe(); }) && !collectMisses)
                return false;

        if (!m_propertyQueries.empty()) {
            void *pit = nullptr;
            while (const void *prop = Backend::class_get_properties(m_klass, &pit)) {
                for (auto &query : m_propertyQueries)
                    if (query.matches(prop))
                        query.capture(prop);
            }
            for (const auto &q : m_propertyQueries)
                if (!check(q.valid(), [&] { return q.describe(); }) && !collectMisses)
                    return false;
        }

        if (!m_methodQueries.empty()) {
            if (g_resolverTrace) {
                char buf[160]{};
                snprintf(buf, sizeof(buf), "  iter methods klass=%p", m_klass);
                g_resolverTrace(buf);
            }
            void *mit = nullptr;
            while (const void *meth = Backend::class_get_methods(m_klass, &mit)) {
                for (auto &query : m_methodQueries)
                    if (query.matches(meth))
                        query.capture(meth);
            }
            for (const auto &q : m_methodQueries)
                if (!check(q.valid(), [&] { return q.describe(); }) && !collectMisses)
                    return false;
        }

        return ok;
    }

    void apply() {
        for (const auto &q : m_fieldQueries)
            q.apply();
        for (const auto &q : m_methodQueries)
            q.apply();
        for (const auto &q : m_propertyQueries)
            q.apply();
        for (const auto &c : m_indexedCollectors)
            c.apply();
    }

    bool resolve() {
        if (!validate())
            return false;
        apply();
        return true;
    }

    bool resolvePartial() {
        const bool ok = validate(true);
        apply();
        return ok;
    }

    const std::vector<std::string> &misses() const {
        return m_misses;
    }

    std::string missReport() const {
        std::string out;
        for (const auto &m : m_misses) {
            if (!out.empty())
                out += ", ";
            out += m;
        }
        return out;
    }

    const void *klass() const {
        return m_klass;
    }

    const void *raw() const {
        return m_klass;
    }
};
)URKUNITY";

