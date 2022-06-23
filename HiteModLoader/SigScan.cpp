#include "pch.h"
#include "SigScan.h"
#include <Psapi.h>

bool SigValid = true;

MODULEINFO moduleInfo;

const MODULEINFO& getModuleInfo()
{
    if (moduleInfo.SizeOfImage)
        return moduleInfo;

    ZeroMemory(&moduleInfo, sizeof(MODULEINFO));
    GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &moduleInfo, sizeof(MODULEINFO));

    return moduleInfo;
}

void* sigScan(const char* signature, const char* mask)
{
    const MODULEINFO& moduleInfo = getModuleInfo();
    const size_t length = strlen(mask);

    for (size_t i = 0; i < moduleInfo.SizeOfImage; i++)
    {
        char* memory = (char*)moduleInfo.lpBaseOfDll + i;

        size_t j;
        for (j = 0; j < length; j++)
            if (mask[j] != '?' && signature[j] != memory[j])
                break;

        if (j == length)
            return memory;
    }

    return nullptr;
}

#define SIG_SCAN(x, ...) \
    void* x##Addr; \
    void* x() \
    { \
        static const char* x##Data[] = { __VA_ARGS__ }; \
        if (!x##Addr) \
        { \
            for (int i = 0; i < _countof(x##Data); i += 2) \
            { \
                x##Addr = sigScan(x##Data[i], x##Data[i + 1]); \
                if (x##Addr) \
                    return x##Addr; \
                } \
            SigValid = false; \
        } \
        return x##Addr; \
    }

// Scans
SIG_SCAN(SigCrifsiowin_CreateFile, "\x40\x53\x55\x56\x57\x41\x54\x41\x56\x41\x57\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x84\x24\x00\x00\x00\x00\x83\x3D\x00\x00", "xxxxxxxxxxxxxx????xxx????xxxxxxx????xx??");
SIG_SCAN(SigCriFsIoWin_Exists,     "\x48\x89\x5C\x24\x00\x57\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x31\xE0\x48\x89\x84\x24\x00\x00\x00\x00\x48\x89\xD3\x48\x89\xCF\x48\x85\xC9\x74\x7C\x48\x85\xD2\x74\x77\x83\x3D\x00\x00\x00\x00\x00\x74\x50\xE8\x00\x00\x00\x00\x48\x8D\x4C\x24\x00\x8B\x15\x00\x00\x00\x00\x81\xF2\x00\x00\x00\x00\x89\x54\x24\x28\x48\x89\x4C\x24\x00\x49\x89\xF8", "xxxx?xxxx????xxx????xxxxxxx????xxxxxxxxxxxxxxxxxx?????xxx????xxxx?xx????xx????xxxxxxxx?xxx");
SIG_SCAN(SigCriErr_Notify,         "\x48\x8B\xC4\x48\x89\x58\x08\x48\x89\x68\x10\x48\x89\x70\x18\x48\x89\x78\x20\x41\x56\x48\x83\xEC\x30\x45\x33\xC9\x48\x8B\xFA\x4C\x39\x0D\x00\x00\x00\x00\x8B\xF1", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx????xx");
