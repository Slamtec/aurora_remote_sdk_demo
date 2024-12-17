/*
 *  SLAMTEC Aurora
 *  Copyright 2013 - 2024 SLAMTEC Co., Ltd.
 *
 *  http://www.slamtec.com
 *
 *  Aurora Remote SDK
 *  Main Header of the SDK
 *
 */


#pragma once

// extern C if in C++
#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>
#include "aurora_pubsdk_common_def.h"
#include "aurora_pubsdk_objects.h"


/**
 * @defgroup Aurora_SDK_Pure_C Aurora Remote SDK Pure C Interface
 * @brief Main functions and structures for the Aurora Remote SDK using pure C
 *
 * This module contains the core functionality of the Aurora Remote SDK,
 * including session management, controller operations, and map management.
 * 
 * A C++ interface is also available for more advanced features.
 *
 * @{
 */

/**
 * @defgroup Session_Management Session Management
 * @brief Functions for creating and managing SDK sessions
 */

/**
 * @defgroup Controller_Operations Controller Operations
 * @brief Functions for controlling and interacting with Aurora servers
 */

/**
 * @defgroup Map_Management Map Management
 * @brief Functions for managing and manipulating map data
 */

 /**
 * @defgroup DataProvider_Operations Data Provider Operations
 * @brief Functions for accessing data from the remote Device
 */

/** @} */ // end of Aurora_SDK group


/**
 * @brief Get the version info of the SDK
 * @ingroup Session_Management Session Management
 * 
 * @param info_out - the version info will be stored in this pointer
 * @return slamtec_aurora_sdk_errorcode_t 
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_get_version_info(slamtec_aurora_sdk_version_info_t * info_out);


/**
 * @brief Create a SDK session
 * @ingroup Session_Management Session Management
 * 
 * @param config - the session configuration, pass NULL to use default configuration
 * @param config_size - the size of the session configuration, pass 0 to use default configuration
 * @param listener - the listener for the session events, pass NULL if no listener is needed
 * @param error_code - if provided, the error code will be stored in this pointer
 * @return the session handle, which should be released by calling slamtec_aurora_sdk_release_session
 */
slamtec_aurora_sdk_session_handle_t AURORA_SDK_API slamtec_aurora_sdk_create_session(const slamtec_aurora_sdk_session_config_t* config, size_t config_size, const slamtec_aurora_sdk_listener_t * listener, slamtec_aurora_sdk_errorcode_t * error_code);

/**
 * @brief Release a SDK session
 * @ingroup Session_Management Session Management
 * 
 * @param handle - the session handle to be released
 */
void AURORA_SDK_API slamtec_aurora_sdk_release_session(slamtec_aurora_sdk_session_handle_t handle);


// controller operations

/**
 * @brief Get the discovered servers list.
 * @details Once the session is created, the SDK will start to discover the servers in the local network in a periodical way using a background thread.
 * @details This function is used to get the discovered servers list.
 * @ingroup Controller_Operations Controller Operations
 * 
 * @param handle - the session handle
 * @param servers - the array to store the discovered servers, caller should provide a valid pointer to a buffer
 * @param max_server_count - the maximum number of servers to be stored in the buffer
 * @return the number of discovered servers, <0 means error
 */
int AURORA_SDK_API slamtec_aurora_sdk_controller_get_discovered_servers(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_server_connection_info_t* servers, size_t max_server_count);

/**
 * @brief Connect to a server
 * @ingroup Controller_Operations Controller Operations
 * 
 * @param handle - the session handle
 * @param server_conn_info - the server connection information, refer to slamtec_aurora_sdk_server_connection_info_t for more details \n
 *                           A server connection info may contains multiple server addresses/protocols/ports, and the SDK will try to connect to the servers in the list in sequence
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_controller_connect(slamtec_aurora_sdk_session_handle_t handle, const slamtec_aurora_sdk_server_connection_info_t* server_conn_info);

/**
 * @brief Disconnect from a server
 * @ingroup Controller_Operations Controller Operations
 * 
 * @param handle - the session handle
 */
void AURORA_SDK_API slamtec_aurora_sdk_controller_disconnect(slamtec_aurora_sdk_session_handle_t handle);

/**
 * @brief Check if the session is connected to a server
 * @ingroup Controller_Operations Controller Operations
 * 
 * @param handle - the session handle
 * @return non-zero means connected
 */
int AURORA_SDK_API slamtec_aurora_sdk_controller_is_connected(slamtec_aurora_sdk_session_handle_t handle);

