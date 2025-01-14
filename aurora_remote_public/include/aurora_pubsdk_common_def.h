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




/**
 * @brief The default port for the remote server
 */
#define SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PORT 7447

/**
 * @brief The default protocol for the remote server
 */
#define SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PROTOCOL "tcp"

/**
 * @brief The default timeout for the remote server
 */
#define SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_TIMEOUT 5000


#define SLAMTEC_AURORA_SDK_LIDAR_2D_GRIDMAP_DEFAULT_RESOLUTION 0.05f //meter
#define SLAMTEC_AURORA_SDK_LIDAR_2D_GRIDMAP_DEFAULT_WIDTH 300 //meter
#define SLAMTEC_AURORA_SDK_LIDAR_2D_GRIDMAP_DEFAULT_HEIGHT 300 //meter


/**
 * @brief The error code value type
 * @details This type is used to represent the error code value. The value is selected from  @ref slamtec_aurora_sdk_errorcode_types
 * @ingroup SDK_Basic_Data_Types Basic Data Types
 */
typedef uint32_t slamtec_aurora_sdk_errorcode_t;

/**
 * @brief The error code types
 * @ingroup SDK_Basic_Data_Types Common Definitions
 */
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



