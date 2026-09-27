#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace ModProjectGenerator {

enum class OutputFilePolicy {
    GeneratedOverwrite,
    EditablePreserve,
    DocumentationPreserve,
};

struct OutputFileSpec {
    std::filesystem::path relativePath;
    OutputFilePolicy policy = OutputFilePolicy::GeneratedOverwrite;
    bool required = true;
    bool moduleFile = false;
    bool writtenByCommonGenerator = true;
};

std::string Identifier(const std::string &text, const char *fallback);
bool WriteText(const std::filesystem::path &path, const std::string &text, std::string *error);

// The generated SDK is emitted without comments. Stripping happens in the
// generators, on the way to disk, so the templates can keep the notes that
// maintain them without each one having to leave them out.
// Walks the text as a lexer would: a regex would corrupt a literal such as
// "https://api.vrchat.cloud" or the character literal '/'.
std::string StripGeneratedComments(const std::string &text);
bool IsSdkOutputPath(const std::filesystem::path &relativePath);

struct ModuleProjectOptions {
    std::filesystem::path projectRoot;
    std::filesystem::path sdkHeaderPath;
    std::string projectName;
    std::string modId;
    std::string backendDisplayName;
    std::string backendModule;
    std::string backendNamespace;
    std::string requiredBackendConstant;
    std::string requiredCapabilityConstant;
    std::string description;
    std::string deployDirectory;
    std::vector<std::filesystem::path> backendModuleFiles;
    std::vector<std::filesystem::path> extraModuleFiles;
    std::vector<std::string> includeDirectories;
    std::vector<std::string> configExtraLines;
    std::vector<std::string> hookModuleImports;
    std::vector<std::string> hookInstallLines;
    std::vector<std::string> hookUninstallLines;
    std::vector<std::string> readmeExtraLayoutLines;
    std::vector<OutputFileSpec> extraOutputFiles;
    bool enableLocalization = false;
    bool logHookSummary = false;
    bool preserveEditableSources = true;
};

bool WriteModuleProject(const ModuleProjectOptions &options, std::string *error);

} // namespace ModProjectGenerator
