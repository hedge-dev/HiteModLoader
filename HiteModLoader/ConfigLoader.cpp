#include "pch.h"
#include "ModLoader.h"
#include <INIReader.h>
#include "Loader.h"
#include <consoleapi.h>

std::string ModsDbIniPath;
std::string SaveFileFallback;
std::string SaveFileOverride;
bool EnableSaveFileRedirection;

void InitConfigLoader()
{
    INIReader config = INIReader("cpkredir.ini");

    std::string logType = config.GetString("CPKREDIR", "LogType", "");
    std::string logFile = config.GetString("CPKREDIR", "LogFile", "cpkredir.log");
    ModsDbIniPath = config.GetString("CPKREDIR", "ModsDbIni", "");
    SaveFileFallback = config.GetString("CPKREDIR", "SaveFileFallback", "");
    SaveFileOverride = config.GetString("CPKREDIR", "SaveFileOverride", "");
    EnableSaveFileRedirection = config.GetBoolean("CPKREDIR", "EnableSaveFileRedirection", false);
    bool useFileLogging = false;

    // TODO: Fix HMM from using cpkredir.sav
    if (SaveFileFallback.find("cpkredir.sav") != std::string::npos)
        SaveFileFallback = "savedata";

    if (!logType.empty())
    {
        ConsoleEnabled = !strcmp(logType.c_str(), "console");
        useFileLogging = !strcmp(logType.c_str(), "file");
    }

    if (ConsoleEnabled)
    {
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        LOG("Starting HiteModLoader %s...", ML_VERSION);
    }

    if (useFileLogging)
    {
        freopen(logFile.c_str(), "w", stdout);
        ConsoleEnabled = true;
    }
}