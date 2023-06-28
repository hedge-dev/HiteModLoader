#include "pch.h"
#include "CriLoader.h"
#include "Loader.h"
#include "ModLoader.h"
#include "SigScan.h"
#include "Helpers.h"

using std::string;

const char* PathSubString(const char* text)
{
    const char* result = strstr(text, "raw");
    if (result)
        return result + 4;
    return text;
}

HOOK_SIG(HANDLE, __fastcall, crifsiowin_CreateFile, SigCrifsiowin_CreateFile, CriChar8* path, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, int dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    // Mod Check
    DWORD attributes = -1;
    for (auto& value : ModIncludePaths)
    {
        string filePath = value + "raw/" + PathSubString(path);
        attributes = GetFileAttributesA(filePath.c_str());
        if (attributes != -1)
        {
            strcpy(path, filePath.c_str());
            LOG("[CriLoader] Loading File: %s", path);
            break;
        }
    }
    return originalcrifsiowin_CreateFile(path, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
        dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HOOK_SIG(CriError, __fastcall, criFsIoWin_Exists, SigCriFsIoWin_Exists, CriChar8* path, bool* exists)
{
    DWORD attributes = -1;
    for (auto& value : ModIncludePaths)
    {
        string filePath = value + "raw/" + PathSubString(path);
        attributes = GetFileAttributesA(filePath.c_str());
        if (attributes != -1)
        {
            strcpy(path, filePath.c_str());
            *exists = true;
            break;
        }
    }

    if (path && attributes == -1)
    {
        // TODO: Handle WCHAR
        attributes = GetFileAttributesA(path);
        *exists = attributes != -1 && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
    }
    else if (!*exists)
    {
        //criError_NotifyGeneric(CRIERR_LEVEL_ERROR, "E2015091137", CRIERR_INVALID_PARAMETER);
        return CRIERR_NG;
    }
    return CRIERR_OK;
}

HOOK_SIG(void, __fastcall, criErr_Notify, SigCriErr_Notify, CriErrorLevel level, const CriChar8* error_id, CriUintPtr p1, CriUintPtr p2)
{
    LOG("[criErr_Notify] Level: %d - %s", level, error_id);
}

// Not actually CreateFile, I ran out of ideas for a name
HOOK_SIG(__int64, __fastcall, CreateFile3, SigCreateFile3, __int64 a1, char* path)
{
    // Mod Check
    DWORD attributes = -1;
    for (auto& value : ModIncludePaths)
    {
        string filePath = value + "raw/" + PathSubString(path);
        attributes = GetFileAttributesA(filePath.c_str());
        if (attributes != -1)
        {
            strcpy(path, filePath.c_str());
            LOG("[CriLoader] Loading File: %s", path);
            break;
        }
    }
    return originalCreateFile3(a1, path);
}

void InitCriLoader()
{
    LOG("Setting up CriLoader...");
    INSTALL_HOOK(crifsiowin_CreateFile);
    INSTALL_HOOK(criFsIoWin_Exists);
    INSTALL_HOOK(criErr_Notify);
    INSTALL_HOOK(CreateFile3);
}