#pragma once
#include <filesystem>
#include <CommonLoader.h>

typedef void DllEvent();

// Events
extern std::vector<DllEvent*> FrameEvents;
extern std::vector<DllEvent*> ExitEvents;

extern void InitCodeLoader(std::vector<std::wstring>& dllPaths);