// Captured type table. Source: live IL2CPP metadata, VRChat 2022.3.22f2-DWR.

namespace VrcGenerated {

// --- VRC.SDKBase.VRC_SceneDescriptor -----------------------------------------
inline const MemberSpec kSceneDescriptorFields[] = {
    { "spawns", "UnityEngine.Transform[]" },
    { "spawnRadius", "System.Single" },
    { "spawnOrder", "VRC.SDKBase.VRC_SceneDescriptor.SpawnOrder" },
    { "spawnOrientation", "VRC.SDKBase.VRC_SceneDescriptor.SpawnOrientation" },
    { "ReferenceCamera", "UnityEngine.GameObject" },
    { "RespawnHeightY", "System.Single" },
    { "ObjectBehaviourAtRespawnHeight", "VRC.SDKBase.VRC_SceneDescriptor.RespawnHeightBehaviour" },
    { "ForbidUserPortals", "System.Boolean" },
    { "interactThruLayers", "System.Int32" },
    { "autoSpatializeAudioSources", "System.Boolean" },
    { "gravity", "UnityEngine.Vector3" },
    { "layerCollisionArr", "System.Boolean[]" },
    { "capacity", "System.Int32" },
    { "contentSex", "System.Boolean" },
    { "contentViolence", "System.Boolean" },
    { "contentGore", "System.Boolean" },
    { "contentOther", "System.Boolean" },
    { "releasePublic", "System.Boolean" },
    { "unityVersion", "System.String" },
    { "udonProducts", "System.Collections.Generic.List<System.String>" },
    { "Name", "System.String" },
    { "NSFW", "System.Boolean" },
    { "SpawnPosition", "UnityEngine.Vector3" },
    { "SpawnLocation", "UnityEngine.Transform" },
    { "DrawDistance", "System.Single" },
    { "useAssignedLayers", "System.Boolean" },
    { "DynamicPrefabs", "System.Collections.Generic.List<UnityEngine.GameObject>" },
    { "sDynamicPrefabs", "System.Collections.Generic.Dictionary<System.String,UnityEngine.GameObject>" },
    { "DynamicMaterials", "System.Collections.Generic.List<UnityEngine.Material>" },
    { "sDynamicMaterials", "System.Collections.Generic.Dictionary<System.String,UnityEngine.Material>" },
    { "LightMapsNear", "UnityEngine.Texture2D[]" },
    { "LightMapsFar", "UnityEngine.Texture2D[]" },
    { "LightMode", "UnityEngine.LightmapsMode" },
    { "RenderAmbientEquatorColor", "UnityEngine.Color" },
    { "RenderAmbientGroundColor", "UnityEngine.Color" },
    { "RenderAmbientIntensity", "System.Single" },
    { "RenderAmbientLight", "UnityEngine.Color" },
    { "RenderAmbientMode", "UnityEngine.Rendering.AmbientMode" },
    { "RenderFog", "System.Boolean" },
    { "RenderFogColor", "UnityEngine.Color" },
    { "RenderFogMode", "UnityEngine.FogMode" },
    { "RenderFogDensity", "System.Single" },
    { "RenderFogLinearStart", "System.Single" },
    { "RenderFogLinearEnd", "System.Single" },
    { "RenderHaloStrength", "System.Single" },
    { "RenderFlareFadeSpeed", "System.Single" },
    { "RenderFlareStrength", "System.Single" },
    { "RenderDefaultReflectionMode", "UnityEngine.Rendering.DefaultReflectionMode" },
    { "RenderDefaultReflectionResolution", "System.Int32" },
    { "RenderReflectionBounces", "System.Int32" },
    { "RenderReflectionIntensity", "System.Single" },
    { "RenderSkybox", "UnityEngine.Material" },
    { "NetworkIDs", "System.Collections.Generic.List<VRC.SDKBase.Network.NetworkIDPair>" },
    { "portraitCameraPositionOffset", "UnityEngine.Vector3" },
    { "portraitCameraRotationOffset", "UnityEngine.Quaternion" },
    { "bundleUnityMajorVersion", "System.Int32" },
};
inline const MemberSpec kSceneDescriptorProperties[] = {
    { "NetworkIDCollection", "System.Collections.Generic.List<VRC.SDKBase.Network.NetworkIDPair>" },
    { "NetworkIDLookup", "System.Collections.Generic.Dictionary<UnityEngine.GameObject,VRC.SDKBase.Network.NetworkIDPair>" },
    { "Instance", "VRC.SDKBase.VRC_SceneDescriptor" },
    { "Exists", "System.Boolean" },
    { "BundleUnityMajorVersion", "System.Int32" },
    { "Scene", "UnityEngine.SceneManagement.Scene" },
};
inline const char *const kFindNetworkIDGameObjectParams[] = { "System.String", "System.Boolean" };
inline const char *const kGetMaterialParams[] = { "System.String" };
inline const char *const kGetNetworkIDGameObjectPathParams[] = { "UnityEngine.GameObject",
                                                                "System.Collections.Generic.HashSet<System.String>" };
inline const char *const kGetNetworkIDObjectsParams[] = { "System.Collections.Generic.List<VRC.SDKBase.INetworkID>" };
inline const char *const kGetPrefabParams[] = { "System.String" };
inline const char *const kSetBundleUnityMajorVersionParams[] = { "System.Int32" };
inline const char *const kSetNetworkIDCollectionParams[] = {
    "System.Collections.Generic.List<VRC.SDKBase.Network.NetworkIDPair>"
};
inline const char *const kSetNetworkIDLookupParams[] = {
    "System.Collections.Generic.Dictionary<UnityEngine.GameObject,VRC.SDKBase.Network.NetworkIDPair>"
};
inline const char *const kSetSceneParams[] = { "UnityEngine.SceneManagement.Scene" };
inline const char *const kPositionPortraitCameraParams[] = { "UnityEngine.Transform" };
inline const MethodSpec kSceneDescriptorMethods[] = {
    { "Awake", "System.Void", nullptr, 0, false },
    { "FindNetworkIDGameObject", "UnityEngine.GameObject", kFindNetworkIDGameObjectParams, 2, false },
    { "GetMaterial", "UnityEngine.Material", kGetMaterialParams, 1, true },
    { "GetNetworkIDGameObjectPath", "System.String", kGetNetworkIDGameObjectPathParams, 2, false },
    { "GetNetworkIDObjects", "System.Void", kGetNetworkIDObjectsParams, 1, false },
    { "GetPrefab", "UnityEngine.GameObject", kGetPrefabParams, 1, true },
    { "GetValidatedSpawnList", "System.Collections.Generic.List<UnityEngine.Transform>", nullptr, 0, false },
    { "OnDestroy", "System.Void", nullptr, 0, false },
    { "PositionPortraitCamera", "System.Void", kPositionPortraitCameraParams, 1, false },
    { "set_BundleUnityMajorVersion", "System.Void", kSetBundleUnityMajorVersionParams, 1, false },
    { "set_NetworkIDCollection", "System.Void", kSetNetworkIDCollectionParams, 1, false },
    { "set_NetworkIDLookup", "System.Void", kSetNetworkIDLookupParams, 1, false },
    { "set_Scene", "System.Void", kSetSceneParams, 1, false },
};
inline const TypeSpec kSceneDescriptor = {
    "VrcSceneDescriptor", "VRC::SDKBase", "VRCSDKBase.dll", "VRC.SDKBase", "VRC_SceneDescriptor",
    false, false, false, false, nullptr, 0,
    kSceneDescriptorFields, static_cast<int>(std::size(kSceneDescriptorFields)),
    kSceneDescriptorProperties, static_cast<int>(std::size(kSceneDescriptorProperties)),
    kSceneDescriptorMethods, static_cast<int>(std::size(kSceneDescriptorMethods)),
};

// --- VRC.SDKBase.VRC_Serialization -------------------------------------------
inline const MemberSpec kSerializationProperties[] = {
    { "AllowPluginTypes", "System.Boolean" },
    { "SurrogateSelector", "System.Runtime.Serialization.ISurrogateSelector" },
    { "Dispatcher", "VRC.SDKBase.VRC_EventDispatcher" },
};
inline const char *const kSetAllowPluginTypesParams[] = { "System.Boolean" };
inline const char *const kRegisterPluginTypeParams[] = { "System.Type" };
inline const char *const kRegisterTypeParams[] = { "System.Type" };
inline const char *const kParameterEncoderParams[] = { "System.Object[]" };
inline const char *const kParameterDecoderParams[] = { "System.Byte[]", "System.Boolean" };
inline const char *const kGameObjectPathParams[] = { "UnityEngine.GameObject" };
inline const char *const kFindGameObjectParams[] = { "System.String" };
inline const MethodSpec kSerializationMethods[] = {
    { "set_AllowPluginTypes", "System.Void", kSetAllowPluginTypesParams, 1, true },
    { "RegisterPluginType", "System.Void", kRegisterPluginTypeParams, 1, true },
    { "RegisterType", "System.Void", kRegisterTypeParams, 1, true },
    { "ParameterEncoder", "System.Byte[]", kParameterEncoderParams, 1, true },
    { "ParameterDecoder", "System.Object[]", kParameterDecoderParams, 2, true },
    { "GetGameObjectPathFallback", "System.String", kGameObjectPathParams, 1, true },
    { "GetGameObjectPath", "System.String", kGameObjectPathParams, 1, true },
    { "FindGameObject", "UnityEngine.GameObject", kFindGameObjectParams, 1, true },
};
inline const TypeSpec kSerialization = {
    "VrcSerialization", "VRC::SDKBase", "VRCSDKBase.dll", "VRC.SDKBase", "VRC_Serialization",
    false, false, true, false, nullptr, 0,
    nullptr, 0,
    kSerializationProperties, static_cast<int>(std::size(kSerializationProperties)),
    kSerializationMethods, static_cast<int>(std::size(kSerializationMethods)),
};

// --- VRC.SDKBase.VRC_AvatarPedestal -------------------------------------------
inline const MemberSpec kAvatarPedestalFields[] = {
    { "blueprintId", "System.String" },
    { "grantBlueprintAccess", "System.Boolean" },
    { "Placement", "UnityEngine.Transform" },
    { "ChangeAvatarsOnUse", "System.Boolean" },
    { "scale", "System.Single" },
    { "avatarImage", "UnityEngine.Texture2D" },
};
inline const char *const kAvatarBlueprintIdParams[] = { "System.String" };
inline const char *const kSetAvatarUseParams[] = { "VRC.SDKBase.VRCPlayerApi" };
inline const MethodSpec kAvatarPedestalMethods[] = {
    { "Awake", "System.Void", nullptr, 0, false },
    { "SwitchAvatar", "System.Void", kAvatarBlueprintIdParams, 1, false },
    { "SetAvatarUse", "System.Void", kSetAvatarUseParams, 1, false },
};
inline const TypeSpec kAvatarPedestal = {
    "VrcAvatarPedestal", "VRC::SDKBase", "VRCSDKBase.dll", "VRC.SDKBase", "VRC_AvatarPedestal",
    false, false, false, false, nullptr, 0,
    kAvatarPedestalFields, static_cast<int>(std::size(kAvatarPedestalFields)),
    nullptr, 0,
    kAvatarPedestalMethods, static_cast<int>(std::size(kAvatarPedestalMethods)),
};

// --- VRC.SDKBase.VRC_SpatialAudioSource ---------------------------------------
inline const MemberSpec kSpatialAudioFields[] = {
    { "Gain", "System.Single" },
    { "Far", "System.Single" },
    { "Near", "System.Single" },
    { "VolumetricRadius", "System.Single" },
    { "EnableSpatialization", "System.Boolean" },
    { "UseAudioSourceVolumeCurve", "System.Boolean" },
};
inline const TypeSpec kSpatialAudio = {
    "VrcSpatialAudioSource", "VRC::SDKBase", "VRCSDKBase.dll", "VRC.SDKBase", "VRC_SpatialAudioSource",
    false, false, false, false, nullptr, 0,
    kSpatialAudioFields, static_cast<int>(std::size(kSpatialAudioFields)),
    nullptr, 0,
    nullptr, 0,
};

// --- VRC.SDKBase.VRC_StereoObject ---------------------------------------------
inline const MemberSpec kStereoObjectFields[] = {
    { "eye", "VRC.SDKBase.VRC_StereoObject.Eye" },
};
inline const TypeSpec kStereoObject = {
    "VrcStereoObject", "VRC::SDKBase", "VRCSDKBase.dll", "VRC.SDKBase", "VRC_StereoObject",
    false, false, false, false, nullptr, 0,
    kStereoObjectFields, static_cast<int>(std::size(kStereoObjectFields)),
    nullptr, 0,
    nullptr, 0,
};

// --- VRC.SDKBase.VRCLayers (enum) ---------------------------------------------
inline const char *const kVrcLayersNames[] = { "Avatar", "Environment", "Interactive" };
inline const TypeSpec kVrcLayers = {
    "VrcLayers", "VRC::SDKBase", "VRCSDKBase.dll", "VRC.SDKBase", "VRCLayers",
    true, false, false, false, kVrcLayersNames, 3,
    nullptr, 0, nullptr, 0, nullptr, 0,
};

// --- VRC.Core.UnityVersion ----------------------------------------------------
inline const char *const kUnityVersionCompareToParams[] = { "VRC.Core.UnityVersion" };
inline const char *const kUnityVersionParseParams[] = { "System.String" };
inline const char *const kUnityVersionTryParseParams[] = { "System.String", "VRC.Core.UnityVersion&" };
inline const MethodSpec kUnityVersionMethods[] = {
    { "CompareTo", "System.Int32", kUnityVersionCompareToParams, 1, false },
    { "Parse", "VRC.Core.UnityVersion", kUnityVersionParseParams, 1, true },
    { "ToString", "System.String", nullptr, 0, false },
    { "TryParse", "System.Boolean", kUnityVersionTryParseParams, 2, true },
};
inline const TypeSpec kUnityVersion = {
    "UnityVersion", "VRC::Core", "VRCCore-Standalone.dll", "VRC.Core", "UnityVersion",
    false, false, false, true, nullptr, 0,
    nullptr, 0, nullptr, 0,
    kUnityVersionMethods, static_cast<int>(std::size(kUnityVersionMethods)),
};

// --- VRC.Core.Endpoints -------------------------------------------------------
inline const MemberSpec kEndpointFields[] = {
    { "Avatar", "System.String" },
    { "FavoriteList", "System.String" },
    { "Feedback", "System.String" },
    { "File", "System.String" },
    { "Moderation", "System.String" },
    { "Notification", "System.String" },
    { "PlayerModeration", "System.String" },
    { "User", "System.String" },
    { "World", "System.String" },
    { "WorldInstance", "System.String" },
};
inline const TypeSpec kEndpoints = {
    "Endpoints", "VRC::Core", "VRCCore-Standalone.dll", "VRC.Core", "Endpoints",
    false, false, true, false, nullptr, 0,
    kEndpointFields, static_cast<int>(std::size(kEndpointFields)),
    nullptr, 0, nullptr, 0,
};

// --- VRC.Core.Logger ----------------------------------------------------------
inline const char *const kAssignLoggerProviderParams[] = { "VRC.Core.ILoggerReceiver" };
inline const char *const kSetForcePassAllErrorsParams[] = { "System.Boolean" };
inline const char *const kSetForcePassAllWarningsParams[] = { "System.Boolean" };
inline const MethodSpec kLoggerMethods[] = {
    { "AssignLoggerProvider", "System.Void", kAssignLoggerProviderParams, 1, true },
    { "set_ForcePassAllErrors", "System.Void", kSetForcePassAllErrorsParams, 1, true },
    { "set_ForcePassAllWarnings", "System.Void", kSetForcePassAllWarningsParams, 1, true },
};
inline const TypeSpec kLogger = {
    "Logger", "VRC::Core", "VRCCore-Standalone.dll", "VRC.Core", "Logger",
    false, false, true, false, nullptr, 0,
    nullptr, 0, nullptr, 0,
    kLoggerMethods, static_cast<int>(std::size(kLoggerMethods)),
};

// --- VRC.Core.ConfigManager ---------------------------------------------------
inline const char *const kConfigIdChangeParams[] = { "System.String" };
inline const char *const kAssignTestLocalParams[] = { "VRC.Core.Config.Interfaces.IReadWriteConfig" };
inline const char *const kAssignTestRemoteParams[] = { "VRC.Core.Config.Interfaces.IReadOnlyConfig" };
inline const MethodSpec kConfigManagerMethods[] = {
    { "Initialize", "System.Void", nullptr, 0, true },
    { "WorldConfigIDChange", "System.Void", kConfigIdChangeParams, 1, true },
    { "ClientConfigIDChange", "System.Void", kConfigIdChangeParams, 1, true },
    { "AssignTestLocalConfig", "System.Void", kAssignTestLocalParams, 1, true },
    { "AssignTestRemoteConfig", "System.Void", kAssignTestRemoteParams, 1, true },
};
inline const char *const kLogPathStringParams[] = { "System.String" };
inline const char *const kLoggingModeParams[] = { "VRC.Core.LoggingMode" };
inline const MethodSpec kVrcLoggerMethods[] = {
    { "IsLogPathValid", "System.Boolean", kLogPathStringParams, 1, true },
    { "OverrideLogFilePath", "System.Void", kLogPathStringParams, 1, true },
    { "SetLoggingMode", "System.Void", kLoggingModeParams, 1, true },
    { "GetCurrentLogLevel", "Microsoft.Extensions.Logging.LogLevel", nullptr, 0, true },
    { "GetLogger", "Microsoft.Extensions.Logging.ILogger", kLogPathStringParams, 1, true },
    { "RemoveStaleLogs", "System.Void", nullptr, 0, true },
    { "GenerateOutputLogFilename", "System.String", nullptr, 0, true },
    { "GetLogDirectory", "System.String", nullptr, 0, true },
};


// --- VRC.Core.VRCLogger -------------------------------------------------------
inline const MemberSpec kVrcLoggerProperties[] = {
    { "IsPathOverridden", "System.Boolean" },
    { "LogFileNamePrefix", "System.Lazy<System.String>" },
    { "LogFilePath", "System.String" },
    { "GlobalLogger", "Microsoft.Extensions.Logging.ILogger" },
};
inline const TypeSpec kVrcLogger = {
    "VRCLogger", "VRC::Core", "VRC.Logging.dll", "VRC.Core", "VRCLogger",
    false, false, true, false, nullptr, 0,
    nullptr, 0,
    kVrcLoggerProperties, static_cast<int>(std::size(kVrcLoggerProperties)),
    kVrcLoggerMethods, static_cast<int>(std::size(kVrcLoggerMethods)),
};

inline const TypeSpec kConfigManager = {
    "ConfigManager", "VRC::Core", "VRCCore-Standalone.dll", "VRC.Core", "ConfigManager",
    false, false, true, false, nullptr, 0,
    nullptr, 0, nullptr, 0,
    kConfigManagerMethods, static_cast<int>(std::size(kConfigManagerMethods)),
};
} // namespace VrcGenerated

