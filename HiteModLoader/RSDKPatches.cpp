#include "pch.h"
#include "HiteModLoader.h"
#include "Helpers.h"
#include "SigScan.h"

void InitRSDKPatches()
{
    WRITE_MEMORY_ARRAY(SigDataHapticsv3(), "Use_Origins");
    WRITE_MEMORY_ARRAY(SigDataHapticsv4(), "USE_ORIGINS");
}