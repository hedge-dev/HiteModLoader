#include "pch.h"
#include "LoggingHandler.h"
#include "HiteModLoader.h"
#include "Helpers.h"
#include "SigScan.h"

HOOK(int, __fastcall, Print1, SigPrint1(), RetroPrintModes printMode, const char* format, const char* p1, const char* p2, const char* p3)
{
    // Display error message on script error
    if (printMode == PRINTMODE_SCRIPTERROR)
    {
        char buffer[0x400] = {};
        sprintf(buffer, format, p1, p2, p3);
        MessageBoxA(NULL, buffer, "Script Error", MB_ICONERROR);
    }
    return printf((std::string("[Engine] ") + format).c_str(), p1, p2, p3);
}

void InitLoggingHandler()
{
    INSTALL_HOOK(Print1);
}