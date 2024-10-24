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

// global services
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_get_version_info(slamtec_aurora_sdk_version_info_t * info_out);


// session operations
slamtec_aurora_sdk_session_handle_t AURORA_SDK_API slamtec_aurora_sdk_create_session(const slamtec_aurora_sdk_session_config_t* config, size_t config_size, const slamtec_aurora_sdk_listener_t * listener, slamtec_aurora_sdk_errorcode_t * error_code);
void AURORA_SDK_API slamtec_aurora_sdk_release_session(slamtec_aurora_sdk_session_handle_t handle);


// controller operations

// returns the number of discovered servers
// <0 means error
int AURORA_SDK_API slamtec_aurora_sdk_controller_get_discovered_servers(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_server_connection_info_t* servers, size_t max_server_count);

// connect to a server
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_controller_connect(slamtec_aurora_sdk_session_handle_t handle, const slamtec_aurora_sdk_server_connection_info_t* server_conn_info);
void AURORA_SDK_API slamtec_aurora_sdk_controller_disconnect(slamtec_aurora_sdk_session_handle_t handle);

// non-zero means connected
int AURORA_SDK_API slamtec_aurora_sdk_controller_is_connected(slamtec_aurora_sdk_session_handle_t handle);

void AURORA_SDK_API slamtec_aurora_sdk_controller_set_low_rate_mode(slamtec_aurora_sdk_session_handle_t handle, int enable);
void AURORA_SDK_API slamtec_aurora_sdk_controller_set_map_data_syncing(slamtec_aurora_sdk_session_handle_t handle, int enable);
void AURORA_SDK_API slamtec_aurora_sdk_controller_resync_map_data(slamtec_aurora_sdk_session_handle_t handle, int invalidate_cache);


slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_controller_set_raw_data_subscription(slamtec_aurora_sdk_session_handle_t handle, int enable);
int AURORA_SDK_API slamtec_aurora_sdk_controller_is_raw_data_subscribed(slamtec_aurora_sdk_session_handle_t handle);

slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_controller_require_map_reset(slamtec_aurora_sdk_session_handle_t handle, uint64_t timeout_ms);
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_controller_require_pure_localization_mode(slamtec_aurora_sdk_session_handle_t handle, uint64_t timeout_ms);
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_controller_require_mapping_mode(slamtec_aurora_sdk_session_handle_t handle, uint64_t timeout_ms);


slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_controller_send_custom_command(slamtec_aurora_sdk_session_handle_t handle, uint64_t timeout_ms, uint64_t cmd, const void* data, size_t data_size, void* response, size_t response_buffer_size, size_t * response_retrieved_size);


// map manager operations

// the created map storage session should be released by calling slamtec_aurora_sdk_mapmanager_release_storage_session
// otherwise, memory leak will happen

// NOTE: the sdk will enter low rate mode during the working session to reduce the data traffic
//       the low rate mode will be automatically disabled after the map streaming operation is done
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_mapmanager_start_storage_session(slamtec_aurora_sdk_session_handle_t handle, const char* map_file_name, slamtec_aurora_sdk_mapstorage_session_type_t session_type, slamtec_aurora_sdk_mapstorage_session_result_callback_t  callback, void * user_data);

int AURORA_SDK_API slamtec_aurora_sdk_mapmanager_is_storage_session_active(slamtec_aurora_sdk_session_handle_t handle);
void AURORA_SDK_API slamtec_aurora_sdk_mapmanager_is_storage_abort_session(slamtec_aurora_sdk_session_handle_t handle);
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_mapmanager_query_storage_status(slamtec_aurora_sdk_session_handle_t handle,  slamtec_aurora_sdk_mapstorage_session_status_t* progress_out);


// dataprovider operations

// get pose in SE3 format
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_get_current_pose_se3(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_pose_se3_t* pose_out);

// get pose with rotation in euler angles (LPY)
// WARNING: gimbal lock may happen
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_get_current_pose(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_pose_t* pose_out);

slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_get_mapping_flags(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_mapping_flag_t * flags_out);

slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_get_last_device_status(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_device_status_t* status_out, uint64_t * timestamp_ns_out);

slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_peek_tracking_data(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_tracking_info_t* tracking_data_out, const slamtec_aurora_sdk_tracking_data_buffer_t* provided_buffer_info);
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_peek_imu_data(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_imu_data_t* imu_data_out, size_t buffer_count, size_t* actual_count_out);

slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_get_imu_info(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_imu_info_t * info_out);



slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_get_global_mapping_info(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_global_map_desc_t* desc_out);
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_get_all_map_info(slamtec_aurora_sdk_session_handle_t handle, slamtec_aurora_sdk_map_desc_t* desc_buffer, size_t buffer_count, size_t* actual_count_out);

// access map data like keyframe and map points data
// a visitor object contains data callback listeners must be provided
// those callbacks set to NULL will be ignored
// 
// the SDK will enter stall state during the data accessing,
// i.e. the background data sync will  paused
//
// if all map data should be accessed, simply pass NULL to the map_ids
//
slamtec_aurora_sdk_errorcode_t AURORA_SDK_API slamtec_aurora_sdk_dataprovider_access_map_data(slamtec_aurora_sdk_session_handle_t handle, const slamtec_aurora_sdk_map_data_visitor_t* visitor, uint32_t* map_ids, size_t map_count);


#ifdef __cplusplus
}
#endif



#ifdef __cplusplus

// include C++ headers
#include "cxx/slamtec_remote_public.hxx"

#endif