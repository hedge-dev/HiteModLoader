#pragma once
#include <filesystem>
#include <CommonLoader.h>
#include <HiteModLoader.h>

typedef void DllEvent();
typedef void DllInitEvent(ModInfo*);

// Events
extern std::vector<DllEvent*> RunCoreEvents;
extern std::vector<DllEvent*> RSDKLoopEvents;
extern std::vector<DllEvent*> ExitEvents;

extern void RaiseEvents(std::vector<DllEvent*> events);
extern void InitCodeLoader();