/**
 * @brief Set the low rate mode
 * @details Ask the controller to subscribe less data from the server, this can reduce the network bandwidth and improve the performance.
 * @details Some SDK operations will set low rate mode during its operation automatically. For example, downloading or uploading maps will set low rate mode.
 * @details Once the low rate mode is set, Raw camera image data and IMU data receiving will be disabled.
 * @ingroup Controller_Operations Controller Operations
 * 
 * @param handle - the session handle
 * @param enable - non-zero to enable, zero to disable
 */
void AURORA_SDK_API slamtec_aurora_sdk_controller_set_low_rate_mode(slamtec_aurora_sdk_session_handle_t handle, int enable);

/**
 * @brief Set the map data syncing
 * @details Ask the controller to sync the map data from the server using a background thread.
 * @details If the caller application wishes to access the map data, it should set this to enabled.
 * @details Not all the map data will be synced at once. The SDK applies a QoS policy to the map data syncing operation to reduce network traffic.
 * @ingroup Controller_Operations Controller Operations
 * 
 * @param handle - the session handle
 * @param enable - non-zero to enable, zero to disable
 */
void AURORA_SDK_API slamtec_aurora_sdk_controller_set_map_data_syncing(slamtec_aurora_sdk_session_handle_t handle, int enable);

/**
 * @brief Resync the map data
 * @details Ask the controller to resync the map data from the server.
 * @details If the invalidate_cache is non-zero, the local map data cache will be invalidated.
 * @ingroup Controller_Operations Controller Operations
 * 
 * @param handle - the session handle
 * @param invalidate_cache - non-zero to invalidate the local map data cache, zero to keep the local map data
 */
void AURORA_SDK_API slamtec_aurora_sdk_controller_resync_map_data(slamtec_aurora_sdk_session_handle_t handle, int invalidate_cache);

/**
 * @brief Set the raw data subscription
 * @details Ask the controller to subscribe the raw camera image data from the server.
 * @details As the raw image data is not compressed, it may cost a lot of network bandwidth.
 * @ingroup Controller_Operations Controller Operations
 * 
 * @param handle - the session handle
 * @param enable - non-zero to enable, zero to disable
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_controller_set_raw_data_subscription(slamtec_aurora_sdk_session_handle_t handle, int enable);

/**
 * @brief Check if the raw data is subscribed
 * @ingroup Controller_Operations Controller Operations
 * 
 * @param handle - the session handle
 * @return non-zero means subscribed
 */
int AURORA_SDK_API slamtec_aurora_sdk_controller_is_raw_data_subscribed(slamtec_aurora_sdk_session_handle_t handle);

