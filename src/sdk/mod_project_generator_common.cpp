#include "mod_project_generator_common.h"
#include "embedded_mod_sdk.h"
#include "embedded_dev_test.h"
#include "mod_sdk.h"
#include "project_ledger.h"
#include "project_manifest.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <random>
#include <sstream>
#include <stdexcept>
#include <system_error>
#include <vector>

namespace ModProjectGenerator {
namespace {

namespace fs = std::filesystem;

bool IsCppKeyword(const std::string &value) {
    static const char *keywords[] = {
        "alignas",       "alignof",     "and",        "and_eq",   "asm",       "auto",
        "bitand",        "bitor",       "bool",       "break",    "case",      "catch",
        "char",          "class",       "compl",      "concept",  "const",     "consteval",
        "constexpr",     "constinit",   "const_cast", "continue", "co_await",  "co_return",
        "co_yield",      "decltype",    "default",    "delete",   "do",        "double",
        "dynamic_cast",  "else",        "enum",       "explicit", "export",    "extern",
        "false",         "float",       "for",        "friend",   "goto",      "if",
        "inline",        "int",         "long",       "mutable",  "namespace", "new",
        "noexcept",      "not",         "not_eq",     "nullptr",  "operator",  "or",
        "or_eq",         "private",     "protected",  "public",   "register",  "reinterpret_cast",
        "requires",      "return",      "short",      "signed",   "sizeof",    "static",
        "static_assert", "static_cast", "struct",     "switch",   "template",  "this",
        "thread_local",  "throw",       "true",       "try",      "typedef",   "typeid",
        "typename",      "union",       "unsigned",   "using",    "virtual",   "void",
        "volatile",      "wchar_t",     "while",      "xor",      "xor_eq"};
    for (const char *keyword : keywords)
        if (value == keyword)
            return true;
    return false;
}

std::string EscapeString(const std::string &text) {
    std::ostringstream out;
    for (char ch : text) {
        switch (ch) {
            case '\\':
                out << "\\\\";
                break;
            case '"':
                out << "\\\"";
                break;
            case '\n':
                out << "\\n";
                break;
            case '\r':
                break;
            case '\t':
                out << "\\t";
                break;
            default:
                out << ch;
                break;
        }
    }
    return out.str();
}

bool IsValidModId(const std::string &value) {
    if (value.empty())
        return false;
    return std::all_of(value.begin(), value.end(),
                       [](unsigned char ch) { return std::isalnum(ch) || ch == '_' || ch == '-'; });
}

std::string ReadExistingModId(const fs::path &path) {
    std::ifstream input(path, std::ios::binary);
    if (!input)
        return {};
    std::ostringstream source;
    source << input.rdbuf();
    const std::string text = source.str();
    const std::string marker = "mod_id";
    const size_t name = text.find(marker);
    if (name == std::string::npos)
        return {};
    const size_t equals = text.find('=', name + marker.size());
    if (equals == std::string::npos)
        return {};
    const size_t first_quote = text.find('"', equals + 1);
    if (first_quote == std::string::npos)
        return {};
    const size_t second_quote = text.find('"', first_quote + 1);
    if (second_quote == std::string::npos)
        return {};
    const std::string value = text.substr(first_quote + 1, second_quote - first_quote - 1);
    return IsValidModId(value) ? value : std::string{};
}

std::string GenerateModId(const std::string &project_name) {
    std::random_device device;
    const std::uint32_t suffix = (static_cast<std::uint32_t>(device()) << 16) ^ static_cast<std::uint32_t>(device());
    std::ostringstream out;
    out << Identifier(project_name, "GeneratedMod") << '_' << std::hex << std::nouppercase << std::setw(8)
        << std::setfill('0') << suffix;
    return out.str();
}

std::string ResolveModId(const ModuleProjectOptions &options) {
    if (const std::string existing = ReadExistingModId(options.projectRoot / "mod/config/mod_config.h");
        !existing.empty()) {
        return existing;
    }
    return IsValidModId(options.modId) ? options.modId : GenerateModId(options.projectName);
}

#include "templates/mod_project_generator_unity.inl"
#include "templates/mod_project_generator_runtime.inl"
#include "templates/mod_project_generator_ui.inl"
#include "templates/mod_project_generator_vrchat.inl"

std::string CMakeLists(const ModuleProjectOptions &options, const std::vector<fs::path> &sourceFiles,
                       const std::vector<fs::path> &moduleFiles) {
    std::ostringstream out;
    out << "cmake_minimum_required(VERSION 3.28)\n\n"
        << "if(POLICY CMP0091)\n"
        << "    cmake_policy(SET CMP0091 NEW)\n"
        << "endif()\n\n"
        << "set(VCPKG_APPLOCAL_DEPS OFF CACHE BOOL \"Generated mods keep dependencies linked into the mod DLL when "
           "possible\" FORCE)\n\n"
        << "project(" << options.projectName << " LANGUAGES CXX)\n\n"
        << "set(CMAKE_CXX_STANDARD 20)\n"
        << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n"
        << "set(CMAKE_CXX_EXTENSIONS OFF)\n"
        << "set(CMAKE_EXPORT_COMPILE_COMMANDS ON)\n"
        << "set(CMAKE_CXX_SCAN_FOR_MODULES OFF)\n\n"
        << "if(WIN32)\n"
        << "    set(CMAKE_MSVC_RUNTIME_LIBRARY \"MultiThreaded$<$<CONFIG:Debug>:Debug>\" CACHE STRING \"Use the static Microsoft C/C++ runtime\" FORCE)\n"
        << "endif()\n\n"
        << "if(NOT WIN32)\n"
        << "    message(FATAL_ERROR \"Generated URKit starter mods target Windows native Unity processes.\")\n"
        << "endif()\n"
        << "if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)\n"
        << "    message(FATAL_ERROR \"Generated URKit starter mods support x64 targets only.\")\n"
        << "endif()\n\n"
        << "set(BUILD_SHARED_LIBS OFF CACHE BOOL \"Build third-party dependencies as shared libraries\" FORCE)\n\n"
        << "function(urk_disable_vs_vcpkg_integration target_name)\n"
        << "    if(MSVC)\n"
        << "        set_target_properties(${target_name} PROPERTIES VS_GLOBAL_VcpkgEnabled \"false\")\n"
        << "    endif()\n"
        << "endfunction()\n\n"
        << "include(FetchContent)\n"
        << "FetchContent_Declare(imgui\n"
        << "    GIT_REPOSITORY https://github.com/ocornut/imgui.git\n"
        << "    GIT_TAG v1.92.9b-docking\n"
        << ")\n"
        << "FetchContent_MakeAvailable(imgui)\n"
        << "add_library(imgui STATIC\n"
        << "    ${imgui_SOURCE_DIR}/imgui.cpp\n"
        << "    ${imgui_SOURCE_DIR}/imgui_draw.cpp\n"
        << "    ${imgui_SOURCE_DIR}/imgui_tables.cpp\n"
        << "    ${imgui_SOURCE_DIR}/imgui_widgets.cpp\n"
        << "    # The Win32 backend is compiled through a shim that binds its viewport\n"
        << "    # window class to this module instead of to the game executable, so two\n"
        << "    # injected mods do not contend for a single window class atom.\n"
        << "    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui_win32_module_scope.cpp\n"
        << "    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp\n"
        << "    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx12.cpp\n"
        << "    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp\n"
        << ")\n"
        << "target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)\n"
        << "set_target_properties(imgui PROPERTIES CXX_SCAN_FOR_MODULES OFF)\n"
        << "if(MSVC)\n"
        << "    target_compile_options(imgui PRIVATE /W0 /permissive- /EHsc /utf-8)\n"
        << "endif()\n"
        << "urk_disable_vs_vcpkg_integration(imgui)\n\n"
        << "add_library(" << options.projectName << " SHARED)\n"
        << "set_target_properties(" << options.projectName << " PROPERTIES CXX_SCAN_FOR_MODULES OFF)\n\n";
    out << "target_include_directories(" << options.projectName
        << " PRIVATE . mod";
    for (const auto &directory : options.includeDirectories)
        out << " " << directory;
    out << ")\n\n";
    out << "set(URK_PROJECT_SOURCES\n";
    for (const auto &file : sourceFiles)
        out << "        " << file.generic_string() << "\n";
    for (const auto &file : moduleFiles)
        out << "        " << file.generic_string() << "\n";
    out << ")\n\n"
        << "# User modules under mod are picked up automatically after CMake reconfigures.\n"
        << "file(GLOB_RECURSE URK_USER_MODULE_FILES CONFIGURE_DEPENDS\n"
        << "    RELATIVE \"${CMAKE_CURRENT_SOURCE_DIR}\"\n"
        << "    \"${CMAKE_CURRENT_SOURCE_DIR}/mod/*.h\"\n"
        << "    \"${CMAKE_CURRENT_SOURCE_DIR}/mod/*.hpp\"\n"
        << "    \"${CMAKE_CURRENT_SOURCE_DIR}/mod/*.cpp\"\n"
        << "    \"${CMAKE_CURRENT_SOURCE_DIR}/mod/*.cc\"\n"
        << "    \"${CMAKE_CURRENT_SOURCE_DIR}/mod/*.cxx\"\n"
        << ")\n\n"
        << "list(REMOVE_ITEM URK_USER_MODULE_FILES ${URK_PROJECT_SOURCES})\n\n"
        << "target_sources(" << options.projectName << "\n"
        << "    PRIVATE\n"
        << "        ${URK_PROJECT_SOURCES}\n"
        << "        ${URK_USER_MODULE_FILES}\n"
        << ")\n\n"
        << "source_group(TREE \"${CMAKE_CURRENT_SOURCE_DIR}\" FILES\n"
        << "    ${URK_PROJECT_SOURCES}\n"
        << "    ${URK_USER_MODULE_FILES}\n"
        << ")\n\n"
        << "target_compile_features(" << options.projectName << " PRIVATE cxx_std_20)\n"
        << "target_compile_definitions(" << options.projectName << " PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)\n"
        << "target_link_libraries(" << options.projectName << " PRIVATE imgui d3d11 d3d12 dxgi opengl32)\n"
        << "if(MSVC)\n"
        << "    target_compile_options(" << options.projectName << " PRIVATE /W4 /permissive- /EHsc /utf-8 /bigobj)\n"
        << "    target_link_options(" << options.projectName << " PRIVATE\n"
        << "        \"$<$<NOT:$<CONFIG:Debug>>:/INCREMENTAL:NO>\"\n"
        << "        \"$<$<NOT:$<CONFIG:Debug>>:/OPT:REF>\"\n"
        << "        \"$<$<NOT:$<CONFIG:Debug>>:/OPT:ICF>\"\n"
        << "    )\n"
        << "endif()\n"
        << "urk_disable_vs_vcpkg_integration(" << options.projectName << ")\n\n"
        << "set(URK_DEPLOY_DIR \"" << EscapeString(options.deployDirectory)
        << "\" CACHE PATH \"Directory where the built mod DLL will be copied\")\n"
        << "if(URK_DEPLOY_DIR)\n"
        << "    add_custom_command(TARGET " << options.projectName << " POST_BUILD\n"
        << "        COMMAND ${CMAKE_COMMAND} -E make_directory "
           "\"${URK_DEPLOY_DIR}\"\n"
        << "        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:" << options.projectName
        << "> \"${URK_DEPLOY_DIR}\"\n";
    if (options.enableLocalization) {
        out << "        COMMAND ${CMAKE_COMMAND} -E make_directory \"${URK_DEPLOY_DIR}/locales\"\n"
            << "        COMMAND ${CMAKE_COMMAND} -E copy_directory \"${CMAKE_CURRENT_SOURCE_DIR}/locales\" "
               "\"${URK_DEPLOY_DIR}/locales\"\n";
    }
    out << "    )\n"
        << "endif()\n";
    return out.str();
}

std::string CMakePresets() {
    return R"URK({
  "version": 3,
  "configurePresets": [
    {
      "name": "clang-debug",
      "displayName": "Clang x64 Debug",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/out/build/${presetName}",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug",
        "CMAKE_CXX_COMPILER": "clang++",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
      }
    },
    {
      "name": "clang-release",
      "displayName": "Clang x64 Release",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/out/build/${presetName}",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Release",
        "CMAKE_CXX_COMPILER": "clang++",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
      }
    },
    {
      "name": "msvc-debug",
      "displayName": "MSVC x64 Debug",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/out/build/${presetName}",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug",
        "CMAKE_CXX_COMPILER": "cl",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
      }
    },
    {
      "name": "msvc-release",
      "displayName": "MSVC x64 Release",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/out/build/${presetName}",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Release",
        "CMAKE_CXX_COMPILER": "cl",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
      }
    }
  ],
  "buildPresets": [
    {
      "name": "clang-debug",
      "configurePreset": "clang-debug"
    },
    {
      "name": "clang-release",
      "configurePreset": "clang-release"
    },
    {
      "name": "msvc-debug",
      "configurePreset": "msvc-debug"
    },
    {
      "name": "msvc-release",
      "configurePreset": "msvc-release"
    }
  ]
}
)URK";
}

