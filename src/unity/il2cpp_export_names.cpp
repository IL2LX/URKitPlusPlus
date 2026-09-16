#include "il2cpp_export_names.h"
#include "logger.h"

#include <cctype>
#include <fstream>
#include <string>

namespace {
std::string Trim(const std::string &value) {
    std::size_t begin = 0;
    while (begin < value.size() && std::isspace(static_cast<unsigned char>(value[begin])))
        ++begin;
    std::size_t end = value.size();
    while (end > begin && std::isspace(static_cast<unsigned char>(value[end - 1])))
        --end;
    return value.substr(begin, end - begin);
}
} // namespace

bool Il2Cpp_LoadExportNameMap(const char *path, Il2CppExportNameMap &map) {
    map.realToObfuscated.clear();
    map.invalidLines = 0;
    if (!path || !*path)
        return false;

    std::ifstream stream(path, std::ios::binary);
    if (!stream.is_open())
        return false;

    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\n')
            line.pop_back();
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        // Strip a UTF-8 BOM if the dump was saved with one.
        if (line.size() >= 3 && line[0] == '\xEF' && line[1] == '\xBB' && line[2] == '\xBF')
            line.erase(0, 3);

        const std::string trimmed = Trim(line);
        // Comments and the dumper banner ("UnityPlayer Dumper by ...") have no
        // bracketed index and are ignored.
        if (trimmed.empty() || trimmed.front() != '[')
            continue;

        const std::size_t close = trimmed.find(']');
        if (close == std::string::npos) {
            ++map.invalidLines;
            continue;
        }
        // Accept both the dumper's "[N] name --> real" and the compact
        // "[N] name = real" spelling.
        std::size_t separator = trimmed.find("-->", close + 1);
        std::size_t realNameStart = separator == std::string::npos ? std::string::npos : separator + 3;
        if (separator == std::string::npos) {
            separator = trimmed.find('=', close + 1);
            realNameStart = separator == std::string::npos ? std::string::npos : separator + 1;
        }
        if (separator == std::string::npos) {
            ++map.invalidLines;
            continue;
        }

        const std::string obfuscated = Trim(trimmed.substr(close + 1, separator - (close + 1)));
        const std::string realName = Trim(trimmed.substr(realNameStart));
        if (obfuscated.empty() || realName.empty() || obfuscated.find(' ') != std::string::npos ||
            realName.find(' ') != std::string::npos) {
            ++map.invalidLines;
            continue;
        }

        map.realToObfuscated[realName] = obfuscated;
    }

    if (map.realToObfuscated.empty()) {
        Log("[IL2CPP][ERROR] Export map %s contained no usable '<name> --> <real>' or '<name> = <real>' entries.",
            path ? path : "<null>");
        return false;
    }
    return true;
}