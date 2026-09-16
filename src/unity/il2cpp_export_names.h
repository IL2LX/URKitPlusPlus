#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>

// Maps a real IL2CPP API name (il2cpp_domain_get, ...) to the obfuscated export
// name the game actually exports. Titles like VRChat rename the il2cpp_* exports
// before shipping GameAssembly.dll, so the loader can only resolve them through
// a dump produced by tools such as "UnityPlayer Dumper":
//
//     [0] FPSHifuDkOi --> il2cpp_init
//     [1] hGXVmqTxPxP --> il2cpp_init_utf16
//     ...
//
// The map is consumed by Il2Cpp_BindExports; it is only ever populated for games
// that request obfuscated-export resolution and is ignored everywhere else.
struct Il2CppExportNameMap {
    // real il2cpp name -> exported (possibly obfuscated) name.
    std::unordered_map<std::string, std::string> realToObfuscated;
    // Lines that looked like map entries but did not parse; informational only.
    std::size_t invalidLines = 0;
};

// Parses a UnityPlayer-Dumper style dump. Returns false when the file cannot be
// opened or yields no usable entries. Valid entries replace any previous map.
bool Il2Cpp_LoadExportNameMap(const char *path, Il2CppExportNameMap &map);