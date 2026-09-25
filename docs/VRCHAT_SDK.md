# VRChat SDK wrappers

The URKit++ project generator ships a set of VRChat-specific wrappers that are
generated into every project targeting VRChat. They sit under
`sdk/VRChat/VRC/` and mirror the managed VRChat SDK namespaces.

| Generated header | Managed type |
|---|---|
| `sdk/VRChat/VRC/SDKBase/VRCPlayerAPI.h` | `VRC.SDKBase.VRCPlayerApi` |
| `sdk/VRChat/VRC/SDKBase/Networking.h` | `VRC.SDKBase.Networking` |
| `sdk/VRChat/VRC/Core/APIUser.h` | `VRC.Core.APIUser` |
| `sdk/VRChat/VRC/Localization/LocalizableStringExtensions.h` | `VRC.Localization.LocalizableString` |
| `sdk/VRChat/VRC/Udon/UdonBehaviour.h` | `VRC.Udon.UdonBehaviour` |
| `sdk/VRChat/VRC/SDKBase/VRC_Pickup.h` | `VRC.SDKBase.VRC_Pickup` |
| `sdk/VRChat/VRC/SDK3/Components/VRCPickup.h` | `VRC.SDK3.Components.VRCPickup` |

All of them are `OutputFilePolicy::GeneratedOverwrite`, so they are rewritten by
the generator and **must not be edited by hand**. If you need a change,
edit the `.inl` template under
`src/sdk/templates/VRChat/VRC/` and regenerate, or copy the wrapper into a
mod-owned header instead.

The templates live in the repository at:

```text
src/sdk/templates/VRChat/
  VRC/
    SDKBase/
      VRCPlayerAPI.inl
      Networking.inl
    Core/
      APIUser.inl
    Localization/
      LocalizableStringExtensions.inl
    Udon/
      UdonBehaviour.inl
    SDK3/
      Components/
        VRCPickup.inl
```

## Quick example

```cpp
#include "sdk/VRChat/VRC/SDKBase/VRCPlayerAPI.h"
#include "sdk/VRChat/VRC/SDKBase/Networking.h"
#include "sdk/VRChat/VRC/Core/APIUser.h"

void demo() {
    // Local player and the network statics
    VRC::SDKBase::VRCPlayerApi local = VRC::SDKBase::Networking::LocalPlayer();
    if (local)
        ModLog::info("local player id: %d", local.GetPlayerId());

    // Every player currently in the room
    for (const VRC::SDKBase::VRCPlayerApi& p : VRC::SDKBase::VRCPlayerApi::GetAllPlayers())
        ModLog::info("player: %s", p.GetDisplayName().c_str());

    // The current user's profile
    VRC::Core::APIUser me = VRC::Core::APIUser::GetCurrentUser();
    if (me) {
        ModLog::info("user: %s (%s)", me.DisplayName().c_str(), me.Id().c_str());
        for (const std::string& tag : me.Tags())
            ModLog::info("  tag: %s", tag.c_str());
    }
}
```

Include each header once; headers are self-contained.

## VRCPlayerApi (`VRC::SDKBase::VRCPlayerApi`)

A wrapper around the engine player object. Statics resolve from
`VRC.SDKBase.VRCPlayerApi`:

- `GetPlayerByObject(Unity::GameObject)` — `GetPlayerByGameObject`
- `GetPlayerById(int)` — `GetPlayerById`
- `GetAllPlayers()` — `get_AllPlayers` -> `List<VRCPlayerApi>`

Instance members:

- `islocal()` — the **field** `isLocal`
- `GetDisplayName()` / `SetDisplayName(string)` — the **field** `displayName`
- `GameObject()` — the **field** `gameObject`
- `GetPlayerId()` — the **property** `playerId`
- `GetPlayerCount()`, `IsPlayerGrounded()`, `IsUserInVR()`, `GetVelocity()`,
  `GetPosition()`, `GetRotation()`, `Respawn()`
- Movement/voice tuning: `GetRunSpeed`/`SetRunSpeed`, `SetJumpImpulse`,
  `GetVoiceGain`, `SetVoiceDistanceNear/Far`, `SetAvatarAudio*`
- Language: `GetAvailableLanguages()` -> `std::vector<std::string>`,
  `GetCurrentLanguage()` -> `std::string`
- Ownership: `IsOwner(GameObject)`, `TakeOwnership(GameObject)`
- Avatar scaling: `GetAvatarEyeHeightAsMeters()`,
  `SetAvatarEyeHeightByMeters`, `SetManualAvatarScalingAllowed`, ...

`TakeOwnership`, `TeleportTo`, `Immobilize`, velocity setters, and avatar-eye
setters use `CallExact` with the declared managed parameter types so overload
lookup matches even for wrappers like `Unity::Vector3`.