std::string VsCodeCppProperties() {
    return R"URK({
  "version": 4,
  "configurations": [
    {
      "name": "Win32-Clang",
      "compilerPath": "C:/Program Files/LLVM/bin/clang++.exe",
      "intelliSenseMode": "windows-clang-x64",
      "cppStandard": "c++20",
      "compileCommands": "${workspaceFolder}/out/build/clang-debug/compile_commands.json",
      "mergeConfigurations": true
    },
    {
      "name": "Win32-MSVC",
      "compilerPath": "cl.exe",
      "intelliSenseMode": "windows-msvc-x64",
      "cppStandard": "c++20",
      "compileCommands": "${workspaceFolder}/out/build/msvc-debug/compile_commands.json",
      "mergeConfigurations": true
    }
  ]
}
)URK";
}

std::string Readme(const ModuleProjectOptions &options) {
    std::ostringstream out;
    out << "# " << options.projectName << "\n\n"
        << "Generated URKit " << options.backendDisplayName << " mod project for Windows x64. The output DLL is a "
           "URKit loader plugin, not a standalone injectable DLL.\n\n"
        << "## Build\n\n"
        << "Use Clang from PowerShell or MSVC from an x64 Visual Studio Developer PowerShell.\n\n"
        << "```powershell\n"
        << "cmake --preset clang-debug\n"
        << "cmake --build --preset clang-debug --parallel\n"
        << "# Or, from an x64 Visual Studio Developer PowerShell:\n"
        << "cmake --preset msvc-debug\n"
        << "cmake --build --preset msvc-debug --parallel\n"
        << "```\n\n"
        << "Use `clang-release` or `msvc-release` for release builds. Builds deploy to the selected game's "
           "`Mods` directory.\n\n"
        << "## Documentation\n\n"
        << "See the [URKit SDK Handbook]"
           "(https://github.com/Jadis0x/URKit/blob/main/docs/SDK_HANDBOOK.md) for the complete "
           "GameObject/component API, Unity access, threading, lifecycle, hooks, UI, highlight rendering, "
           "and unload rules. The highlight chapter explains exactly how generated overlays are "
           "submitted through DX11, DX12, or OpenGL.\n\n"
        << "Include `sdk/unity/unity.h` for Unity work. Resolve the target object and component, then access the "
           "required member. Keep Unity calls on the main thread and check `Unity::last_error()` after an "
           "unexpected empty result.\n\n"
        << "## Project files\n\n"
        << "- `mod/lifecycle/mod_runtime.cpp`: game/runtime and main-thread work.\n"
        << "- `mod/hooks/mod_hooks.cpp`: exact, validated hook installation.\n"
        << "- `mod/lifecycle/mod_network.cpp`: HTTPS setup and policy.\n"
        << "- `mod/support/mod_log.cpp`: shared logging.\n"
        << "- `mod/config/mod_config.h` and `mod/ui/theme.h`: metadata and styling.\n\n"
        << "Files under `sdk/`, `mod/generated/`, native hook support, and the build profiles are refreshed by "
           "the generator. Files under `mod/ui/`, other user-owned files, and new sources under `mod/` are "
           "preserved.\n\n"
        << "## MCP runtime tests\n\n"
        << "Generated projects include `sdk/dev_test.h`. Export `URK_DevTestCount`, `URK_DevTestDescribe`, and "
           "`URK_DevTestRun` from a user-owned source under `mod/` to make runtime tests discoverable through "
           "`URKitDevBridge.dll`. The development MCP guide is available in the URKit release documentation.\n\n"
        << "## Runtime constraints\n\n"
        << "- `sdk/mod_sdk.h` is the ABI source of truth. Check version, size, backend, capability, and function "
           "pointers before use.\n"
        << "- Include `sdk/unity/unity.h` for normal Unity work. Use exact overload helpers when required and keep "
           "Unity calls on the main thread.\n"
        << "- Resolve stable metadata once instead of repeating lookups in update/render callbacks.\n"
        << "- Detach hooks, callbacks, coroutines, workers, and UI before unload.\n"
        << "- DX11, DX12, and OpenGL overlays are supported; Vulkan is not.\n";
    for (const std::string &line : options.readmeExtraLayoutLines)
        out << "- " << line << "\n";
    if (options.enableLocalization)
        out << "- Locale JSON files under `locales/` are preserved and deployed beside the DLL.\n";
    out << "\n## Stripped members\n\n"
        << "IL2CPP and Mono builds drop UnityEngine members the game never calls. A stripped member returns "
           "the default value, which reads the same as a legitimate empty result.\n\n"
        << "- Probe first: `Unity::has_method(Unity::GameObjectType, \"get_scene\", 0)`, "
           "`Unity::has_property(type, \"tag\")`, or a wrapper helper such as "
           "`Unity::GameObject::scene_available()`.\n"
        << "- Disable the feature when the member is gone. Do not report the default as data.\n"
        << "- Against your own Unity project, `Assets/link.xml` with "
           "`<linker><assembly fullname=\"UnityEngine.CoreModule\" preserve=\"all\"/></linker>` keeps the "
           "member. A shipped game gives you no such option.\n";
    out << "\nStart troubleshooting with `URKit_logs.log` beside the game executable.\n";
    return out.str();
}

struct PlannedWrite {
    fs::path relativePath;
    OutputFilePolicy policy;
    std::string content;
    bool required = true;
    bool moduleFile = false;
};

OutputFileSpec OutputSpecForWrite(const PlannedWrite &write) {
    return {write.relativePath, write.policy, write.required, write.moduleFile, true};
}

bool WriteIfMissing(const fs::path &path, const std::string &text, std::string *error) {
    std::error_code ec;
    if (fs::exists(path, ec) && !ec)
        return true;
    return WriteText(path, text, error);
}

bool WriteEditablePreserve(const ModuleProjectOptions &options, const PlannedWrite &write,
                           const std::string &content, std::string *error) {
    const fs::path destination = options.projectRoot / write.relativePath;
    std::error_code ec;
    const bool exists = fs::exists(destination, ec);
    if (ec) {
        if (error)
            *error = "cannot inspect editable file " + destination.string() + ": " + ec.message();
        return false;
    }
    if (exists)
        return true;

    return WriteText(destination, content, error);
}

bool WriteByPolicy(const ModuleProjectOptions &options, const PlannedWrite &write, std::string *error) {
    const fs::path destination = options.projectRoot / write.relativePath;
    const std::string content =
        IsSdkOutputPath(write.relativePath) ? StripGeneratedComments(write.content) : write.content;
    switch (write.policy) {
        case OutputFilePolicy::GeneratedOverwrite:
            return WriteText(destination, content, error);
        case OutputFilePolicy::EditablePreserve:
            return options.preserveEditableSources ? WriteEditablePreserve(options, write, content, error)
                                                   : WriteText(destination, content, error);
        case OutputFilePolicy::DocumentationPreserve:
            return WriteIfMissing(destination, content, error);
    }
    if (error)
        *error = "unknown output file policy for " + write.relativePath.generic_string();
    return false;
}

bool IsRegularNonEmpty(const fs::path &path, std::string *error, const char *prefix) {
    std::error_code ec;
    const bool regular = fs::is_regular_file(path, ec);
    const auto size = regular && !ec ? fs::file_size(path, ec) : 0;
    if (!regular || ec || size == 0) {
        if (error)
            *error = std::string(prefix) + ": " + path.string();
        return false;
    }
    return true;
}

void AppendExternalOutputSpecs(const ModuleProjectOptions &options, std::vector<OutputFileSpec> &specs) {
    for (const auto &moduleFile : options.backendModuleFiles) {
        specs.push_back({moduleFile, OutputFilePolicy::GeneratedOverwrite, true, true, false});
    }
    for (const auto &moduleFile : options.extraModuleFiles) {
        specs.push_back({moduleFile, OutputFilePolicy::EditablePreserve, true, true, false});
    }
    for (const auto &extra : options.extraOutputFiles) {
        if (!extra.writtenByCommonGenerator)
            specs.push_back(extra);
    }
}

std::vector<fs::path> CollectModuleFiles(const std::vector<OutputFileSpec> &specs) {
    std::vector<fs::path> moduleFiles;
    for (const auto &file : specs) {
        if (file.moduleFile)
            moduleFiles.push_back(file.relativePath);
    }
    return moduleFiles;
}

bool IsCppSourceFile(const fs::path &path) {
    const std::string extension = path.extension().string();
    return extension == ".cpp" || extension == ".cxx" || extension == ".cc";
}

std::vector<fs::path> CollectSourceFiles(const std::vector<OutputFileSpec> &specs) {
    std::vector<fs::path> sourceFiles;
    for (const auto &file : specs) {
        if (!file.moduleFile && IsCppSourceFile(file.relativePath))
            sourceFiles.push_back(file.relativePath);
    }
    return sourceFiles;
}

bool ValidateOutputSpecs(const fs::path &root, const std::vector<OutputFileSpec> &specs, std::string *error) {
    for (const auto &file : specs) {
        if (!file.required)
            continue;
        if (!IsRegularNonEmpty(root / file.relativePath, error, "missing generated project file"))
            return false;
    }
    return true;
}

bool ReadTextFile(const fs::path &path, std::string &text, std::string *error) {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        if (error)
            *error = "cannot read " + path.string();
        return false;
    }
    std::ostringstream out;
    out << input.rdbuf();
    text = out.str();
    if (text.empty()) {
        if (error)
            *error = "empty file: " + path.string();
        return false;
    }
    return true;
}

bool ReadSdkHeader(const ModuleProjectOptions &options, std::string &text, std::string *error) {
    if (!options.sdkHeaderPath.empty())
        return ReadTextFile(options.sdkHeaderPath, text, error);

    text.assign(kEmbeddedModSdkHeader.begin(), kEmbeddedModSdkHeader.end());
    if (text.empty()) {
        if (error)
            *error = "embedded canonical sdk/mod_sdk.h is empty";
        return false;
    }
    return true;
}

bool LooksLikeCxxSource(const fs::path &path) {
    const std::string extension = path.extension().string();
    return extension == ".h" || extension == ".hpp" || extension == ".cpp" || extension == ".cc";
}

// Keep in sync with the repository's own .clang-format. Generated projects
// ship without the URKit source tree, so the style is inlined here instead
// of looked up on disk.
const wchar_t *ClangFormatStyle() {
    return L"{BasedOnStyle: Microsoft, IndentWidth: 4, TabWidth: 4, UseTab: Never, "
           L"BreakBeforeBraces: Attach, AllowShortIfStatementsOnASingleLine: Never, "
           L"AllowShortLoopsOnASingleLine: false, AllowShortBlocksOnASingleLine: Never, "
           L"AllowShortFunctionsOnASingleLine: None, IndentCaseLabels: true, ColumnLimit: 0, "
           L"SortIncludes: Never}";
}

