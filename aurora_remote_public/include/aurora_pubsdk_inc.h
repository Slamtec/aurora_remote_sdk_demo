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


 /**
* @defgroup LIDAR2DMap_Operations LIDAR 2D Map Operations
* @brief Functions for accessing LIDAR 2D Map data
*/



/**
* @defgroup AutoFloorDetection_Operations Auto Floor Detection Operations
* @brief Functions for accessing Auto Floor Detection data
*/


/**
 * @defgroup Utility_Functions Utility Functions
 * @brief Utility functions
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



// utility functions
////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Convert a quaternion to Euler angles in RPY order
 * @ingroup Utility_Functions Utility Functions
 * 
 * @param q - the quaternion to be converted, the quaternion must be normalized
 * @param euler_out - the Euler angles will be stored in this pointer
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_convert_quaternion_to_euler(const slamtec_aurora_sdk_quaternion_t* q, slamtec_aurora_sdk_euler_angle_t* euler_out);



// controller operations
////////////////////////////////////////////////////////////////////////////////////////
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
 * @brief Require the remote Device to enable/disable the loop closure
 * @ingroup Controller_Operations Controller Operations
 *
 * @param enable - 1 to enable the loop closure, 0 to disable
 * @param handle - the session handle
 * @param timeout_ms - the timeout in milliseconds
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_controller_set_loop_closure(slamtec_aurora_sdk_session_handle_t handle, int enable, uint64_t timeout_ms);

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
////////////////////////////////////////////////////////////////////////////////////////

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
////////////////////////////////////////////////////////////////////////////////////////
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
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_get_relocalization_status(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_relocalization_status_type_t * status_out, uint64_t * timestamp_ns_out);


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
 * @brief Peek the most recent single layer LiDAR scan data and its pose
 * @details The most recent LIDAR scan data that its pose can be estimated by the tracking pose.
 * @details As the scan pose is calculated based on the tracking pose, the scan data may not always be the latest one. If the latest scan data is needed, caller should set forceLatest to true.
 * @ingroup DataProvider_Operations Data Provider Operations
 * 
 * @param handle - the session handle
 * @param header_out - the header will be stored in this pointer
 * @param scan_points_out - the scan points will be stored in this pointer. Set to NULL if not interested in the scan points.
 * @param buffer_size - the buffer sizes, set to 0 if not interested in the scan points.
 * @param scanpose - the scan pose will be stored in this pointer, set to NULL if not interested in the scan pose.
 * @param forceLatest - if true, the function will return the latest scan data and its pose
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_peek_recent_lidar_scan_singlelayer(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_lidar_singlelayer_scandata_info_t* header_out, slamtec_aurora_sdk_lidar_scan_point_t* scan_points_out, size_t buffer_count, slamtec_aurora_sdk_pose_se3_t * scanpose, int forceLatest);

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




// LIDAR 2D Map operations
////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Get the supported grid resolution range of the LIDAR 2D map
 * @details Caller can use this function to get the supported grid resolution range.  The resolution is the size of the grid cell in meters. 
 * @details The map resultion can be specified in the generation options. If the resolution is not in the supported range, the map will not be generated.
 * @ingroup LIDAR2DMap_Operations LIDAR 2D Map Operations
 * 
 * @param handle - the session handle
 * @param min_resolution - the minimum resolution will be stored in this pointer
 * @param max_resolution - the maximum resolution will be stored in this pointer
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_lidar2dmap_get_supported_grid_resultion_range(slamtec_aurora_sdk_session_handle_t handle, float* min_resolution, float* max_resolution);

/**
 * @brief Get the supported maximum grid cell count of the LIDAR 2D map
 * @details Caller can use this function to get the supported maximum grid cell count. Each cell is stored as a byte. 
 * @details For example, for a map with 100 meters width and 100 meters height, if the resolution is 0.1 meter, the maximum cell count will be (100/0.1) * (100/0.1) =  1,000,000 .
 * @details If the cell count is larger than the supported maximum, the map will not be generated.
 * @ingroup LIDAR2DMap_Operations LIDAR 2D Map Operations
 * 
 * @param handle - the session handle
 * @param max_cell_count - the maximum cell count will be stored in this pointer
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_lidar2dmap_get_supported_max_grid_cell_count(slamtec_aurora_sdk_session_handle_t handle, size_t* max_cell_count);

/**
 * @brief Require the LIDAR 2D preview map to be redrawn
 * @details Caller can use this function to require the LIDAR 2D preview map to be redrawn.
 * @details The preview map is the map that been generated on the fly during the mapping process by the background thread.
 * @details It is very useful for the caller to visualize the map in real time.
 * @ingroup LIDAR2DMap_Operations LIDAR 2D Map Operations
 * 
 * @param handle - the session handle
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_lidar2dmap_previewmap_require_redraw(slamtec_aurora_sdk_session_handle_t handle);

/**
 * @brief Start the background map generation for the LIDAR 2D preview map
 * @details Caller can use this function to start the background map generation for the LIDAR 2D preview map. Otherwise, the preview map will not be updated.
 * @details The preview map is the map that been generated on the fly during the mapping process by the background thread.
 * @details It is very useful for the caller to visualize the map in real time.
 * @ingroup LIDAR2DMap_Operations LIDAR 2D Map Operations
 * 
 * @param handle - the session handle
 * @param build_options - the generation options
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_lidar2dmap_previewmap_start_background_update(slamtec_aurora_sdk_session_handle_t handle, const slamtec_aurora_sdk_2d_gridmap_generation_options_t * build_options);

/**
 * @brief Stop the background map generation for the LIDAR 2D preview map
 * @details Caller can use this function to stop the background map generation for the LIDAR 2D preview map.
 * @ingroup LIDAR2DMap_Operations LIDAR 2D Map Operations
 * 
 * @param handle - the session handle
 */
