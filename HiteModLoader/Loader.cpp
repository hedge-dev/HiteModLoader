#include "pch.h"
#include "Loader.h"
#include "SigScan.h"
#include "Helpers.h"
#include "ConfigLoader.h"
#include "CriLoader.h"
#include "CodeLoader.h"
#include "ModLoader.h"
#include "DataPackLoader.h"
#include "SaveRedirection.h"
#include "LoggingHandler.h"
#include "RSDKPatches.h"

bool ConsoleEnabled;
Platform CurrentPlatform = Platform_Epic; // Default to Epic
ModLoader ModLoaderData;

bool ReturnToTitle = false;

HOOK(bool, __fastcall, SteamAPI_RestartAppIfNecessary, PROC_ADDRESS("steam_api64.dll", "SteamAPI_RestartAppIfNecessary"), uint32_t appid)
{
    originalSteamAPI_RestartAppIfNecessary(appid);
    std::ofstream ofs("steam_appid.txt");
    ofs << appid;
    ofs.close();
    return false;
}

HOOK(bool, __fastcall, SteamAPI_IsSteamRunning, PROC_ADDRESS("steam_api64.dll", "SteamAPI_IsSteamRunning"))
{
    originalSteamAPI_IsSteamRunning();
    return true;
}

HOOK_SIG(void*, __fastcall, RunCore, SigRunCore, void* a1)
{
    void* result = originalRunCore(a1);
    RaiseEvents(RunCoreEvents);
    CommonLoader::RaiseUpdates();
    return result;
}

HOOK_SIG(void*, __fastcall, Engine_HandleGameLoop, SigEngine_HandleGameLoop, void* a1)
{
    if (ReturnToTitle)
    {
        int args[3]{ NOTIFY_BACK_TO_MAINMENU, 1, 0 };
        auto NotifyCallback = (void(__fastcall*)(int*))(SigNotifyCallback());
        NotifyCallback(args);

        // Old code
        //auto gameState = ReadDataPointer((size_t)SigEngine_HandleGameLoop(), 0x23, 0x07) + 0x20;
        //*(char*)gameState = 8;
        ReturnToTitle = false;
    }

    void* result = originalEngine_HandleGameLoop(a1);
    RaiseEvents(RSDKLoopEvents);
    return result;
}

void AddIncludePath(const char* path, bool first)
{
    if (first)
        ModIncludePaths.insert(ModIncludePaths.begin(), std::string(path));
    else
        ModIncludePaths.push_back(std::string(path));
}

int GetIncludePaths(const char** paths, size_t count)
{
    if (!paths)
        return ModIncludePaths.size();

    int index = 0;
    for (auto& value : ModIncludePaths)
    {
        if (index < count)
            paths[index] = value.c_str();
        else
            return count;
        index++;
    }
    return index;
}

void InitLoaders()
{
    CommonLoader::Init();
    // Check signatures
    if (!SigValid)
    {
        MessageBoxW(nullptr, L"Failed to install mod loader (possibly unsupported game version)", L"Hite Mod Loader", MB_ICONERROR);
        return;
    }
   
    // Install hooks
    INSTALL_HOOK(SteamAPI_RestartAppIfNecessary);
    INSTALL_HOOK(SteamAPI_IsSteamRunning);
    INSTALL_HOOK(RunCore);
    INSTALL_HOOK(Engine_HandleGameLoop);

    // Detect Steam
    if (PROC_ADDRESS("steam_api64.dll", "SteamAPI_RestartAppIfNecessary"))
        CurrentPlatform = Platform_Steam;

    // Init loaders
    InitConfigLoader();
    InitCriLoader();
    InitModLoader();
    InitDataPackLoader();
    
    ModLoaderData.AddInclude = AddIncludePath;
    ModLoaderData.GetIncludePaths = GetIncludePaths;

    InitCodeLoader();

    // Handlers
    if (EnableSaveFileRedirection)
        InitSaveRedirection();
    InitLoggingHandler();
    InitRSDKPatches();

    // Init CommonLoader
    LOG("Loading Codes...");
    CommonLoader::LoadAssembly((GetDirectoryPath(ModsDbIniPath) + "/Codes.dll").c_str());
    CommonLoader::RaiseInitializers();

    LOG("Initialisation Complete!");
}