// Generated code is assembled from concatenated raw strings, so guard clauses
// and cleanup chains land on a single line with no wrapping. This is
// best-effort: a mod author's machine may not have clang-format on PATH, so
// any failure here silently keeps the unformatted text rather than failing
// generation.
void TryFormatCxxSource(const fs::path &destination, std::string &text) {
    if (!LooksLikeCxxSource(destination))
        return;

    std::error_code ec;
    const fs::path tempDir = fs::temp_directory_path(ec);
    if (ec)
        return;
    const fs::path tempFile = tempDir / ("urk-fmt-" + std::to_string(GetCurrentProcessId()) + "-" +
                                         std::to_string(reinterpret_cast<std::uintptr_t>(&text)) +
                                         destination.extension().string());

    {
        std::ofstream tempOutput(tempFile, std::ios::binary | std::ios::trunc);
        tempOutput << text;
        if (!tempOutput)
            return;
    }

    std::wstring commandLine =
        L"clang-format.exe -i -style=\"" + std::wstring(ClangFormatStyle()) + L"\" \"" + tempFile.wstring() + L"\"";

    SECURITY_ATTRIBUTES inheritableHandle{};
    inheritableHandle.nLength = sizeof(inheritableHandle);
    inheritableHandle.bInheritHandle = TRUE;
    HANDLE nul = CreateFileW(L"NUL", GENERIC_WRITE, FILE_SHARE_WRITE, &inheritableHandle, OPEN_EXISTING, 0, nullptr);
    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);
    if (nul != INVALID_HANDLE_VALUE) {
        startupInfo.dwFlags = STARTF_USESTDHANDLES;
        startupInfo.hStdOutput = nul;
        startupInfo.hStdError = nul;
    }
    PROCESS_INFORMATION processInfo{};
    const BOOL started = CreateProcessW(nullptr, commandLine.data(), nullptr, nullptr,
                                        nul != INVALID_HANDLE_VALUE, CREATE_NO_WINDOW, nullptr, nullptr, &startupInfo,
                                        &processInfo);
    if (nul != INVALID_HANDLE_VALUE)
        CloseHandle(nul);

    if (started) {
        const DWORD waitResult = WaitForSingleObject(processInfo.hProcess, 5000);
        DWORD exitCode = 1;
        if (waitResult == WAIT_OBJECT_0 && GetExitCodeProcess(processInfo.hProcess, &exitCode) && exitCode == 0) {
            std::ifstream formattedFile(tempFile, std::ios::binary);
            if (formattedFile.is_open()) {
                std::ostringstream buffer;
                buffer << formattedFile.rdbuf();
                text = buffer.str();
            }
        } else if (waitResult == WAIT_TIMEOUT) {
            TerminateProcess(processInfo.hProcess, 1);
        }
        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
    }

    fs::remove(tempFile, ec);
}

} // namespace

std::string StripGeneratedComments(const std::string &in) {
    std::string out;
    out.reserve(in.size());

    const std::size_t n = in.size();
    std::size_t i = 0;
    // Indentation is held back until the line is known to contain code. Dropping
    // it eagerly, which this did at first, stripped the indentation off every
    // line in the SDK rather than just off the ones that were only comments.
    bool atLineStart = true;
    std::string pendingIndent;

    const auto emit = [&](char c) {
        if (atLineStart && (c == ' ' || c == '\t')) {
            pendingIndent += c;
            return;
        }
        if (atLineStart) {
            out += pendingIndent;
            pendingIndent.clear();
        }
        out += c;
        atLineStart = c == '\n';
        if (atLineStart) pendingIndent.clear();
    };
    const auto dropLine = [&]() { pendingIndent.clear(); };

    while (i < n) {
        const char c = in[i];

        // A raw string literal can hold anything, including // and /*, so it has
        // to be consumed whole rather than scanned.
        if (c == 'R' && i + 1 < n && in[i + 1] == '"') {
            const std::size_t open = in.find('(', i + 2);
            if (open != std::string::npos) {
                const std::string delimiter = in.substr(i + 2, open - (i + 2));
                const std::string terminator = ")" + delimiter + "\"";
                const std::size_t close = in.find(terminator, open);
                const std::size_t end = close == std::string::npos ? n : close + terminator.size();
                for (std::size_t k = i; k < end; ++k) emit(in[k]);
                i = end;
                continue;
            }
        }

        if (c == '"' || c == '\'') {
            const char quote = c;
            emit(c);
            ++i;
            while (i < n) {
                if (in[i] == '\\' && i + 1 < n) {
                    emit(in[i]);
                    emit(in[i + 1]);
                    i += 2;
                    continue;
                }
                emit(in[i]);
                const bool closed = in[i] == quote;
                ++i;
                if (closed) break;
            }
            continue;
        }

        if (c == '/' && i + 1 < n && in[i + 1] == '/') {
            dropLine();
            while (i < n && in[i] != '\n') ++i;
            continue;
        }

        if (c == '/' && i + 1 < n && in[i + 1] == '*') {
            dropLine();
            i += 2;
            while (i + 1 < n && !(in[i] == '*' && in[i + 1] == '/')) {
                if (in[i] == '\n') emit('\n');
                ++i;
            }
            i = (i + 1 < n) ? i + 2 : n;
            continue;
        }

        emit(c);
        ++i;
    }
    dropLine();

    // A stripped block leaves a run of blank lines. Collapse it so the result
    // reads as deliberate rather than as something taken out.
    std::string collapsed;
    collapsed.reserve(out.size());
    int blankRun = 0;
    for (const char k : out) {
        if (k == '\n') {
            ++blankRun;
            if (blankRun <= 1) collapsed += k;
        } else {
            blankRun = 0;
            collapsed += k;
        }
    }
    return collapsed;
}

bool IsSdkOutputPath(const fs::path &relativePath) {
    return relativePath.generic_string().rfind("sdk/", 0) == 0;
}

std::string Identifier(const std::string &text, const char *fallback) {
    std::string out;
    for (unsigned char ch : text) {
        if (std::isalnum(ch) || ch == '_')
            out.push_back(static_cast<char>(ch));
        else if (!out.empty() && out.back() != '_')
            out.push_back('_');
    }
    while (!out.empty() && out.front() == '_')
        out.erase(out.begin());
    while (!out.empty() && out.back() == '_')
        out.pop_back();
    if (out.empty() || std::isdigit(static_cast<unsigned char>(out.front())) || IsCppKeyword(out))
        out = fallback && *fallback ? fallback : "GeneratedMod";
    return out;
}

bool WriteText(const fs::path &path, const std::string &text, std::string *error) {
    std::error_code ec;
    if (const auto parent = path.parent_path(); !parent.empty()) {
        fs::create_directories(parent, ec);
        if (ec) {
            if (error)
                *error = "cannot create " + parent.string() + ": " + ec.message();
            return false;
        }
    }
    std::string formatted = text;
    TryFormatCxxSource(path, formatted);
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    output << formatted;
    if (!output) {
        if (error)
            *error = "cannot write " + path.string();
        return false;
    }
    return true;
}

