// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Loader.h"
#include "Helpers.h"
#include "SigScan.h"
#include "StubFunctions.h"

static bool Loaded = false;

HOOK_SIG(void, __fastcall, Main, SigMain)
{
    if (!Loaded)
    {
        Loaded = true;
        InitLoaders();
    }
    originalMain();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        HookSystemDLL(hModule);
        INSTALL_HOOK(Main);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

