#include "pch.h"
#include "HiteModLoader.h"
#include "Helpers.h"
#include "SigScan.h"

HOOK(int, __fastcall, Print1, SigPrint1(), __int64 a1, const char* format, const char* p1, const char* p2, const char* p3)
{
    return printf((std::string("[Sonic Origins] ") + format).c_str(), p1, p2, p3);
}

void InitLoggingHandler()
{
    INSTALL_HOOK(Print1);
}