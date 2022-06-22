#pragma once

// Types
typedef char CriChar8;
typedef signed int CriSint32;
typedef unsigned int CriUint32;
typedef unsigned int* CriUintPtr;

typedef enum
{
    CRIERR_LEVEL_ERROR = 0,
    CRIERR_LEVEL_WARNING = 1,
    CRIERR_LEVEL_ENUM_BE_SINT32 = 0x7FFFFFFF
} CriErrorLevel;

typedef enum
{
    CRIERR_OK = 0,
    CRIERR_NG = -1,
    CRIERR_INVALID_PARAMETER = -2,
    CRIERR_FAILED_TO_ALLOCATE_MEMORY = -3,
    CRIERR_UNSAFE_FUNCTION_CALL = -4,
    CRIERR_FUNCTION_NOT_IMPLEMENTED = -5,
    CRIERR_LIBRARY_NOT_INITIALIZED = -6,
    CRIERR_ENUM_BE_SINT32 = 0x7FFFFFFF
} CriError;

// Functions
void InitCriLoader();
