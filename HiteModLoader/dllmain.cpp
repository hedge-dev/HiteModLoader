// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Loader.h"

#pragma comment(linker, "/EXPORT:D3D11CreateDevice=C:\\Windows\\System32\\d3d11.D3D11CreateDevice")
#pragma comment(linker, "/EXPORT:D3D11CoreCreateDevice=C:\\Windows\\System32\\d3d11.D3D11CoreCreateDevice")
#pragma comment(linker, "/EXPORT:D3D11CreateDeviceAndSwapChain=C:\\Windows\\System32\\d3d11.D3D11CreateDeviceAndSwapChain")

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        InitLoaders();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

