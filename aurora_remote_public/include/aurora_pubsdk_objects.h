/*
 *  SLAMTEC Aurora
 *  Copyright 2013 - 2024 SLAMTEC Co., Ltd.
 *
 *  http://www.slamtec.com
 *
 *  Aurora Remote SDK
 *  Data Objects
 *
 */


#pragma once


typedef void* slamtec_aurora_sdk_session_handle_t;
typedef void* slamtec_aurora_sdk_mapstorage_session_handle_t;

static inline int slamtec_aurora_sdk_is_valid_handle(void* handle) {
    return handle != NULL;
}





typedef struct _slamtec_aurora_sdk_version_info_t
{
    const char* sdk_name;
    const char* sdk_version_string;
    const char* sdk_build_time;

    uint32_t sdk_feature_flags;
} slamtec_aurora_sdk_version_info_t;



typedef struct _slamtec_aurora_sdk_session_config_t
{
    uint32_t version;
    uint32_t reserved; // must be 0
} slamtec_aurora_sdk_session_config_t;


typedef struct _slamtec_aurora_sdk_connection_info_t
{
    char protocol_type[16]; //null-terminated string, for example "tcp", "udp"
    char address[64]; // null-terminated string
    uint16_t port; // SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PORT
} slamtec_aurora_sdk_connection_info_t;


typedef struct _slamtec_aurora_sdk_server_connection_info_t
{
    // multiple connection method of a server can be supported
    // for example, a server can support both tcp and udp
    // or a server can support both ipv4 and ipv6

    // the sdk will try to connect to the server with the first connection method
    slamtec_aurora_sdk_connection_info_t connection_info[8];
    uint32_t connection_count;
} slamtec_aurora_sdk_server_connection_info_t;

// -- map storage related
typedef uint32_t slamtec_aurora_sdk_mapstorage_session_type_t;

enum slamtec_aurora_sdk_mapstorage_session_type_types {
    SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_TYPE_UPLOAD = 0,
    SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_TYPE_DOWNLOAD = 1,
};


enum slamtec_aurora_sdk_mapstorage_session_status_flags_t {
    SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_STATUS_FINISHED = 2,
    SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_STATUS_WORKING = 1,
    SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_STATUS_IDLE = 0,
    SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_STATUS_FAILED = -1,
    SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_STATUS_ABORTED = -2,
    SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_STATUS_REJECTED = -3,
    SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_STATUS_TIMEOUT = -4,
};

typedef struct _slamtec_aurora_sdk_mapstorage_session_status_t {
    float progress; //0-100
    int8_t flags; // value selected from enum slamtec_aurora_sdk_mapstorage_session_status_flags_t
} slamtec_aurora_sdk_mapstorage_session_status_t;


// -- tracking and mapping data

typedef struct _slamtec_aurora_sdk_position3d_t {
    double x;
    double y;
    double z;
} slamtec_aurora_sdk_position3d_t;;

typedef struct _slamtec_aurora_sdk_quaternion_t {
    double x;
    double y;
    double z;
    double w;
} slamtec_aurora_sdk_quaternion_t;

typedef struct _slamtec_aurora_sdk_euler_angle_t {
    double roll;
    double pitch;
    double yaw;
} slamtec_aurora_sdk_euler_angle_t;


typedef struct _slamtec_aurora_sdk_pose_se3_t {
    slamtec_aurora_sdk_position3d_t translation;
    slamtec_aurora_sdk_quaternion_t quaternion;
} slamtec_aurora_sdk_pose_se3_t;

typedef struct _slamtec_aurora_sdk_pose_t {
    slamtec_aurora_sdk_position3d_t translation;
    slamtec_aurora_sdk_euler_angle_t rpy;
} slamtec_aurora_sdk_pose_t;


enum slamtec_aurora_sdk_mapping_flag_types {
    SLAMTEC_AURORA_SDK_MAPPING_FLAG_NONE = 0,
    SLAMTEC_AURORA_SDK_MAPPING_FLAG_LOC_MODE = (0x1 << 0),
    SLAMTEC_AURORA_SDK_MAPPING_FLAG_LC_DISABLED = (0x1 << 2),
    SLAMTEC_AURORA_SDK_MAPPING_FLAG_LOSTED = (0x1 << 16),
    SLAMTEC_AURORA_SDK_MAPPING_FLAG_STORAGE_IN_PROGRESS = (0x1 << 17),
};

typedef uint32_t slamtec_aurora_sdk_mapping_flag_t; //bitwise OR of enum slamtec_aurora_sdk_mapping_flag_types


