#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>

// Decodes an obfuscated IL2CPP symbol dump. Games like VRChat ship
// GameAssembly.dll with every class, method, field, property and event
// renamed. A <ExeDir>\URKit\DeobfuscationMap.json dump maps the obfuscated
// names the runtime reports back to the readable names a mod author knows:
//
//     {
//       "assemblies": [
//         {
//           "name": "Assembly-CSharp.dll",
//           "classes": [
//             {
//               "obfuscated": "GameManager",
//               "name": "PlayerManager",
//               "namespace": "Assembly-CSharp",
//               "methods":    { "DinAodNqkeq": "Update" },
//               "fields":     { "XuEcQocQjm": "isConnected" },
//               "properties": { "KdzAqlBxyq": "IsGrounded" },
//               "events":     { "BcmOupwZhb": "OnReceivedData" }
//             }
//           ]
//         }
//       ]
//     }
//
// Two ways to key a record, distinguished by the "obfuscated" value:
//  * A Beebyte ciphertext (U+00CC..U+00CF charset, 8-23 chars) keys by the
//    name the runtime reports; the record replaces that name directly.
//  * Anything else is a STRUCTURAL name, the name the loader generates when
//    there is no mapping (e.g. "MonoBehaviourPublicAPOb_vOb_lBoStObBo_UUnique"
//    -> "VRCPlayer"). Class and member maps are then keyed by those generated
//    structural names and override them, so a dump of resolved names can be
//    edited into an override map without ever touching ciphertext.
// Both keying styles can coexist in one file.
//
// Resolution is pass-through: a class (or member) that has no entry in the map
// keeps the name exactly as the game reports it. This intentionally matches a
// dumped file against the full CLR dump; symbols that were never dumped (or
// are new in the current game version) are simply not renamed.
enum class Il2CppSymbolKind {
    Method,
    Field,
    Property,
    Event,
};

// Symbols for one obfuscated class. Member maps are keyed by the obfuscated
// member name and hold the readable name. Missing realName/members mean the
// symbol is absent and therefore must be reported as-is.
struct Il2CppClassSymbols {
    std::string realName;
    // The Il2CppInterop structural name (e.g. "MonoBehaviourPublicAPOb_vOb_lBo1ObBoStUnique")
    // generated for this class by Il2Cpp_BuildStructuralNames. It is the name the
    // structural-keyed records (and the beebyte CSV dump) are keyed by, and it is
    // never written back to the symbol-file JSON.
    std::string structuralName;
    std::unordered_map<std::string, std::string> methods;
    std::unordered_map<std::string, std::string> fields;
    std::unordered_map<std::string, std::string> properties;
    std::unordered_map<std::string, std::string> events;
};

// The whole dump. Keyed by the obfuscated (runtime-reported) class name.
struct Il2CppSymbolMap {
    std::unordered_map<std::string, Il2CppClassSymbols> classes;
    // Beebyte accessor-leak pairs recovered from the live metadata: obfuscated
    // name -> plaintext name. Beebyte renames the name once per build, so one
    // recovered pair renames EVERY member with that ciphertext. Filled at
    // launch by Il2Cpp_BuildBeebyteLeakPairs.
    std::unordered_map<std::string, std::string> globalPairs;
    // Entries that looked like class records but did not parse.
    std::size_t invalidEntries = 0;
};

// Parses a symbols.json dump. Returns false when the file cannot be opened,
// is not valid JSON, or contains no usable class records. A successful load
// replaces any previous map contents.
bool Il2Cpp_LoadSymbolMap(const char *path, Il2CppSymbolMap &map);

// Resolves an obfuscated class name to its readable name. Returns nullptr
// when there is no mapping, signalling the caller to keep the original name.
const char *Il2Cpp_ResolveClassName(const Il2CppSymbolMap &map, const char *obfClassName);

// Resolves an obfuscated member (method/field/property/event) inside a class
// to its readable name. Returns nullptr when the class or member has no
// mapping (pass-through), not even when the class is unmapped but the member
// name looks familiar: unmatched classes stay fully obfuscated.
const char *Il2Cpp_ResolveMemberName(const Il2CppSymbolMap &map, Il2CppSymbolKind kind,
                                     const char *obfClassName, const char *obfMemberName);

// True when a name is Beebyte ciphertext: 8..23 Unicode chars over the
// U+00CC..U+00CF alphabet (validated as UTF-8, which is how the runtime and
// the symbol dump both report it).
bool Il2Cpp_IsBeebyteCipher(const char *name);

// Resolves an obfuscated name from the global accessor-leak pair table
// (map.globalPairs). Returns nullptr when there is no pair, so the caller
// keeps the name as the game reports it.
const char *Il2Cpp_ResolveGlobalPair(const Il2CppSymbolMap &map, const char *obfName);
