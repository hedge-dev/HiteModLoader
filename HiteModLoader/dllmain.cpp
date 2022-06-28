// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Loader.h"
#include "Helpers.h"

#pragma comment(linker, "/EXPORT:D3D11CreateDevice=C:\\Windows\\System32\\d3d11.D3D11CreateDevice")
#pragma comment(linker, "/EXPORT:D3D11CoreCreateDevice=C:\\Windows\\System32\\d3d11.D3D11CoreCreateDevice")
#pragma comment(linker, "/EXPORT:D3D11CreateDeviceAndSwapChain=C:\\Windows\\System32\\d3d11.D3D11CreateDeviceAndSwapChain")

static bool Loaded = false;

HOOK(void, __fastcall, GetStartupInfoWHook, PROC_ADDRESS("Kernel32.dll", "GetStartupInfoW"), void* lpStartupInfo)
{
    if (!Loaded)
    {
        Loaded = true;
        InitLoaders();
    }
    originalGetStartupInfoWHook(lpStartupInfo);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        INSTALL_HOOK(GetStartupInfoWHook);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

