# Reflection DSL in generated mods

`MethodHandler`, `ResolvedMethod`, and `ClassResolver` are ports of the
IL2CPP-SDK reflection API into URKit's generated Unity SDK. They let you
resolve and call managed members by name at runtime, and inspect a class's
fields, properties, and methods against **live metadata**, on both Mono and
IL2CPP through one backend-neutral path.

Everything below ships in the generated project under
`sdk/unity/unity.h` and is usable with unqualified names (`MethodHandler`,
`ClassResolver`, ...) because `aliases.inl` re-exports them at SDK scope.

## When to use it

- You know a class and a member name but the SDK wrapper doesn't expose a
  typed binding yet. Resolve it once, bind it, use it.
- You're writing generic tooling (logs, inspectors, auto-bindings) that must
  work on any game as long as a name matches.
- You need a field offset or a native method pointer to build a cache that
  survives rebuilds.

Don't use the DSL in a hot loop. It walks metadata on the Unity main thread.
`ResolvedMethod` is the one exception — resolve once, then call per frame with
a cached handle.

## MethodHandler and ResolvedMethod

`MethodHandler` mirrors the SDK's static API and is built on `ResolvedMethod`,
which is the handle you actually cache.

```cpp
#include "sdk/unity/unity.h"

using URK::Unity::ResolvedMethod;

// Resolve by class + method name; argc = -1 matches any arity.
static const ResolvedMethod getDelta =
    URK::Unity::MethodHandler::resolve("UnityEngine.Time", "get_deltaTime");

// Match only the 1-arg overload if you need to disambiguate.
static const ResolvedMethod destroy =
    URK::Unity::MethodHandler::resolve("UnityEngine.Object", "Destroy", 1);
```

### Invoking

`ResolvedMethod` itself has `invoke()` (static) and `call()` (instance),
while `MethodHandler` provides `invoke_raw()` and `invoke<T>()` for the
SDK-compatible entry points.

```cpp
// Static, no target.
float dt = getDelta.invoke<float>();

// Instance: pass the managed Object separately from the arguments.
static const ResolvedMethod setPose =
    URK::Unity::ResolvedMethod::resolve("Avatar", "SetPose", 2);
URK::Unity::Object avatar = /* ... */;
setPose.call(avatar, poseIndex, poseValue);
```

Argument-count checking: `resolve()` with `argc = -1` disables the check and
the runtime rejects a mismatch when you invoke. With a fixed `argc` the SDK
checks the call site's argument pack against the resolved arity and reports an
error instead of calling.

Resolution errors go to `URK::Unity::last_error()` / `clear_error()` just like
the rest of the SDK, so:

```cpp
if (!getDelta) {
  const char* detail = URK::Unity::last_error();
  /* handle it */
}
```

`MethodHandler::invoke_raw(method, obj, ptr_array)` returns the raw result
pointer and is useful when you already have a parameter array.

## ClassResolver

`ClassResolver` is a fluent query builder over one class handle. You register
queries, then run them in a single pass over the live metadata. Queries read
matching fields/properties/methods and write results into your variables
(offsets, raw handles, native pointers, names).

### Starting one

```cpp
// From a class handle.
URK::Unity::ClassResolver res(someClass);

// Or by fully-qualified name. The class is resolved through the same cached
// scan the SDK uses for TypeRef, so assembly-agnostic by default.
URK::Unity::ClassResolver res = URK::Unity::ClassResolver::by_name("UnityEngine.UI.Button");
```

### Registering queries

Every query is created, configured, and assigned destinations with chained
calls. All of these return `*this`, so you can keep building.

```cpp
int onClickOffset = 0;
void* onClickHandle = nullptr;
std::string onClickName;

res.field()
    .byName("m_OnClick")
    .required()              // fail validation if not found
    .toOffset(onClickOffset)
    .toPtr(onClickHandle)    // raw field handle
    .toName(onClickName)
    .label("Button.m_OnClick");
```

The query kinds and their purposes:

| Query | Created by | Matches | Can capture |
| --- | --- | --- | --- |
| `FieldQuery` | `field()` | name, type, static/instance, self-typed fields | offset, raw fields handle, name |
| `MethodQuery` | `method()` | name, param count, param types, return type | native method pointer, raw handle, all matches' pointers |
| `PropertyQuery` | `property()` | name, type, index, static/instance, has getter/setter | getter/setter pointers or raw handles, name |
| `FieldCounter` | `counter()` | type, static/instance | count only, with expectations |
| `IndexedFieldCollector` | `collector()` | type, static/instance | N fields by their position, each to offset/ptr/name |

