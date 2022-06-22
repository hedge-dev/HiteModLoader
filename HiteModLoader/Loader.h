#pragma once
#include <HiteModLoader.h>

#define LOG(x, ...) \
    { \
        if (ConsoleEnabled) \
        { \
            printf("[HML] "##x##"\n", __VA_ARGS__); \
        } \
    }

extern bool ConsoleEnabled;

extern void InitLoaders();