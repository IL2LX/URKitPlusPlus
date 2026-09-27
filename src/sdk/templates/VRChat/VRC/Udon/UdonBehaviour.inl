std::string VRChatUdonBehaviourModule() {
    return R"URKUDONBEHAVIOUR(#pragma once

#include "sdk/unity/unity.h"
#include "sdk/unity/unity_inspect.h"

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

        // UdonBehaviour exposes no GetPublicVariableNames, so the names come from
        // the variable table's own VariableSymbols collection and each entry is
        // read back through the table's accessors.
        //
        // The names are deliberately NOT taken from the dictionary internals
        // behind IUdonVariableTable. Walking _publicVariables._entries needs the
        // runtime's array_ref_at export, and without it the whole listing came
        // back empty even though the behaviour plainly had variables. The public
        // path only needs object_get_class, which is always present.
        // value is the boxed System.Object the variable holds, which on its own
        // is opaque: a System.Single arrives as a boxed float with nothing to
        // read off it. value_info is that value resolved against the variable's
        // declared type, so a string arrives as its text, an array as its
        // length, and a numeric or bool as the number itself.
        struct ProgramVariable {
            std::string name;
            std::string type_name;
            Unity::Object value;
            URK::Unity::Inspect::ValueInfo value_info;
        };

        std::vector<ProgramVariable> ListProgramVariables() const {
            std::vector<ProgramVariable> out;

            const Unity::Object table = GetField<Unity::Object>("publicVariables");
            if (!table) {
                URK::Unity::detail::set_error("UdonBehaviour has no publicVariables table");
                return out;
            }

            const Unity::Object symbols = table.GetProperty<Unity::Object>("VariableSymbols");
            if (!symbols) {
                URK::Unity::detail::set_error("UdonBehaviour variable table has no VariableSymbols collection");
                return out;
            }

            // VariableSymbols is an IReadOnlyCollection<string>. There is no
            // managed indexer reachable without the generic interface, so it is
            // walked as an enumerator, which needs no type arguments.
            Unity::Object enumerator = symbols.Call<Unity::Object>("GetEnumerator");
            if (!enumerator) {
                URK::Unity::detail::set_error("UdonBehaviour VariableSymbols has no enumerator");
                return out;
            }

            for (int guard = 0; guard < 4096; ++guard) {
                if (!enumerator.Call<bool>("MoveNext")) break;

                const Unity::Object current = enumerator.GetProperty<Unity::Object>("Current");
                if (!current) continue;
                const std::string name = URK::Unity::detail::managed_string_to_utf8(current.handle());
                if (name.empty()) continue;

                ProgramVariable entry;
                entry.name = name;

                // The table's own accessors, so the declared type and the value
                // come from the same place the runtime reads them.
                void *typeHandle = nullptr;
                if (table.CallExact<bool>("TryGetVariableType", { "System.String", "System.Type&" },
                                         name, &typeHandle)) {
                    const Unity::Object type{ typeHandle };
                    if (type) entry.type_name = type.GetProperty<std::string>("FullName");
                }

                void *valueHandle = nullptr;
                if (table.CallExact<bool>("TryGetVariableValue", { "System.String", "System.Object&" },
                                         name, &valueHandle)) {
                    entry.value = Unity::Object{ valueHandle };
                }

                // Resolve the boxed object against the declared type. This is the
                // same path a method return value takes, so a numeric or bool
                // variable comes back unboxed and a string comes back as its
                // text instead of a bare pointer.
                if (valueHandle && typeHandle) {
                    entry.value_info =
                        URK::Unity::Inspect::invoke_result_value(entry.type_name, typeHandle, valueHandle,
                                                                 "UdonBehaviour::ListProgramVariables");
                } else {
                    URK::Unity::Inspect::ValueInfo none{};
                    none.type_name = entry.type_name;
                    none.kind = URK::Unity::Inspect::ValueKind::Null;
                    none.display = valueHandle ? std::string("<type unavailable>") : std::string("null");
                    entry.value_info = none;
                }

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
