#include <pch.h>
#include "SaveRedirection.h"
#include "Loader.h"
#include "ConfigLoader.h"
#include "SigScan.h"
#include "Helpers.h"
#include <filesystem>

std::string SaveFilePath = "";

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
            {
                if (CreateDirectoryRecursively(dir))
                {
                    lpFileName = redirectedPath.c_str();
                }
                else
                {
                    std::string error = std::string("Failed to create directories") +
                        "\n\nThe game will now save to the normal location." +
                        "\nCode: " + std::to_string(GetLastError()) +
                        "\nPath: " + std::string(dir.begin(), dir.end());
                    LOG("[SaveRedirection] Save redirection error: %s", error.c_str());
                    MessageBoxA(NULL, error.c_str(), "Save Redirection Error!", MB_ICONERROR);
                }
            }
            else
            {
                lpFileName = redirectedPath.c_str();
            }

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
        // I forgot to keep the string in scope
        return originalKernelBaseCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
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

    if (SaveFilePath.empty())
    {
        LOG("[SaveRedirection] SaveFilePath is empty! Save redirection will be disabled!");
        return;
    }

    INSTALL_HOOK(KernelBaseCreateFileA);
    INSTALL_HOOK(Kernel32GetFileAttributesA);
}