void AURORA_SDK_API slamtec_aurora_sdk_lidar2dmap_previewmap_stop_background_update(slamtec_aurora_sdk_session_handle_t handle);

/**
 * @brief Check if the background map generation for the LIDAR 2D preview map is running
 * @details Caller can use this function to check if the background map generation for the LIDAR 2D preview map is running.
 * @ingroup LIDAR2DMap_Operations LIDAR 2D Map Operations
 * 
 * @param handle - the session handle
 * @return non-zero means running
 */
int AURORA_SDK_API slamtec_aurora_sdk_lidar2dmap_previewmap_is_background_updating(slamtec_aurora_sdk_session_handle_t handle);

/**
 * @brief Get the dirty rect of the LIDAR 2D preview map and reset the dirty rect
 * @details Caller can use this function to get the dirty rect of the LIDAR 2D preview map and reset the dirty rect.
 * @details The dirty rect is the area that has been updated by the background map generation thread.
 * @ingroup LIDAR2DMap_Operations LIDAR 2D Map Operations
 * 
 * @param handle - the session handle
 * @param dirty_rect_out - the dirty rect will be stored in this pointer
 * @param map_big_change - the map big change flag will be stored in this pointer. If there is a big change, the map will be redrawn and this flag will be set to true. It commonly happens when there is a loop closure or a new map has been loaded.
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_lidar2dmap_previewmap_get_and_reset_update_dirty_rect(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_rect_t* dirty_rect_out, int * map_big_change);
/*
 * @brief Get the generation options of the LIDAR 2D preview map
 * @details Caller can use this function to get the current generation options of the LIDAR 2D preview map.
 * @details If the auto floor detection is enabled, caller can use this function to get the current height range used to generate the preview map.
 * @ingroup LIDAR2DMap_Operations LIDAR 2D Map Operations
 * 
 * @param handle - the session handle
 * @param options_out - the generation options will be stored in this pointer
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_lidar2dmap_previewmap_get_generation_options(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_2d_gridmap_generation_options_t* options_out);

/**
 * @brief Set the auto floor detection for the LIDAR 2D preview map
 * @details Caller can use this function to set the auto floor detection for the LIDAR 2D preview map.
 * @details If the auto floor detection is enabled, the detector will update the height range used to generate the preview map.
 * @details The height range is based on the current floor description.
 * @details Auto floor detection is useful for multiple floor scenarios. 
 * @ingroup LIDAR2DMap_Operations LIDAR 2D Map Operations
 * 
 * @param handle - the session handle
 * @param enable - if true, the auto floor detection will be enabled
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_lidar2dmap_previewmap_set_auto_floor_detection(slamtec_aurora_sdk_session_handle_t handle, int enable);

/**
 * @brief Check if the auto floor detection is enabled for the LIDAR 2D preview map
 * @details Caller can use this function to check if the auto floor detection is enabled for the LIDAR 2D preview map.
 * @ingroup LIDAR2DMap_Operations LIDAR 2D Map Operations
 * 
 * @param handle - the session handle
 * @return non-zero means enabled
 */
