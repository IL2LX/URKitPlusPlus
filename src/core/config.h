#pragma once
#include <string>
#include <vector>

struct Config {
    bool showConsole = true;
    bool logFullTimestamp = false;
    bool logPidTid = false;
    bool safeMode = false;
    bool dumpSymbols = false;
    bool autoDump = false;
    bool logUnityExports = false;
    bool accessorLeaks = true;
    bool structuralNames = true;
    int initDelayMs = 0;
    std::string runtime = "auto";
    std::string modsDir = "Mods";
    std::string configPath;
    std::vector<std::string> modPaths;
    std::vector<std::string> warnings;
};

Config Config_Load();
Config Config_Load(const std::string &iniPath);
