/*
 *  SLAMTEC Aurora
 *  Copyright 2013 - 2024 SLAMTEC Co., Ltd.
 *
 *  http://www.slamtec.com
 * 
 *  Aurora Remote SDK
 *  Common Definitions
 * 
 */


#pragma once

#include <stdint.h>

#ifdef _WIN32

#define AURORA_MODULE_EXPORT  __declspec(dllexport)
#define AURORA_MODULE_IMPORT  __declspec(dllimport)


#else

#define AURORA_MODULE_EXPORT  __attribute__ ((visibility ("default"))) 
#define AURORA_MODULE_IMPORT

#endif




#if defined(AURORA_SDK_EXPORTS)
#define AURORA_SDK_API AURORA_MODULE_EXPORT
#else
#define AURORA_SDK_API AURORA_MODULE_IMPORT
#endif





#define SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PORT 7447
#define SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PROTOCOL "tcp"
#define SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_TIMEOUT 5000

typedef uint32_t slamtec_aurora_sdk_errorcode_t;

enum slamtec_aurora_sdk_errorcode_types {
    SLAMTEC_AURORA_SDK_ERRORCODE_OK = 0,
    SLAMTEC_AURORA_SDK_ERRORCODE_OP_FAILED = -1,
    SLAMTEC_AURORA_SDK_ERRORCODE_INVALID_ARGUMENT = -2,
    SLAMTEC_AURORA_SDK_ERRORCODE_NOT_SUPPORTED = -3,
    SLAMTEC_AURORA_SDK_ERRORCODE_NOT_IMPLEMENTED = -4,
    SLAMTEC_AURORA_SDK_ERRORCODE_TIMEOUT = -5,
    SLAMTEC_AURORA_SDK_ERRORCODE_IO_ERROR = -6,
    SLAMTEC_AURORA_SDK_ERRORCODE_NOT_READY = -7,
};