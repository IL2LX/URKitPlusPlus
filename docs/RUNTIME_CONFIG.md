# RuntimeConfig

RuntimeConfig persists your mod's user settings as a JSON file on disk. It is a
single header (`mod/config/RuntimeConfig.h`, generated) with no runtime cost
until you call into it.

This page covers:

- Where and how settings are stored
- Including and opening the config
- Reading and writing values (`Get*` / `Set*`)
- Checking and removing keys (`Has` / `Remove`)
- Saving back to disk
- Colors and Unity colors
- Good-to-know details (defaults, corruption, naming)

## Where settings are stored

One JSON file is created per mod, named after your mod's DLL, inside the game's
`URKit` user-data directory:

```text
<GameExeDir>\URKit\UserData\Configs\<YourModName>.json
```

`OpenOrCreateConfig("Aurora")` opens `Aurora.json` (the `.json` extension is
appended for you). If the file does not exist yet, it is created empty; if it
already exists, its contents are loaded into memory.

The file is shaped `category -> module -> setting`:

```json
{
  "Movement": {
    "Flight": {
      "speed": 1.5,
      "maxPlayers": 8,
      "wasEnabled": true,
      "tint": [1, 0.5, 0.25, 1]
    }
  }
}
```

Every access is fully qualified by `(category, module, key)`. In URKit the
category is usually `Modules::category_name(category())` and the module is
`name()`.

## Including and opening

```cpp
#include "mod/config/RuntimeConfig.h"
```

Open once per mod, early (your mod entry or init), before any module reads or
writes:

```cpp
RuntimeConfig::OpenOrCreateConfig("Aurora");  // opens (or creates) Aurora.json
```

A corrupt file is left untouched; you simply get defaults in memory until the
file is overwritten by a later save.

## Reading values (`Get*`)

Every getter returns the default if the key is missing or cannot be converted:

```cpp
float speed   = RuntimeConfig::GetFloat("Movement", "Flight", "speed", 1.0f);
int   maxP    = RuntimeConfig::GetInt("Movement", "Flight", "maxPlayers", 8);
bool  enabled = RuntimeConfig::GetBool("Movement", "Flight", "wasEnabled", false);
double height = RuntimeConfig::GetDouble("Movement", "Flight", "height", 2.0);
std::string nickname = RuntimeConfig::GetString("Movement", "Flight", "nickname", "");
```

## Writing values (`Set*`)

```cpp
RuntimeConfig::SetFloat("Movement", "Flight", "speed", speed);
RuntimeConfig::SetInt("Movement", "Flight", "maxPlayers", maxP);
RuntimeConfig::SetBool("Movement", "Flight", "wasEnabled", enabled);
RuntimeConfig::SetDouble("Movement", "Flight", "height", height);
RuntimeConfig::SetString("Movement", "Flight", "nickname", "New Name");
```

Set* updates the in-memory store. Nothing hits disk until `SaveConfig()`.

## Colors

Use either the built-in color type or a Unity color:

```cpp
// Built-in type (same layout as Unity Color: r, g, b, a):
const RuntimeConfig::Color tint{1.0f, 0.5f, 0.25f, 1.0f};
RuntimeConfig::SetColor("Visuals", "Highlight", "tint", tint);

RuntimeConfig::Color saved{};
RuntimeConfig::GetColor("Visuals", "Highlight", "tint", saved, RuntimeConfig::Color{});
```

```cpp
// Directly with a Unity color:
URK::Unity::Color highlight{1.0f, 0.5f, 0.25f, 1.0f};
RuntimeConfig::SetColor("Visuals", "Highlight", "tint", highlight);

URK::Unity::Color loaded{};
RuntimeConfig::GetColor("Visuals", "Highlight", "tint", loaded, URK::Unity::Color{});
```

Colors are stored in the JSON as an RGBA array: `[1, 0.5, 0.25, 1]`.
`GetColor` returns `false` when the stored value is missing/wrong; `output` is
then set to the fallback.

## Checking and removing (`Has` / `Remove`)

```cpp
if (RuntimeConfig::Has("Movement", "Flight", "speed")) {
    // key exists in the in-memory store
}

RuntimeConfig::Remove("Movement", "Flight", "nickname");  // returns true if it was removed
```

## Saving back to disk

Writes the whole in-memory store back to the mod's `.json` file. Call it when
you are done editing (menu close, module disable, or a periodic autosave):

```cpp
RuntimeConfig::SaveConfig();
```

## Quick reference

| Function | Description |
| --- | --- |
| `OpenOrCreateConfig(modName)` | Open (or create) `<modName>.json` and load it into memory |
| `SaveConfig()` | Write the whole in-memory store back to the open file |
| `GetBool / GetInt / GetFloat / GetDouble / GetString / GetColor(category, module, key, fallback)` | Read a value; returns `fallback` if missing |
| `SetBool / SetInt / SetFloat / SetDouble / SetString / SetColor(category, module, key, value)` | Write a value in memory |
| `Has(category, module, key)` | Whether the key exists |
| `Remove(category, module, key)` | Remove a key; returns `true` if it was present |

All getters/setters mix types freely: a stored integer can be read back as a
`float`, a stored string like `"1.5"` can be read back as a number, etc.
Missing or unparsable values never throw or crash — you get the fallback.