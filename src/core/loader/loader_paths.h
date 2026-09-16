#pragma once

#include "config.h"

#include <string>

std::string Loader_ExeDir();
std::string Loader_ModsDir(const Config &config);
std::string Loader_GameName();
std::string Loader_WindowsError(unsigned long error);

// Returns the <ExeDir>\URKit\ directory, creating it if necessary. Used to
// store game-specific support files such as the IL2CPP export map.
std::string Loader_UrKitDir();

// True when the current process executable is VRChat.exe (case-insensitive).
bool Loader_IsVRChat();
