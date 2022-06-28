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
SIG_SCAN(SigRunCore,               "\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x57\x48\x83\xEC\x50\x48\x8B\x99\x00\x00\x00\x00\x48\x8B\xF9\x48\x8B\x81\x00\x00\x00\x00\x48\x8D\x34\xC3\x48\x3B\xDE\x74", "xxxx?xxxx?xxxxxxxx????xxxxxx????xxxxxxxx")

SIG_SCAN(SigEngine_LoadFile,       "\x48\x89\x5C\x24\x00\x57\x48\x81\xEC\x00\x00\x00\x00\x48\x83\x79\x00\x00\x41\x8B\xF8\x48\x8B\xD9\x0F\x85\x00\x00\x00\x00\x41\xB8\x00\x00\x00\x00\x48\x8D\x4C\x24", "xxxx?xxxx????xxx??xxxxxxxx????xx????xxxx");
SIG_SCAN(SigEngine_HandleGameLoop, "\x48\x83\xEC\x28\x8B\x05\x00\x00\x00\x00\x83\xF8\x03\x0F\x85\x00\x00\x00\x00\x80\x3D\x00\x00\x00\x00\x00\x0F\x84\x00\x00\x00\x00\x0F\xB6\x05\x00\x00\x00\x00\xFF", "xxxxxx????xxxxx????xx?????xx????xxx????x")

SIG_SCAN(SigCrifsiowin_CreateFile, "\x40\x53\x55\x56\x57\x41\x54\x41\x56\x41\x57\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x84\x24\x00\x00\x00\x00\x83\x3D\x00\x00", "xxxxxxxxxxxxxx????xxx????xxxxxxx????xx??");
SIG_SCAN(SigCriFsIoWin_Exists,     "\x48\x89\x5C\x24\x00\x57\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x31\xE0\x48\x89\x84\x24\x00\x00\x00\x00\x48\x89\xD3\x48\x89\xCF\x48\x85\xC9", "xxxx?xxxx????xxx????xxxxxxx????xxxxxxxxx");
SIG_SCAN(SigCriErr_Notify,         "\x48\x8B\xC4\x48\x89\x58\x08\x48\x89\x68\x10\x48\x89\x70\x18\x48\x89\x78\x20\x41\x56\x48\x83\xEC\x30\x45\x33\xC9\x48\x8B\xFA\x4C\x39\x0D\x00\x00\x00\x00\x8B\xF1", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx????xx");

SIG_SCAN(SigPrint1,                "\x4C\x89\x44\x24\x00\x4C\x89\x4C\x24\x00\xC3", "xxxx?xxxx?x");