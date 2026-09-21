std::string VRChatPlayerApiModule() {
    return R"URKVRCPLAYERAPI(#pragma once

#include "sdk/unity/unity.h"

namespace VRC::SDKBase {

struct VRCPlayerApi : Unity::Object {
    using Unity::Object::Object;
    static constexpr Unity::TypeRef unity_type() {
        return {"VRCSDKBase.dll", "VRC.SDKBase", "VRCPlayerApi"};
    }
};

} // namespace VRC::SDKBase
)URKVRCPLAYERAPI";
}