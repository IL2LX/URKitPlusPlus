std::string VRChatNetworkingModule() {
    return R"URKVRCNETWORKING(#pragma once

#include "sdk/vrchat/SDKBase/VRCPlayerAPI.h"

#include <string>
#include <vector>

namespace VRC::SDKBase {

inline constexpr Unity::TypeRef kNetworking{"VRCSDKBase.dll", "VRC.SDKBase", "Networking"};

namespace Networking {

inline constexpr Unity::TypeRef T{"VRCSDKBase.dll", "VRC.SDKBase", "Networking"};
namespace d = URK::Unity::detail;

// no arguments
inline VRCPlayerApi LocalPlayer() { return d::InvokeStatic<VRCPlayerApi>(T, "get_LocalPlayer"); }
inline VRCPlayerApi Master() { return d::InvokeStatic<VRCPlayerApi>(T, "get_Master"); }
inline VRCPlayerApi InstanceOwner() { return d::InvokeStatic<VRCPlayerApi>(T, "get_InstanceOwner"); }
inline bool IsMaster() { return d::InvokeStatic<bool>(T, "get_IsMaster"); }
inline bool IsNetworkSettled() { return d::InvokeStatic<bool>(T, "get_IsNetworkSettled"); }
inline int ServerTimeMs() { return d::InvokeStatic<int>(T, "GetServerTimeInMilliseconds"); }

// one GameObject
inline VRCPlayerApi GetOwner(Unity::GameObject o) { return d::InvokeStatic<VRCPlayerApi>(T, "GetOwner", o); }
inline bool IsOwner(Unity::GameObject o) { return d::InvokeStatic<bool>(T, "IsOwner", o); }
inline bool IsObjectReady(Unity::GameObject o) { return d::InvokeStatic<bool>(T, "IsObjectReady", o); }
inline std::string UniqueName(Unity::GameObject o) { return d::InvokeStatic<std::string>(T, "GetUniqueName", o); }

inline void SetOwner(VRCPlayerApi p, Unity::GameObject o) {
    d::InvokeStatic<void>(T, "SetOwner", p, o);
}

inline std::vector<Unity::GameObject> PlayerObjects(VRCPlayerApi p) {
    return d::StaticArrayCall<Unity::GameObject>(T, "GetPlayerObjects", p);
}
}

}
)URKVRCNETWORKING";
}