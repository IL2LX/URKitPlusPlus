// Metadata-driven VRChat type wrappers.
//
// The type table below was captured from live IL2CPP metadata through the
// runtime explorer (VRChat 2022.3.22f2-DWR). Method signatures are only present
// for members that were captured individually; a type listed with zero methods
// is exposed through its fields and properties alone.

namespace VrcGenerated {

struct MemberSpec {
    const char *name;
    const char *managed_type;
};

struct MethodSpec {
    const char *name;
    const char *return_type;
    const char *const *params;
    int param_count;
    bool is_static;
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

// Managed enum values are not present in the captured metadata, so enums are
// surfaced as their underlying integer rather than guessed at. Every managed
// enum is an int32 underneath, so this is a lossless widening.
inline bool IsManagedEnum(const std::string &managed) {
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

inline std::string EmitType(const TypeSpec &spec, const std::string &rel_path) {
    std::string out;
    out += "#pragma once\n\n";
    out += "#include \"sdk/unity/unity.h\"\n";
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
            out += "    " + cpp + " " + std::string(field.name) + "() const {\n";
            out += "        return GetField<" + cpp + ">(\"" + std::string(field.name) + "\");\n";
            out += "    }\n";
            out += "    void set_" + std::string(field.name) + "(" + cpp + " value) const {\n";
            out += "        SetField<" + cpp + ">(\"" + std::string(field.name) + "\", value);\n";
            out += "    }\n\n";
        }

        for (int i = 0; i < spec.property_count; ++i) {
            const MemberSpec &prop = spec.properties[i];
            if (IsSkippableField(prop.name, prop.managed_type))
                continue;
            const std::string cpp = ManagedToCpp(prop.managed_type);
            out += "    " + cpp + " " + std::string(prop.name) + "() const {\n";
            out += "        return GetProperty<" + cpp + ">(\"" + std::string(prop.name) + "\");\n";
            out += "    }\n\n";
        }

        for (int i = 0; i < spec.method_count; ++i) {
            const MethodSpec &method = spec.methods[i];
            if (IsPropertyAccessor(method.name))
                continue;
            const std::string ret = ManagedToCpp(method.return_type);
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
            out += "\"" + std::string(method.name) + "\", {";
            for (int p = 0; p < method.param_count; ++p)
                out += (p ? ", " : "") + std::string("\"") + method.params[p] + "\"";
            out += "}";
            for (int p = 0; p < method.param_count; ++p)
                out += ", " + ParamName(p);
            out += ");\n    }\n\n";
        }
        out += "};\n";
    }

    out += "} // namespace " + std::string(spec.cpp_namespace) + "\n";
    (void)rel_path;
    return out;
}

} // namespace VrcGenerated
