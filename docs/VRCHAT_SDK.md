# VRChat SDK wrappers

The URKit++ project generator ships a set of VRChat-specific wrappers that are
generated into every project targeting VRChat. Most sit under `sdk/VRChat/VRC/`
and mirror the managed VRChat SDK namespaces; two more sit directly under
`sdk/VRChat/`.

| Generated header | Managed type |
|---|---|
| `sdk/VRChat/VRC/SDKBase/VRCPlayerAPI.h` | `VRC.SDKBase.VRCPlayerApi` |
| `sdk/VRChat/VRC/SDKBase/Networking.h` | `VRC.SDKBase.Networking` |
| `sdk/VRChat/VRC/Core/APIUser.h` | `VRC.Core.APIUser` |
| `sdk/VRChat/VRC/Localization/LocalizableStringExtensions.h` | `VRC.Localization.LocalizableString` |
| `sdk/VRChat/VRC/Udon/UdonBehaviour.h` | `VRC.Udon.UdonBehaviour` |
| `sdk/VRChat/VRC/SDKBase/VRC_Pickup.h` | `VRC.SDKBase.VRC_Pickup` |
| `sdk/VRChat/VRC/SDK3/Components/VRCPickup.h` | `VRC.SDK3.Components.VRCPickup` |
| `sdk/VRChat/HighlightsFX.h` | `HighlightsFX` |
| `sdk/VRChat/Menus.h` | *(mod-side helper, no managed type)* |
| `sdk/VRChat/VRC/SDKBase/VRC_SceneDescriptor.h` | `VRC.SDKBase.VRC_SceneDescriptor` |
| `sdk/VRChat/VRC/SDKBase/VRC_Serialization.h` | `VRC.SDKBase.VRC_Serialization` |
| `sdk/VRChat/VRC/SDKBase/VRC_AvatarPedestal.h` | `VRC.SDKBase.VRC_AvatarPedestal` |
| `sdk/VRChat/VRC/SDKBase/VRC_SpatialAudioSource.h` | `VRC.SDKBase.VRC_SpatialAudioSource` |
| `sdk/VRChat/VRC/SDKBase/VRC_StereoObject.h` | `VRC.SDKBase.VRC_StereoObject` |
| `sdk/VRChat/VRC/SDKBase/VRCLayers.h` | `VRC.SDKBase.VRCLayers` (enum) |
| `sdk/VRChat/VRC/Core/UnityVersion.h` | `VRC.Core.UnityVersion` |
| `sdk/VRChat/VRC/Core/Endpoints.h` | `VRC.Core.Endpoints` |
| `sdk/VRChat/VRC/Core/Logger.h` | `VRC.Core.Logger` |
| `sdk/VRChat/VRC/Core/ConfigManager.h` | `VRC.Core.ConfigManager` |
| `sdk/VRChat/VRC/Core/VRCLogger.h` | `VRC.Core.VRCLogger` |

These fall into three groups, each with a different authoring style:

- **Hand-written** — the seven `VRC/` wrappers plus `HighlightsFX`, authored
  member by member. See the sections below.