/**
 * @brief Require the remote Device to reset the map, a.k.a. clear all the map data and restart the mapping process
 * @ingroup Controller_Operations Controller Operations
 * 
 * @param handle - the session handle
 * @param timeout_ms - the timeout in milliseconds
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_controller_require_map_reset(slamtec_aurora_sdk_session_handle_t handle, uint64_t timeout_ms);

/**
 * @brief Require the remote Device to enter the pure localization mode, the map data will not be updated
 * @ingroup Controller_Operations Controller Operations
 * 
 * @param handle - the session handle
 * @param timeout_ms - the timeout in milliseconds
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_controller_require_pure_localization_mode(slamtec_aurora_sdk_session_handle_t handle, uint64_t timeout_ms);


/**
 * @brief Require the remote Device to enter the mapping mode
 * @ingroup Controller_Operations Controller Operations
 * 
 * @param handle - the session handle
 * @param timeout_ms - the timeout in milliseconds
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_controller_require_mapping_mode(slamtec_aurora_sdk_session_handle_t handle, uint64_t timeout_ms);

/**
 * @brief Require the remote Device to enter the relocalization mode
 * @ingroup Controller_Operations Controller Operations
 * 
 * @param handle - the session handle
 * @param timeout_ms - the timeout in milliseconds
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_controller_require_relocalization(slamtec_aurora_sdk_session_handle_t handle, uint64_t timeout_ms);


/**
 * @brief Require the remote Device to cancel the relocalization process
 * @ingroup Controller_Operations Controller Operations
 * 
 * @param handle - the session handle
 * @param timeout_ms - the timeout in milliseconds
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_controller_cancel_relocalization(slamtec_aurora_sdk_session_handle_t handle, uint64_t timeout_ms);

/**
 * @brief Send a custom command to the remote Device
 * @ingroup Controller_Operations Controller Operations
 * 
 * @param handle - the session handle
 * @param timeout_ms - the timeout in milliseconds
 * @param cmd - the command to be sent
 * @param data - the data buffer contains the command payload to be sent
 * @param data_size - the size of the data buffer
 * @param response - the response buffer to store the response data
 * @param response_buffer_size - the size of the response buffer
 * @param response_retrieved_size - the size of the response data retrieved
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_controller_send_custom_command(slamtec_aurora_sdk_session_handle_t handle, uint64_t timeout_ms, uint64_t cmd, const void* data, size_t data_size, void* response, size_t response_buffer_size, size_t * response_retrieved_size);


// map manager operations


/**
 * @brief Start a map storage session
 * @details Ask the remote Device to start a map storage session.
 * @details If there is already an active map storage session, the new request will be rejected.
 * @details NOTICE: the SDK will enter low rate mode during the working session to reduce the data traffic
 * @details the low rate mode will be automatically disabled after the map streaming operation is done
 * @ingroup Map_Management Map Management
 * 
 * @param handle - the session handle
 * @param map_file_name - the name of the map file to be stored or to be loaded
 * @param session_type - the type of the map storage session, e.g. Load or Store.\n Refer to slamtec_aurora_sdk_mapstorage_session_type_t for more details
 * @param callback - the callback function to be called when the map storage session is completed. The operation result will be passed to the callback function
 * @param user_data - the user data to be passed to the callback function
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_mapmanager_start_storage_session(slamtec_aurora_sdk_session_handle_t handle, const char* map_file_name, slamtec_aurora_sdk_mapstorage_session_type_t session_type, slamtec_aurora_sdk_mapstorage_session_result_callback_t  callback, void * user_data);


/**
 * @brief Abort the current map storage session
 * @details Caller can use this function to abort the current map storage session.
 * @details If a map storage session is not created by the current SDK instance, the request will be rejected.
 * @ingroup Map_Management Map Management
 * 
 * @param handle - the session handle
 */
void AURORA_SDK_API slamtec_aurora_sdk_mapmanager_abort_session(slamtec_aurora_sdk_session_handle_t handle);


/**
 * @brief Check if the map storage session is in progress
 * @details Caller can use this function to check if the map storage session is still in progress. 
 * @ingroup Map_Management Map Management
 * 
 * @param handle - the session handle
 * @return non-zero means active
 */
int AURORA_SDK_API slamtec_aurora_sdk_mapmanager_is_storage_session_active(slamtec_aurora_sdk_session_handle_t handle);



/**
 * @brief Query the progress of the current map storage session
 * @details Caller can use this function to query the progress of the map saving or loading operation from the remote Device.
 * @ingroup Map_Management Map Management
 * 
 * @param handle - the session handle
 * @param progress_out - the progress will be stored in this pointer
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_mapmanager_query_storage_status(slamtec_aurora_sdk_session_handle_t handle,  slamtec_aurora_sdk_mapstorage_session_status_t* progress_out);


// dataprovider operations

/**
 * @brief Get the current pose (base to world) in SE3 format
 * @details Caller can use this function to get the current pose in SE3 format.
 * @details The pose data retrieved is the cached data from previous fetched by the background data sync thread.
 * @details The pose data may be outdated. If caller needs the latest pose data, it should using the SDK listener to get the pose update event.
 * @ingroup DataProvider_Operations Data Provider Operations
 * 
 * @param handle - the session handle
 * @param pose_out - the pose will be stored in this pointer
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_get_current_pose_se3(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_pose_se3_t* pose_out);

/**
 * @brief Get the current pose (base to world) in Euler angles (Roll-Pitch-Yaw  RPY order) format
 * @details Caller can use this function to get the current pose in Euler angles format.
 * @details The pose data retrieved is the cached data from previous fetched by the background data sync thread.
 * @details The pose data may be outdated. If caller needs the latest pose data, it should using the SDK listener to get the pose update event.
 * @details WARNING: gimbal lock may happen, please use the SE3 pose if possible.
 * @ingroup DataProvider_Operations Data Provider Operations
 * 
 * @param handle - the session handle
 * @param pose_out - the pose will be stored in this pointer
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_get_current_pose(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_pose_t* pose_out);

/**
 * @brief Get the current mapping flags
 * @details Caller can use this function to get the current SLAM working flags, e.g. whether Loop Closure is disabled or not
 * @ingroup DataProvider_Operations Data Provider Operations
 * 
 * @param handle - the session handle
 * @param flags_out - the flags will be stored in this pointer
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_get_mapping_flags(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_mapping_flag_t * flags_out);

/**
 * @brief Get the last device status
 * @details Caller can use this function to get the last device status.
 * @ingroup DataProvider_Operations Data Provider Operations
 * 
 * @param handle - the session handle
 * @param status_out - the status will be stored in this pointer
 * @param timestamp_ns_out - the timestamp will be stored in this pointer
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_get_last_device_status(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_device_status_t* status_out, uint64_t * timestamp_ns_out);

/**
* @brief Get the relocalization status
* @details Caller can use this function to get the relocalization status.
* @ingroup DataProvider_Operations Data Provider Operations
* 
* @param handle - the session handle
* @param status_out - the status will be stored in this pointer
* @param timestamp_ns_out - the timestamp will be stored in this pointer
* @return the error code
*/  
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_get_relocalization_status(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_relocalization_status_t * status_out, uint64_t * timestamp_ns_out);


