# VRChat SDK wrappers

The URKit++ project generator ships a set of VRChat-specific wrappers that are
generated into every project targeting VRChat. They sit under
`sdk/VRChat/VRC/` and mirror the managed VRChat SDK namespaces.

| Generated header | Managed type |
|---|---|
| `sdk/VRChat/VRC/SDKBase/VRCPlayerAPI.h` | `VRC.SDKBase.VRCPlayerApi` |
| `sdk/VRChat/VRC/SDKBase/Networking.h` | `VRC.SDKBase.Networking` |
| `sdk/VRChat/VRC/Core/APIUser.h` | `VRC.Core.APIUser` |

All three are `OutputFilePolicy::GeneratedOverwrite`, so they are rewritten by
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