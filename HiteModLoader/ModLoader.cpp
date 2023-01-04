#include <pch.h>
#include "ModLoader.h"
#include "Loader.h"
#include "ConfigLoader.h"
#include "SaveRedirection.h"
#include <INIReader.h>

std::vector<std::string> ModIncludePaths;
std::vector<std::pair<Mod*, std::wstring>> ModCodePaths;
std::vector<Mod*> Mods;

void LoadMod(const std::string& filePath)
{
    if (filePath.empty())
        return;

    const INIReader modIni(filePath);

    if (modIni.ParseError() == -1)
    {
        LOG(" - Failed to load mod \"%s\"", filePath.c_str())
            return;
    }

    const std::string title = modIni.GetString("Desc", "Title", std::string());

    if (modIni.ParseError() != 0)
    {
        LOG(" - %s (error at line %d)", title.c_str(), modIni.ParseError());
        return;
    }
    else
    {
        LOG(" - %s", title.c_str());
    }

    const std::string directoryPath = GetDirectoryPath(filePath);

    const long includeDirCount = modIni.GetInteger("Main", "IncludeDirCount", 0);

    for (long i = 0; i < includeDirCount; i++)
    {
        const std::string includeDir = modIni.GetString("Main", "IncludeDir" + std::to_string(i), std::string());

        if (includeDir.empty())
            continue;

        ModIncludePaths.push_back(directoryPath + "/" + includeDir + "/");
    }

    auto mod = new Mod();
    auto modTitle = new std::string(title.c_str());
    auto modPath = new std::string(directoryPath.c_str());
    
    mod->Name = modTitle->c_str();
    mod->Path = modPath->c_str();
    Mods.push_back(mod);

    const std::string dllFilePath = modIni.GetString("Main", "DLLFile", std::string());

    if (!dllFilePath.empty())
        ModCodePaths.push_back(std::pair<Mod*, std::wstring>(mod, ConvertMultiByteToWideChar(directoryPath + "/" + dllFilePath)));

    const std::string saveFilePath = modIni.GetString("Main", "SaveFile", std::string());

    if (SaveFilePath.empty() && !saveFilePath.empty())
        SaveFilePath = directoryPath + "\\" + saveFilePath;
}

void LoadModsDatabase(const std::string& filePath)
{
    LOG("Loading mods...");
    if (filePath.empty())
        return;

    const INIReader modsDBIni(filePath);

    if (modsDBIni.ParseError() != 0)
    {
        LOG("Failed to load \"%s\"", filePath.c_str());
        return;
    }

    // Handle reverse order loading
    // ReverseLoadOrder = modsDBIni.GetBoolean("Main", "ReverseLoadOrder", false);

    const long activeModCount = modsDBIni.GetInteger("Main", "ActiveModCount", 0);

    for (long i = 0; i < activeModCount; i++)
    {
        const std::string activeMod = modsDBIni.GetString("Main", "ActiveMod" + std::to_string(i), std::string());

        if (activeMod.empty())
            continue;

        std::string modFilePath = modsDBIni.GetString("Mods", activeMod, std::string());
        std::replace(modFilePath.begin(), modFilePath.end(), '\\', '/');

        LoadMod(modFilePath);
    }
}


void InitModLoader()
{
	LoadModsDatabase(ModsDbIniPath);
}