#include "pch.h"
#include "Loader.h"
#include "SigScan.h"
#include "Helpers.h"
#include "ConfigLoader.h"
#include "CriLoader.h"
#include "CodeLoader.h"
#include "ModLoader.h"
#include "DataPackLoader.h"

bool ConsoleEnabled;
Platform CurrentPlatform = Platform_Epic; // Default to Epic

HOOK(bool, __fastcall, SteamAPI_RestartAppIfNecessary, PROC_ADDRESS("steam_api64.dll", "SteamAPI_RestartAppIfNecessary"), uint32_t appid)
{
    originalSteamAPI_RestartAppIfNecessary(appid);
    std::ofstream ofs("steam_appid.txt");
    ofs << appid;
    ofs.close();
    CurrentPlatform = Platform_Steam;
    return false;
}

HOOK(bool, __fastcall, SteamAPI_IsSteamRunning, PROC_ADDRESS("steam_api64.dll", "SteamAPI_IsSteamRunning"))
{
    originalSteamAPI_IsSteamRunning();
    CurrentPlatform = Platform_Steam;
    return true;
}

HOOK(void, __fastcall, SteamAPI_Shutdown, PROC_ADDRESS("steam_api64.dll", "SteamAPI_Shutdown"))
{
    LOG("Exiting...");
    RaiseEvents(ExitEvents);
    originalSteamAPI_Shutdown();
}

HOOK(void*, __fastcall, RunCore, SigRunCore(), void* a1)
{
    void* result = originalRunCore(a1);
    RaiseEvents(UpdateEvents);
    CommonLoader::CommonLoader::RaiseUpdates();
    return result;
}

HOOK(void*, __fastcall, Engine_HandleGameLoop, SigEngine_HandleGameLoop(), void* a1)
{
    void* result = originalEngine_HandleGameLoop(a1);
    RaiseEvents(FrameEvents);
    return result;
}

void InitLoaders()
{
    // Check signatures
    if (!SigValid)
    {
        MessageBoxW(nullptr, L"Failed to install mod loader (possibly unsupported game version)", L"Hite Mod Loader", MB_ICONERROR);
        return;
    }
    
    // Install hooks
    INSTALL_HOOK(SteamAPI_RestartAppIfNecessary);
    INSTALL_HOOK(SteamAPI_IsSteamRunning);
    INSTALL_HOOK(SteamAPI_Shutdown);
    INSTALL_HOOK(RunCore);
    INSTALL_HOOK(Engine_HandleGameLoop);

    // Init loaders
    InitConfigLoader();
    InitCriLoader();
    InitModLoader();
    InitDataPackLoader();
    InitCodeLoader();

    // Init CommonLoader
    LOG("Loading Codes...");
    CommonLoader::CommonLoader::InitializeAssemblyLoader((GetDirectoryPath(ModsDbIniPath) + "Codes.dll").c_str());
    CommonLoader::CommonLoader::RaiseInitializers();
    LOG("Initialisation Complete!");
}