### Fields vs properties (important)

`VRCPlayerApi` mixes **fields** and **properties** on the managed side:

- `isLocal`, `displayName`, `gameObject` are **fields** — read with
  `GetField`/`SetField`.
- `playerId` is a **property** — read with `GetProperty` (equivalent to
  `Call("get_playerId")`).

Do not assume a `get_*` method exists for every member. If a call reports
`same-arity candidates=0`, the member is likely a field, not a property.

## Networking (`VRC::SDKBase::Networking`)

`Networking` is a managed **namespace**, so the wrapper exposes free functions
(namespace `VRC::SDKBase::Networking`) plus the `kNetworking` TypeRef:

- `LocalPlayer()`, `Master()`, `InstanceOwner()`
- `IsMaster()`, `IsNetworkSettled()`
- `ServerTimeMs()` — `GetServerTimeInMilliseconds`
- `GoToRoom(string)`
- `GetOwner(GameObject)`, `IsOwner(GameObject)`, `IsObjectReady(GameObject)`,
  `UniqueName(GameObject)`
- `SetOwner(VRCPlayerApi, GameObject)`
- `PlayerObjects(VRCPlayerApi)` — `GetPlayerObjects` -> array of
  `Unity::GameObject`

## APIUser (`VRC::Core::APIUser`)

A wrapper around the VRChat user profile type (`VRC.Core.APIUser` in
`VRCCore-Standalone.dll`). This type is **not** obfuscated, so method names are
stable.

### Getting an instance

- `APIUser::GetCurrentUser()` — the static `get_CurrentUser`, returns the local
  user's profile.

(Other users' profiles are reachable through the game's player lookup, e.g.
`VRC.Player`/`PlayerManager` in `Assembly-CSharp.dll`, which is outside these
wrappers because its member names are obfuscated. Use the inspection helpers in
`SDK_HANDBOOK.md` to discover them by type at runtime.)

### Getters

Most members are read as properties (`Call("get_xxx")`) and return
`std::string` or `bool`:

- Identity: `Id()`, `DisplayName()`, `Username()`, `Pronouns()`, `Bio()`,
  `UserIcon()`, `Note()`, `DiscordID()`
- Avatar: `AvatarID()`, `FallbackId()`, `CurrentAvatarImageUrl()`,
  `CurrentAvatarThumbnailImageUrl()`, `CurrentAvatarAssetUrl()`
- Presence/status: `Location()`, `HomeLocation()`, `Status()`,
  `StatusDescription()`, `LastLogin()`, `LastActivity()`, `DateJoined()`,
  `LastPlatform()`, `Platform()`, `LastMobile()`, `IsOnMobile()`
- Profile media: `IconUrl()`, `ThumbnailUrl()`, `ProfilePicImageUrl()`,
  `ProfilePicThumbnailImageUrl()`, `BannerUrl()`, `BannerColor()`,
  `BannerType()`, `IconFrame()`, `ProfileEffect()`, `NameplateEffect()`
- Flags: `IsFriend()`, `IsSelf()`, `IsAdult()`, `IsAgeVerified()`,
  `IsBoopingEnabled()`, `Populated()`
- Content powers: `CanPublishWorlds()`, `CanPublishAvatars()`,
  `CanPublishWorldsAndAvatars()`, `CanPublishAllContent()`, `CanPublishProps()`
- Moderation/trust: `HasModerationPowers()`, `HasNoPowers()`,
  `HasScriptingAccess()`, `HasVIPAccess()`, `HasSuperPowers()`,
  `IsUntrusted()`, `IsEarlyAdopter()`, `IsSupporter()`, `IsCreator()`,
  trust levels `HasBasicTrustLevel()` ... `HasLegendTrustLevel()`,
  `HasNegativeTrustLevel()`, `HasVeryNegativeTrustLevel()`
- Friends: `HasRequestedToBeFriend()`, `HasFriendRequestPending()`
- Status toggles: `CanSetStatusOffline()`, `StatusIsSetToOffline()`,
  `StatusIsSetToJoinMe()`, `StatusIsSetToAskMe()`, `CanSeeAllUsersStatus()`

### Setters

Setters mirror the matching `set_xxx` methods and take one argument, e.g.
`SetDisplayName(string)`, `SetUsername(string)`, `SetBio(string)`,
`SetAvatarID(string)`, `SetIsFriend(bool)`, `SetAllowAvatarCopying(bool)`,
`SetLocation(string)`, `SetPronouns(string)`.

### List getters (List<String>)

Several members return `System.Collections.Generic.List<System.String>`.
The wrapper converts these to `std::vector<std::string>` by calling
`get_Count` and the `List<T>` indexer `get_Item`:

