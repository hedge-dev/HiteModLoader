#include "pch.h"
#include "LoggingHandler.h"
#include "HiteModLoader.h"
#include "Helpers.h"
#include "SigScan.h"
#include "Loader.h"

HOOK(int, __fastcall, Print1, SigPrint1(), RetroPrintModes printMode, const char* format, const char* p1, const char* p2, const char* p3)
{
    bool newLine = false;

    // Display error message on script error
    if (printMode == PRINTMODE_SCRIPTERROR)
    {
        char buffer[0x400] = {};
        sprintf(buffer, format, p1, p2, p3);
        MessageBoxA(NULL, buffer, "Script Error", MB_ICONERROR);

        // Add new line as this message is not intended to be written to a console
        newLine = true;

        // Return to title screen
        ReturnToTitle = true;
    }
    return printf((std::string("[Engine] ") + format + (newLine ? "\n" : "")).c_str(), p1, p2, p3);
}

void InitLoggingHandler()
{
    INSTALL_HOOK(Print1);
}