/**
 * @brief Peek the tracking data
 * @details Caller can use this function to peek the latest tracking data.
 * @details The tracking data is the cached data from previous fetched by the background data sync thread.
 * @details The tracking data may be outdated. If caller needs the latest tracking data, it should using the SDK listener to get the tracking update event.
 * @ingroup DataProvider_Operations Data Provider Operations
 * 
 * @param handle - the session handle
 * @param tracking_data_out - the tracking data will be stored in this pointer
 * @param provided_buffer_info - the buffer information
 * @return the error code, SLAMTEC_AURORA_SDK_ERRORCODE_NOT_READY will be returned if there is no tracking data available
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_peek_tracking_data(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_tracking_info_t* tracking_data_out, const slamtec_aurora_sdk_tracking_data_buffer_t* provided_buffer_info);

/**
 * @brief Peek the IMU data
 * @details Caller can use this function to peek the latest cached IMU data.
 * @details The IMU data is the cached data from previous fetched by the background data sync thread.
 * @details The IMU data may be outdated. If caller needs the latest IMU data, it should using the SDK listener to get the IMU update event.
 * @ingroup DataProvider_Operations Data Provider Operations
 * 
 * @param handle - the session handle
 * @param imu_data_out - the IMU data will be stored in this pointer
 * @param buffer_count - the buffer count
 * @param actual_count_out - the actual count of the IMU data
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_peek_imu_data(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_imu_data_t* imu_data_out, size_t buffer_count, size_t* actual_count_out);

/**
 * @brief Get the IMU info
 * @details Caller can use this function to get the IMU info.
 * @ingroup DataProvider_Operations Data Provider Operations
 * 
 * @param handle - the session handle
 * @param info_out - the IMU info will be stored in this pointer
 * @return the error code
 */

slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_get_imu_info(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_imu_info_t * info_out);


/**
 * @brief Get the global mapping info
 * @details Caller can use this function to get the global mapping info.
 * @ingroup DataProvider_Operations Data Provider Operations
 * 
 * @param handle - the session handle
 * @param desc_out - the global mapping info will be stored in this pointer
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_get_global_mapping_info(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_global_map_desc_t* desc_out);

/**
 * @brief Get all map info
 * @details Caller can use this function to get all map description info.
 * @ingroup DataProvider_Operations Data Provider Operations
 * 
 * @param handle - the session handle
 * @param desc_buffer - the buffer to store the map info
 * @param buffer_count - the buffer count
 * @param actual_count_out - the actual count of the map info
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_get_all_map_info(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_map_desc_t* desc_buffer, size_t buffer_count, size_t* actual_count_out);



/**
 * @brief Access the map data like keyframe and map points data
 * @details Caller can use this function to access the map data like keyframe and map points data.
 * @details A visitor object contains data callback listeners must be provided
 * @details those callbacks set to NULL will be ignored
 * @details the SDK will enter stall state during the data accessing,
 * @details i.e. the background data sync will  paused
 * @details if all map data should be accessed, simply pass NULL to the map_ids
 * @ingroup DataProvider_Operations Data Provider Operations
 * 
 * @param handle - the session handle
 * @param visitor - the visitor object contains data callback listeners
 * @param map_ids - the map ids to be accessed
 * @param map_count - the map count
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_access_map_data(slamtec_aurora_sdk_session_handle_t handle, const slamtec_aurora_sdk_map_data_visitor_t* visitor, uint32_t* map_ids, size_t map_count);


#ifdef __cplusplus
}
#endif



#ifdef __cplusplus

// include C++ headers
#include "cxx/slamtec_remote_public.hxx"

#endif