- **Metadata-driven** — the eleven `VrcGenerated` headers, emitted from a
  captured metadata table. See
  [Metadata-driven types](#metadata-driven-types).
- **Mod-side helper** — `Menus.h`, which wraps no managed type at all.

All of them are `OutputFilePolicy::GeneratedOverwrite`, so they are rewritten by
the generator and **must not be edited by hand**. If you need a change,
edit the `.inl` template under
`src/sdk/templates/VRChat/` and regenerate, or copy the wrapper into a
mod-owned header instead.

The templates live in the repository at:

```text
src/sdk/templates/VRChat/
  HighlightsFX.inl
  Menus.inl
  vrchat_generated_emit.inl        # the emitter
  vrchat_generated_table.inl       # the captured metadata table
  VRC/
    SDKBase/
      VRCPlayerAPI.inl
      Networking.inl
      VRC_Pickup.inl
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

That is the complete set — eleven `.inl` files. The eight metadata-driven
headers under `VRC/SDKBase/` and `VRC/Core/` have **no** `.inl` of their own;
they are emitted at build time from `vrchat_generated_table.inl`.

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

## HighlightsFX (`HighlightsFX`)

A wrapper around the VRChat silhouette-highlight component (`HighlightsFX` in
`Assembly-CSharp.dll`, namespace empty). This is **obfuscated** code, so unlike
`VRC.Core.APIUser` its member names are not stable — the literals below are from
one specific build and are resolved through the deobfuscation map.

`HighlightsFX` has no public factory, so `Instance()` goes through the
deobfuscated static `Method_HighlightsFX_0`:

```cpp
#include "sdk/VRChat/HighlightsFX.h"

if (HighlightsFX fx = HighlightsFX::Instance()) {
    fx.EnableOutline(renderer, true);
    fx.EnableOutline(renderer, Unity::Color::red(), false);
}
```

- `Instance()` — the deobfuscated static accessor
- `EnableOutline(Unity::Renderer, bool)`
- `EnableOutline(Unity::Renderer, Unity::Color, bool)` — sets an explicit color

> The type carries a `bool` and a `Color` through the *same* deobfuscated name,
> `Method_Void_Renderer_Boolean_0`, with the arity shifted by one. That is an
> artefact of the obfuscator, not a typo, and it is why the two overloads above
> differ only in argument count. Treat these names as build-specific.

## Menus (`VRC::poll_menus`)

`sdk/VRChat/Menus.h` wraps **no managed type**. It is a small mod-side helper
that detects when the VRChat menu canvases come up and fires the mod's
corresponding init hook:

```cpp
#include "sdk/VRChat/Menus.h"

void on_frame() {
    VRC::poll_menus();
}
```

- Looks for `Canvas_QuickMenu(Clone)` and fires `Modules::System::QuickMenuInit()`
  the first time it appears
- Looks for `Canvas_MainMenu(Clone)` and fires `Modules::System::MainMenuInit()`
  the first time it appears

Each is guarded by a `static bool`, so it fires **once per process** even if
`poll_menus()` is called every frame. There is deliberately no "already seen"
reset: the canvases are recreated when the user reopens a menu, but the
one-shot semantics are intentional.

`Menus.h` includes `modules/modules.h` and depends on the mod's own
`Modules::System` entry points, so it is only usable from a generated mod
project.

## Metadata-driven types

Eleven headers are not hand-written. They are emitted at build time by
`VrcGenerated::EmitType` (`vrchat_generated_emit.inl`) from a table of
`TypeSpec` entries captured from live IL2CPP metadata
(`vrchat_generated_table.inl`, VRChat 2022.3.22f2-DWR).

| Header | C++ name | Kind | Image |
|---|---|---|---|
| `VRC/SDKBase/VRC_SceneDescriptor.h` | `VRC::SDKBase::VrcSceneDescriptor` | component | `VRCSDKBase.dll` |
| `VRC/SDKBase/VRC_Serialization.h` | `VRC::SDKBase::VrcSerialization` | static class | `VRCSDKBase.dll` |
| `VRC/SDKBase/VRC_AvatarPedestal.h` | `VRC::SDKBase::VrcAvatarPedestal` | component | `VRCSDKBase.dll` |
| `VRC/SDKBase/VRC_SpatialAudioSource.h` | `VRC::SDKBase::VrcSpatialAudioSource` | component | `VRCSDKBase.dll` |
| `VRC/SDKBase/VRC_StereoObject.h` | `VRC::SDKBase::VrcStereoObject` | component | `VRCSDKBase.dll` |
| `VRC/SDKBase/VRCLayers.h` | `VRC::SDKBase::VrcLayers` | enum | `VRCSDKBase.dll` |
| `VRC/Core/UnityVersion.h` | `VRC::Core::UnityVersion` | value type | `VRCCore-Standalone.dll` |
| `VRC/Core/Endpoints.h` | `VRC::Core::Endpoints` | static class | `VRCCore-Standalone.dll` |
| `VRC/Core/Logger.h` | `VRC::Core::Logger` | static class | `VRCCore-Standalone.dll` |
| `VRC/Core/ConfigManager.h` | `VRC::Core::ConfigManager` | static class | `VRCCore-Standalone.dll` |
| `VRC/Core/VRCLogger.h` | `VRC::Core::VRCLogger` | static class | `VRC.Logging.dll` |

### Emission rules

The emitter picks a shape from the `TypeSpec` flags:

- **enum** — emitted as `using VrcLayers = int;` plus one
  `inline constexpr` per member. The literal values are **not** captured from
  metadata, so each member is `constexpr <name>{}`, i.e. all zero. Treat enum
  constants as opaque and prefer the hand-written enums
  (see [Mirrored enums](#mirrored-enums)) when a real value is needed.
- **interface** — no callable surface is emitted at all.
- **static class / value type** — a namespace of `InvokeStaticExact` free
  functions. Every property captured on these is static, so it is reached
  through its `get_` accessor rather than an instance read.
- **component** — a `struct` deriving `Unity::Component`, with `GetField` /
  `set_` / `GetProperty` / `CallExact` members generated per entry.

### Two dispatch rules worth knowing

Both exist to avoid silently binding to the wrong overload:

- A parameter is only passed through *inferred* dispatch when it is a plain
  primitive (`IsDispatchPrimitive`). Everything else goes through the explicit
  managed-name path — otherwise a handle is inferred as `System.Object` and the
  call binds to the wrong overload.
- A `static` method is never dispatched through the instance. Doing so would
  pass `this` as the first argument and shift every parameter by one, so the
  emitter routes statics to `InvokeStaticExact`.

### Adding a type

Append a `TypeSpec` to `vrchat_generated_table.inl` plus a `VRChatGenerated*()`
factory, and register the output in the `writes` list in
`src/sdk/mod_project_generator_common.cpp`. No `.inl` per type is needed.

Note the captured table is a snapshot of one build. `VRC_SceneDescriptor` and
the `VRC.Core` logging/config types in particular are VRChat-internal and can
shift between releases; re-capture from live metadata rather than hand-editing
the table.

## Codegen conventions

Generated VRChat headers follow a few rules:

- Wrappers derive from `Unity::Object`, so they are cheap handles and are
  null-testable (`if (player)`).
- `try`/`null` result checks are done with `if (!wrapper)`; a failed invoke
  leaves `last_error()` populated (`Unity::last_error()`).
- Managed generic lists have no dedicated wrapper type; the live handle is held
  as a generic `Unity::Object` and walked with `Call`.
- When adding a new VRChat **`.inl` template**, register it in all three places:
  `src/sdk/templates/mod_project_generator_vrchat.inl` (the `#include`),
  the `writes` list in `src/sdk/mod_project_generator_common.cpp`, and the
  `URK_SDK_TEMPLATE_FILES` list in `cmake/URKitSources.cmake`.
  The third is easy to forget and only shows up as an IDE/`HEADER_FILE_ONLY`
  inconsistency — the build still succeeds, because the `.inl` is reached
  transitively through `mod_project_generator_vrchat.inl`.
- When adding a new **metadata-driven type**, no new `.inl` is needed. See
  [Adding a type](#adding-a-type).

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