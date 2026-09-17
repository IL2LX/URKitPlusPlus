#pragma once

#include <string>

// Initializes logging. If showConsole is true, allocates a console window.
// When fullTimestamp is true, log lines carry "[yyyy-MM-dd HH:mm:ss.mmm]";
// otherwise they carry a short "[HH:mm:ss]" prefix. When showPidTid is true,
// a "[pid:x tid:y]" tag is appended after the timestamp.
// When logDirectory is empty, opens URKit_logs.log next to the game executable.
// Injected sessions pass an external directory so the game folder stays untouched.
void Log_Init(bool showConsole, bool fullTimestamp, bool showPidTid, const std::string &logDirectory = std::string());
void Log_Shutdown();

// Prints the startup ASCII-art banner to the console (if active) and log file.
// No timestamp prefix; the console output uses a distinct cyan color.
void Log_Banner();

// printf-style, timestamped, process/thread tagged, written to console (if any)
// + file + debugger.
void Log(const char *fmt, ...);