bool WriteModuleProject(const ModuleProjectOptions &options, std::string *error) {
    ModuleProjectOptions resolved_options = options;
    resolved_options.modId = ResolveModId(options);
    const ModuleProjectOptions &project = resolved_options;
    std::string sdkHeader;
    if (!ReadSdkHeader(project, sdkHeader, error))
        return false;
    std::string devTestHeader(kEmbeddedDevTestHeader.begin(), kEmbeddedDevTestHeader.end());
    if (devTestHeader.empty()) {
        if (error)
            *error = "embedded canonical sdk/dev_test.h is empty";
        return false;
    }
    const UnityModuleSet unityModules = BuildUnityModuleSet(project);
    std::vector<PlannedWrite> writes = {
        {"sdk/mod_sdk.h", OutputFilePolicy::GeneratedOverwrite, sdkHeader, true, false},
        {"sdk/dev_test.h", OutputFilePolicy::GeneratedOverwrite, devTestHeader, true, true},
        {"sdk/runtime_api.h", OutputFilePolicy::GeneratedOverwrite, ModSdkModule(), true, true},
        {"sdk/runtime_bootstrap.h", OutputFilePolicy::GeneratedOverwrite, RuntimeBootstrapModule(project), true, true},
        {"sdk/hook_api.h", OutputFilePolicy::GeneratedOverwrite, HooksRuntimeModule(), true, true},
        {"sdk/network_api.h", OutputFilePolicy::GeneratedOverwrite, NetworkRuntimeModule(), true, true},
        {"sdk/events.h", OutputFilePolicy::GeneratedOverwrite, EventsModule(), true, true},
        {"sdk/coroutines.h", OutputFilePolicy::GeneratedOverwrite, CoroutinesModule(), true, true},
        {"sdk/mod_async.h", OutputFilePolicy::GeneratedOverwrite, ModAsyncModule(), true, true},
        {"mod/config/RuntimeConfig.h", OutputFilePolicy::GeneratedOverwrite, RuntimeConfigModule(), true, true},
        {"sdk/unity/unity.h", OutputFilePolicy::GeneratedOverwrite, unityModules.publicHeader, true, true},
        {"sdk/unity/unity_types.h", OutputFilePolicy::GeneratedOverwrite, unityModules.types, true, true},
        {"sdk/unity/unity_invoke.h", OutputFilePolicy::GeneratedOverwrite, unityModules.invoke, true, true},
        {"sdk/unity/unity_components.h", OutputFilePolicy::GeneratedOverwrite, unityModules.components, true, true},
        {"sdk/unity/unity_inspect.h", OutputFilePolicy::GeneratedOverwrite, unityModules.inspect, true, true},
        {"sdk/unity/unity_shortcuts.h", OutputFilePolicy::GeneratedOverwrite, unityModules.shortcuts, true, true},
        {"sdk/VRChat/VRC/SDKBase/VRCPlayerAPI.h", OutputFilePolicy::GeneratedOverwrite, VRChatPlayerApiModule(), true, true},
        {"sdk/VRChat/VRC/SDKBase/Networking.h", OutputFilePolicy::GeneratedOverwrite, VRChatNetworkingModule(), true, true},
        {"sdk/VRChat/VRC/Core/APIUser.h", OutputFilePolicy::GeneratedOverwrite, VRChatApiUserModule(), true, true},
        {"sdk/VRChat/VRC/Localization/LocalizableStringExtensions.h", OutputFilePolicy::GeneratedOverwrite,
         VRChatLocalizableStringExtensionsModule(), true, true},
        {"sdk/VRChat/VRC/Udon/UdonBehaviour.h", OutputFilePolicy::GeneratedOverwrite, VRChatUdonBehaviourModule(), true,
         true},
        {"sdk/VRChat/VRC/SDKBase/VRC_Pickup.h", OutputFilePolicy::GeneratedOverwrite, VRChatVrcPickupModule(), true,
         true},
        {"sdk/VRChat/VRC/SDK3/Components/VRCPickup.h", OutputFilePolicy::GeneratedOverwrite,
         VRChatVrcPickupSdk3Module(), true, true},
        {"sdk/VRChat/HighlightsFX.h", OutputFilePolicy::GeneratedOverwrite, VRChatHighlightsFxModule(), true, true},
        {"sdk/VRChat/Menus.h", OutputFilePolicy::GeneratedOverwrite, VRChatMenusModule(), true, true},
        {"sdk/VRChat/VRC/SDKBase/VRC_SceneDescriptor.h", OutputFilePolicy::GeneratedOverwrite,
         VRChatGeneratedSceneDescriptor(), true, true},
        {"sdk/VRChat/VRC/SDKBase/VRC_Serialization.h", OutputFilePolicy::GeneratedOverwrite,
         VRChatGeneratedSerialization(), true, true},
        {"sdk/VRChat/VRC/SDKBase/VRC_AvatarPedestal.h", OutputFilePolicy::GeneratedOverwrite,
         VRChatGeneratedAvatarPedestal(), true, true},
        {"sdk/VRChat/VRC/SDKBase/VRC_SpatialAudioSource.h", OutputFilePolicy::GeneratedOverwrite,
         VRChatGeneratedSpatialAudio(), true, true},
        {"sdk/VRChat/VRC/SDKBase/VRC_StereoObject.h", OutputFilePolicy::GeneratedOverwrite,
         VRChatGeneratedStereoObject(), true, true},
        {"sdk/VRChat/VRC/SDKBase/VRCLayers.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedLayers(), true,
         true},
        {"sdk/VRChat/VRC/Core/UnityVersion.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedUnityVersion(),
         true, true},
        {"sdk/VRChat/VRC/Core/Endpoints.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedEndpoints(), true,
         true},
        {"sdk/VRChat/VRC/Core/Logger.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedLogger(), true, true},
        {"sdk/VRChat/VRC/Core/ConfigManager.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedConfigManager(),
         true, true},
        {"sdk/VRChat/VRC/Core/VRCLogger.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcLogger(), true,
         true},

// >>> VRCHAT_GENERATED_WRITES >>>
    {"sdk/VRChat/VRC/Core/AnalyticsEventOptions.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAnalyticsEventOptions(), true, true},
    {"sdk/VRChat/VRC/Core/AnalyticsInterface.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAnalyticsInterface(), true, true},
    {"sdk/VRChat/VRC/Core/AnalyticsSDK.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAnalyticsSDK(), true, true},
    {"sdk/VRChat/VRC/Core/Annotations/VRChatInternalAPI.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRChatInternalAPI(), true, true},
    {"sdk/VRChat/VRC/Core/API.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPI(), true, true},
    {"sdk/VRChat/VRC/Core/API2FA.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPI2FA(), true, true},
    {"sdk/VRChat/VRC/Core/ApiAccountUpgrade.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiAccountUpgrade(), true, true},
    {"sdk/VRChat/VRC/Core/APIActivationSuccessfulTargetResult.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPIActivationSuccessfulTargetResult(), true, true},
    {"sdk/VRChat/VRC/Core/ApiAdminAssetBundle.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiAdminAssetBundle(), true, true},
    {"sdk/VRChat/VRC/Core/ApiAdminAssetBundleFile.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiAdminAssetBundleFile(), true, true},
    {"sdk/VRChat/VRC/Core/ApiAuthContinue.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiAuthContinue(), true, true},
    {"sdk/VRChat/VRC/Core/ApiAvatar.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiAvatar(), true, true},
    {"sdk/VRChat/VRC/Core/ApiAvatarModeration.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiAvatarModeration(), true, true},
    {"sdk/VRChat/VRC/Core/ApiAvatarStyle.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiAvatarStyle(), true, true},
    {"sdk/VRChat/VRC/Core/ApiBadge.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiBadge(), true, true},
    {"sdk/VRChat/VRC/Core/ApiBindingCommandReference.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiBindingCommandReference(), true, true},
    {"sdk/VRChat/VRC/Core/ApiCache.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiCache(), true, true},
    {"sdk/VRChat/VRC/Core/ApiCacheObject.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiCacheObject(), true, true},
    {"sdk/VRChat/VRC/Core/ApiCalendarEntriesContainer.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiCalendarEntriesContainer(), true, true},
    {"sdk/VRChat/VRC/Core/APICalendarEntry.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPICalendarEntry(), true, true},
    {"sdk/VRChat/VRC/Core/APICalendarEntryUserInterest.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPICalendarEntryUserInterest(), true, true},
    {"sdk/VRChat/VRC/Core/ApiCalendarResults.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiCalendarResults(), true, true},
    {"sdk/VRChat/VRC/Core/ApiCampaignAnonymizationStatus.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiCampaignAnonymizationStatus(), true, true},
    {"sdk/VRChat/VRC/Core/ApiCampaignContributor.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiCampaignContributor(), true, true},
    {"sdk/VRChat/VRC/Core/APICampaignInfo.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPICampaignInfo(), true, true},
    {"sdk/VRChat/VRC/Core/ApiCampaignReward.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiCampaignReward(), true, true},
    {"sdk/VRChat/VRC/Core/ApiCampaignRewardProgression.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiCampaignRewardProgression(), true, true},
    {"sdk/VRChat/VRC/Core/ApiCertificateVerifier.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiCertificateVerifier(), true, true},
    {"sdk/VRChat/VRC/Core/ApiContainer.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiContainer(), true, true},
    {"sdk/VRChat/VRC/Core/ApiCredentials.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiCredentials(), true, true},
    {"sdk/VRChat/VRC/Core/ApiDictContainer.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiDictContainer(), true, true},
    {"sdk/VRChat/VRC/Core/ApiDroneSkin.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiDroneSkin(), true, true},
    {"sdk/VRChat/VRC/Core/ApiEconomyStatus.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiEconomyStatus(), true, true},
    {"sdk/VRChat/VRC/Core/ApiEconomyStore.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiEconomyStore(), true, true},
    {"sdk/VRChat/VRC/Core/APIEmoji.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPIEmoji(), true, true},
    {"sdk/VRChat/VRC/Core/ApiFieldAttribute.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiFieldAttribute(), true, true},
    {"sdk/VRChat/VRC/Core/ApiFile.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiFile(), true, true},
    {"sdk/VRChat/VRC/Core/APIGiftBundle.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPIGiftBundle(), true, true},
    {"sdk/VRChat/VRC/Core/APIGroup.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPIGroup(), true, true},
    {"sdk/VRChat/VRC/Core/APIGroupAnnouncement.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPIGroupAnnouncement(), true, true},
    {"sdk/VRChat/VRC/Core/ApiGroupGalleryImage.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiGroupGalleryImage(), true, true},
    {"sdk/VRChat/VRC/Core/APIGroupInvite.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPIGroupInvite(), true, true},
    {"sdk/VRChat/VRC/Core/APIGroupJoinResponse.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPIGroupJoinResponse(), true, true},
    {"sdk/VRChat/VRC/Core/APIGroupList.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPIGroupList(), true, true},
    {"sdk/VRChat/VRC/Core/APIGroupLocations.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPIGroupLocations(), true, true},
    {"sdk/VRChat/VRC/Core/APIGroupMember.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPIGroupMember(), true, true},
    {"sdk/VRChat/VRC/Core/APIGroupMemberList.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPIGroupMemberList(), true, true},
    {"sdk/VRChat/VRC/Core/APIGroupPosts.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPIGroupPosts(), true, true},
    {"sdk/VRChat/VRC/Core/APIGroupRole.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPIGroupRole(), true, true},
    {"sdk/VRChat/VRC/Core/APIGroupRoleList.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPIGroupRoleList(), true, true},
    {"sdk/VRChat/VRC/Core/ApiImage.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiImage(), true, true},
    {"sdk/VRChat/VRC/Core/ApiInfoPushSystem.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiInfoPushSystem(), true, true},
    {"sdk/VRChat/VRC/Core/ApiInventoryBundle.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiInventoryBundle(), true, true},
    {"sdk/VRChat/VRC/Core/ApiInventoryBundleDrop.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiInventoryBundleDrop(), true, true},
    {"sdk/VRChat/VRC/Core/ApiInventoryItem.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiInventoryItem(), true, true},
    {"sdk/VRChat/VRC/Core/ApiInventoryItemContainer.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiInventoryItemContainer(), true, true},
    {"sdk/VRChat/VRC/Core/ApiInventoryJweToken.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiInventoryJweToken(), true, true},
    {"sdk/VRChat/VRC/Core/ApiLedgerTransaction.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiLedgerTransaction(), true, true},
    {"sdk/VRChat/VRC/Core/ApiLedgerTransactions.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiLedgerTransactions(), true, true},
    {"sdk/VRChat/VRC/Core/ApiLicense.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiLicense(), true, true},
    {"sdk/VRChat/VRC/Core/ApiLicenseNote.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiLicenseNote(), true, true},
    {"sdk/VRChat/VRC/Core/ApiListContainer.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiListContainer(), true, true},
    {"sdk/VRChat/VRC/Core/ApiLocalizableString.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiLocalizableString(), true, true},
    {"sdk/VRChat/VRC/Core/ApiMessage.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiMessage(), true, true},
    {"sdk/VRChat/VRC/Core/ApiModel.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiModel(), true, true},
    {"sdk/VRChat/VRC/Core/ApiModeration.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiModeration(), true, true},
    {"sdk/VRChat/VRC/Core/ApiMutualFriend.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiMutualFriend(), true, true},
    {"sdk/VRChat/VRC/Core/ApiMutualGroup.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiMutualGroup(), true, true},
    {"sdk/VRChat/VRC/Core/ApiNotification.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiNotification(), true, true},
    {"sdk/VRChat/VRC/Core/ApiOnlineMode.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiOnlineMode(), true, true},
    {"sdk/VRChat/VRC/Core/ApiPagedTransactions.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiPagedTransactions(), true, true},
    {"sdk/VRChat/VRC/Core/ApiPendingTransaction.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiPendingTransaction(), true, true},
    {"sdk/VRChat/VRC/Core/ApiPlayerModeration.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiPlayerModeration(), true, true},
    {"sdk/VRChat/VRC/Core/ApiPortalSkin.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiPortalSkin(), true, true},
    {"sdk/VRChat/VRC/Core/ApiPrint.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiPrint(), true, true},
    {"sdk/VRChat/VRC/Core/ApiProduct.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiProduct(), true, true},
    {"sdk/VRChat/VRC/Core/APIProductGiftingCheck.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPIProductGiftingCheck(), true, true},
    {"sdk/VRChat/VRC/Core/ApiProductPurchaseStatus.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiProductPurchaseStatus(), true, true},
    {"sdk/VRChat/VRC/Core/ApiProductVariant.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiProductVariant(), true, true},
    {"sdk/VRChat/VRC/Core/ApiProp.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiProp(), true, true},
    {"sdk/VRChat/VRC/Core/ApiPropItemMetadata.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiPropItemMetadata(), true, true},
    {"sdk/VRChat/VRC/Core/ApiPurchase.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiPurchase(), true, true},
    {"sdk/VRChat/VRC/Core/ApiPurchaseCancelSubscriptionInfo.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiPurchaseCancelSubscriptionInfo(), true, true},
    {"sdk/VRChat/VRC/Core/APIPurchasedGiftBundle.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPIPurchasedGiftBundle(), true, true},
    {"sdk/VRChat/VRC/Core/ApiPurchaseSubscriptionCancelledInfo.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiPurchaseSubscriptionCancelledInfo(), true, true},
    {"sdk/VRChat/VRC/Core/APIQueue.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPIQueue(), true, true},
    {"sdk/VRChat/VRC/Core/ApiReport.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiReport(), true, true},
    {"sdk/VRChat/VRC/Core/ApiReportDetails.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiReportDetails(), true, true},
    {"sdk/VRChat/VRC/Core/ApiReportResponse.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiReportResponse(), true, true},
    {"sdk/VRChat/VRC/Core/ApiReportsContainer.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiReportsContainer(), true, true},
    {"sdk/VRChat/VRC/Core/APIResponseHandler.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPIResponseHandler(), true, true},
    {"sdk/VRChat/VRC/Core/ApiSearchGroupResults.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiSearchGroupResults(), true, true},
    {"sdk/VRChat/VRC/Core/ApiSearchUserResults.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiSearchUserResults(), true, true},
    {"sdk/VRChat/VRC/Core/ApiSearchWorldResults.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiSearchWorldResults(), true, true},
    {"sdk/VRChat/VRC/Core/ApiServerEnvironment.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiServerEnvironment(), true, true},
    {"sdk/VRChat/VRC/Core/ApiSharedConnectionCounts.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiSharedConnectionCounts(), true, true},
    {"sdk/VRChat/VRC/Core/ApiSteamVRChatFinalizeTransactionResponse.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiSteamVRChatFinalizeTransactionResponse(), true, true},
    {"sdk/VRChat/VRC/Core/ApiSteamVRChatSubscriptionSteamTransaction.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiSteamVRChatSubscriptionSteamTransaction(), true, true},
    {"sdk/VRChat/VRC/Core/ApiSteamVRChatSubscriptionTransaction.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiSteamVRChatSubscriptionTransaction(), true, true},
    {"sdk/VRChat/VRC/Core/ApiSteamVRChatSubscriptionWalletInformation.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiSteamVRChatSubscriptionWalletInformation(), true, true},
    {"sdk/VRChat/VRC/Core/APISticker.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPISticker(), true, true},
    {"sdk/VRChat/VRC/Core/ApiStoreShelf.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiStoreShelf(), true, true},
    {"sdk/VRChat/VRC/Core/ApiStringContainer.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiStringContainer(), true, true},
    {"sdk/VRChat/VRC/Core/APISubscription.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPISubscription(), true, true},
    {"sdk/VRChat/VRC/Core/ApiTokenBundle.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiTokenBundle(), true, true},
    {"sdk/VRChat/VRC/Core/ApiTransaction.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiTransaction(), true, true},
    {"sdk/VRChat/VRC/Core/APITutorial.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPITutorial(), true, true},
    {"sdk/VRChat/VRC/Core/APIUIColorPalette.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPIUIColorPalette(), true, true},
    {"sdk/VRChat/VRC/Core/ApiUserIcon.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiUserIcon(), true, true},
    {"sdk/VRChat/VRC/Core/ApiUserPermission.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiUserPermission(), true, true},
    {"sdk/VRChat/VRC/Core/ApiUserPermissions.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiUserPermissions(), true, true},
    {"sdk/VRChat/VRC/Core/ApiUserPlatformList.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiUserPlatformList(), true, true},
    {"sdk/VRChat/VRC/Core/ApiUserPlatforms.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiUserPlatforms(), true, true},
    {"sdk/VRChat/VRC/Core/ApiViewfinderSkin.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiViewfinderSkin(), true, true},
    {"sdk/VRChat/VRC/Core/ApiVRChatAdminSubscriptionTransaction.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiVRChatAdminSubscriptionTransaction(), true, true},
    {"sdk/VRChat/VRC/Core/ApiVRChatProductDetails.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiVRChatProductDetails(), true, true},
    {"sdk/VRChat/VRC/Core/ApiVRChatSubscription.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiVRChatSubscription(), true, true},
    {"sdk/VRChat/VRC/Core/ApiVRChatSubscriptionBaseResponse.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiVRChatSubscriptionBaseResponse(), true, true},
    {"sdk/VRChat/VRC/Core/ApiVRChatSubscriptionError.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiVRChatSubscriptionError(), true, true},
    {"sdk/VRChat/VRC/Core/ApiWarpEffectSkin.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiWarpEffectSkin(), true, true},
    {"sdk/VRChat/VRC/Core/ApiWorld.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiWorld(), true, true},
    {"sdk/VRChat/VRC/Core/ApiWorldInstance.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcApiWorldInstance(), true, true},
    {"sdk/VRChat/VRC/Core/AreaBase.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAreaBase(), true, true},
    {"sdk/VRChat/VRC/Core/AssetVersion.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAssetVersion(), true, true},
    {"sdk/VRChat/VRC/Core/BaseConfig.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcBaseConfig(), true, true},
    {"sdk/VRChat/VRC/Core/Burst/DisposableJobHandle.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcDisposableJobHandle(), true, true},
    {"sdk/VRChat/VRC/Core/CameraSystems/StackedCamera.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcStackedCamera(), true, true},
    {"sdk/VRChat/VRC/Core/CaptchaArea.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcCaptchaArea(), true, true},
    {"sdk/VRChat/VRC/Core/Config/Interfaces/IReadOnlyConfig.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIReadOnlyConfig(), true, true},
    {"sdk/VRChat/VRC/Core/Config/Interfaces/IReadWriteConfig.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIReadWriteConfig(), true, true},
    {"sdk/VRChat/VRC/Core/DiscordAccessTokenResponse.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcDiscordAccessTokenResponse(), true, true},
    {"sdk/VRChat/VRC/Core/DiscordDeviceCodeResponse.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcDiscordDeviceCodeResponse(), true, true},
    {"sdk/VRChat/VRC/Core/ExtensionMethods.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcExtensionMethods(), true, true},
    {"sdk/VRChat/VRC/Core/FavoriteArea.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcFavoriteArea(), true, true},
    {"sdk/VRChat/VRC/Core/FavoriteListModel.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcFavoriteListModel(), true, true},
    {"sdk/VRChat/VRC/Core/FavoriteModel.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcFavoriteModel(), true, true},
    {"sdk/VRChat/VRC/Core/FavoritePrivacy.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcFavoritePrivacy(), true, true},
    {"sdk/VRChat/VRC/Core/FavoritePrivacyExtensions.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcFavoritePrivacyExtensions(), true, true},
    {"sdk/VRChat/VRC/Core/FavoriteType.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcFavoriteType(), true, true},
    {"sdk/VRChat/VRC/Core/FavoriteTypeExtensions.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcFavoriteTypeExtensions(), true, true},
    {"sdk/VRChat/VRC/Core/GroupInstanceAccessType.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcGroupInstanceAccessType(), true, true},
    {"sdk/VRChat/VRC/Core/IBundleSignatureHolder.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIBundleSignatureHolder(), true, true},
    {"sdk/VRChat/VRC/Core/ILoggerReceiver.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcILoggerReceiver(), true, true},
    {"sdk/VRChat/VRC/Core/InstanceAccessType.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcInstanceAccessType(), true, true},
    {"sdk/VRChat/VRC/Core/InstanceAccessTypeExtensions.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcInstanceAccessTypeExtensions(), true, true},
    {"sdk/VRChat/VRC/Core/IVRCLogger.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIVRCLogger(), true, true},
    {"sdk/VRChat/VRC/Core/LicenseType.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcLicenseType(), true, true},
    {"sdk/VRChat/VRC/Core/ListingType.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcListingType(), true, true},
    {"sdk/VRChat/VRC/Core/LocalConfig.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcLocalConfig(), true, true},
    {"sdk/VRChat/VRC/Core/LoggingMode.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcLoggingMode(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/Codec/StateManagement/AbstractObjectStateManager.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAbstractObjectStateManager(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/Codec/StateManagement/PersistenceObjectStateManager.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPersistenceObjectStateManager(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/Codec/StateManagement/SimpleObjectStateManager.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcSimpleObjectStateManager(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/DecodeParameters32.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcDecodeParameters32(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/DecodeParameters8.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcDecodeParameters8(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/FixedByteBufferAllocator32.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcFixedByteBufferAllocator32(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/FixedByteBufferAllocator8.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcFixedByteBufferAllocator8(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/FlatBufferConfig.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcFlatBufferConfig(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/FlatBufferSerializerCodec.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcFlatBufferSerializerCodec(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/IEvent.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIEvent(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/IFlatBufferNetworkSerializer.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIFlatBufferNetworkSerializer(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/ILoggableClass.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcILoggableClass(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/INetworkReadyReceiver.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcINetworkReadyReceiver(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/ISyncPhysics.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcISyncPhysics(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/ITimedValue.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcITimedValue(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/ITweenableValue.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcITweenableValue(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/IVRC_FlatBufferSerializer.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIVRC_FlatBufferSerializer(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/IVRC_PersistentSerializer.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIVRC_PersistentSerializer(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/NetworkUpdateRates.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcNetworkUpdateRates(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/NumericExtensions.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcNumericExtensions(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/Pose/Configuration.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcConfiguration(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/Pose/DirectQuantizedPose.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcDirectQuantizedPose(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/Pose/IPoseRecorder.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIPoseRecorder(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/Pose/PoseContents.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPoseContents(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/Pose/PoseEvent.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPoseEvent(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/Pose/QuantizedPose.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcQuantizedPose(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/PositionEvent.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPositionEvent(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/QuantizedSerialization.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcQuantizedSerialization(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/RoomNetworkProperty.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcRoomNetworkProperty(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/TimeProxy.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcTimeProxy(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/Tools/BitConverterSpan.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcBitConverterSpan(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/Tools/BunchCollection.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcBunchCollection(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/Tools/ByteManipulation.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcByteManipulation(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/Tools/Hash.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcHash(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/Tween/AnimationEvent.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAnimationEvent(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/Tween/AnimatorEvent.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAnimatorEvent(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/Tween/TweenFunctions.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcTweenFunctions(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/VRCNetworkBehaviourTypeId.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCNetworkBehaviourTypeId(), true, true},
    {"sdk/VRChat/VRC/Core/Networking/VRCPhotonEvent.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCPhotonEvent(), true, true},
    {"sdk/VRChat/VRC/Core/NetworkRegion.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcNetworkRegion(), true, true},
    {"sdk/VRChat/VRC/Core/NetworkRegionExtensions.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcNetworkRegionExtensions(), true, true},
    {"sdk/VRChat/VRC/Core/NullLogger.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcNullLogger(), true, true},
    {"sdk/VRChat/VRC/Core/PagedApiCalendarResult.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPagedApiCalendarResult(), true, true},
    {"sdk/VRChat/VRC/Core/PagedApiGroupResult.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPagedApiGroupResult(), true, true},
    {"sdk/VRChat/VRC/Core/ProductType.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcProductType(), true, true},
    {"sdk/VRChat/VRC/Core/RemoteConfig.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcRemoteConfig(), true, true},
    {"sdk/VRChat/VRC/Core/Services/APIEventSource.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPIEventSource(), true, true},
    {"sdk/VRChat/VRC/Core/Services/APIEventSourceMethods.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAPIEventSourceMethods(), true, true},
    {"sdk/VRChat/VRC/Core/Services/AvatarsService.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAvatarsService(), true, true},
    {"sdk/VRChat/VRC/Core/Services/EconomyService.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcEconomyService(), true, true},
    {"sdk/VRChat/VRC/Core/Services/EventsService.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcEventsService(), true, true},
    {"sdk/VRChat/VRC/Core/Services/GroupsService.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcGroupsService(), true, true},
    {"sdk/VRChat/VRC/Core/Services/IAvatarsService.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIAvatarsService(), true, true},
    {"sdk/VRChat/VRC/Core/Services/IEventsService.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIEventsService(), true, true},
    {"sdk/VRChat/VRC/Core/Services/IGroupsService.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIGroupsService(), true, true},
    {"sdk/VRChat/VRC/Core/Services/StoresService.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcStoresService(), true, true},
    {"sdk/VRChat/VRC/Core/Source/Config/NoAllocByteSetStorage.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcNoAllocByteSetStorage(), true, true},
    {"sdk/VRChat/VRC/Core/TimeInterval.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcTimeInterval(), true, true},
    {"sdk/VRChat/VRC/Core/UdonAnalytics/IUdonAnalyticsEvent.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIUdonAnalyticsEvent(), true, true},
    {"sdk/VRChat/VRC/Core/UdonAnalytics/UdonAnalyticsCache.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcUdonAnalyticsCache(), true, true},
    {"sdk/VRChat/VRC/Core/UdonAnalytics/UdonAnalyticsEvent.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcUdonAnalyticsEvent(), true, true},
    {"sdk/VRChat/VRC/Core/UpdateDelegator.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcUpdateDelegator(), true, true},
    {"sdk/VRChat/VRC/Core/UrlAllowlistConfig.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcUrlAllowlistConfig(), true, true},
    {"sdk/VRChat/VRC/Core/VRCEvent.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCEvent(), true, true},
    {"sdk/VRChat/VRC/Core/VRChatTestProtocol.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRChatTestProtocol(), true, true},
    {"sdk/VRChat/VRC/Core/VTP_PacketID.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVTP_PacketID(), true, true},
    {"sdk/VRChat/VRC/Core/ZLoggerHandlerLogger.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcZLoggerHandlerLogger(), true, true},
    {"sdk/VRChat/VRC/Dynamics/AimVRCConstraintBinding.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAimVRCConstraintBinding(), true, true},
    {"sdk/VRChat/VRC/Dynamics/AnimParameterAccessAvatarSDK.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAnimParameterAccessAvatarSDK(), true, true},
    {"sdk/VRChat/VRC/Dynamics/BoneBuffer.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcBoneBuffer(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ChainBuffer.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcChainBuffer(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ChainId.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcChainId(), true, true},
    {"sdk/VRChat/VRC/Dynamics/CollisionBroadphase_HashGrid.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcCollisionBroadphase_HashGrid(), true, true},
    {"sdk/VRChat/VRC/Dynamics/CollisionBroadphase_HybridSAP.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcCollisionBroadphase_HybridSAP(), true, true},
    {"sdk/VRChat/VRC/Dynamics/CollisionScene.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcCollisionScene(), true, true},
    {"sdk/VRChat/VRC/Dynamics/CollisionShapes.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcCollisionShapes(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ContactBase.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcContactBase(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ContactEnterInfo.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcContactEnterInfo(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ContactExitInfo.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcContactExitInfo(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ContactManager.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcContactManager(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ContactReceiver.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcContactReceiver(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ContactReceiverProxy.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcContactReceiverProxy(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ContactSender.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcContactSender(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ContactSenderProxy.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcContactSenderProxy(), true, true},
    {"sdk/VRChat/VRC/Dynamics/DynamicsComponent.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcDynamicsComponent(), true, true},
    {"sdk/VRChat/VRC/Dynamics/DynamicsUsage.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcDynamicsUsage(), true, true},
    {"sdk/VRChat/VRC/Dynamics/DynamicsUsageFlags.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcDynamicsUsageFlags(), true, true},
    {"sdk/VRChat/VRC/Dynamics/DynamicsUsageFlagsExtensions.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcDynamicsUsageFlagsExtensions(), true, true},
    {"sdk/VRChat/VRC/Dynamics/FixedTransformAccessArray.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcFixedTransformAccessArray(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ICollisionBroadphase.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcICollisionBroadphase(), true, true},
    {"sdk/VRChat/VRC/Dynamics/IContactReceiverUdonEmitter.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIContactReceiverUdonEmitter(), true, true},
    {"sdk/VRChat/VRC/Dynamics/IMemoryBufferList.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIMemoryBufferList(), true, true},
    {"sdk/VRChat/VRC/Dynamics/IMemoryBufferSpanList.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIMemoryBufferSpanList(), true, true},
    {"sdk/VRChat/VRC/Dynamics/IParameterSetup.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIParameterSetup(), true, true},
    {"sdk/VRChat/VRC/Dynamics/IPhysBoneDebugDrawer.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIPhysBoneDebugDrawer(), true, true},
    {"sdk/VRChat/VRC/Dynamics/IPhysBoneUdonEmitter.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIPhysBoneUdonEmitter(), true, true},
    {"sdk/VRChat/VRC/Dynamics/IVRCConstraintBinding.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIVRCConstraintBinding(), true, true},
    {"sdk/VRChat/VRC/Dynamics/LookAtVRCConstraintBinding.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcLookAtVRCConstraintBinding(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ManagedTypes/VRCAimConstraintBase.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCAimConstraintBase(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ManagedTypes/VRCLookAtConstraintBase.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCLookAtConstraintBase(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ManagedTypes/VRCParentConstraintBase.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCParentConstraintBase(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ManagedTypes/VRCPositionConstraintBase.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCPositionConstraintBase(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ManagedTypes/VRCRotationConstraintBase.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCRotationConstraintBase(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ManagedTypes/VRCScaleConstraintBase.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCScaleConstraintBase(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ManagedTypes/VRCWorldUpConstraintBase.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCWorldUpConstraintBase(), true, true},
    {"sdk/VRChat/VRC/Dynamics/MathUtil.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcMathUtil(), true, true},
    {"sdk/VRChat/VRC/Dynamics/MemoryBuffer.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcMemoryBuffer(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ParentChangeDetector.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcParentChangeDetector(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ParentVRCConstraintBinding.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcParentVRCConstraintBinding(), true, true},
    {"sdk/VRChat/VRC/Dynamics/PhysBoneGrabbedInfo.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPhysBoneGrabbedInfo(), true, true},
    {"sdk/VRChat/VRC/Dynamics/PhysBoneGroup.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPhysBoneGroup(), true, true},
    {"sdk/VRChat/VRC/Dynamics/PhysBoneManager.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPhysBoneManager(), true, true},
    {"sdk/VRChat/VRC/Dynamics/PhysBonePosedInfo.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPhysBonePosedInfo(), true, true},
    {"sdk/VRChat/VRC/Dynamics/PhysBoneReleasedInfo.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPhysBoneReleasedInfo(), true, true},
    {"sdk/VRChat/VRC/Dynamics/PhysBoneRoot.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPhysBoneRoot(), true, true},
    {"sdk/VRChat/VRC/Dynamics/PhysBoneRootDefinition.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPhysBoneRootDefinition(), true, true},
    {"sdk/VRChat/VRC/Dynamics/PhysBoneUnPosedInfo.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPhysBoneUnPosedInfo(), true, true},
    {"sdk/VRChat/VRC/Dynamics/PositionVRCConstraintBinding.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPositionVRCConstraintBinding(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ReadTransformJob.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcReadTransformJob(), true, true},
    {"sdk/VRChat/VRC/Dynamics/RootsBuffer.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcRootsBuffer(), true, true},
    {"sdk/VRChat/VRC/Dynamics/RotationVRCConstraintBinding.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcRotationVRCConstraintBinding(), true, true},
    {"sdk/VRChat/VRC/Dynamics/ScaleVRCConstraintBinding.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcScaleVRCConstraintBinding(), true, true},
    {"sdk/VRChat/VRC/Dynamics/VRCConstraintBase.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCConstraintBase(), true, true},
    {"sdk/VRChat/VRC/Dynamics/VRCConstraintGroup.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCConstraintGroup(), true, true},
    {"sdk/VRChat/VRC/Dynamics/VRCConstraintGrouper.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCConstraintGrouper(), true, true},
    {"sdk/VRChat/VRC/Dynamics/VRCConstraintJob.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCConstraintJob(), true, true},
    {"sdk/VRChat/VRC/Dynamics/VRCConstraintJobData.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCConstraintJobData(), true, true},
    {"sdk/VRChat/VRC/Dynamics/VRCConstraintManager.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCConstraintManager(), true, true},
    {"sdk/VRChat/VRC/Dynamics/VRCConstraintOffsetBaker.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCConstraintOffsetBaker(), true, true},
    {"sdk/VRChat/VRC/Dynamics/VRCConstraintPlayerLoopStage.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCConstraintPlayerLoopStage(), true, true},
    {"sdk/VRChat/VRC/Dynamics/VRCConstraintPositionMode.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCConstraintPositionMode(), true, true},
    {"sdk/VRChat/VRC/Dynamics/VRCConstraintRotationMode.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCConstraintRotationMode(), true, true},
    {"sdk/VRChat/VRC/Dynamics/VRCConstraintScaleMode.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCConstraintScaleMode(), true, true},
    {"sdk/VRChat/VRC/Dynamics/VRCConstraintSource.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCConstraintSource(), true, true},
    {"sdk/VRChat/VRC/Dynamics/VRCConstraintSourceKeyableList.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCConstraintSourceKeyableList(), true, true},
    {"sdk/VRChat/VRC/Dynamics/VRCConstraintSynchronizeResult.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCConstraintSynchronizeResult(), true, true},
    {"sdk/VRChat/VRC/Dynamics/VRCDynamicsScheduler.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCDynamicsScheduler(), true, true},
    {"sdk/VRChat/VRC/Dynamics/VRCPhysBoneBase.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCPhysBoneBase(), true, true},
    {"sdk/VRChat/VRC/Dynamics/VRCPhysBoneColliderBase.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCPhysBoneColliderBase(), true, true},
    {"sdk/VRChat/VRC/Economy/IProduct.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIProduct(), true, true},
    {"sdk/VRChat/VRC/Economy/Store.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcStore(), true, true},
    {"sdk/VRChat/VRC/Economy/UdonProduct.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcUdonProduct(), true, true},
    {"sdk/VRChat/VRC/Economy/UdonProductsCategory.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcUdonProductsCategory(), true, true},
    {"sdk/VRChat/VRC/InventoryEffects/DroneSkinMapCore.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcDroneSkinMapCore(), true, true},
    {"sdk/VRChat/VRC/InventoryEffects/InventoryContentType.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcInventoryContentType(), true, true},
    {"sdk/VRChat/VRC/InventoryEffects/InventoryEffectAssetReference.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcInventoryEffectAssetReference(), true, true},
    {"sdk/VRChat/VRC/InventoryEffects/InventoryEffectDescription.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcInventoryEffectDescription(), true, true},
    {"sdk/VRChat/VRC/InventoryEffects/InventoryEffectDescriptionKeys.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcInventoryEffectDescriptionKeys(), true, true},
    {"sdk/VRChat/VRC/InventoryEffects/LocalPositionTracker.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcLocalPositionTracker(), true, true},
    {"sdk/VRChat/VRC/InventoryEffects/WarpEffect.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcWarpEffect(), true, true},
    {"sdk/VRChat/VRC/InventoryEffects/WarpEffectMap.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcWarpEffectMap(), true, true},
    {"sdk/VRChat/VRC/InventoryEffects/WarpEffectParams.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcWarpEffectParams(), true, true},
    {"sdk/VRChat/VRC/Localization/LocalizableString.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcLocalizableString(), true, true},
    {"sdk/VRChat/VRC/Localization/LocalizableStringFormatter.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcLocalizableStringFormatter(), true, true},
    {"sdk/VRChat/VRC/Localization/LocalizationAsset.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcLocalizationAsset(), true, true},
    {"sdk/VRChat/VRC/Localization/LocalizationDatabase.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcLocalizationDatabase(), true, true},
    {"sdk/VRChat/VRC/SDK3/Avatars/Components/VRCAvatarDescriptor.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCAvatarDescriptor(), true, true},
    {"sdk/VRChat/VRC/SDK3/Avatars/Components/VRCHeadChop.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCHeadChop(), true, true},
    {"sdk/VRChat/VRC/SDK3/Avatars/Components/VRCImpostorEnvironment.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCImpostorEnvironment(), true, true},
    {"sdk/VRChat/VRC/SDK3/Avatars/Components/VRCImpostorSettings.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCImpostorSettings(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/AbstractUdonBehaviour.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAbstractUdonBehaviour(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/MirrorClearFlags.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcMirrorClearFlags(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/MultipleDisplayUtilities.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcMultipleDisplayUtilities(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/SetPropertyUtility.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcSetPropertyUtility(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/Video/VideoError.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVideoError(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/Video/VRCDepthkit/VRCDepthkitMetadata.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCDepthkitMetadata(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/Video/VRCDepthkit/VRCDepthkitVideo.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCDepthkitVideo(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/VRCAvatarPedestal.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCAvatarPedestal(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/VRCEnablePersistence.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCEnablePersistence(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/VRCInputFieldKeyboardOverride.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCInputFieldKeyboardOverride(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/VRCInteractable.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCInteractable(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/VRCMirrorReflection.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCMirrorReflection(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/VRCObjectPool.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCObjectPool(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/VRCObjectSync.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCObjectSync(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/VRCOpenMenu.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCOpenMenu(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/VRCPlayerObject.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCPlayerObject(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/VRCPortalMarker.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCPortalMarker(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/VRCSceneDescriptor.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCSceneDescriptor(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/VRCSpatialAudioSource.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCSpatialAudioSource(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/VRCStation.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCStation(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/VRCTMPDropdownExtension.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCTMPDropdownExtension(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/VRCUiShape.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCUiShape(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/VRCUrlInputField.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCUrlInputField(), true, true},
    {"sdk/VRChat/VRC/SDK3/Components/VRCVisualDamage.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCVisualDamage(), true, true},
    {"sdk/VRChat/VRC/SDK3/ControllerColliderPlayerHit.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcControllerColliderPlayerHit(), true, true},
    {"sdk/VRChat/VRC/SDK3/Data/DataDictionary.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcDataDictionary(), true, true},
    {"sdk/VRChat/VRC/SDK3/Data/DataError.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcDataError(), true, true},
    {"sdk/VRChat/VRC/SDK3/Data/DataList.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcDataList(), true, true},
    {"sdk/VRChat/VRC/SDK3/Data/DataToken.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcDataToken(), true, true},
    {"sdk/VRChat/VRC/SDK3/Data/JsonDictionary.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcJsonDictionary(), true, true},
    {"sdk/VRChat/VRC/SDK3/Data/JsonExportType.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcJsonExportType(), true, true},
    {"sdk/VRChat/VRC/SDK3/Data/JsonList.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcJsonList(), true, true},
    {"sdk/VRChat/VRC/SDK3/Data/JsonType.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcJsonType(), true, true},
    {"sdk/VRChat/VRC/SDK3/Data/ParseState.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcParseState(), true, true},
    {"sdk/VRChat/VRC/SDK3/Data/TokenType.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcTokenType(), true, true},
    {"sdk/VRChat/VRC/SDK3/Data/VRCJson.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCJson(), true, true},
    {"sdk/VRChat/VRC/SDK3/Dynamics/Constraint/Components/VRCAimConstraint.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCAimConstraint(), true, true},
    {"sdk/VRChat/VRC/SDK3/Dynamics/Constraint/Components/VRCLookAtConstraint.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCLookAtConstraint(), true, true},
    {"sdk/VRChat/VRC/SDK3/Dynamics/Constraint/Components/VRCParentConstraint.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCParentConstraint(), true, true},
    {"sdk/VRChat/VRC/SDK3/Dynamics/Constraint/Components/VRCPositionConstraint.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCPositionConstraint(), true, true},
    {"sdk/VRChat/VRC/SDK3/Dynamics/Constraint/Components/VRCRotationConstraint.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCRotationConstraint(), true, true},
    {"sdk/VRChat/VRC/SDK3/Dynamics/Constraint/Components/VRCScaleConstraint.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCScaleConstraint(), true, true},
    {"sdk/VRChat/VRC/SDK3/Dynamics/Contact/Components/VRCContactReceiver.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCContactReceiver(), true, true},
    {"sdk/VRChat/VRC/SDK3/Dynamics/Contact/Components/VRCContactSender.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCContactSender(), true, true},
    {"sdk/VRChat/VRC/SDK3/Dynamics/PhysBone/Components/VRCPhysBone.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCPhysBone(), true, true},
    {"sdk/VRChat/VRC/SDK3/Dynamics/PhysBone/Components/VRCPhysBoneCollider.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCPhysBoneCollider(), true, true},
    {"sdk/VRChat/VRC/SDK3/Dynamics/PhysBone/Components/VRCPhysBoneRoot.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCPhysBoneRoot(), true, true},
    {"sdk/VRChat/VRC/SDK3/Image/ImageDownloader.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcImageDownloader(), true, true},
    {"sdk/VRChat/VRC/SDK3/Image/ImageLoadError.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcImageLoadError(), true, true},
    {"sdk/VRChat/VRC/SDK3/Image/IVRCImageDownload.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIVRCImageDownload(), true, true},
    {"sdk/VRChat/VRC/SDK3/Image/TextureInfo.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcTextureInfo(), true, true},
    {"sdk/VRChat/VRC/SDK3/Image/VRCImageDownloader.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCImageDownloader(), true, true},
    {"sdk/VRChat/VRC/SDK3/Image/VRCImageDownloadError.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCImageDownloadError(), true, true},
    {"sdk/VRChat/VRC/SDK3/Image/VRCImageDownloadState.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCImageDownloadState(), true, true},
    {"sdk/VRChat/VRC/SDK3/Network/Stats.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcStats(), true, true},
    {"sdk/VRChat/VRC/SDK3/Network/VRCNetworkBehaviour.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCNetworkBehaviour(), true, true},
    {"sdk/VRChat/VRC/SDK3/Network/VRCUdonSyncType.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCUdonSyncType(), true, true},
    {"sdk/VRChat/VRC/SDK3/Network/VRCUdonSyncTypeConverter.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCUdonSyncTypeConverter(), true, true},
    {"sdk/VRChat/VRC/SDK3/Persistence/PlayerData.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPlayerData(), true, true},
    {"sdk/VRChat/VRC/SDK3/Platform/ScreenUpdateData.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcScreenUpdateData(), true, true},
    {"sdk/VRChat/VRC/SDK3/Platform/ScreenUpdateType.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcScreenUpdateType(), true, true},
    {"sdk/VRChat/VRC/SDK3/Props/Components/CustomAttribute.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcCustomAttribute(), true, true},
    {"sdk/VRChat/VRC/SDK3/Props/Components/CustomAttributeType.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcCustomAttributeType(), true, true},
    {"sdk/VRChat/VRC/SDK3/Props/Components/VRCPropDescriptor.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCPropDescriptor(), true, true},
    {"sdk/VRChat/VRC/SDK3/Props/SpawnType.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcSpawnType(), true, true},
    {"sdk/VRChat/VRC/SDK3/Props/VRCPropApi.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCPropApi(), true, true},
    {"sdk/VRChat/VRC/SDK3/Props/VRCPropUtilities.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCPropUtilities(), true, true},
    {"sdk/VRChat/VRC/SDK3/Props/WorldSpawnPlacement.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcWorldSpawnPlacement(), true, true},
    {"sdk/VRChat/VRC/SDK3/Rendering/VRCAsyncGPUReadbackRequest.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCAsyncGPUReadbackRequest(), true, true},
    {"sdk/VRChat/VRC/SDK3/Rendering/VRCCameraMode.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCCameraMode(), true, true},
    {"sdk/VRChat/VRC/SDK3/Rendering/VRCCameraSettings.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCCameraSettings(), true, true},
    {"sdk/VRChat/VRC/SDK3/Rendering/VRCQualitySettings.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCQualitySettings(), true, true},
    {"sdk/VRChat/VRC/SDK3/StringLoading/IVRCStringDownload.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIVRCStringDownload(), true, true},
    {"sdk/VRChat/VRC/SDK3/StringLoading/MaxBufferDownloadHandler.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcMaxBufferDownloadHandler(), true, true},
    {"sdk/VRChat/VRC/SDK3/StringLoading/VRCStringDownload.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCStringDownload(), true, true},
    {"sdk/VRChat/VRC/SDK3/StringLoading/VRCStringDownloader.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCStringDownloader(), true, true},
    {"sdk/VRChat/VRC/SDK3/UdonNetworkCalling/NetworkCallableAttribute.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcNetworkCallableAttribute(), true, true},
    {"sdk/VRChat/VRC/SDK3/UdonNetworkCalling/NetworkCalling.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcNetworkCalling(), true, true},
    {"sdk/VRChat/VRC/SDK3/UdonNetworkCalling/NetworkCallingEntrypointMetadata.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcNetworkCallingEntrypointMetadata(), true, true},
    {"sdk/VRChat/VRC/SDK3/UdonNetworkCalling/NetworkCallingParameterMetadata.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcNetworkCallingParameterMetadata(), true, true},
    {"sdk/VRChat/VRC/SDK3/Video/Components/AVPro/VRCAVProVideoPlayer.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCAVProVideoPlayer(), true, true},
    {"sdk/VRChat/VRC/SDK3/Video/Components/AVPro/VRCAVProVideoScreen.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCAVProVideoScreen(), true, true},
    {"sdk/VRChat/VRC/SDK3/Video/Components/AVPro/VRCAVProVideoSpeaker.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCAVProVideoSpeaker(), true, true},
    {"sdk/VRChat/VRC/SDK3/Video/Components/Base/BaseVRCVideoPlayer.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcBaseVRCVideoPlayer(), true, true},
    {"sdk/VRChat/VRC/SDK3/Video/Components/VRCUnityVideoPlayer.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCUnityVideoPlayer(), true, true},
    {"sdk/VRChat/VRC/SDK3/Video/Interfaces/AVPro/IAVProVideoPlayerInternal.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIAVProVideoPlayerInternal(), true, true},
    {"sdk/VRChat/VRC/SDK3/Video/Interfaces/IVRCVideoPlayer.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIVRCVideoPlayer(), true, true},
    {"sdk/VRChat/VRC/SDK3/VRCTestMarker.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCTestMarker(), true, true},
    {"sdk/VRChat/VRC/SDKBase/Editor/Attributes/CurveAttribute.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcCurveAttribute(), true, true},
    {"sdk/VRChat/VRC/SDKBase/Editor/Attributes/HelpBoxAttribute.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcHelpBoxAttribute(), true, true},
    {"sdk/VRChat/VRC/SDKBase/IAnimParameterAccess.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIAnimParameterAccess(), true, true},
    {"sdk/VRChat/VRC/SDKBase/INetworkIDContainer.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcINetworkIDContainer(), true, true},
    {"sdk/VRChat/VRC/SDKBase/InputManager.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcInputManager(), true, true},
    {"sdk/VRChat/VRC/SDKBase/IValidChecker.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIValidChecker(), true, true},
    {"sdk/VRChat/VRC/SDKBase/IVRC_Destructible.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIVRC_Destructible(), true, true},
    {"sdk/VRChat/VRC/SDKBase/Network/NetworkIDAssignment.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcNetworkIDAssignment(), true, true},
    {"sdk/VRChat/VRC/SDKBase/Network/NetworkIDPair.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcNetworkIDPair(), true, true},
    {"sdk/VRChat/VRC/SDKBase/Platform/VRCOrientation.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCOrientation(), true, true},
    {"sdk/VRChat/VRC/SDKBase/Source/IVRCInteractable.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIVRCInteractable(), true, true},
    {"sdk/VRChat/VRC/SDKBase/Utilities.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcUtilities(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VersionHelper.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVersionHelper(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_AnimatorLayerControl.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAnimatorLayerControl(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_AnimatorLocomotionControl.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAnimatorLocomotionControl(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_AnimatorPlayAudio.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAnimatorPlayAudio(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_AnimatorTemporaryPoseSpace.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAnimatorTemporaryPoseSpace(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_AnimatorTrackingControl.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAnimatorTrackingControl(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_AvatarDescriptor.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAvatarDescriptor(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_AvatarParameterDriver.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcAvatarParameterDriver(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_DataStorage.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcDataStorage(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_DestructibleStandard.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcDestructibleStandard(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_EventDispatcher.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcEventDispatcher(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_EventDispatcherLocal.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcEventDispatcherLocal(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_EventHandler.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcEventHandler(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_GunStats.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcGunStats(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_IKFollower.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcIKFollower(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_Interactable.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcInteractable(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_KeyEvents.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcKeyEvents(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_Label.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcLabel(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_MetadataListener.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcMetadataListener(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_MidiNoteIn.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcMidiNoteIn(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_MirrorReflection.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcMirrorReflection(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_NpcApi.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcNpcApi(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_NPCSpawn.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcNPCSpawn(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_ObjectApi.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcObjectApi(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_ObjectSpawn.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcObjectSpawn(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_OscButtonIn.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcOscButtonIn(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_Panorama.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPanorama(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_PhysicsRoot.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPhysicsRoot(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_PlayableLayerControl.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPlayableLayerControl(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_PortalMarker.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPortalMarker(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_PropApi.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPropApi(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_PropController.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPropController(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_PropDescriptor.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcPropDescriptor(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_SlideShow.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcSlideShow(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_SpecialLayer.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcSpecialLayer(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_TimedEvents.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcTimedEvents(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_Trigger.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcTrigger(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_TriggerColliderEventTrigger.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcTriggerColliderEventTrigger(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_UiShape.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcUiShape(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_UseEvents.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcUseEvents(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_VisualDamage.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVisualDamage(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_Water.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcWater(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRC_WebPanel.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcWebPanel(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRCCustomAction.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCCustomAction(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRCDroneApi.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCDroneApi(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRCGraphics.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCGraphics(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRCInputMethod.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCInputMethod(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRCInputSetting.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCInputSetting(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRCRenderTexture.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCRenderTexture(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRCShader.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCShader(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRCTriggerRelay.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCTriggerRelay(), true, true},
    {"sdk/VRChat/VRC/SDKBase/VRCUrl.h", OutputFilePolicy::GeneratedOverwrite, VRChatGeneratedVrcVRCUrl(), true, true},
// <<< VRCHAT_GENERATED_WRITES <<<
        {"mod/config/mod_config.h", OutputFilePolicy::EditablePreserve, ConfigModule(project), true, false},
        {"mod/support/mod_log.h", OutputFilePolicy::EditablePreserve, ModLogHeader(), true, false},
        {"mod/support/mod_log.cpp", OutputFilePolicy::EditablePreserve, ModLogSource(), true, false},
        {"mod/modules/modules.h", OutputFilePolicy::EditablePreserve, ModulesHeaderModule(), true, false},
        {"mod/modules/modules.cpp", OutputFilePolicy::EditablePreserve, ModulesSourceModule(), true, false},
        {"mod/modules/Visuals/Visuals.h", OutputFilePolicy::EditablePreserve, VisualsHeaderModule(), true, false},
        {"mod/modules/Visuals/Visuals.cpp", OutputFilePolicy::EditablePreserve, VisualsSourceModule(), true, false},
        {"mod/hooks/mod_hooks.h", OutputFilePolicy::EditablePreserve, ModHooksHeader(), true, false},
        {"mod/hooks/mod_hooks.cpp", OutputFilePolicy::EditablePreserve, ModHooksSource(project), true, false},
        {"mod/lifecycle/mod_network.h", OutputFilePolicy::EditablePreserve, NetworkInitHeader(), true, false},
        {"mod/lifecycle/mod_network.cpp", OutputFilePolicy::EditablePreserve, NetworkInitSource(), true, false},
        {"mod/lifecycle/mod_runtime.h", OutputFilePolicy::EditablePreserve, GameRuntimeHeader(), true, false},
        {"mod/lifecycle/mod_runtime.cpp", OutputFilePolicy::EditablePreserve, GameRuntimeSource(project), true, false},
        {"mod/ui/theme.h", OutputFilePolicy::EditablePreserve, ThemeModule(), true, true},
        {"mod/ui/localization.h", OutputFilePolicy::EditablePreserve, LocalizationModule(), true, true},
        {"mod/ui/widgets.h", OutputFilePolicy::EditablePreserve, WidgetsModule(), true, true},
        {"mod/ui/tabs/about_tab.h", OutputFilePolicy::EditablePreserve, AboutTabModule(), true, true},
        {"mod/ui/tabs/config_tab.h", OutputFilePolicy::EditablePreserve, ConfigTabModule(), true, true},
        {"mod/ui/menu.h", OutputFilePolicy::EditablePreserve, UiModule(), true, true},
        {"mod/ui/highlight.h", OutputFilePolicy::EditablePreserve, HighlightModule(), true, true},
        {"mod/hooks/dx11_viewport_swap_chain.h", OutputFilePolicy::GeneratedOverwrite,
         Dx11ViewportSwapChainHeaderModule(), true, true},
        {"mod/hooks/dx11_viewport_swap_chain.cpp", OutputFilePolicy::GeneratedOverwrite,
         Dx11ViewportSwapChainSourceModule(), true, false},
        {"mod/hooks/dx11_state_guard.h", OutputFilePolicy::GeneratedOverwrite, Dx11StateGuardHeaderModule(), true,
         true},
        {"mod/hooks/dx11_state_guard.cpp", OutputFilePolicy::GeneratedOverwrite, Dx11StateGuardSourceModule(), true,
         false},
        {"mod/hooks/dx12_overlay_resources.h", OutputFilePolicy::GeneratedOverwrite,
         Dx12OverlayResourcesHeaderModule(), true, true},
        {"mod/hooks/dx12_overlay_resources.cpp", OutputFilePolicy::GeneratedOverwrite,
         Dx12OverlayResourcesSourceModule(), true, false},
        {"mod/hooks/dxgi_hook_discovery.h", OutputFilePolicy::GeneratedOverwrite,
         DxgiHookDiscoveryHeaderModule(), true, true},
        {"mod/hooks/dxgi_hook_discovery.cpp", OutputFilePolicy::GeneratedOverwrite,
         DxgiHookDiscoverySourceModule(), true, false},
        {"mod/hooks/win32_input_coordinates.h", OutputFilePolicy::GeneratedOverwrite,
         Win32InputCoordinatesHeaderModule(), true, true},
        {"mod/hooks/win32_input_coordinates.cpp", OutputFilePolicy::GeneratedOverwrite,
         Win32InputCoordinatesSourceModule(), true, false},
        {"mod/hooks/win32_message_pump.h", OutputFilePolicy::GeneratedOverwrite,
         Win32MessagePumpHeaderModule(), true, true},
        {"mod/hooks/win32_message_pump.cpp", OutputFilePolicy::GeneratedOverwrite,
         Win32MessagePumpSourceModule(), true, false},
        {"third_party/imgui_win32_module_scope.cpp", OutputFilePolicy::GeneratedOverwrite,
         ImGuiWin32ModuleScopeSourceModule(), true, false},
        {"mod/hooks/win32_viewport_policy.h", OutputFilePolicy::GeneratedOverwrite,
         Win32ViewportPolicyHeaderModule(), true, true},
        {"mod/hooks/win32_viewport_policy.cpp", OutputFilePolicy::GeneratedOverwrite,
         Win32ViewportPolicySourceModule(), true, false},
        {"mod/hooks/render_imgui_hook.h", OutputFilePolicy::GeneratedOverwrite, RenderHookHeaderModule(), true, true},
        {"mod/hooks/render_imgui_hook.cpp", OutputFilePolicy::GeneratedOverwrite, RenderHookSourceModule(), true, false},
        {"mod/hooks/unity_log_hook.h", OutputFilePolicy::EditablePreserve, UnityLogHookModule(project), true, true},
        {"mod/generated/mod_lifecycle.h", OutputFilePolicy::GeneratedOverwrite, ModLifecycleHeader(), true, true},
        {"mod/generated/mod_lifecycle.cpp", OutputFilePolicy::GeneratedOverwrite, ModLifecycleSource(project), true,
         false},
        {"mod/generated/mod_entry.cpp", OutputFilePolicy::GeneratedOverwrite, ModEntrySource(), true, false},
        {"README.md", OutputFilePolicy::DocumentationPreserve, Readme(project), true, false},
        {".clangd", OutputFilePolicy::GeneratedOverwrite,
         "CompileFlags:\n  CompilationDatabase: out/build/clang-debug\n  Compiler: clang++\n\nIndex:\n  Background: Build\n",
         true, false},
        {"CMakePresets.json", OutputFilePolicy::GeneratedOverwrite, CMakePresets(), true, false},
        {".vscode/c_cpp_properties.json", OutputFilePolicy::GeneratedOverwrite, VsCodeCppProperties(), true, false},
    };
    if (project.enableLocalization) {
        writes.push_back({"locales/" + project.modId + "/en.json", OutputFilePolicy::EditablePreserve,
                          EnglishLocaleModule(), true, false});
        writes.push_back({"locales/" + project.modId + "/tr.json", OutputFilePolicy::EditablePreserve,
                          TurkishLocaleModule(), true, false});
        writes.push_back({"locales/" + project.modId + "/ja.json", OutputFilePolicy::EditablePreserve,
                          JapaneseLocaleModule(), true, false});
        writes.push_back({"locales/" + project.modId + "/zh.json", OutputFilePolicy::EditablePreserve,
                          ChineseLocaleModule(), true, false});
        writes.push_back({"locales/" + project.modId + "/ru.json", OutputFilePolicy::EditablePreserve,
                          RussianLocaleModule(), true, false});
        writes.push_back({"locales/" + project.modId + "/uk.json", OutputFilePolicy::EditablePreserve,
                          UkrainianLocaleModule(), true, false});
        writes.push_back({"locales/" + project.modId + "/es.json", OutputFilePolicy::EditablePreserve,
                          SpanishLocaleModule(), true, false});
        writes.push_back({"locales/" + project.modId + "/fr.json", OutputFilePolicy::EditablePreserve,
                          FrenchLocaleModule(), true, false});
    }
    for (const auto &extra : project.extraOutputFiles) {
        if (extra.writtenByCommonGenerator) {
            writes.push_back({extra.relativePath, extra.policy, {}, extra.required, extra.moduleFile});
        }
    }

    std::vector<OutputFileSpec> outputSpecs;
    outputSpecs.reserve(writes.size() + project.backendModuleFiles.size() + project.extraModuleFiles.size() +
                        project.extraOutputFiles.size() + 1);
    for (const auto &write : writes)
        outputSpecs.push_back(OutputSpecForWrite(write));
    AppendExternalOutputSpecs(project, outputSpecs);

    const std::vector<fs::path> sourceFiles = CollectSourceFiles(outputSpecs);
    const std::vector<fs::path> moduleFiles = CollectModuleFiles(outputSpecs);
    writes.push_back({"CMakeLists.txt", OutputFilePolicy::GeneratedOverwrite,
                      CMakeLists(project, sourceFiles, moduleFiles), true, false});
    outputSpecs.push_back(OutputSpecForWrite(writes.back()));

    for (const auto &write : writes)
        if (!WriteByPolicy(project, write, error))
            return false;

    if (!ValidateOutputSpecs(project.projectRoot, outputSpecs, error))
        return false;

    if (project.deployDirectory.empty())
        return true;

    UrkProject::Manifest manifest;
    manifest.sdkVersion = URK_SDK_VERSION;
    manifest.backend = project.backendNamespace == "URK::il2cpp" ? UrkProject::Backend::Il2Cpp : UrkProject::Backend::Mono;
    manifest.projectName = project.projectName;
    manifest.gameDirectory = std::filesystem::path(project.deployDirectory).parent_path();
    manifest.modsDirectory = std::filesystem::path(project.deployDirectory).filename().string();
    manifest.enableLocalization = project.enableLocalization;
    if (!UrkProject::WriteManifest(project.projectRoot, manifest, error))
        return false;

    std::vector<fs::path> generatedPaths;
    generatedPaths.reserve(outputSpecs.size());
    for (const OutputFileSpec &spec : outputSpecs)
        if (spec.policy == OutputFilePolicy::GeneratedOverwrite)
            generatedPaths.push_back(spec.relativePath);
    return UrkProject::WriteGeneratedFileLedger(project.projectRoot, URK_SDK_VERSION, generatedPaths, error);
}

} // namespace ModProjectGenerator