enum slamtec_aurora_sdk_device_status_types {
    SLAMTEC_AURORA_SDK_DEVICE_INITED = 0,
    SLAMTEC_AURORA_SDK_DEVICE_INIT_FAILED,
    SLAMTEC_AURORA_SDK_DEVICE_LOOP_CLOSURE,
    SLAMTEC_AURORA_SDK_DEVICE_OPTIMIZATION_COMPLETED,
    SLAMTEC_AURORA_SDK_DEVICE_TRACKING_LOST,
    SLAMTEC_AURORA_SDK_DEVICE_TRACKING_RECOVERED,
    SLAMTEC_AURORA_SDK_DEVICE_MAP_UPDATED,
    SLAMTEC_AURORA_SDK_DEVICE_MAP_CLEARED,
    SLAMTEC_AURORA_SDK_DEVICE_MAP_SWITCHED,
    SLAMTEC_AURORA_SDK_DEVICE_MAP_LOADING_STARTED,
    SLAMTEC_AURORA_SDK_DEVICE_MAP_SAVING_STARTED,
    SLAMTEC_AURORA_SDK_DEVICE_MAP_LOADING_COMPLETED,
    SLAMTEC_AURORA_SDK_DEVICE_MAP_SAVING_COMPLETED,
};

typedef uint32_t slamtec_aurora_sdk_device_status_t; //value selected from enum slamtec_aurora_sdk_device_status_types



typedef struct _slamtec_aurora_sdk_image_desc_t {
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t format; // 0: gray, 1: rgb, 2: rgba
    uint32_t data_size;
} slamtec_aurora_sdk_image_desc_t;


typedef struct _slamtec_aurora_sdk_keypoint_t {
    float x;
    float y;
    uint8_t flags;  // 0: unmatched, non-zero: matched
} slamtec_aurora_sdk_keypoint_t;


typedef struct _slamtec_aurora_sdk_tracking_data_buffer_t {
    void* imgdata_left; //buffer to hold image data of the left camera
                        //the buffer should be provided by the caller
                        //nullptr to disable image data copy
    size_t imgdata_left_size; // size of the buffer

    void* imgdata_right; 
    size_t imgdata_right_size;


    slamtec_aurora_sdk_keypoint_t* keypoints_left; //buffer to hold keypoints of the left camera
    size_t keypoints_left_buffer_count; // size of the buffer

    slamtec_aurora_sdk_keypoint_t* keypoints_right;
    size_t keypoints_right_buffer_count;
} slamtec_aurora_sdk_tracking_data_buffer_t;



enum slamtec_aurora_sdk_tracking_status_t {
    SLAMTEC_AURORA_TRACKING_STATUS_UNKNOWN = 0,
    SLAMTEC_AURORA_TRACKING_STATUS_SYS_NOT_READY,
    SLAMTEC_AURORA_TRACKING_STATUS_NOT_INIT,
    SLAMTEC_AURORA_TRACKING_STATUS_NO_IMG,
    SLAMTEC_AURORA_TRACKING_STATUS_OK,
    SLAMTEC_AURORA_TRACKING_STATUS_LOST,
    SLAMTEC_AURORA_TRACKING_STATUS_LOST_RECOVERED,
};



// NOTICE: the image provided by tracking info has been lossy compressed during transmission
// to retrieve  the original image, please use the raw image data callback
// The raw image subscription must be enabled first
typedef struct _slamtec_aurora_sdk_tracking_info {
    uint64_t timestamp_ns;
    slamtec_aurora_sdk_image_desc_t left_image_desc;
    slamtec_aurora_sdk_image_desc_t right_image_desc;
    uint32_t is_stereo;
    uint32_t tracking_status; // from slamtec_aurora_sdk_tracking_status_t

    slamtec_aurora_sdk_pose_se3_t pose;

    uint32_t keypoints_left_count;
    uint32_t keypoints_right_count;
} slamtec_aurora_sdk_tracking_info_t;

typedef struct _slamtec_aurora_sdk_imu_data_t {
    uint64_t timestamp_ns;
    uint32_t imu_id;
    double acc[3];  // in g (1g = 9.8m/s^2)
    double gyro[3]; // in dps
} slamtec_aurora_sdk_imu_data_t;


typedef struct _slamtec_aurora_sdk_imu_info_t {
    int valid; // non-zero for valid data
    slamtec_aurora_sdk_pose_se3_t tcb;
    double cov_noise[6]; // gyro to accel
    double cov_random_walk[6]; // gyro to accel
} slamtec_aurora_sdk_imu_info_t;



// -- Map Objects

