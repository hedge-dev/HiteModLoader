#pragma once
#include <cstdio>
#include <fstream>
#include <vector>
#include <Shlwapi.h>

#define VERSION "0.1.1.0"
#define PATH_LIMIT 0x400

inline bool FileExists(const char* fileName)
{
    return GetFileAttributesA(fileName) != -1;
}

inline bool DirExists(const std::string& dirName_in)
{
    DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;
    if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
        return true;
    return false;
}

inline std::string GetDirectoryPath(const std::string& path)
{
    std::string directoryPath;

    const size_t index = path.find_last_of("\\/");
    if (index != std::string::npos)
        directoryPath = path.substr(0, index + 1);

    return directoryPath;
}

inline std::wstring ConvertMultiByteToWideChar(const std::string& value)
{
    WCHAR wideChar[PATH_LIMIT];
    MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, wideChar, _countof(wideChar));

    return std::wstring(wideChar);
}

enum Platform
{
    Platform_All    = 0,
    Platform_Steam  = 1,
    Platform_Epic   = 2,
    Platform_Switch = 3
};

struct Mod
{
    const char* Name;
    const char* Path;
};

struct ModInfo
{
    int ModLoaderVersion = 1;
    std::vector<Mod*>* ModList;
    Mod* CurrentMod;
    Platform CurrentPlatform;
};

// IDA Types
typedef uint64_t _QWORD;
typedef uint32_t _DWORD;
typedef uint16_t _WORD;
typedef uint8_t _BYTE;