| Wrapper | Managed getter |
|---|---|
| `BioLinks()` | `get_bioLinks` |
| `CurrentAvatarTags()` | `get_currentAvatarTags` |
| `FriendIDs()` | `get_friendIDs` |
| `Tags()` | `get_tags` |
| `StatusHistory()` | `get_statusHistory` |

If a future `APIUser` member returns a `List<T>` of something other than
`string`, it needs its own wrapper or a type-based discovery helper; the
generated code currently only enumerates string lists.

## UdonBehaviour (`VRC::Udon::UdonBehaviour`)

A wrapper around `VRC.Udon.UdonBehaviour` from `VRC.Udon.dll`. The type derives
from `Unity::MonoBehaviour`, so it inherits `gameObject()`, `transform()`,
`GetComponent<T>()` and `enabled()` on top of the members below.

The wrapper also mirrors the three managed enums it takes, with the literal
values dumped from the shipping client:

| C++ enum | Managed enum |
|---|---|
| `SyncType` (`Unknown`, `None`, `Continuous`, `Manual`) | `VRC.SDKBase.Networking.SyncType` |
| `NetworkEventTarget` (`All`, `Owner`, `Others`, `Self`) | `VRC.Udon.Common.Interfaces.NetworkEventTarget` |
| `EventTiming` (`Update`, `LateUpdate`, `PostLateUpdate`, `FixedUpdate`) | `VRC.Udon.Common.Enums.EventTiming` |

All three have a `System.Int32` underlying type, so enum-typed arguments and
returns are passed through directly.

### Fields vs properties

`UdonBehaviour` mixes both, same as `VRCPlayerApi`:

- `Reliable`, `SynchronizePosition`, `AllowCollisionOwnershipTransfer` and
  `publicVariables` are **fields** — read with `GetField`/`SetField`.
- `SyncMethod`, `IsInteractive`, `HasDoneStart`, `ProgramId`, ... are
  **properties** — read with `GetProperty`, written with the matching
  `set_*` call.

`PublicVariables()` returns the live `IUdonVariableTable` as a generic
`Unity::Object`; per-variable access goes through the program-variable members.

### Custom events

- `SendCustomEvent(string)`
- `SendCustomEventDelayedFrames(string, int, EventTiming = Update)`
- `SendCustomEventDelayedSeconds(string, float, EventTiming = Update)`
- `SendCustomNetworkEvent(NetworkEventTarget, string[, object x0..x2])` — the
  `System.Object` parameters are passed as opaque managed pointers, so a value
  type has to be boxed by the caller.
- `RequestSerialization()`, `SerializePublicVariables()`,
  `DeserializePublicVariables()`

### Program variables

- `RunProgram(string)` / `RunProgram(uint32)`
- `GetProgramVariable(string)` -> boxed `Unity::Object`, or null when the symbol
  is missing. It is routed through `TryGetProgramVariable` on purpose: the
  managed type also declares `GetProgramVariable<T>(string)`, and the two share
  an identical parameter list, so an exact lookup on the non-generic overload is
  reported as ambiguous by `find_method_exact`.
- `SetProgramVariable(string, void *)` — a Unity object can be passed directly
- `TryGetProgramVariable(string, void **)` — the out-parameter form
- `GetProgramVariableType(string)` -> `Unity::TypeObject`

### Interaction entry points

`Interact()`, `OnPickup()`, `OnPickupUseDown()`, `OnPickupUseUp()`,
`OnDrop()` and `OnNetworkReady()` are the managed entry points VRChat invokes,
wrapped so a mod can trigger them directly.

## VRCPickup (`VRC::SDK3::Components::VRCPickup`)

`VRC.SDK3.Components.VRCPickup` (`VRCSDK3.dll`) is a thin subclass. The entire
pickup API lives on its base `VRC.SDKBase.VRC_Pickup` (`VRCSDKBase.dll`), so both
are wrapped and the SDK3 component derives from the base:

```
VRC::SDKBase::VrcPickup                 // VRC.SDKBase.VRC_Pickup
  ^-- VRC::SDK3::Components::VRCPickup  // + the `version` field
```

`VRCPickup` adds only `version`; everything below is inherited. The managed
chain continues `VRC_Pickup -> VRCNetworkBehaviour -> VRC_Interactable ->
MonoBehaviour`, flattened to `Unity::MonoBehaviour` because method lookup walks
the live class rather than the C++ base.

### Mirrored enums

