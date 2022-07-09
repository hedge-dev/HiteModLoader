#pragma once
#include <HiteModLoader.h>

#define LOG(x, ...) \
    { \
        if (ConsoleEnabled) \
        { \
            printf("[HML] "##x##"\n", __VA_ARGS__); \
        } \
    }

// Taken from Rubberduckycooly/Sonic-1-2-2013-Decompilation/RSDKv4/Userdata.cpp#L1225
enum NotifyCallbackIDs
{
    NOTIFY_DEATH_EVENT        = 128,
    NOTIFY_TOUCH_SIGNPOST     = 129,
    NOTIFY_HUD_ENABLE         = 130,
    NOTIFY_ADD_COIN           = 131,
    NOTIFY_KILL_ENEMY         = 132,
    NOTIFY_SAVESLOT_SELECT    = 133,
    NOTIFY_FUTURE_PAST        = 134,
    NOTIFY_GOTO_FUTURE_PAST   = 135,
    NOTIFY_BOSS_END           = 136,
    NOTIFY_SPECIAL_END        = 137,
    NOTIFY_DEBUGPRINT         = 138,
    NOTIFY_KILL_BOSS          = 139,
    NOTIFY_TOUCH_EMERALD      = 140,
    NOTIFY_STATS_ENEMY        = 141,
    NOTIFY_STATS_CHARA_ACTION = 142,
    NOTIFY_STATS_RING         = 143,
    NOTIFY_STATS_MOVIE        = 144,
    NOTIFY_STATS_PARAM_1      = 145,
    NOTIFY_STATS_PARAM_2      = 146,
    NOTIFY_CHARACTER_SELECT   = 147,
    NOTIFY_SPECIAL_RETRY      = 148,
    NOTIFY_TOUCH_CHECKPOINT   = 149,
    NOTIFY_ACT_FINISH         = 150,
    NOTIFY_1P_VS_SELECT       = 151,
    NOTIFY_CONTROLLER_SUPPORT = 152,
    NOTIFY_STAGE_RETRY        = 153,
    NOTIFY_SOUND_TRACK        = 154,
    NOTIFY_GOOD_ENDING        = 155,
    NOTIFY_BACK_TO_MAINMENU   = 156,
    NOTIFY_LEVEL_SELECT_MENU  = 157,
    NOTIFY_PLAYER_SET         = 158,
    NOTIFY_EXTRAS_MODE        = 159,
    NOTIFY_SPIN_DASH_TYPE     = 160,
    NOTIFY_TIME_OVER          = 161,
};

extern bool ConsoleEnabled;
extern Platform CurrentPlatform;
extern ModLoader ModLoaderData;
extern bool ReturnToTitle;

extern void InitLoaders();