int AURORA_SDK_API slamtec_aurora_sdk_lidar2dmap_previewmap_is_auto_floor_detection(slamtec_aurora_sdk_session_handle_t handle);

/**
 * @brief Get the handle of the LIDAR 2D preview map
 * @details Caller can use this function to get the handle of the LIDAR 2D preview map.
 * @details The handle can be used to access the preview map data.
 * @details The handle cannot be released by the caller.
 * @ingroup LIDAR2DMap_Operations LIDAR 2D Map Operations
 * 
 * @param handle - the session handle
 * @return the handle of the LIDAR 2D preview map
 */
const slamtec_aurora_sdk_occupancy_grid_2d_handle_t AURORA_SDK_API slamtec_aurora_sdk_lidar2dmap_previewmap_get_gridmap_handle(slamtec_aurora_sdk_session_handle_t handle);

/**
 * @brief Generate the full map on demand
 * @details Caller can use this function to generate the full map on demand and return the handle of the generated map.
 * @details The caller thread will be blocked until the map is generated or the timeout occurs.
 * @details The caller should release the map handle by calling slamtec_aurora_sdk_lidar2dmap_gridmap_release when the map is no longer needed.
 * @ingroup LIDAR2DMap_Operations LIDAR 2D Map Operations
 * 
 * @param handle - the session handle
 * @param generated_gridmap_handle_out - the handle of the generated full map will be stored in this pointer
 * @param build_options - the generation options
 * @param wait_for_data_sync - if non-zero, the function will wait for the map data to be synced before generating the map
 * @param timeout_ms - the timeout in milliseconds
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_lidar2dmap_generate_fullmap(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_occupancy_grid_2d_handle_t* generated_gridmap_handle_out, const slamtec_aurora_sdk_2d_gridmap_generation_options_t* build_options, int wait_for_data_sync, uint64_t timeout_ms);

/**
 * @brief Release the LIDAR 2D map
 * @details Caller can use this function to release a generated LIDAR 2D map object.
 * @ingroup LIDAR2DMap_Operations LIDAR 2D Map Operations
 * 
 * @param gridmap_handle - the handle of the LIDAR 2D map
 */
void AURORA_SDK_API slamtec_aurora_sdk_lidar2dmap_gridmap_release(slamtec_aurora_sdk_occupancy_grid_2d_handle_t gridmap_handle);