| C++ enum | Managed enum |
|---|---|
| `PickupOrientation` (`Any`, `Grip`, `Gun`) | `VRC_Pickup.PickupOrientation` |
| `AutoHoldMode` (`AutoDetect`, `Sometimes`, `No`, `Yes`) | `VRC_Pickup.AutoHoldMode` |
| `PickupHand` (`None`, `Left`, `Right`) | `VRC_Pickup.PickupHand` |
| `VrcBroadcastType` (10 members) | `VRC_EventHandler.VrcBroadcastType` |
| `VRCPickup::Version` (`Version_1_0`, `Version_1_1`) | `VRCPickup.Version` |
| `Unity::ForceMode` (`Force`, `Impulse`, `VelocityChange`, `Acceleration`) | `UnityEngine.ForceMode` |

`ForceMode` lives in the Unity layer, not here — its literals are **not**
contiguous (`Acceleration` is `5`), so it is spelled out explicitly.

### Usage

```cpp
#include "sdk/VRChat/VRC/SDK3/Components/VRCPickup.h"

if (Unity::GameObject go = Unity::GameObject::Find( "Prop" ))
    if (VRC::SDK3::Components::VRCPickup pickup = go.GetComponent<VRC::SDK3::Components::VRCPickup>() ) {
        ModLog::info( "held=%d hand=%d text=%s", pickup.IsHeld(), pickup.currentHand(),
                      pickup.InteractionText().c_str() );

        if ( pickup.IsHeld() )
            pickup.Drop();
    }
```

### Fields vs properties

The same split as `VRCPlayerApi`:

- Fields (`GetField`/`SetField`): `MomentumTransferMethod`, `DisallowTheft`,
  `orientation`, `AutoHold`, `InteractionText`, `UseText`, `useEventBroadcastType`,
  `pickupDropEventBroadcastType`, `UseDownEventName`, `UseUpEventName`,
  `PickupEventName`, `DropEventName`, `ThrowVelocityBoost*`, `currentlyHeldBy`,
  `currentLocalPlayer`, `pickupable`, `proximity`, `allowManipulationWhenEquipped`
- Properties (`GetProperty`): `IsHeld`, `currentHand`, `currentPlayer`, `Proximity`

`version` on the SDK3 component is a **field**, not a property. The decompiled
source renders it with get/set accessors, but live metadata exposes a field and
no `set_version` method, so it is accessed with `GetField`/`SetField`.

### Methods

- `Drop()`, `Drop(VRCPlayerApi instigator)`
- `GenerateHapticEvent(duration = 0.25f, amplitude = 0.5f, frequency = 0.5f)`
- `PlayHaptics()`
- `VrcPickup::IsGlobalAutoHoldPickup(AutoHoldMode, PickupOrientation)` — static;
  reports whether a global setting overrides the per-pickup value
- `IsGlobalAutoHoldPickup()` — the instance overload of the same question

Unity lifecycle methods (`Awake`, `Reset`, `OnDestroy`) and `ProvideEvents()`
are not wrapped. `VrcPickup::OnAwake` / `ForceDrop` / `OnDestroyed` /
`HapticEvent` static delegates are also unwrapped: their managed parameter is a
closed generic delegate, which exact overload lookup cannot spell.

## Codegen conventions

Generated VRChat headers follow a few rules:

- Wrappers derive from `Unity::Object`, so they are cheap handles and are
  null-testable (`if (player)`).
- `try`/`null` result checks are done with `if (!wrapper)`; a failed invoke
  leaves `last_error()` populated (`Unity::last_error()`).
- Managed generic lists have no dedicated wrapper type; the live handle is held
  as a generic `Unity::Object` and walked with `Call`.
- When adding a new VRChat template, register it in all three places:
  `src/sdk/templates/mod_project_generator_vrchat.inl`, the `writes` list in
  `src/sdk/mod_project_generator_common.cpp`, and the
  `URK_SDK_TEMPLATE_FILES` list in `cmake/URKitSources.cmake`.

## Local reference corpus

A full decompile of the VRChat client is available on this machine at
`C:\Users\Biscuit\Desktop\VRCDecompile` (`VRChat/`, `VRCCore/`, `VRCSDK2/`,
`Unity/`, `Photon/`, `SteamVR/`). Treat it as a **read-only** reference — do not
edit, reformat, or regenerate anything in it.

Prefer it over runtime probing when wrapping a managed type. The `unity-runtime-explorer`
MCP is bounded and lossy in ways that matter here:

- Some enum literals are stripped from IL2CPP metadata, so `inspect_type` returns
  `metadata_unavailable` and the members cannot be read from the live game at
  all. `VRC.SDKBase.Networking.SyncType` is one of these.
- `inspect_type` results can exceed the bridge message limit, forcing many
  narrow queries for one type.
- Accessibility (`public` vs `internal`) is not surfaced, so it is impossible to
  tell a supported entry point from an internal one.

Cross-check anything version-sensitive against live metadata: a decompile is a
snapshot of one build and will lag behind the installed client.