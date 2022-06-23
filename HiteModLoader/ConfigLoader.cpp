#include "pch.h"
#include "ModLoader.h"
#include <INIReader.h>
#include "Loader.h"
#include <consoleapi.h>

std::string ModsDbIniPath;

void InitConfigLoader()
{
    INIReader config = INIReader("cpkredir.ini");

    std::string logType = config.GetString("CPKREDIR", "LogType", "");
    std::string logFile = config.GetString("CPKREDIR", "LogFile", "cpkredir.log");
    ModsDbIniPath = config.GetString("CPKREDIR", "ModsDbIni", "");
    bool useFileLogging = false;

    if (!logType.empty())
    {
        ConsoleEnabled = !strcmp(logType.c_str(), "console");
        useFileLogging = !strcmp(logType.c_str(), "file");
    }

    if (ConsoleEnabled)
    {
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        LOG("Starting HiteModLoader %s...", VERSION);
    }

    if (useFileLogging)
    {
        freopen(logFile.c_str(), "w", stdout);
        ConsoleEnabled = true;
    }
}