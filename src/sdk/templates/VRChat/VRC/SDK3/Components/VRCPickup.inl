std::string VRChatVrcPickupSdk3Module() {
    return R"URKVRCPICKUP(#pragma once

#include "sdk/VRChat/VRC/SDKBase/VRC_Pickup.h"

namespace VRC::SDK3::Components
{
    inline constexpr Unity::TypeRef kVrcPickupSdk3{ "VRCSDK3.dll", "VRC.SDK3.Components", "VRCPickup" };

    struct VRCPickup : VRC::SDKBase::VrcPickup {
        VRCPickup() = default;
        explicit VRCPickup(void* handle) : VRC::SDKBase::VrcPickup(handle) {
        }

        enum class Version : int { Version_1_0 = 0, Version_1_1 = 1 };

        static constexpr Unity::TypeRef unity_type() {
            return kVrcPickupSdk3;
        }

        Version version() const {
            return GetField<Version>("version");
        }

        void SetVersion(Version value) const {
            SetField<Version>("version", value);
        }
    };
} // namespace VRC::SDK3::Components
)URKVRCPICKUP";
}
