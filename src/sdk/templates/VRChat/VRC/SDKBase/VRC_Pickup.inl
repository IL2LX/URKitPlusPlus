std::string VRChatVrcPickupModule() {
    return R"URKVRCPICKUPBASE(#pragma once

#include "sdk/unity/unity.h"
#include "sdk/VRChat/VRC/SDKBase/VRCPlayerAPI.h"

#include <string_view>

namespace VRC::SDKBase
{
    enum class PickupOrientation : int { Any = 0, Grip = 1, Gun = 2 };

    enum class AutoHoldMode : int { AutoDetect = 0, Sometimes = 1, No = 2, Yes = 3 };

    enum class PickupHand : int { None = 0, Left = 1, Right = 2 };

    enum class VrcBroadcastType : int {
        Always = 0,
        Master = 1,
        Local = 2,
        Owner = 3,
        AlwaysUnbuffered = 4,
        MasterUnbuffered = 5,
        OwnerUnbuffered = 6,
        AlwaysBufferOne = 7,
        MasterBufferOne = 8,
        OwnerBufferOne = 9
    };

    inline constexpr Unity::TypeRef kVrcPickup{"VRCSDKBase.dll", "VRC.SDKBase", "VRC_Pickup"};

    struct VrcPickup : Unity::MonoBehaviour {
        VrcPickup() = default;
        explicit VrcPickup(void* handle) : Unity::MonoBehaviour(handle) {
        }

        static constexpr Unity::TypeRef unity_type() {
            return kVrcPickup;
        }

        // --- Fields

        Unity::GameObject GameObject() const {
            return GetProperty<Unity::GameObject>("gameObject");
        }

        std::string GameObjectName() const {
            const Unity::GameObject go = GameObject();
            return go ? go.name() : std::string{};
        }

        Unity::ForceMode MomentumTransferMethod() const {
            return GetField<Unity::ForceMode>("MomentumTransferMethod");
        }

        void SetMomentumTransferMethod(Unity::ForceMode value) const {
            SetField<Unity::ForceMode>("MomentumTransferMethod", value);
        }

        bool DisallowTheft() const {
            return GetField<bool>("DisallowTheft");
        }

        void SetDisallowTheft(bool value) const {
            SetField<bool>("DisallowTheft", value);
        }

        bool allowManipulationWhenEquipped() const {
            return GetField<bool>("allowManipulationWhenEquipped");
        }

        void SetAllowManipulationWhenEquipped(bool value) const {
            SetField<bool>("allowManipulationWhenEquipped", value);
        }

        bool pickupable() const {
            return GetField<bool>("pickupable");
        }

        void SetPickupable(bool value) const {
            SetField<bool>("pickupable", value);
        }

        Unity::Transform ExactGrip() const {
            return GetField<Unity::Transform>("ExactGrip");
        }

        Unity::Transform ExactGun() const {
            return GetField<Unity::Transform>("ExactGun");
        }

        PickupOrientation Orientation() const {
            return GetField<PickupOrientation>("orientation");
        }

        void SetOrientation(PickupOrientation value) const {
            SetField<PickupOrientation>("orientation", value);
        }

        AutoHoldMode AutoHold() const {
            return GetField<AutoHoldMode>("AutoHold");
        }

        void SetAutoHold(AutoHoldMode value) const {
            SetField<AutoHoldMode>("AutoHold", value);
        }

        std::string InteractionText() const {
            return GetField<std::string>("InteractionText");
        }

        void SetInteractionText(std::string_view value) const {
            SetField<std::string>("InteractionText", std::string{value});
        }

        std::string UseText() const {
            return GetField<std::string>("UseText");
        }

        void SetUseText(std::string_view value) const {
            SetField<std::string>("UseText", std::string{value});
        }

        VrcBroadcastType UseEventBroadcastType() const {
            return GetField<VrcBroadcastType>("useEventBroadcastType");
        }

        void SetUseEventBroadcastType(VrcBroadcastType value) const {
            SetField<VrcBroadcastType>("useEventBroadcastType", value);
        }

        VrcBroadcastType PickupDropEventBroadcastType() const {
            return GetField<VrcBroadcastType>("pickupDropEventBroadcastType");
        }

        void SetPickupDropEventBroadcastType(VrcBroadcastType value) const {
            SetField<VrcBroadcastType>("pickupDropEventBroadcastType", value);
        }

        std::string UseDownEventName() const {
            return GetField<std::string>("UseDownEventName");
        }

        void SetUseDownEventName(std::string_view value) const {
            SetField<std::string>("UseDownEventName", std::string{value});
        }

        std::string UseUpEventName() const {
            return GetField<std::string>("UseUpEventName");
        }

        void SetUseUpEventName(std::string_view value) const {
            SetField<std::string>("UseUpEventName", std::string{value});
        }

        std::string PickupEventName() const {
            return GetField<std::string>("PickupEventName");
        }

        void SetPickupEventName(std::string_view value) const {
            SetField<std::string>("PickupEventName", std::string{value});
        }

        std::string DropEventName() const {
            return GetField<std::string>("DropEventName");
        }

        void SetDropEventName(std::string_view value) const {
            SetField<std::string>("DropEventName", std::string{value});
        }

        float ThrowVelocityBoostMinSpeed() const {
            return GetField<float>("ThrowVelocityBoostMinSpeed");
        }

        void SetThrowVelocityBoostMinSpeed(float value) const {
            SetField<float>("ThrowVelocityBoostMinSpeed", value);
        }

        float ThrowVelocityBoostScale() const {
            return GetField<float>("ThrowVelocityBoostScale");
        }

        void SetThrowVelocityBoostScale(float value) const {
            SetField<float>("ThrowVelocityBoostScale", value);
        }

        /* The local player's hand transform while held, otherwise null. */
        Unity::Component CurrentlyHeldBy() const {
            return GetField<Unity::Component>("currentlyHeldBy");
        }

        VRCPlayerApi CurrentLocalPlayer() const {
            return GetField<VRCPlayerApi>("currentLocalPlayer");
        }

        // --- Properties

        bool IsHeld() const {
            return GetProperty<bool>("IsHeld");
        }

        PickupHand CurrentHand() const {
            return GetProperty<PickupHand>("currentHand");
        }

        VRCPlayerApi CurrentPlayer() const {
            return GetProperty<VRCPlayerApi>("currentPlayer");
        }

        float Proximity() const {
            return GetProperty<float>("Proximity");
        }

        // --- Methods

        void Drop() const {
            CallExact<void>("Drop", {});
        }

        void Drop(VRCPlayerApi instigator) const {
            CallExact<void>("Drop", { "VRC.SDKBase.VRCPlayerApi" }, instigator);
        }

        void GenerateHapticEvent(float duration = 0.25f, float amplitude = 0.5f, float frequency = 0.5f) const {
            CallExact<void>("GenerateHapticEvent", { "System.Single", "System.Single", "System.Single" }, duration, amplitude, frequency);
        }

        void PlayHaptics() const {
            CallExact<void>("PlayHaptics", {});
        }

        static bool IsGlobalAutoHoldPickup(AutoHoldMode autoHold, PickupOrientation orientation) {
            return URK::Unity::detail::InvokeStatic<bool>(kVrcPickup, "IsGlobalAutoHoldPickup", autoHold, orientation);
        }

        bool IsGlobalAutoHoldPickup() const {
            return Call<bool>("IsGlobalAutoHoldPickup");
        }
    };
} // namespace VRC::SDKBase
)URKVRCPICKUPBASE";
}
