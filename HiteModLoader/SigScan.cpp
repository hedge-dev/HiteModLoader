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
SIG_SCAN(SigCrifsiowin_CreateFile, "\x89\x4C\x24\x08\x55", "xxx?x");
SIG_SCAN(SigCriFsIoWin_Exists, "\x89\x4C\x24\x08\x55", "xxx?x");
SIG_SCAN(SigCriErr_Notify, "\x89\x4C\x24\x08\x55", "xxx?x");