namespace {
std::string VRChatGeneratedSceneDescriptor() {
    return VrcGenerated::EmitType(VrcGenerated::kSceneDescriptor, "sdk/VRChat/VRC/SDKBase/VRC_SceneDescriptor.h");
}
std::string VRChatGeneratedSerialization() {
    return VrcGenerated::EmitType(VrcGenerated::kSerialization, "sdk/VRChat/VRC/SDKBase/VRC_Serialization.h");
}
std::string VRChatGeneratedAvatarPedestal() {
    return VrcGenerated::EmitType(VrcGenerated::kAvatarPedestal, "sdk/VRChat/VRC/SDKBase/VRC_AvatarPedestal.h");
}
std::string VRChatGeneratedSpatialAudio() {
    return VrcGenerated::EmitType(VrcGenerated::kSpatialAudio, "sdk/VRChat/VRC/SDKBase/VRC_SpatialAudioSource.h");
}
std::string VRChatGeneratedStereoObject() {
    return VrcGenerated::EmitType(VrcGenerated::kStereoObject, "sdk/VRChat/VRC/SDKBase/VRC_StereoObject.h");
}
std::string VRChatGeneratedLayers() {
    return VrcGenerated::EmitType(VrcGenerated::kVrcLayers, "sdk/VRChat/VRC/SDKBase/VRCLayers.h");
}
std::string VRChatGeneratedUnityVersion() {
    return VrcGenerated::EmitType(VrcGenerated::kUnityVersion, "sdk/VRChat/VRC/Core/UnityVersion.h");
}
std::string VRChatGeneratedEndpoints() {
    return VrcGenerated::EmitType(VrcGenerated::kEndpoints, "sdk/VRChat/VRC/Core/Endpoints.h");
}
std::string VRChatGeneratedLogger() {
    return VrcGenerated::EmitType(VrcGenerated::kLogger, "sdk/VRChat/VRC/Core/Logger.h");
}
std::string VRChatGeneratedConfigManager() {
    return VrcGenerated::EmitType(VrcGenerated::kConfigManager, "sdk/VRChat/VRC/Core/ConfigManager.h");
}
std::string VRChatGeneratedVrcLogger() {
    return VrcGenerated::EmitType(VrcGenerated::kVrcLogger, "sdk/VRChat/VRC/Core/VRCLogger.h");
}
} // namespace
