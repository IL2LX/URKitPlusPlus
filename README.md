<img src="https://images-ext-1.discordapp.net/external/uFJStwAB9SGTKPgCxhglvA7OrP3Bk0fdli4p7trdp0s/https/repository-images.githubusercontent.com/1373106960/7860043a-3b25-4388-b188-a6afa986a53c?format=webp" width="950">

URKit++ is a fork of
[URKit](https://github.com/Jadis0x/URKit) that adds **deobfuscation support**: 
it can read a `DeobfuscationMap.json`, load custom export-name maps for
UnityPlayer, and dump rename maps from live IL2CPP metadata. Everything from
upstream URKit (Unity object access, managed calls, hooks, lifecycle, network,
ImGui) works the same.

[![GitHub Release](https://img.shields.io/github/v/release/Jadis0x/URKit?label=Release)](https://github.com/Jadis0x/URKit/releases/latest)
[![Downloads](https://img.shields.io/github/downloads/Jadis0x/URKit/total?label=Downloads)](https://github.com/Jadis0x/URKit/releases)
[![GitHub Stars](https://img.shields.io/github/stars/Jadis0x/URKit?style=flat&label=Stars)](https://github.com/Jadis0x/URKit/stargazers)
[![Discord](https://img.shields.io/badge/Discord-Join%20Server-5865F2?logo=discord&logoColor=white)](https://discord.com/invite/XC7RUpGp6e)
[![Support](https://img.shields.io/badge/Support-GitHub%20Issues-blue?logo=github)](https://github.com/Jadis0x/URKit/issues)
[![License](https://img.shields.io/github/license/Jadis0x/URKit)](https://github.com/Jadis0x/URKit/blob/main/LICENSE)

## What URKit++ changes over URKit

- **Deobfuscation map** (`URKit\DeobfuscationMap.json`): maps obfuscated
  IL2CPP class and member names back to their readable names. The loader loads
  it at backend start, and every lookup (find class, method, field, and the
  generated SDK helpers) resolves through it, so mods can use real names like
  `ToolTip` instead of `MonoBehaviourPublicIPointerEnterHandler...Unique`. See
  [docs/example_deobfuscation_map.json](docs/example_deobfuscation_map.json).
- **UnityPlayer export map** (`URKit\UnityPlayerExports.txt`): custom
  export-name file so IL2CPP binding still works when the game's export names
  are obfuscated (`[N] obfuscated = real_il2cpp_name`, format in
  [docs/example_exports.txt](docs/example_exports.txt)). When present, export
  resolution goes through the map instead of exact-name matching.
- **Rename-map dumper** (`AutoDump=1` in `URKit_config.ini`): at startup the
  loader writes `URKit\Dumps\DeobfuscationMap.json.dump`,
  `.readable`, and `.beebyte.csv` (a Beebyte-compatible CSV you can feed back
  into the map).
- **On-the-fly name recovery**: even without a map, the loader builds readable
  structural names at launch (`StructuralNames=1`) and recovers Beebyte
  accessor-leak pairs (`AccessorLeaks=1`). Both default on.
- **Reverse name lookups**: `find_class`/`AddComponent` resolve deobfuscated
  names to the runtime's obfuscated metadata key, so component-by-real-name
  works even for classes only described by the map.
- **Reflection DSL in generated projects**: the generated Unity SDK ships the
  IL2CPP-SDK `MethodHandler`/`ClassResolver` reflection API — cached
  `ResolvedMethod` handles (`MethodHandler::resolve`/`invoke`), and the fluent
  `ClassResolver` query DSL (`field()`/`method()`/`property()`/`counter()`/
  `collector()` with `toOffset`/`toPtr`/`required`), working on both Mono and
  IL2CPP. Details below.

Example symbol file: [docs/example_symbols.json](docs/example_symbols.json).

<img src="showcase/ss1.png" width="550">

## What's in a release

The underlying toolkit is URKit: a native C++ modding toolkit for Windows x64
Unity games. It supports Mono and IL2CPP through one loader ABI, and gives you
Unity object access, managed method calls, hooks, lifecycle callbacks,
networking, and an ImGui overlay, generated straight into a buildable CMake
project.

- `urk-sdk.exe`: generates a Mono or IL2CPP mod project.
- `urk-updater.exe`: updates a generated project without touching your own files.
- `version.dll`, `winhttp.dll`, `winmm.dll`: proxy loaders. Install the one the game actually imports.
- `URKitInjector.dll`: a proxy-free loader for manual injection workflows.
- `urk-dev-mcp.exe` + `URKitDevBridge.dll`: optional MCP server for AI coding assistants (build, deploy, runtime diagnostics).

Put the proxy DLL next to the game executable, and built mods in the game's
`Mods` directory. Only install one proxy, and don't rename it.

## Reflection DSL in generated projects

The generated Unity SDK now ships the IL2CPP-SDK reflection DSL, working on
both Mono and IL2CPP backends through one backend-neutral `detail::Backend`.

- **`MethodHandler`** with `ResolvedMethod`: resolve a managed method once from
  a dot-qualified class name (`MethodHandler::resolve("UnityEngine.Time",
  "get_deltaTime")` or the exact-signature form), cache the handle, and invoke
  it per frame with `invoke_raw`/`invoke<T>` for statics or a target for
  instance calls — no repeated metadata walks.
- **`ClassResolver`**: a fluent query builder over a single class handle that
  validates against live metadata and captures offsets, raw handles, and
  native method pointers. Starts from a handle or by name, then chains
  `field()`, `method()`, `property()`, `counter()`, and `collector()` queries
  with `byName`/`byTypeName`/`byType`, `required()`, `toOffset()`, `toPtr()`,
  `toName()`, `toGetter()`/`toSetter()`, and `collectAll()`/`toPtrList()`.
  Options like `.field().byName("m_OnClick").required().toOffset(off).toPtr(p)`
  mirror the SDK's `FieldQuery`/`MethodQuery`/`PropertyQuery`/`FieldCounter`/
  `IndexedFieldCollector` API. `validate()` reports required-member misses,
  `resolvePartial()` applies whatever matched, and `missReport()` returns the
  failures.
- **`SetResolverTrace(fn)`**: install a callback to log each resolver's
  validation/iteration steps for debugging.

Deobfuscation-aware name resolution and the SDK's SEH guards are handled by
the existing deobfuscation pipeline rather than replicated inside the DSL;
`deobfuscate()` is accepted for API parity but registers nothing. Full guide:
[docs/REFLECTION_DSL.md](docs/REFLECTION_DSL.md).

## Quick start

```powershell
./urk-sdk.exe --game-exe C:\Games\Example\Example.exe --backend auto --name MyMod
cd <GameDir>\urk-sdk-output\MyMod\project
cmake --preset clang-debug
cmake --build --preset clang-debug --parallel
```

That builds a debug DLL and drops it in the game's `Mods` directory. You need
CMake 3.28+, Ninja, and either LLVM/Clang or the MSVC toolchain (VS 2022 Build
Tools or newer). Use `clang-release` / `msvc-release` when you're ready to
ship, and `msvc-debug` if you'd rather build with `cl.exe`.

Launch the game and check `URKit_logs.log` next to the executable. You
should see the mod initialize. If that file doesn't exist, the game isn't
importing the proxy you installed.

## Learn the API

New to URKit? Start with [Getting Started](docs/GETTING_STARTED.md). It
walks through writing an actual mod, from "hello log" to reading and changing
game state, step by step.

For a video walkthrough, check out the [URKit SDK Tutorial Series](https://youtube.com/playlist?list=PLP9lUXoova70&si=ATR-n7l7tVEliZlN).

Once you know the basics, [docs/SDK_HANDBOOK.md](docs/SDK_HANDBOOK.md) is the
full reference: object search, threading rules, hooks, the render/highlight
pipeline, caching, and a diagnostics chapter for when something doesn't work.
[ARCHITECTURE.md](ARCHITECTURE.md) covers how URKit itself is put together,
and [docs/DEV_MCP.md](docs/DEV_MCP.md) covers the AI-assistant MCP server.

Updating an existing project, migrating a hand-patched SDK, or automating the
updater from a script: that's all in the handbook's first chapter.

## Security software and false positives

The proxy loaders and `URKitInjector.dll` load into a game process and can
install API hooks, which is normal mod-loader behavior, but it can trip
antivirus heuristics. A detection alone doesn't mean a release is malicious.

Only download releases from this repository, and check the published SHA-256
digest. Release binaries are self-signed for tamper identification, not as a
trust signal: Windows won't trust that certificate by default. Don't disable
your antivirus to run URKit. If you get a false positive, report it to the
vendor (or open an issue here) with the detection name, the file, and its
SHA-256 digest.

URKit is available under the [MIT License](LICENSE).
