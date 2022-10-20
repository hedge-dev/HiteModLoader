#include <pch.h>
#include "SaveRedirection.h"
#include "Loader.h"
#include "ConfigLoader.h"
#include "SigScan.h"
#include "Helpers.h"
#include <filesystem>

std::string SaveFilePath;

HOOK(HANDLE, __fastcall, KernelBaseCreateFileA, PROC_ADDRESS("Kernel32.dll", "CreateFileA"), 
    LPCSTR                lpFileName,
    DWORD                 dwDesiredAccess,
    DWORD                 dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD                 dwCreationDisposition,
    DWORD                 dwFlagsAndAttributes,
    HANDLE                hTemplateFile)
{
    std::string filePath = lpFileName;

    if (filePath.find("data.dat") != std::string::npos)
    {
        size_t splitPos = filePath.substr(0, filePath.length() - 9).find_last_of('/');
        if (splitPos == std::string::npos)
        {
            LOG("[SaveRedirection] Substring failed! filePath = \"%s\"", filePath.c_str());
            return originalKernelBaseCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
        }
        std::string relativePath = filePath.substr(splitPos + 1);
        std::string redirectedPath = SaveFilePath + "/" + relativePath;
        if (dwDesiredAccess & GENERIC_WRITE)
        {
            LOG("[SaveRedirection] Saving \"%s\" to \"%s\"", relativePath.c_str(), redirectedPath.c_str());

            // Create directories
            auto dir = GetDirectoryPath(redirectedPath);
            if (!DirExists(dir))
                std::filesystem::create_directories(dir);

            // Ensure dir exists
            if (DirExists(dir))
                lpFileName = redirectedPath.c_str();
            else
                MessageBoxA(NULL, "Failed to handle save redirection!\n\nClose this process now if you do not want your main save file overwritten!!!"
                    , "Save Redirection Error!", MB_ICONERROR);
        } else if (dwDesiredAccess & GENERIC_READ)
        {
            if (GetFileAttributesA(redirectedPath.c_str()) == -1)
            {
                LOG("[SaveRedirection] Loading main save for \"%s\"", relativePath.c_str());
            } else
            {
                LOG("[SaveRedirection] Loading \"%s\" from \"%s\"", relativePath.c_str(), redirectedPath.c_str());
                lpFileName = redirectedPath.c_str();
            }
        }
    }
    return originalKernelBaseCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

// NOTE: If saves are still broken, its prob this hook below
HOOK(DWORD, __fastcall, Kernel32GetFileAttributesA, PROC_ADDRESS("Kernel32.dll", "GetFileAttributesA"), LPCSTR lpFileName)
{
    std::string filePath = lpFileName;

    if (filePath.find("data.dat") != std::string::npos)
    {
        size_t splitPos = filePath.substr(0, filePath.length() - 9).find_last_of('/');
        if (splitPos == std::string::npos)
        {
            LOG("[SaveRedirection] Substring failed! filePath = \"%s\"", filePath.c_str());
            return originalKernel32GetFileAttributesA(lpFileName);
        }
        std::string relativePath = filePath.substr(splitPos + 1);
        std::string redirectedPath = SaveFilePath + "/" + relativePath;

        if (originalKernel32GetFileAttributesA(redirectedPath.c_str()) == -1)
        {
            LOG("[SaveRedirection] No redirected save found, using main save for \"%s\"", relativePath.c_str());
        }
        else
        {
            LOG("[SaveRedirection] Redirecting \"%s\" to \"%s\"", relativePath.c_str(), redirectedPath.c_str());
            lpFileName = redirectedPath.c_str();
        }
    }
    return originalKernel32GetFileAttributesA(lpFileName);
}

void InitSaveRedirection()
{
    LOG("Setting up SaveRedirection...");
    
    // Handle fall backs
    if (SaveFilePath.empty())
        SaveFilePath = SaveFileOverride.c_str();
    if (SaveFilePath.empty())
        SaveFilePath = SaveFileFallback.c_str();

    LOG("[SaveRedirection] SaveFilePath = \"%s\"", SaveFilePath.c_str());

    INSTALL_HOOK(KernelBaseCreateFileA);
    INSTALL_HOOK(Kernel32GetFileAttributesA);
}