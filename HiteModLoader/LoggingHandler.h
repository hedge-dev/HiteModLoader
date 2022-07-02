#pragma once

enum RetroPrintModes
{
	PRINTMODE_NORMAL,
	PRINTMODE_POPUP,
	PRINTMODE_ERROR,
	PRINTMODE_FATAL,
	PRINTMODE_SCRIPTERROR
};

extern void InitLoggingHandler();