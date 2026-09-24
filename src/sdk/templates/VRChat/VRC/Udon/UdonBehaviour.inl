std::string VRChatUdonBehaviourModule() {
    return R"URKUDONBEHAVIOUR(#pragma once

#include "sdk/unity/unity.h"

#include <cstdint>
#include <string_view>

namespace VRC::Udon
{
    enum class SyncType : std::int32_t {
        None = 0,
        Any = 1,
        Continuous = 2,
        Manual = 3,
        NoVariableSync = 4,
    };

    enum class NetworkEventTarget : std::int32_t {
        All = 0,
        Owner = 1,
        Others = 2,
        Self = 3,
    };

    enum class EventTiming : std::int32_t {
        Update = 0,
        LateUpdate = 1,
        PostLateUpdate = 2,
        FixedUpdate = 3,
    };

    inline constexpr Unity::TypeRef kUdonBehaviour{ "VRC.Udon.dll", "VRC.Udon", "UdonBehaviour" };

    struct UdonBehaviour : Unity::MonoBehaviour {
        UdonBehaviour() = default;
        explicit UdonBehaviour(void* handle) : Unity::MonoBehaviour(handle) {
        }

        static constexpr Unity::TypeRef unity_type() {
            return kUdonBehaviour;
        }

        // --- Fields

        bool Reliable() const {
            return GetField<bool>("Reliable");
        }

        void SetReliable(bool value) const {
            SetField<bool>("Reliable", value);
        }

        bool SynchronizePosition() const {
            return GetField<bool>("SynchronizePosition");
        }

        void SetSynchronizePosition(bool value) const {
            SetField<bool>("SynchronizePosition", value);
        }

        bool AllowCollisionOwnershipTransfer() const {
            return GetField<bool>("AllowCollisionOwnershipTransfer");
        }

        void SetAllowCollisionOwnershipTransfer(bool value) const {
            SetField<bool>("AllowCollisionOwnershipTransfer", value);
        }

        Unity::Object PublicVariables() const {
            return GetField<Unity::Object>("publicVariables");
        }

        // --- Properties

        SyncType SyncMethod() const {
            return GetProperty<SyncType>("SyncMethod");
        }

        void SetSyncMethod(SyncType value) const {
            CallExact<void>("set_SyncMethod", { "VRC.SDKBase.Networking.SyncType" }, value);
        }

        bool SyncIsContinuous() const {
            return GetProperty<bool>("SyncIsContinuous");
        }

        bool SyncIsManual() const {
            return GetProperty<bool>("SyncIsManual");
        }

        bool IsNetworkingSupported() const {
            return GetProperty<bool>("IsNetworkingSupported");
        }

        void SetIsNetworkingSupported(bool value) const {
            CallExact<void>("set_IsNetworkingSupported", { "System.Boolean" }, value);
        }

        bool IsInitialized() const {
            return GetProperty<bool>("IsInitialized");
        }

        bool HasDoneStart() const {
            return GetProperty<bool>("HasDoneStart");
        }

        bool HasError() const {
            return GetProperty<bool>("HasError");
        }

        bool IsInteractive() const {
            return GetProperty<bool>("IsInteractive");
        }

        std::string InteractionText() const {
            return GetProperty<std::string>("InteractionText");
        }

        void SetInteractionText(std::string_view value) const {
            CallExact<void>("set_InteractionText", { "System.String" }, value);
        }

        bool DisableInteractive() const {
            return GetProperty<bool>("DisableInteractive");
        }

        void SetDisableInteractive(bool value) const {
            CallExact<void>("set_DisableInteractive", { "System.Boolean" }, value);
        }

        bool DisableEventProcessing() const {
            return GetProperty<bool>("DisableEventProcessing");
        }

        void SetDisableEventProcessing(bool value) const {
            CallExact<void>("set_DisableEventProcessing", { "System.Boolean" }, value);
        }

        std::int32_t ProgramId() const {
            return GetProperty<std::int32_t>("ProgramId");
        }

        std::uint64_t ProgramSize() const {
            return GetProperty<std::uint64_t>("ProgramSize");
        }

        std::int32_t UpdateOrder() const {
            return GetProperty<std::int32_t>("UpdateOrder");
        }

        Unity::Object OnInit() const {
            return GetProperty<Unity::Object>("OnInit");
        }

        Unity::Object RequestSerializationHook() const {
            return GetProperty<Unity::Object>("RequestSerializationHook");
        }

        // --- Custom events

        void SendCustomEvent(std::string_view eventName) const {
            CallExact<void>("SendCustomEvent", { "System.String" }, eventName);
        }

        void SendCustomEventDelayedFrames(std::string_view eventName, int delayFrames, EventTiming timing = EventTiming::Update) const {
            CallExact<void>("SendCustomEventDelayedFrames", { "System.String", "System.Int32", "VRC.Udon.Common.Enums.EventTiming" }, eventName, delayFrames, timing);
        }

        void SendCustomEventDelayedSeconds(std::string_view eventName, float delaySeconds, EventTiming timing = EventTiming::Update) const {
            CallExact<void>("SendCustomEventDelayedSeconds", { "System.String", "System.Single", "VRC.Udon.Common.Enums.EventTiming" }, eventName, delaySeconds, timing);
        }

        void SendCustomNetworkEvent(NetworkEventTarget target, std::string_view eventName) const {
            CallExact<void>("SendCustomNetworkEvent", { "VRC.Udon.Common.Interfaces.NetworkEventTarget", "System.String" }, target, eventName);
        }

        void SendCustomNetworkEvent(NetworkEventTarget target, std::string_view eventName, void* parameter0) const {
            CallExact<void>("SendCustomNetworkEvent", { "VRC.Udon.Common.Interfaces.NetworkEventTarget", "System.String", "System.Object" }, target, eventName, parameter0);
        }

        void SendCustomNetworkEvent(NetworkEventTarget target, std::string_view eventName, void* parameter0,
            void* parameter1) const {
            CallExact<void>("SendCustomNetworkEvent", { "VRC.Udon.Common.Interfaces.NetworkEventTarget", "System.String", "System.Object", "System.Object" }, target, eventName, parameter0, parameter1);
        }

        void SendCustomNetworkEvent(NetworkEventTarget target, std::string_view eventName, void* parameter0, void* parameter1, void* parameter2) const {
            CallExact<void>("SendCustomNetworkEvent", { "VRC.Udon.Common.Interfaces.NetworkEventTarget", "System.String", "System.Object", "System.Object", "System.Object" }, target, eventName, parameter0, parameter1, parameter2);
        }

        void RequestSerialization() const {
            CallExact<void>("RequestSerialization", {});
        }

        void SerializePublicVariables() const {
            CallExact<void>("SerializePublicVariables", {});
        }

        void DeserializePublicVariables() const {
            CallExact<void>("DeserializePublicVariables", {});
        }

        // --- Program access

        void RunProgram(std::string_view eventName) const {
            CallExact<void>("RunProgram", { "System.String" }, eventName);
        }

        void RunProgram(std::uint32_t entryPoint) const {
            CallExact<void>("RunProgram", { "System.UInt32" }, entryPoint);
        }

        bool TryGetProgramVariable(std::string_view symbolName, void** value) const {
            return CallExact<bool>("TryGetProgramVariable", { "System.String", "System.Object&" }, symbolName, value);
        }

        Unity::Object GetProgramVariable(std::string_view symbolName) const {
            void* value = nullptr;
            TryGetProgramVariable(symbolName, &value);
            return Unity::Object{ value };
        }

        void SetProgramVariable(std::string_view symbolName, void* value) const {
            CallExact<void>("SetProgramVariable", { "System.String", "System.Object" }, symbolName, value);
        }

        Unity::TypeObject GetProgramVariableType(std::string_view symbolName) const {
            return Unity::TypeObject{CallExact<Unity::Object>("GetProgramVariableType", {"System.String"}, symbolName).handle() };
        }

        // --- Interaction entry points

        void Interact() const {
            CallExact<void>("Interact", {});
        }

        void OnPickup() const {
            CallExact<void>("OnPickup", {});
        }

        void OnPickupUseDown() const {
            CallExact<void>("OnPickupUseDown", {});
        }

        void OnPickupUseUp() const {
            CallExact<void>("OnPickupUseUp", {});
        }

        void OnDrop() const {
            CallExact<void>("OnDrop", {});
        }

        void OnNetworkReady() const {
            CallExact<void>("OnNetworkReady", {});
        }
    };
} // namespace VRC::Udon
)URKUDONBEHAVIOUR";
}
