#include "loader_paths.h"
#include "logger.h"
#include "platform_paths.h"

#include <windows.h>

#include <cstring>
#include <filesystem>
#include <string>

std::string Loader_ExeDir() {
    return Platform_ExeDir();
}

std::string Loader_WindowsError(unsigned long error) {
    return Platform_WindowsError(error);
}

std::string Loader_ModsDir(const Config &config) {
    std::filesystem::path configured(config.modsDir);
    std::filesystem::path directory =
        configured.is_absolute() ? configured : std::filesystem::path(Platform_ExeDir()) / configured;

    std::error_code error;
    std::filesystem::create_directories(directory, error);
    if (error) {
        Log("[mods] Failed to create mod directory %s: %s", directory.string().c_str(), error.message().c_str());
    }

    std::string result = directory.string();
    if (!result.empty() && result.back() != '\\' && result.back() != '/')
        result += '\\';
    return result;
}

std::string Loader_GameName() {
    return Platform_GameName();
}

std::string Loader_UrKitDir() {
    std::filesystem::path directory = std::filesystem::path(Platform_ExeDir()) / "URKit";
    std::error_code error;
    std::filesystem::create_directories(directory, error);
    if (error) {
        Log("[urkit] Failed to create URKit directory %s: %s", directory.string().c_str(),
            error.message().c_str());
    }
    std::string result = directory.string();
    if (!result.empty() && result.back() != '\\' && result.back() != '/')
        result += '\\';
    return result;
}

bool Loader_IsVRChat() {
    char path[MAX_PATH]{};
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    const char *name = strrchr(path, '\\');
    name = name ? name + 1 : path;
    return _stricmp(name, "VRChat.exe") == 0;
}