/**
 * @brief Get the resolution of the LIDAR 2D map
 * @details Caller can use this function to get the resolution of a LIDAR 2D map object .
 * @ingroup LIDAR2DMap_Operations LIDAR 2D Map Operations
 * 
 * @param gridmap_handle - the handle of the LIDAR 2D map
 * @param resolution_out - the resolution will be stored in this pointer
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_lidar2dmap_gridmap_get_resolution(const slamtec_aurora_sdk_occupancy_grid_2d_handle_t gridmap_handle, float* resolution_out);

/**
 * @brief Get the current dimension of the LIDAR 2D map
 * @details Caller can use this function to get the current dimension of a LIDAR 2D map object.
 * @ingroup LIDAR2DMap_Operations LIDAR 2D Map Operations
 * 
 * @param gridmap_handle - the handle of the LIDAR 2D map
 * @param dimension_out - the dimension will be stored in this pointer. The dimension is in logical size unit, i.e. in meters.
 * @param get_max_capcity - if non-zero, the function will return the maximum capacity of the map instead of the current dimension.
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_lidar2dmap_gridmap_get_dimension(const slamtec_aurora_sdk_occupancy_grid_2d_handle_t gridmap_handle, slamtec_aurora_sdk_2dmap_dimension_t* dimension_out, int get_max_capcity);


/**
 * @brief Read the cell data of the LIDAR 2D map
 * @details Caller can use this function to read the cell data of a LIDAR 2D map object.
 * @details The caller is required to provide a buffer to store the cell data.
 * @details The buffer size must be at least as large as the number of cells in the rect otherwise the function will return SLAMTEC_AURORA_SDK_ERRORCODE_INVALID_ARGUMENT.
 * @details There is an exception that if either the cell_buffer is set to NULL or the cell_buffer_size is set to 0, the function will return SLAMTEC_AURORA_SDK_ERRORCODE_OK and the info_out will be filled with the fetch info.
 * @details In order to get the number of cells in the rect, caller can use the info_out->cell_width and info_out->cell_height fields to calculate the number of cells.
 * @details The retrieved map data may have different origin point (x,y) compared to the one specified in the fetch_rect. The caller can use the info_out to check the real point value.
 * @ingroup LIDAR2DMap_Operations LIDAR 2D Map Operations
 * 
 * @param gridmap_handle - the handle of the LIDAR 2D map
 * @param fetch_rect - the rect in logic uints to be fetched
 * @param info_out - the fetch info will be stored in this pointer
 * @param cell_buffer - the buffer to store the cell data
 * @param cell_buffer_size - the buffer size
 * @param l2p_mapping - if non-zero, the function will perform log-odd to linear (0-255) mapping to each cell value. For visualization purpose, this is very useful.
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_lidar2dmap_gridmap_read_cell_data(const slamtec_aurora_sdk_occupancy_grid_2d_handle_t gridmap_handle, const slamtec_aurora_sdk_rect_t* fetch_rect, slamtec_aurora_sdk_2d_gridmap_fetch_info_t* info_out, uint8_t* cell_buffer, size_t cell_buffer_size, int l2p_mapping);


// Auto Floor Detection operations
////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Get the floor detection description of the current floor detected
 * @details Caller can use this function to get the current floor detection information, e.g. the typical height of the floor and the height range of the floor.
 * @ingroup AutoFloorDetection_Operations Auto Floor Detection Operations
 * 
 * @param handle - the session handle
 * @param desc_out - the floor detection description will be stored in this pointer
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_autofloordetection_get_current_detection_desc(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_floor_detection_desc_t* desc_out);



/**
 * @brief Get the floor detection descriptions of all floors detected
 * @details Caller can use this function to get the floor detection information of all floors detected.
 * @ingroup AutoFloorDetection_Operations Auto Floor Detection Operations
 * 
 * @param handle - the session handle
 * @param desc_buffer - the buffer to store the floor detection information. If set to NULL, the function will return the number of floors detected.
 * @param buffer_count - the buffer count. If set to 0, the function will return the number of floors detected.
 * @param actual_count_out - the actual count of the floor detection information
 * @param current_floor_id - the current floor id will be stored in this pointer
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_autofloordetection_get_all_detection_info(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_floor_detection_desc_t* desc_buffer, size_t buffer_count, size_t* actual_count_out, int * current_floor_id);


/**
 * @brief Get the floor detection histogram
 * @details Caller can use this function to get the floor detection histogram.
 * @ingroup AutoFloorDetection_Operations Auto Floor Detection Operations
 * 
 * @param handle - the session handle
 * @param header_out - the header will be stored in this pointer
 * @param histogram_buffer - the buffer to store the histogram data. If set to NULL, the function will return the number of histogram bins.
 * @param buffer_count - the buffer count. If set to 0, the function will return the number of histogram bins.
 * @return the error code
 */
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_autofloordetection_get_detection_histogram(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_floor_detection_histogram_info_t* header_out, float* histogram_buffer, size_t buffer_count);


#ifdef __cplusplus
}
#endif



#ifdef __cplusplus

// include C++ headers
#include "cxx/slamtec_remote_public.hxx"

#endif