### Capturing targets (offsets vs pointers)

Two capture ranges exist. **Offsets** (`toOffset`, `bindOffset`) are integers
into the object's memory layout — you'd use them with
`reinterpret_cast`-based cast helpers or a generic `ReadField`. **Pointers**
(`toPtr`, `toMethodRaw`, `toFieldRaw`) are live metadata handles: the native
method pointer for methods (usable as a raw function pointer on IL2CPP) or the
raw `Il2CppClassField`/`Il2CppMethod` object pointers.

### Static vs instance

Fields, methods, and properties all accept `isStatic()` / `notStatic()`
filtering. The statics in a `MethodQuery` are checked against the resolved
method; for properties URKit looks at the getter's (or setter's) staticness.

### Counters and collectors for array-like fields

Obfuscated classes often have repeated fields of the same type (e.g. several
`Int32[]` fields that a game stores per-slot). `FieldCounter` validates the
expected count without capturing anything:

```cpp
res.counter()
    .byTypeName("UnityEngine.UI.ColorBlock")
    .expectExact(1);          // or expectMin/expectMax/expectRange
```

`IndexedFieldCollector` captures all matching fields in declaration order and
binds a subset by position:

```cpp
int color0 = 0;
int color1 = 0;
res.collector()
    .byTypeName("UnityEngine.UI.ColorBlock")
    .requireCount(2)
    .bindOffset(0, color0)
    .bindOffset(1, color1);
```

### Running the resolution

```cpp
if (res.validate()) {
  res.apply();
}
```

Or use the combined convenience methods:

- `resolve()` — validate; if anything required is missing, returns false and
  applies nothing. Your destinations are untouched.
- `resolvePartial()` — validate but apply whatever matched, and report
  failures. Use this when optional members may or may not exist.
- `validate()` / `apply()` — the explicit two-step form.
- `includeInherited(true)` — walk parent classes so inherited fields are
  matched (matching methods/properties still use `get_fields`-style
  iteration per class; see the template source in
  `src/sdk/templates/unity/invoke.inl` for the exact scope).

After resolution, check `res.missReport()` for a comma-separated list of
queries that missed, or look at each query's `matched()`/`valid()`.

Example — the full flow for a Button:

```cpp
URK::Unity::ClassResolver button =
    URK::Unity::ClassResolver::by_name("UnityEngine.UI.Button");

int interactableOffset = 0;
void* onClickGetter = nullptr;
std::vector<void*> pointerClickMethods;

button.field()
    .byName("m_Interactable")
    .toOffset(interactableOffset)
    .label("Selectable.m_Interactable");
button.property()
    .byName("onClick")
    .required()
    .toGetter(onClickGetter)
    .label("Button.onClick");
button.method()
    .byName("OnPointerClick")
    .collectAll()
    .toPtrList(pointerClickMethods)
    .label("Button.OnPointerClick");

button.resolve();   // fails if onClick property is missing
```

### FieldCounter and IndexedFieldCollector details

- `FieldCounter`: `.expectExact(n)` / `.expectMin(n)` / `.expectMax(n)` /
  `.expectRange(min, max)`. `count()` reports the number found.
- `IndexedFieldCollector`: `.requireCount(n)` requires at least `n` matches
  to stay valid; `bind(index, ...)` accepts `(index, int& offset)`,
  `(index, void*& ptr)`, `(index, int& offset, void*& ptr)`, or with name
  destinations. Out-of-range bindings are skipped silently.

## SetResolverTrace

Install a trace callback to see what a resolver is doing (class validation,
iteration marks):

```cpp
URK::Unity::SetResolverTrace([](const char* msg) {
  ModLog::info("[resolver] %s", msg);
});

/* run resolutions here */

URK::Unity::SetResolverTrace(nullptr);
```

This is a global hook across all resolvers in the module, useful for debugging
a missed binding. Remove it (`nullptr`) once you're done.

## What is intentionally not here

- **`deobfuscate()`** exists on the query types for API parity with the SDK but
  registers nothing. Deobfuscated name resolution already happens inside the
  SDK's normal lookups (`find_class`, typed wrappers, the loader's map), so the
  DSL doesn't re-implement it.
- **SEH guards** around metadata calls are handled by the SDK's normal error
  reporting path, not per-resolver exception handling. Treat a failed resolve
  as "not found" and check `last_error()`.

## Threading

Everything on this page must run on the Unity main thread, like the rest of
the SDK's managed-call surface. Cache `ResolvedMethod` handles (function-local
statics are fine); do not re-resolve per frame.