typedef struct _slamtec_aurora_sdk_global_map_desc_t {
    uint64_t lastMPCountToFetch;
    uint64_t lastKFCountToFetch;
    uint64_t lastMapCountToFetch;

    uint64_t lastMPRetrieved;
    uint64_t lastKFRetrieved;


    uint64_t totalMPCount;
    uint64_t totalKFCount;
    uint64_t totalMapCount;

    uint64_t totalMPCountFetched;
    uint64_t totalKFCountFetched;
    uint64_t totalMapCountFetched;


    uint64_t currentActiveMPCount;
    uint64_t currentActiveKFCount;

    uint32_t activeMapID;
} slamtec_aurora_sdk_global_map_desc_t;

typedef struct _slamtec_aurora_sdk_map_desc_t {
    uint32_t map_id;
    uint32_t map_flags;
    uint64_t keyframe_count;
    uint64_t map_point_count;

    uint64_t keyframe_id_start;
    uint64_t keyframe_id_end;

    uint64_t map_point_id_start;
    uint64_t map_point_id_end;
} slamtec_aurora_sdk_map_desc_t;


typedef struct _slamtec_aurora_sdk_keyframe_desc_t {
    uint64_t id;
    uint64_t parent_id;
    uint32_t map_id;

    double timestamp;

    slamtec_aurora_sdk_pose_se3_t pose_se3;
    slamtec_aurora_sdk_pose_t pose;
    size_t looped_frame_count;
    size_t connected_frame_count;
} slamtec_aurora_sdk_keyframe_desc_t;



typedef struct _slamtec_aurora_sdk_map_point_desc_t {
    uint64_t id;
    uint32_t map_id;

    double timestamp;

    slamtec_aurora_sdk_position3d_t position;
    uint32_t flags;
} slamtec_aurora_sdk_map_point_desc_t;

// callbacks

typedef void (*slamtec_aurora_sdk_mapstorage_session_result_callback_t)(void* user_data, int is_ok);

typedef void (*slamtec_aurora_sdk_on_image_data_callback_t)(void* user_data, uint64_t timestamp_ns, const slamtec_aurora_sdk_image_desc_t* left_desc, const void* left_data, const slamtec_aurora_sdk_image_desc_t* right_desc, const void* right_data);
typedef void (*slamtec_aurora_sdk_on_tracking_data_callback_t)(void* user_data, const slamtec_aurora_sdk_tracking_info_t* tracking_data, const slamtec_aurora_sdk_tracking_data_buffer_t* provided_buffer_info);
typedef void (*slamtec_aurora_sdk_on_imu_data_callback_t)(void* user_data, const slamtec_aurora_sdk_imu_data_t* imu_data, size_t imu_data_count);
typedef void (*slamtec_aurora_sdk_on_mapping_flags_callback_t)(void* user_data, slamtec_aurora_sdk_mapping_flag_t mapping_flags);
typedef void (*slamtec_aurora_sdk_on_device_status_callback_t)(void* user_data, uint64_t timestamp_ns, slamtec_aurora_sdk_device_status_t status);


typedef struct _slamtec_aurora_sdk_listener_t {
    void* user_data;

    // set to NULL to ignore a callback
    slamtec_aurora_sdk_on_image_data_callback_t on_raw_image_data;
    slamtec_aurora_sdk_on_tracking_data_callback_t on_tracking_data;
    slamtec_aurora_sdk_on_imu_data_callback_t on_imu_data;
    slamtec_aurora_sdk_on_mapping_flags_callback_t on_mapping_flags;
    slamtec_aurora_sdk_on_device_status_callback_t on_device_status;
    
} slamtec_aurora_sdk_listener_t;


// map visitor
typedef void (*slamtec_aurora_sdk_on_map_keyframe_callback_t)(void* user_data, const slamtec_aurora_sdk_keyframe_desc_t* keyframe_desc, const uint64_t * looped_frame_ids, const uint64_t * connected_frame_ids);
typedef void (*slamtec_aurora_sdk_on_map_point_callback_t)(void* user_data, const slamtec_aurora_sdk_map_point_desc_t* map_point_desc);
typedef void (*slamtec_aurora_sdk_on_map_callback_t)(void* user_data, const slamtec_aurora_sdk_map_desc_t* map_desc);

typedef struct _slamtec_aurora_sdk_map_data_visitor_t {
    void* user_data;

    // set to NULL to ignore a callback
    slamtec_aurora_sdk_on_map_keyframe_callback_t on_keyframe;
    slamtec_aurora_sdk_on_map_point_callback_t on_map_point;
    slamtec_aurora_sdk_on_map_callback_t on_map_desc;
} slamtec_aurora_sdk_map_data_visitor_t;