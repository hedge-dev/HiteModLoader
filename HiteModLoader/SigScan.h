#pragma once

extern bool SigValid;

// HE
extern void* SigRunCore();

// RSDK
extern void* SigEngine_LoadFile();
extern void* SigEngine_HandleGameLoop();

// Cri
extern void* SigCrifsiowin_CreateFile();
extern void* SigCriFsIoWin_Exists();
extern void* SigCriErr_Notify();

// Logging
extern void* SigPrint1();