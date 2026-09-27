std::string VRChatUdonBehaviourModule() {
    return R"URKUDONBEHAVIOUR(#pragma once

#include "sdk/unity/unity.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace VRC::Udon
{
    enum class SyncType : std::int32_t {
        Unknown = 0,
        None = 1,
        Continuous = 2,
        Manual = 3,
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

        // A missing symbol and a present-but-null variable both yield a falsy
        // Object, so the two are told apart through Unity::last_error() rather
        // than left for the caller to guess at.
        Unity::Object GetProgramVariable(std::string_view symbolName) const {
            void* value = nullptr;
            if (!TryGetProgramVariable(symbolName, &value)) {
                URK::Unity::detail::set_error(std::string("UdonBehaviour has no program variable named ") +
                                             std::string(symbolName));
                return Unity::Object{ nullptr };
            }
            return Unity::Object{ value };
        }

        // There is no managed API that lists a behaviour's variables:
        // GetProgramVariable needs a name the caller already has, and
        // UdonBehaviour exposes no GetPublicVariableNames. The names are the keys
        // of the dictionary behind publicVariables, so they are read directly:
        //
        //   publicVariables  IUdonVariableTable
        //     _publicVariables  Dictionary<string, IUdonVariable>
        //       _entries  Dictionary.Entry<string, IUdonVariable>[]
        //         key / value
        //           <Value>k__BackingField
        //
        // _keys would need the collection enumerator called, which is more
        // fragile than reading the entry array, and _count alone is not enough
        // because deleted slots are skipped.
        struct ProgramVariable {
            std::string name;
            std::string type_name;
            Unity::Object value;
        };

        std::vector<ProgramVariable> ListProgramVariables() const {
            std::vector<ProgramVariable> out;

            const Unity::Object table = GetField<Unity::Object>("publicVariables");
            if (!table) {
                URK::Unity::detail::set_error("UdonBehaviour has no publicVariables table");
                return out;
            }

            const Unity::Object map = table.GetField<Unity::Object>("_publicVariables");
            if (!map) {
                URK::Unity::detail::set_error("UdonBehaviour publicVariables has no _publicVariables dictionary");
                return out;
            }

            void* entries = map.GetField<void*>("_entries");
            if (!entries) {
                URK::Unity::detail::set_error("UdonBehaviour variable dictionary has no _entries array");
                return out;
            }

            const auto array = URK::Unity::detail::RootedObjectArray<Unity::Object>::from_managed_array(
                entries, "UdonBehaviour::ListProgramVariables");
            if (!array) return out;

            for (const Unity::Object &slot : array) {
                if (!slot) continue; // a removed entry leaves a null slot behind

                const Unity::Object variable = slot.GetField<Unity::Object>("value");
                if (!variable) continue;

                ProgramVariable entry;
                // The dictionary key is authoritative: SymbolName is the same
                // string but reading a property can invoke user code.
                entry.name = slot.GetField<std::string>("key");
                if (entry.name.empty())
                    entry.name = variable.GetProperty<std::string>("SymbolName");
                if (entry.name.empty()) continue;

                const Unity::Object declared = variable.GetProperty<Unity::Object>("DeclaredType");
                if (declared) entry.type_name = declared.GetProperty<std::string>("FullName");

                entry.value = variable.GetProperty<Unity::Object>("Value");
                out.push_back(std::move(entry));
            }
            return out;
        }

        std::vector<std::string> ListProgramVariableNames() const {
            std::vector<std::string> names;
            for (const ProgramVariable &variable : ListProgramVariables())
                names.push_back(variable.name);
            return names;
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
