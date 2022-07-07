#include "pch.h"
#include "CodeLoader.h"
#include "ModLoader.h"
#include "Loader.h"

// Events
std::vector<DllEvent*> RunCoreEvents;
std::vector<DllEvent*> RSDKLoopEvents;
std::vector<DllEvent*> ExitEvents;

void RaiseEvents(std::vector<DllEvent*> events)
{
    for (auto& e : events)
        e();
}

void InitCodeLoader()
{
    LOG("Loading code mods...");
    if (!ModCodePaths.empty())
    {
        std::vector<DllEvent*> postInitEvents;

        WCHAR currentDirectory[PATH_LIMIT];
        WCHAR dllDirectory[PATH_LIMIT];

        GetCurrentDirectoryW(_countof(currentDirectory), currentDirectory);
        GetDllDirectoryW(_countof(dllDirectory), dllDirectory);

        auto ModsInfo = new ModInfo();
        ModsInfo->ModList = &Mods;
        ModsInfo->CurrentPlatform = CurrentPlatform;
        ModsInfo->ModLoader = &ModLoaderData;

        for (auto& modCode : ModCodePaths)
        {
            const std::wstring directoryPath = std::filesystem::path(modCode.second).parent_path().wstring();

            SetCurrentDirectoryW(directoryPath.c_str());
            SetDllDirectoryW(directoryPath.c_str());

            const HMODULE module = LoadLibraryW(modCode.second.c_str());

            if (!module)
            {
                LPWSTR buffer = nullptr;

                const DWORD msgSize = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    nullptr, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&buffer, 0, NULL);

                const std::wstring msg = L"Failed to load \"" + modCode.second + L"\"\n" + std::wstring(buffer, msgSize);
                MessageBoxW(nullptr, msg.c_str(), L"Hite Mod Loader", MB_OK);

                LocalFree(buffer);

                LOG(" - Failed to load \"%ls\"", modCode.second.c_str())

                continue;
            }

            ModsInfo->CurrentMod = modCode.first;

            auto initEvent = (DllInitEvent*)GetProcAddress(module, "Init");
            auto postInitEvent = (DllEvent*)GetProcAddress(module, "PostInit");
            auto onFrameEvent = (DllEvent*)GetProcAddress(module, "OnFrame");
            auto onRSDKFrameEvent = (DllEvent*)GetProcAddress(module, "OnRsdkFrame");
            auto onExitEvent = (DllEvent*)GetProcAddress(module, "OnExit");

            if (initEvent)
                initEvent(ModsInfo);

            if (postInitEvent)
                postInitEvents.push_back(postInitEvent);

            if (onFrameEvent)
                RunCoreEvents.push_back(onFrameEvent);

            if (onRSDKFrameEvent)
                RSDKLoopEvents.push_back(onRSDKFrameEvent);

            if (onExitEvent)
                ExitEvents.push_back(onExitEvent);
        }

        for (auto& dllEvent : postInitEvents)
            dllEvent();

        SetCurrentDirectoryW(currentDirectory);
        SetDllDirectoryW(dllDirectory);
    }
}