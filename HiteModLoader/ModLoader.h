#pragma once
#include <HiteModLoader.h>

extern std::vector<std::string> ModIncludePaths;
extern std::vector<std::pair<Mod*, std::wstring>> ModCodePaths;
extern std::vector<Mod*> Mods;

extern void InitModLoader();