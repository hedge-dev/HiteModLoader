#include "pch.h"
#include "CodeLoader.h"
#include "Loader.h"

// Events
std::vector<DllEvent*> FrameEvents;
std::vector<DllEvent*> ExitEvents;

void InitCodeLoader(std::vector<std::wstring>& dllPaths)
{
    LOG("Loading code mods...");
    if (!dllPaths.empty())
    {
        std::vector<DllEvent*> postInitEvents;

        WCHAR currentDirectory[PATH_LIMIT];
        WCHAR dllDirectory[PATH_LIMIT];

        GetCurrentDirectoryW(_countof(currentDirectory), currentDirectory);
        GetDllDirectoryW(_countof(dllDirectory), dllDirectory);

        for (auto& dllFilePath : dllPaths)
        {
            const std::wstring directoryPath = std::filesystem::path(dllFilePath).parent_path().wstring();

            SetCurrentDirectoryW(directoryPath.c_str());
            SetDllDirectoryW(directoryPath.c_str());

            const HMODULE module = LoadLibraryW(dllFilePath.c_str());

            if (!module)
            {
                LPWSTR buffer = nullptr;

                const DWORD msgSize = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    nullptr, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&buffer, 0, NULL);

                const std::wstring msg = L"Failed to load \"" + dllFilePath + L"\"\n" + std::wstring(buffer, msgSize);
                MessageBoxW(nullptr, msg.c_str(), L"Hite Mod Loader", MB_OK);

                LocalFree(buffer);

                LOG(" - Failed to load \"%ls\"", dllFilePath.c_str())

                continue;
            }

            auto initEvent = (DllEvent*)GetProcAddress(module, "Init");
            auto postInitEvent = (DllEvent*)GetProcAddress(module, "PostInit");
            auto onFrameEvent = (DllEvent*)GetProcAddress(module, "OnFrame");
            auto onExitEvent = (DllEvent*)GetProcAddress(module, "OnExit");

            if (initEvent)
                initEvent();

            if (postInitEvent)
                postInitEvents.push_back(postInitEvent);

            if (onFrameEvent)
                FrameEvents.push_back(onFrameEvent);

            if (onExitEvent)
                FrameEvents.push_back(onExitEvent);
        }

        for (auto& dllEvent : postInitEvents)
            dllEvent();

        SetCurrentDirectoryW(currentDirectory);
        SetDllDirectoryW(dllDirectory);
    }
}