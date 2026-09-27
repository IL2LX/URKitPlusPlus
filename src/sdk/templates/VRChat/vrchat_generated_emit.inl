// Metadata-driven VRChat type wrappers.
//
// The type table below was captured from live IL2CPP metadata through the
// runtime explorer (VRChat 2022.3.22f2-DWR). Method signatures are only present
// for members that were captured individually; a type listed with zero methods
// is exposed through its fields and properties alone.

namespace VrcGenerated {

// Every managed enum in the capture, so IsManagedEnum does not need a hand edit
// per release. Defined by vrchat_generated_enums.inl, which is included ahead of
// this file and defines URK_VRCHAT_GENERATED_ENUMS. The guard matters because
// vrchat_table_gen includes this emitter without the generated file, in order to
// produce it.
#if defined(URK_VRCHAT_GENERATED_ENUMS)
inline const char *const *VrcGeneratedEnumNames();
#endif

struct MemberSpec {
    const char *name;
    const char *managed_type;
    // Name to try when `name` is absent, or null. VRChat renames internals
    // between releases, and a single extra string in the table absorbs that
    // without re-capturing every signature.
    const char *alt_name;
};

struct MethodSpec {
    const char *name;
    const char *return_type;
    const char *const *params;
    int param_count;
    bool is_static;
    const char *alt_name;
};

struct TypeSpec {
    const char *cpp_name;
    const char *cpp_namespace;
    const char *image;
    const char *managed_namespace;
    const char *managed_class;
    bool is_enum;
    bool is_interface;
    bool is_static_class;
    bool is_value_type;
    const char *const *enum_names;
    int enum_count;
    const MemberSpec *fields;
    int field_count;
    const MemberSpec *properties;
    int property_count;
    const MethodSpec *methods;
    int method_count;
};

inline bool IsSkippableField(const std::string &name, const std::string &type) {
    if (name.empty() || name.front() == '_' || name.front() == '<')
        return true;
    if (name.rfind("k__BackingField", 0) == 0)
        return true;
    return type.find("Delegate") != std::string::npos || type.find("System.Action") != std::string::npos ||
           type.find("System.Func") != std::string::npos;
}

inline bool IsPropertyAccessor(const std::string &name) {
    return name.rfind("get_", 0) == 0 || name.rfind("set_", 0) == 0 || name.rfind("add_", 0) == 0 ||
           name.rfind("remove_", 0) == 0;
}

// The name a lookup should actually use: the recorded name, or the fallback when
// one is supplied. Emitted as a literal so a table edit is the only thing needed
// to retarget a renamed member.
inline std::string ResolvedName(const char *name, const char *alt_name) {
    if (alt_name != nullptr && alt_name[0] != '\0')
        return std::string(alt_name);
    return std::string(name);
}

// Managed enum values are not present in the captured metadata, so enums are
// surfaced as their underlying integer rather than guessed at. Every managed
// enum is an int32 underneath, so this is a lossless widening.
inline bool IsManagedEnum(const std::string &managed) {
    // Generated from the capture, so a new enum does not need a hand edit here.
    // Without it a member of an unknown enum type is emitted as void* and read
    // as a pointer where the value is really an integer.
#if defined(URK_VRCHAT_GENERATED_ENUMS)
    for (const char *const *it = VrcGeneratedEnumNames(); it != nullptr && *it != nullptr; ++it) {
        if (managed == *it)
            return true;
    }
#endif
    static const char *kEnums[] = {
        "VRC.SDKBase.VRCLayers",
        "VRC.SDKBase.VRC_SceneDescriptor.SpawnOrder",
        "VRC.SDKBase.VRC_SceneDescriptor.SpawnOrientation",
        "VRC.SDKBase.VRC_SceneDescriptor.RespawnHeightBehaviour",
        "VRC.SDKBase.VRC_StereoObject.Eye",
        "VRC.SDKBase.VRC_AvatarPedestal.Placement",
        "UnityEngine.LayerMask",
        "UnityEngine.LightmapsMode",
        "UnityEngine.Rendering.AmbientMode",
        "UnityEngine.Rendering.DefaultReflectionMode",
        "UnityEngine.FogMode",
        "UnityEngine.LogType",
        "VRC.Core.LoggingMode",
        "Microsoft.Extensions.Logging.LogLevel",
    };
    for (const char *known : kEnums) {
        if (managed == known)
            return true;
    }
    return false;
}

inline std::string ManagedToCpp(const std::string &managed) {
    if (IsManagedEnum(managed))
        return "int";

    static const std::pair<const char *, const char *> kPrimitives[] = {
        { "System.Void", "void" },
        { "System.Boolean", "bool" },
        { "System.Int32", "std::int32_t" },
        { "System.Int64", "std::int64_t" },
        { "System.Int16", "std::int16_t" },
        { "System.UInt32", "std::uint32_t" },
        { "System.Byte", "std::uint8_t" },
        { "System.SByte", "std::int8_t" },
        { "System.Char", "char" },
        { "System.Single", "float" },
        { "System.Double", "double" },
        { "System.String", "std::string" },
    };
    for (const auto &entry : kPrimitives) {
        if (managed == entry.first)
            return entry.second;
    }

    static const std::pair<const char *, const char *> kUnityTypes[] = {
        { "UnityEngine.GameObject", "Unity::GameObject" },
        { "UnityEngine.Transform", "Unity::Transform" },
        { "UnityEngine.Component", "Unity::Component" },
        { "UnityEngine.Object", "Unity::Object" },
        { "UnityEngine.Texture2D", "Unity::Texture2D" },
        { "UnityEngine.Sprite", "Unity::Sprite" },
        { "UnityEngine.Material", "Unity::Material" },
        { "UnityEngine.Shader", "Unity::Shader" },
        { "UnityEngine.Camera", "Unity::Camera" },
        { "UnityEngine.Renderer", "Unity::Renderer" },
        { "UnityEngine.AudioSource", "Unity::AudioSource" },
        { "UnityEngine.RenderTexture", "Unity::RenderTexture" },
        { "UnityEngine.Cubemap", "Unity::Cubemap" },
        { "UnityEngine.Skybox", "Unity::Skybox" },
        { "UnityEngine.Color", "Unity::Color" },
        { "UnityEngine.Vector2", "Unity::Vector2" },
        { "UnityEngine.Vector3", "Unity::Vector3" },
        { "UnityEngine.Vector4", "Unity::Vector4" },
        { "UnityEngine.Quaternion", "Unity::Quaternion" },
        { "UnityEngine.Rect", "Unity::Rect" },
        { "UnityEngine.LayerMask", "Unity::LayerMask" },
        { "UnityEngine.Matrix4x4", "Unity::Matrix4x4" },
        { "UnityEngine.Plane", "Unity::Plane" },
        { "UnityEngine.Mesh", "Unity::Object" },
        { "UnityEngine.AudioClip", "Unity::Object" },
        { "UnityEngine.MonoBehaviour", "Unity::Component" },
        { "UnityEngine.ScriptableObject", "Unity::Object" },
    };
    for (const auto &entry : kUnityTypes) {
        if (managed == entry.first)
            return entry.second;
    }

    static const std::pair<const char *, const char *> kVrcTypes[] = {
        { "VRC.SDKBase.VRC_SceneDescriptor", "VrcSceneDescriptor" },
        { "VRC.SDKBase.VRCPlayerApi", "VRC::SDKBase::VRCPlayerApi" },
        { "VRC.SDKBase.Network.VRCNetworkBehaviour", "Unity::Component" },
    };
    for (const auto &entry : kVrcTypes) {
        if (managed == entry.first)
            return entry.second;
    }

    return "void*";
}

// A parameter is only safe for inferred dispatch when it is a plain primitive.
// Everything else must go through the explicit-name path, otherwise a handle is
// silently inferred as System.Object and the call binds to the wrong overload.
inline bool IsDispatchPrimitive(const std::string &managed) {
    return managed == "System.Boolean" || managed == "System.Int32" || managed == "System.Int64" ||
           managed == "System.Int16" || managed == "System.Byte" || managed == "System.SByte" ||
           managed == "System.Char" || managed == "System.Single" || managed == "System.Double";
}

inline std::string ParamName(std::size_t index) {
    return "arg" + std::to_string(index);
}

// Headers a type actually needs, decided by what its members reference rather
// than by which namespace it lands in. Keying this off the namespace gave every
// VRC::SDKBase type the VRCPlayerAPI include whether it used one or not, while
// the types outside that namespace which did use VRCPlayerApi in a method
// signature got nothing and only compiled through a transitive include.
inline std::string RequiredIncludes(const TypeSpec &spec) {
    bool needsPlayerApi = false;
    bool needsApiUser = false;

    const auto note = [&](const char *managed) {
        if (managed == nullptr)
            return;
        const std::string type(managed);
        if (type == "VRC.SDKBase.VRCPlayerApi")
            needsPlayerApi = true;
        else if (type == "VRC.Core.APIUser")
            needsApiUser = true;
    };

    for (int i = 0; i < spec.field_count; ++i)
        note(spec.fields[i].managed_type);
    for (int i = 0; i < spec.property_count; ++i)
        note(spec.properties[i].managed_type);
    for (int i = 0; i < spec.method_count; ++i) {
        const MethodSpec &method = spec.methods[i];
        note(method.return_type);
        for (int p = 0; p < method.param_count; ++p)
            note(method.params[p]);
    }

    std::string includes;
    if (needsPlayerApi)
        includes += "#include \"sdk/VRChat/VRC/SDKBase/VRCPlayerAPI.h\"\n";
    if (needsApiUser)
        includes += "#include \"sdk/VRChat/VRC/Core/APIUser.h\"\n";
    return includes;
}

inline std::string EmitType(const TypeSpec &spec, const std::string &rel_path) {
    std::string out;
    out += "#pragma once\n\n";
    out += "#include \"sdk/unity/unity.h\"\n";
    out += RequiredIncludes(spec);
    // Retained because a hand-written VRC::Core or VRC::SDKBase type may reach
    // for these through a void* member, which carries no type name to inspect.
    if (spec.cpp_namespace == std::string("VRC::Core"))
        out += "#include \"sdk/VRChat/VRC/Core/APIUser.h\"\n";
    if (spec.cpp_namespace == std::string("VRC::SDKBase"))
        out += "#include \"sdk/VRChat/VRC/SDKBase/VRCPlayerAPI.h\"\n";
    out += "\n";
    out += "namespace " + std::string(spec.cpp_namespace) + " {\n";
    out += "inline constexpr Unity::TypeRef k" + std::string(spec.cpp_name) + "{ \"" + std::string(spec.image) +
           "\", \"" + std::string(spec.managed_namespace) + "\", \"" + std::string(spec.managed_class) + "\" };\n\n";

    if (spec.is_enum) {
        out += "using " + std::string(spec.cpp_name) + " = int;\n";
        for (int i = 0; i < spec.enum_count; ++i)
            out += "inline constexpr " + std::string(spec.cpp_name) + " " + spec.enum_names[i] + "{};\n";
        out += "\n";
    } else if (spec.is_interface) {
        out += "// Interface type: no callable surface is emitted.\n";
    } else if (spec.is_static_class || spec.is_value_type) {
        out += "namespace " + std::string(spec.cpp_name) + " {\n";
        // Every property captured on these types is static, so it is reached
        // through its get_ accessor rather than an instance read.
        for (int i = 0; i < spec.property_count; ++i) {
            const MemberSpec &prop = spec.properties[i];
            if (IsSkippableField(prop.name, prop.managed_type))
                continue;
            const std::string cpp = ManagedToCpp(prop.managed_type);
            out += "inline " + cpp + " " + std::string(prop.name) + "() {\n";
            out += "    return URK::Unity::detail::InvokeStaticExact<" + cpp + ">(k" + std::string(spec.cpp_name) +
                   ", \"get_" + std::string(prop.name) + "\", {});\n";
            out += "}\n";
        }
        for (int i = 0; i < spec.method_count; ++i) {
            const MethodSpec &method = spec.methods[i];
            if (IsPropertyAccessor(method.name) || !method.is_static)
                continue;
            const std::string ret = ManagedToCpp(method.return_type);
            bool all_primitive = true;
            for (int p = 0; p < method.param_count; ++p) {
                if (!IsDispatchPrimitive(method.params[p]))
                    all_primitive = false;
            }
            out += "inline " + ret + " " + std::string(method.name) + "(";
            for (int p = 0; p < method.param_count; ++p)
                out += (p ? ", " : "") + std::string(ManagedToCpp(method.params[p])) + " " + ParamName(p);
            out += ") {\n";
            if (method.param_count == 0 && all_primitive && ret != "void*") {
                out += "    return URK::Unity::detail::InvokeStaticExact<" + ret + ">(k" + std::string(spec.cpp_name) +
                       ", \"" + std::string(method.name) + "\", {});\n";
            } else {
                out += "    return URK::Unity::detail::InvokeStaticExact<" + ret + ">(k" + std::string(spec.cpp_name) +
                       ", \"" + std::string(method.name) + "\", {";
                for (int p = 0; p < method.param_count; ++p)
                    out += (p ? ", " : "") + std::string("\"") + method.params[p] + "\"";
                out += "}";
                for (int p = 0; p < method.param_count; ++p)
                    out += ", " + ParamName(p);
                out += ");\n";
            }
            out += "}\n";
        }
        out += "} // namespace " + std::string(spec.cpp_name) + "\n";
    } else {
        out += "struct " + std::string(spec.cpp_name) + " : Unity::Component {\n";
        out += "    using Unity::Component::Component;\n";
        out += "    static constexpr Unity::TypeRef unity_type() { return k" + std::string(spec.cpp_name) + "; }\n\n";

        for (int i = 0; i < spec.field_count; ++i) {
            const MemberSpec &field = spec.fields[i];
            if (IsSkippableField(field.name, field.managed_type))
                continue;
            const std::string cpp = ManagedToCpp(field.managed_type);
            const std::string member = ResolvedName(field.name, field.alt_name);
            out += "    " + cpp + " " + std::string(field.name) + "() const {\n";
            out += "        return GetField<" + cpp + ">(\"" + member + "\");\n";
            out += "    }\n";
            // The Try form reports whether the member still exists upstream, so a
            // VRChat rename is a checkable condition rather than a silent default.
            out += "    bool try_" + std::string(field.name) + "(" + cpp + " &out) const {\n";
            out += "        return TryGetField<" + cpp + ">(\"" + member + "\", out);\n";
            out += "    }\n";
            out += "    void set_" + std::string(field.name) + "(" + cpp + " value) const {\n";
            out += "        SetField<" + cpp + ">(\"" + member + "\", value);\n";
            out += "    }\n";
            out += "    bool try_set_" + std::string(field.name) + "(" + cpp + " value) const {\n";
            out += "        return TrySetField<" + cpp + ">(\"" + member + "\", value);\n";
            out += "    }\n\n";
        }

        for (int i = 0; i < spec.property_count; ++i) {
            const MemberSpec &prop = spec.properties[i];
            if (IsSkippableField(prop.name, prop.managed_type))
                continue;
            const std::string cpp = ManagedToCpp(prop.managed_type);
            const std::string member = ResolvedName(prop.name, prop.alt_name);
            out += "    " + cpp + " " + std::string(prop.name) + "() const {\n";
            out += "        return GetProperty<" + cpp + ">(\"" + member + "\");\n";
            out += "    }\n";
            out += "    bool try_" + std::string(prop.name) + "(" + cpp + " &out) const {\n";
            out += "        return TryGetProperty<" + cpp + ">(\"" + member + "\", out);\n";
            out += "    }\n\n";
        }

        for (int i = 0; i < spec.method_count; ++i) {
            const MethodSpec &method = spec.methods[i];
            if (IsPropertyAccessor(method.name))
                continue;
            const std::string ret = ManagedToCpp(method.return_type);
            const std::string member = ResolvedName(method.name, method.alt_name);
            // A static method must not be dispatched through the instance: doing
            // so would pass `this` as the first argument and shift every
            // parameter by one.
            const bool instance_call = !method.is_static;
            out += instance_call ? "    " : "    static ";
            out += ret + " " + std::string(method.name) + "(";
            for (int p = 0; p < method.param_count; ++p)
                out += (p ? ", " : "") + std::string(ManagedToCpp(method.params[p])) + " " + ParamName(p);
            out += instance_call ? ") const {\n        return CallExact<" : ") {\n        return URK::Unity::detail::InvokeStaticExact<";
            out += ret + ">(";
            if (!instance_call)
                out += "unity_type(), ";
            out += "\"" + member + "\", {";
            for (int p = 0; p < method.param_count; ++p)
                out += (p ? ", " : "") + std::string("\"") + method.params[p] + "\"";
            out += "}";
            for (int p = 0; p < method.param_count; ++p)
                out += ", " + ParamName(p);
            out += ");\n    }\n";

            // The Try form surfaces whether the method still exists upstream,
            // which is the difference between "returned a default" and "renamed".
            // A void method has nothing to hand back, so it gets no out parameter:
            // `void &out` does not compile.
            const bool returns_void = ret == "void";
            out += instance_call ? "    bool try_" : "    static bool try_";
            out += std::string(method.name) + "(";
            for (int p = 0; p < method.param_count; ++p)
                out += (p ? ", " : "") + std::string(ManagedToCpp(method.params[p])) + " " + ParamName(p);
            if (returns_void) {
                out += instance_call ? ") const {\n" : ") {\n";
            } else {
                // The out parameter carries its own separator only when the method
                // already has parameters, otherwise the signature opens with a comma.
                if (method.param_count > 0) out += ",";
                out += " " + ret + " &out";
                out += instance_call ? ") const {\n" : ") {\n";
            }
            if (returns_void)
                out += instance_call ? "        CallExact<void>(\"" + member + "\", {"
                                    : "        URK::Unity::detail::InvokeStaticExact<void>(unity_type(), \"" +
                                          member + "\", {";
            else
                out += instance_call
                            ? "        out = CallExact<" + ret + ">(\"" + member + "\", {"
                            : "        out = URK::Unity::detail::InvokeStaticExact<" + ret +
                                  ">(unity_type(), \"" + member + "\", {";
            for (int p = 0; p < method.param_count; ++p)
                out += (p ? ", " : "") + std::string("\"") + method.params[p] + "\"";
            out += "}";
            for (int p = 0; p < method.param_count; ++p)
                out += ", " + ParamName(p);
            out += ");\n        return URK::Unity::detail::fallback_error() == nullptr;\n    }\n\n";
        }
        out += "};\n";
    }

    out += "} // namespace " + std::string(spec.cpp_namespace) + "\n";
    (void)rel_path;
    return out;
}

} // namespace VrcGenerated
