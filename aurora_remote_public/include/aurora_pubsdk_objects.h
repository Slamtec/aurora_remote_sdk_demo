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
 /**
  * @defgroup SDK_Basic_Data_Types SDK Basic Data Types
  * @brief Basic data types used in the SDK
  *
  * @{
  */
 
 
 /**
  * @brief The session handle type
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The session handle is used to identify a session.
  */
 typedef void* slamtec_aurora_sdk_session_handle_t;
 
 /**
  * @brief The map storage session handle type
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The map storage session handle is used to identify a map storage session.
  */
 typedef void* slamtec_aurora_sdk_mapstorage_session_handle_t;
 
 /**
  * @brief The occupancy grid 2D handle type
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The occupancy grid 2D handle is used to access a occupancy 2D grid map.
  */
 typedef void* slamtec_aurora_sdk_occupancy_grid_2d_handle_t;
 
 
 /**
  * @brief Check if the handle is valid
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details Caller can use this function to check if the handle is valid.
  */
 static inline int slamtec_aurora_sdk_is_valid_handle(void* handle) {
     return handle != NULL;
 }
 
 
 /**
  * @brief The version info structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The version info structure contains the version information of the SDK.
  */
 typedef struct _slamtec_aurora_sdk_version_info_t
 {
     /**
      * @brief The SDK name
      * @details The SDK name is the name of the SDK.
      */
     const char* sdk_name; 
 
     /**
      * @brief The SDK version string
      * @details The SDK version string is the version string of the SDK.
      */
     const char* sdk_version_string;
 
     /**
      * @brief The SDK build time
      * @details The SDK build time is the build time of the SDK.
      */
     const char* sdk_build_time;
 
     /**
      * @brief The SDK feature flags
      * @details The SDK feature flags are the feature flags of the SDK.
      */
     uint32_t sdk_feature_flags;
 } slamtec_aurora_sdk_version_info_t;
 
 
 /**
  * @brief The session config structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The session config structure contains the session configuration.
  */
 typedef struct _slamtec_aurora_sdk_session_config_t
 {
     /**
      * @brief The session config version
      * @details The session config version is the version of the session config. Set to 0 for now.
      */
     uint32_t version;
 
     /**
      * @brief The reserved field
      * @details The reserved field must be 0.
      */
     uint32_t reserved; // must be 0
 } slamtec_aurora_sdk_session_config_t;
 
 
 /**
  * @brief The connection info structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The connection info structure describes a connection method to a server.
  */
 typedef struct _slamtec_aurora_sdk_connection_info_t
 {
     /**
      * @brief The protocol type
      * @details The protocol type is the protocol type of the connection. For example "tcp", "udp".
      * @details The protocol type must be null-terminated.
      * @details Set to SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PROTOCOL for most cases.
      */
     char protocol_type[16]; //null-terminated string
 
     /**
      * @brief The address
      * @details The address is the address of the connection.
      * @details The address must be null-terminated.
      */
     char address[64]; // null-terminated string
 
     /**
      * @brief The port
      * @details The port is the port of the connection.
      * @details Set to SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PORT for most cases.
      */
     uint16_t port; 
 } slamtec_aurora_sdk_connection_info_t;
 
 
 /**
  * @brief The server connection info structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The server connection info structure describes multiple connection methods to a server.
  */
 typedef struct _slamtec_aurora_sdk_server_connection_info_t
 {
     // multiple connection method of a server can be supported
     // for example, a server can support both tcp and udp
     // or a server can support both ipv4 and ipv6
 
     // the sdk will try to connect to the server with the first connection method
     /**
      * @brief The connection methods
      * @details The connection methods are the connection methods to the server.
      * @details Multiple connection method of a server can be supported.
      * @details The sdk will try to connect to the server with the first connection method.
      * @details The maximum number of connection methods is 8.
      */
     slamtec_aurora_sdk_connection_info_t connection_info[8];
 
     /**
      * @brief The number of connection methods
      * @details The number of connection methods is the number of connection methods to the server.
      */
     uint32_t connection_count;
 } slamtec_aurora_sdk_server_connection_info_t;
 
 // -- map storage related
 
 /**
  * @brief The map storage session type
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The map storage session type should be a value selected from enum slamtec_aurora_sdk_mapstorage_session_type_types.
  */
 typedef uint32_t slamtec_aurora_sdk_mapstorage_session_type_t;
 
 
 /**
  * @brief The map storage session type
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The map storage session type is the type of the map storage session.
  */
 enum slamtec_aurora_sdk_mapstorage_session_type_types {
     /**
      * @brief The upload session type
      * @details This type tells the SDK to upload a map to the server.
      */
     SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_TYPE_UPLOAD = 0,
     /**
      * @brief The download session type
      * @details This type tells the SDK to download a map from the server.
      */
     SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_TYPE_DOWNLOAD = 1,
 };
 
 
 /**
  * @brief The map storage session status flags
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The map storage session status flags are the status flags of the map storage session.
  */
 enum slamtec_aurora_sdk_mapstorage_session_status_flags_t {
     /**
      * @brief The finished status flag
      * @details This flag tells the SDK that the map storage session is finished without any error.
      */
     SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_STATUS_FINISHED = 2,
     /**
      * @brief The working status flag
      * @details This flag tells the SDK that the map storage session is working.
      */
     SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_STATUS_WORKING = 1,
     /**
      * @brief The idle status flag
      * @details This flag tells the SDK that the map storage session is idle.
      */
     SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_STATUS_IDLE = 0,
     /**
      * @brief The failed status flag
      * @details This flag tells the SDK that the previous map storage session is failed.
      */
     SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_STATUS_FAILED = -1,
     /**
      * @brief The aborted status flag
      * @details This flag tells the SDK that the previous map storage session is aborted.
      */
     SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_STATUS_ABORTED = -2,
     /**
      * @brief The rejected status flag
      * @details This flag tells the SDK that the previous map storage session is rejected.
      */
     SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_STATUS_REJECTED = -3,
     /**
      * @brief The timeout status flag
      * @details This flag tells the SDK that the previous map storage session is timeout.
      */
     SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_STATUS_TIMEOUT = -4,
 };
 
 /**
  * @brief The map storage session status structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The map storage session status structure contains the status of the map storage session.
  */
 typedef struct _slamtec_aurora_sdk_mapstorage_session_status_t {
     /**
      * @brief The progress of the map storage session
      * @details The progress of the map storage session is the progress of the map storage session.
      * @details The progress is a value between 0 and 100.
      */
     float progress; //0-100
 
     /**
      * @brief The status flags of the map storage session
      * @details The status flags of the map storage session are the status flags of the map storage session.
      * @details The status flags should be a value selected from enum slamtec_aurora_sdk_mapstorage_session_status_flags_t.
      */
     int8_t flags; // value selected from enum slamtec_aurora_sdk_mapstorage_session_status_flags_t
 } slamtec_aurora_sdk_mapstorage_session_status_t;
 
 
 // -- tracking and mapping data
 
 /**
  * @brief The 3D position structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The 3D position structure contains the 3D position.
  */
 typedef struct _slamtec_aurora_sdk_position3d_t {
     /**
      * @brief The x coordinate
      * @details The x coordinate is the x coordinate of the 3D position.
      */
     double x;
     /**
      * @brief The y coordinate
      * @details The y coordinate is the y coordinate of the 3D position.
      */
     double y;
     /**
      * @brief The z coordinate
      * @details The z coordinate is the z coordinate of the 3D position.
      */
     double z;
 } slamtec_aurora_sdk_position3d_t;;
 
 /**
  * @brief The quaternion structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The quaternion structure contains the quaternion of a rotation.
  */ 
 typedef struct _slamtec_aurora_sdk_quaternion_t {
     /**
      * @brief The x component of the quaternion
      * @details The x component of the quaternion is the x component of the quaternion.
      */
     double x;
     /**
      * @brief The y component of the quaternion
      * @details The y component of the quaternion is the y component of the quaternion.
      */
     double y;
     /**
      * @brief The z component of the quaternion
      * @details The z component of the quaternion is the z component of the quaternion.
      */
     double z;
     /**
      * @brief The w component of the quaternion
      * @details The w component of the quaternion is the w component of the quaternion.
      */
     double w;
 } slamtec_aurora_sdk_quaternion_t;
 
 /**
  * @brief The Euler angle structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The Euler angle structure contains the Euler angle of a rotation.
  */
 typedef struct _slamtec_aurora_sdk_euler_angle_t {
     /**
      * @brief The roll angle
      * @details The roll angle is the roll angle of the Euler angle.
      */
     double roll;
     /**
      * @brief The pitch angle
      * @details The pitch angle is the pitch angle of the Euler angle.
      */
     double pitch;
     /**
      * @brief The yaw angle
      * @details The yaw angle is the yaw angle of the Euler angle.
      */
     double yaw;
 } slamtec_aurora_sdk_euler_angle_t;
 
 /**
  * @brief The SE3 pose structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The SE3 pose structure contains the SE3 pose of a transformation.
  */
 typedef struct _slamtec_aurora_sdk_pose_se3_t {
     /**
      * @brief The translation of the SE3 pose
      * @details The translation of the SE3 pose is the translation of the SE3 pose.
      */
     slamtec_aurora_sdk_position3d_t translation;
     /**
      * @brief The quaternion of the SE3 pose
      * @details The quaternion of the SE3 pose is the quaternion of the SE3 pose.
      */
     slamtec_aurora_sdk_quaternion_t quaternion;
 } slamtec_aurora_sdk_pose_se3_t;
 
 /**
  * @brief The pose structure using translation and Euler angle
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The pose structure using translation and Euler angle contains the pose of a transformation.
  */
 typedef struct _slamtec_aurora_sdk_pose_t {
     /**
      * @brief The translation of the pose
      * @details The translation of the pose is the translation of the pose.
      */
     slamtec_aurora_sdk_position3d_t translation;
     /**
      * @brief The Euler angle of the pose
      * @details The Euler angle of the pose is the Euler angle of the pose.
      */
     slamtec_aurora_sdk_euler_angle_t rpy;
 } slamtec_aurora_sdk_pose_t;
 
 
 /**
  * @brief The mapping flag types
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details Multiple mapping flags can be combined using bitwise OR.
  */
 enum slamtec_aurora_sdk_mapping_flag_types {
     /**
      * @brief The none mapping flag
      */
     SLAMTEC_AURORA_SDK_MAPPING_FLAG_NONE = 0,
     /**
      * @brief The local mapping flag
      * @details This flag tells the SDK that the mapping is in local mode (mapping disabled).
      */
     SLAMTEC_AURORA_SDK_MAPPING_FLAG_LOC_MODE = (0x1 << 0),
     /**
      * @brief The loop closure disabled mapping flag
      * @details This flag tells the SDK that the loop closure is disabled.
      */
     SLAMTEC_AURORA_SDK_MAPPING_FLAG_LC_DISABLED = (0x1 << 2),
     /**
      * @brief The GBA running mapping flag
      * @details This flag tells the SDK that the global bundle adjustment is running.
      */
     SLAMTEC_AURORA_SDK_MAPPING_FLAG_GBA_RUNNING = (0x1 << 3),
     /**
      * @brief The lost mapping flag
      * @details This flag tells the SDK that the tracking is lost.
      */
     SLAMTEC_AURORA_SDK_MAPPING_FLAG_LOSTED = (0x1 << 16),
     /**
      * @brief The storage in progress mapping flag
      * @details This flag tells the SDK that the mapping is in storage in progress.
      */
     SLAMTEC_AURORA_SDK_MAPPING_FLAG_STORAGE_IN_PROGRESS = (0x1 << 17),
 };
 
 /**
  * @brief The mapping flag value
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The mapping flag value is the value of the mapping flag.
  */
 typedef uint32_t slamtec_aurora_sdk_mapping_flag_t; //bitwise OR of enum slamtec_aurora_sdk_mapping_flag_types
 
 
 enum slamtec_aurora_sdk_connection_status {
     SLAMTEC_AURORA_SDK_CONNECTION_STATUS_LOST = 0,
     SLAMTEC_AURORA_SDK_CONNECTION_STATUS_RESTORED = 1,
     SLAMTEC_AURORA_SDK_CONNECTION_STATUS_DEVICE_CONFIG_CHANGED = 2,
 };
 
 typedef uint32_t slamtec_aurora_sdk_connection_status_t; //value selected from enum slamtec_aurora_sdk_connection_status
 
 
 /**
  * @brief The device status types
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The device status types are the status types of the device.
  */
 enum slamtec_aurora_sdk_device_status_types {
     /**
      * @brief The device has been inited
      */
     SLAMTEC_AURORA_SDK_DEVICE_INITED = 0,
     /**
      * @brief The device initialization failed
      */
     SLAMTEC_AURORA_SDK_DEVICE_INIT_FAILED,
     /**
      * @brief The device has detected a loop closure
      */
     SLAMTEC_AURORA_SDK_DEVICE_LOOP_CLOSURE,
     /**
      * @brief The device optimization completed
      */
     SLAMTEC_AURORA_SDK_DEVICE_OPTIMIZATION_COMPLETED,
     /**
      * @brief The device tracking is lost
      */
     SLAMTEC_AURORA_SDK_DEVICE_TRACKING_LOST,
     /**
      * @brief The device tracking is recovered
      */
     SLAMTEC_AURORA_SDK_DEVICE_TRACKING_RECOVERED,
     /**
      * @brief The device map is updated
      */
     SLAMTEC_AURORA_SDK_DEVICE_MAP_UPDATED,
     /**
      * @brief The device map is cleared
      */
     SLAMTEC_AURORA_SDK_DEVICE_MAP_CLEARED,
     /**
      * @brief The device map is switched
      */
     SLAMTEC_AURORA_SDK_DEVICE_MAP_SWITCHED,
     /**
      * @brief The device map loading started
      */
     SLAMTEC_AURORA_SDK_DEVICE_MAP_LOADING_STARTED,
     /**
      * @brief The device map saving started
      */
     SLAMTEC_AURORA_SDK_DEVICE_MAP_SAVING_STARTED,
     /**
      * @brief The device map loading completed
      */
     SLAMTEC_AURORA_SDK_DEVICE_MAP_LOADING_COMPLETED,
     /**
      * @brief The device map saving completed
      */
     SLAMTEC_AURORA_SDK_DEVICE_MAP_SAVING_COMPLETED,
     /**
      * @brief The device is in relocalization
      */
     SLAMTEC_AURORA_SDK_DEVICE_RELOCALIZATION_SUCCESS,
     /**
      * @brief The device relocalization failed
      */
     SLAMTEC_AURORA_SDK_DEVICE_RELOCALIZATION_FAILED,
     /**
      * @brief The device relocalization is cancelled
      */
     SLAMTEC_AURORA_SDK_DEVICE_RELOCALIZATION_CANCELLED,
     /**
      * @brief The device relocalization is started
      */
     SLAMTEC_AURORA_SDK_DEVICE_RELOCALIZATION_STARTED,
 };
 
 /**
  * @brief The device status value
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The device status value is the value of the device status.
  */
 typedef uint32_t slamtec_aurora_sdk_device_status_t; //value selected from enum slamtec_aurora_sdk_device_status_types
 
 
 /**
  * @brief The device relocalization status types
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The device relocalization status of the last relocalization process
  */
 enum slamtec_aurora_sdk_device_relocalization_status_types {
     SLAMTEC_AURORA_SDK_DEVICE_RELOCALIZATION_STATUS_NONE = 0,
     SLAMTEC_AURORA_SDK_DEVICE_RELOCALIZATION_STATUS_IN_PROGRESS,
     SLAMTEC_AURORA_SDK_DEVICE_RELOCALIZATION_STATUS_SUCCEED,
     SLAMTEC_AURORA_SDK_DEVICE_RELOCALIZATION_STATUS_FAILED,
 };
 
 /**
  * @brief The device relocalization status value
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The device relocalization status value is the value of the device relocalization status.
  */
 typedef uint32_t slamtec_aurora_sdk_device_relocalization_status_t; //value selected from enum slamtec_aurora_sdk_device_relocalization_status_types
 
 
 /**
  * @brief The device status structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The device status structure contains the status of the device.
  */
 typedef struct _slamtec_aurora_sdk_device_status_desc
 {
     slamtec_aurora_sdk_device_status_t status;
     uint64_t timestamp_ns;
 } slamtec_aurora_sdk_device_status_desc_t;
 
 
 
 enum slamtec_aurora_sdk_device_hw_feature_bitmaps {
     SLAMTEC_AURORA_SDK_HW_FEATURE_BIT_2D_LIDAR = (0x1ULL<<0),
 
     SLAMTEC_AURORA_SDK_HW_FEATURE_BIT_IMU = (0x1ULL<<4),
     
 };
 
 enum slamtec_aurora_sdk_device_sensing_feature_bitmaps {
     SLAMTEC_AURORA_SDK_SENSING_FEATURE_BIT_VSLAM_VIO = (0x1ULL<<0),
     SLAMTEC_AURORA_SDK_SENSING_FEATURE_BIT_VSLAM_SPARSE_MAPPING = (0x1ULL<<1),
     SLAMTEC_AURORA_SDK_SENSING_FEATURE_BIT_VSLAM_LOOP_CLOSURE = (0x1ULL<<2),
     SLAMTEC_AURORA_SDK_SENSING_FEATURE_BIT_VSLAM_LOCALIZATION = (0x1ULL<<3),
     SLAMTEC_AURORA_SDK_SENSING_FEATURE_BIT_VSLAM_GLOBAL_LOCALIZATION = (0x1ULL<<4),
 
 
     SLAMTEC_AURORA_SDK_SENSING_FEATURE_BIT_STEREO_DENSE_DISPARITY = (0x1ULL<<16),
     SLAMTEC_AURORA_SDK_SENSING_FEATURE_BIT_SEMANTIC_SEGMENTATION = (0x1ULL<<17),
 };
 
 enum slamtec_aurora_sdk_device_sw_feature_bitmaps {
     SLAMTEC_AURORA_SDK_SW_FEATURE_BIT_CAMREA_PREVIEW_STREAM = (0x1ULL<<0),
 };
 
 
 
 
 
 /**
  * @brief The device basic info structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The device basic info structure contains the basic info of the device.
  */
 typedef struct _slamtec_aurora_sdk_device_basic_info
 {
     uint16_t  model_major;
     uint16_t  model_sub;
     uint16_t  model_revision;
 
     char      firmware_version_string[32];
     char      firmware_build_date[16];
     char      firmware_build_time[16];
 
     uint8_t   device_sn[16];
     char      device_name[16];
 
     uint64_t  hwfeature_bitmaps; // check enum slamtec_aurora_sdk_device_hw_feature_bitmaps
     uint64_t  sensing_feature_bitmaps; // check enum slamtec_aurora_sdk_device_sensing_feature_bitmaps
     uint64_t  swfeature_bitmaps; // check enum slamtec_aurora_sdk_device_sw_feature_bitmaps
 
     uint64_t  device_uptime_us; // in microseconds
 } slamtec_aurora_sdk_device_basic_info_t;
 
 
 
 enum slamtec_aurora_sdk_relocalization_status_types {
     SLAMTEC_AURORA_SDK_RELOCALIZATION_NONE = 0,
     SLAMTEC_AURORA_SDK_RELOCALIZATION_STARTED,
     SLAMTEC_AURORA_SDK_RELOCALIZATION_SUCCEED,
     SLAMTEC_AURORA_SDK_RELOCALIZATION_FAILED,
     SLAMTEC_AURORA_SDK_RELOCALIZATION_ABORTED
 };
 
 typedef uint32_t slamtec_aurora_sdk_relocalization_status_type_t;
 
 typedef struct _slamtec_aurora_sdk_relocalization_status
 {
     slamtec_aurora_sdk_relocalization_status_type_t status;
     uint64_t timestamp_ns;
 } slamtec_aurora_sdk_relocalization_status_t;
 
 
 enum slamtec_aurora_sdk_enhanced_image_type {
     SLAMTEC_AURORA_SDK_ENHANCED_IMAGE_TYPE_NONE = 0,
     SLAMTEC_AURORA_SDK_ENHANCED_IMAGE_TYPE_DEPTH,
     SLAMTEC_AURORA_SDK_ENHANCED_IMAGE_TYPE_SEMANTIC,
 };
 
 typedef uint32_t slamtec_aurora_sdk_enhanced_image_type_t;
 
 /**
  * @brief The image description structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The image description structure contains the description of an image.
  */
 typedef struct _slamtec_aurora_sdk_image_desc_t {
     /**
      * @brief The width of the image
      * @details The width of the image is the width of the image.
      */
     uint32_t width;
     /**
      * @brief The height of the image
      * @details The height of the image is the height of the image.
      */
     uint32_t height;
     /**
      * @brief The stride of the image
      * @details The stride of the image is the stride of the image.
      */
     uint32_t stride;
     /**
      * @brief The format of the image
      * @details The format of the image is the format of the image.
      * @details 0: gray(uint8_t), 1: rgb, 2: rgba, 3: depth (float32), 4: point3D (floatx3 XYZ)
      */
     uint32_t format; // 0: gray, 1: rgb, 2: rgba, 3: depth, 4: point3D
     /**
      * @brief The size of the image data in bytes
      * @details The size of the image data is the size of the image data in bytes.
      */
     uint32_t data_size;
 } slamtec_aurora_sdk_image_desc_t;
 
 
 /**
  * @brief The keypoint structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The keypoint structure contains the keypoint of an image.
  */
 typedef struct _slamtec_aurora_sdk_keypoint_t {
     /**
      * @brief The x coordinate of the keypoint
      * @details The x coordinate of the keypoint is the x coordinate of the keypoint.
      */
     float x;
     /**
      * @brief The y coordinate of the keypoint
      * @details The y coordinate of the keypoint is the y coordinate of the keypoint.
      */
     float y;
     /**
      * @brief The flags of the keypoint
      * @details The flags of the keypoint are the flags of the keypoint.
      * @details 0: unmatched, non-zero: matched
      */
     uint8_t flags;  // 0: unmatched, non-zero: matched
 } slamtec_aurora_sdk_keypoint_t;
 
 
 /**
  * @brief The tracking data buffer structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The tracking data buffer structure contains the buffer to hold image data and keypoints of a tracking frame
  * @details The buffer should be provided by the caller when invoking the peek interface
  */
 typedef struct _slamtec_aurora_sdk_tracking_data_buffer_t {
     /**
      * @brief The buffer to hold image data of the left camera
      * @details The buffer should be provided by the caller, 
      * @details nullptr to disable image data copy
      */
     void* imgdata_left; //buffer to hold image data of the left camera
                         //the buffer should be provided by the caller
                         //nullptr to disable image data copy
 
     /**
      * @brief The size of the buffer to hold image data of the left camera
      * @details The size of the buffer is the size of the buffer.
      */
     size_t imgdata_left_size; // size of the buffer
 
 
     /**
      * @brief The buffer to hold image data of the right camera
      * @details The buffer should be provided by the caller, 
      * @details nullptr to disable image data copy
      */
     void* imgdata_right; 
     /**
      * @brief The size of the buffer to hold image data of the right camera
      * @details The size of the buffer is the size of the buffer.
      */
     size_t imgdata_right_size;
 
 
     /**
      * @brief The buffer to hold keypoints of the left camera
      * @details The buffer should be provided by the caller
      */
     slamtec_aurora_sdk_keypoint_t* keypoints_left; //buffer to hold keypoints of the left camera
     /**
      * @brief The size of the buffer to hold keypoints of the left camera
      * @details The size of the buffer is the size of the buffer.
      */
     size_t keypoints_left_buffer_count; // size of the buffer
 
     /**
      * @brief The buffer to hold keypoints of the right camera
      * @details The buffer should be provided by the caller
      */
     slamtec_aurora_sdk_keypoint_t* keypoints_right;
 
     /**
      * @brief The size of the buffer to hold keypoints of the right camera
      * @details The size of the buffer is the size of the buffer.
      */
     size_t keypoints_right_buffer_count;
 } slamtec_aurora_sdk_tracking_data_buffer_t;
 
 
 
 /**
  * @brief The tracking status types
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The tracking status types are the status types of the tracking.
  */
 enum slamtec_aurora_sdk_tracking_status_t {
     SLAMTEC_AURORA_TRACKING_STATUS_UNKNOWN = 0,
     SLAMTEC_AURORA_TRACKING_STATUS_SYS_NOT_READY,
     SLAMTEC_AURORA_TRACKING_STATUS_NOT_INIT,
     SLAMTEC_AURORA_TRACKING_STATUS_NO_IMG,
     SLAMTEC_AURORA_TRACKING_STATUS_OK,
     SLAMTEC_AURORA_TRACKING_STATUS_LOST,
     SLAMTEC_AURORA_TRACKING_STATUS_LOST_RECOVERED,
 };
 
 
 
 
 /**
  * @brief The tracking info structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The tracking info structure contains the tracking information.
  * @details NOTICE: the image provided by tracking info has been lossy compressed during transmission
  * @details to retrieve the original image, please use the raw image data callback
  * @details The raw image subscription must be enabled first
  */
 typedef struct _slamtec_aurora_sdk_tracking_info {
     /**
      * @brief The timestamp of the tracking
      * @details The nanoseconds timestamp of the tracking frame
      */
     uint64_t timestamp_ns;
 
     /**
      * @brief The description of the left image
      * @details The description of the left image is the description of the left image.
      */
     slamtec_aurora_sdk_image_desc_t left_image_desc;
     /**
      * @brief The description of the right image
      * @details The description of the right image is the description of the right image.
      */
     slamtec_aurora_sdk_image_desc_t right_image_desc;
 
     /**
      * @brief Whether the tracking is stereo
      */
     uint32_t is_stereo;
 
     /**
      * @brief The tracking status value
      * @details The tracking status value is the value of the slamtec_aurora_sdk_tracking_status_t
      */
     uint32_t tracking_status; // from slamtec_aurora_sdk_tracking_status_t
 
     /**
      * @brief The pose of the tracking (base to world)
      * @details The pose of the tracking is the pose of the tracking.
      */
     slamtec_aurora_sdk_pose_se3_t pose;
 
     /**
      * @brief The count of the keypoints of the left image
      * @details The count of the keypoints of the left image is the count of the keypoints of the left image.
      */
     uint32_t keypoints_left_count;
     /**
      * @brief The count of the keypoints of the right image
      * @details The count of the keypoints of the right image is the count of the keypoints of the right image.
      */
     uint32_t keypoints_right_count;
 } slamtec_aurora_sdk_tracking_info_t;
 
 
 
 /**
  * @brief The stereo image pair description structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The stereo image pair description structure contains the stereo image pair description.
  */
 typedef struct _slamtec_aurora_sdk_stereo_image_pair_desc_t {
     /**
      * @brief The timestamp of the stereo image pair
      * @details The nanoseconds timestamp of the stereo image pair
      */
     uint64_t timestamp_ns;
 
     /**
      * @brief Whether the tracking is stereo
      */
     uint32_t is_stereo;
 
     /**
      * @brief The description of the left image
      * @details The description of the left image is the description of the left image.
      */
     slamtec_aurora_sdk_image_desc_t left_image_desc;
     /**
      * @brief The description of the right image
      * @details The description of the right image is the description of the right image.
      */
     slamtec_aurora_sdk_image_desc_t right_image_desc;
 } slamtec_aurora_sdk_stereo_image_pair_desc_t;
 
 /**
  * @brief The stereo image pair buffer structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The stereo image pair buffer structure contains the stereo image pair buffer.
  */
 typedef struct _slamtec_aurora_sdk_stereo_image_pair_buffer_t {
     /**
      * @brief The buffer to hold image data of the left camera
      * @details The buffer should be provided by the caller, 
      * @details nullptr to disable image data copy
      */
     void* imgdata_left; //buffer to hold image data of the left camera
     /**
      * @brief The buffer to hold image data of the right camera
      * @details The buffer should be provided by the caller, 
      * @details nullptr to disable image data copy
      */
     void* imgdata_right;
 
     /**
      * @brief The size of the buffer to hold image data of the left camera
      * @details The size of the buffer is the size of the buffer.
      */
     size_t imgdata_left_size;
 
     /**
      * @brief The size of the buffer to hold image data of the right camera
      * @details The size of the buffer is the size of the buffer.
      */
     size_t imgdata_right_size;
 } slamtec_aurora_sdk_stereo_image_pair_buffer_t;
 
 
 /**
  * @brief The IMU data structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The IMU data structure contains the IMU data.
  */
 typedef struct _slamtec_aurora_sdk_imu_data_t {
     /**
      * @brief The timestamp of the IMU data
      * @details The nanoseconds timestamp of the IMU data
      */
     uint64_t timestamp_ns;
     /**
      * @brief The ID of the IMU
      * @details The ID of the IMU is the ID of the IMU.
      */
     uint32_t imu_id;
     /**
      * @brief The acceleration data of the IMU
      * @details The acceleration data of the IMU is the acceleration data of the IMU.
      * @details in g (1g = 9.8m/s^2)
      */
     double acc[3];  // in g (1g = 9.8m/s^2)
     /**
      * @brief The gyro data of the IMU
      * @details The gyro data of the IMU is the gyro data of the IMU.
      * @details in dps
      */
     double gyro[3]; // in dps
 } slamtec_aurora_sdk_imu_data_t;
 
 
 /**
  * @brief The IMU info structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The IMU info structure contains the IMU info.
  */
 typedef struct _slamtec_aurora_sdk_imu_info_t {
     /**
      * @brief Whether the IMU data is valid
      * @details non-zero for valid data
      */
     int valid; // non-zero for valid data
 
 
     /**
      * @brief The transform from base to camera
      */
     slamtec_aurora_sdk_pose_se3_t tcb;
 
     /**
      * @brief The transform from IMU to camera
      */
     slamtec_aurora_sdk_pose_se3_t tc_imu;
 
     /**
      * @brief The covariance of the noise
      * @details The covariance of the noise is the covariance of the noise.
      */
     double cov_noise[6]; // gyro to accel
     /**
      * @brief The covariance of the random walk
      * @details The covariance of the random walk is the covariance of the random walk.
      */
     double cov_random_walk[6]; // gyro to accel
 } slamtec_aurora_sdk_imu_info_t;
 
 /**
  * @brief The LIDAR scan point structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The LIDAR scan point structure contains data of a single LIDAR scan point.
  */
 typedef struct _slamtec_aurora_sdk_lidar_scan_point_t {
     /**
      * @brief The distance of the scan point in meters
      */
     float dist; 
     /**
      * @brief The angle of the scan point in radians using the right-hand coordinate system
      */
     float angle;
     /**
      * @brief The quality (RSSI) of the scan point
      */
     uint8_t quality;
 } slamtec_aurora_sdk_lidar_scan_point_t;
 
 /**
  * @brief The single layer LIDAR scan data header structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The single layer LIDAR scan data header structure contains the description of a single layer (2D) LIDAR scan data.
  */
 typedef struct _slamtec_aurora_sdk_lidar_singlelayer_scandata_info_t {
     /**
      * @brief The timestamp of the scan data
      * @details The nanoseconds timestamp of the scan data
      */
     uint64_t timestamp_ns;
     /**
      * @brief The ID of the layer
      * @details The ID of the layer is the ID of the layer.
      */
     int32_t layer_id;
     /**
      * @brief The ID of the binded Visual keyframe
      * @details The ID of the binded Visual keyframe is the ID of the binded Visual keyframe if applicable, otherwise it is 0
      */
     uint64_t binded_kf_id;
 
     /**
      * @brief The yaw change of the scan data
      * @details The yaw rotation change of the scan data during the scan sample is taken
      */
     float dyaw;
     /**
      * @brief The count of the scan points
      * @details The count of the scan points is the count of the scan points.
      */
     uint32_t scan_count;
 } slamtec_aurora_sdk_lidar_singlelayer_scandata_info_t;
 
 
 /**
  * @brief The 2D gridmap dimension structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The 2D gridmap dimension structure contains the dimension information of a 2D gridmap.
  */
 typedef struct _slamtec_aurora_sdk_2d_gridmap_dimension_t{
     /**
      * @brief The minimum x coordinate of the gridmap in meters
      */
     float min_x;
     /**
      * @brief The minimum y coordinate of the gridmap in meters
      */
     float min_y;
     /**
      * @brief The maximum x coordinate of the gridmap in meters
      */
     float max_x;
     /**
      * @brief The maximum y coordinate of the gridmap in meters
      */
     float max_y;
 } slamtec_aurora_sdk_2dmap_dimension_t;
 
 
 /**
  * @brief The rectangle structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The rectangle structure represents a rectangle in 2D Gridmap
  */
 typedef struct _slamtec_aurora_sdk_rect_t {
     /**
      * @brief The x coordinate of the rectangle
      */
     float x;
     /**
      * @brief The y coordinate of the rectangle
      */
     float y;
     /**
      * @brief The width of the rectangle
      */
     float width;
     /**
      * @brief The height of the rectangle
      */
     float height;
 } slamtec_aurora_sdk_rect_t;
 
 
 /**
  * @brief The 2D gridmap generation options structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The 2D gridmap generation options is used to guide the LIDAR 2D Grid Map builder to generate a map
  */
 typedef struct _slamtec_aurora_sdk_2d_gridmap_generation_options_t {
     /**
      * @brief The resolution of the gridmap
      */
     float resolution;
     /**
      * @brief The width of the gridmap canvas
      */
     float map_canvas_width;
     /**
      * @brief The height of the gridmap canvas
      */
     float map_canvas_height;
     /**
      * @brief Whether to generate the active map only
      */
     int   active_map_only;
     /**
      * @brief Whether the height range is specified
      * @details If the height range is specified, the gridmap will be generated using the LIDAR scan with the pose within the specified height range
      */
     int  height_range_specified;
     /**
      * @brief The minimum height of LIDAR scan pose to be included in the gridmap
      */
     float min_height; //only valid when height_range_specified is true
     /**
      * @brief The maximum height of LIDAR scan pose to be included in the gridmap
      */
     float max_height; //only valid when height_range_specified is true
 } slamtec_aurora_sdk_2d_gridmap_generation_options_t;
 
 
 /**
  * @brief The 2D gridmap fetch info structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The 2D gridmap fetch info structure is used to describe the actual retrieved 2D gridmap data
  */
 typedef struct _slamtec_aurora_sdk_2d_gridmap_fetch_info_t {
     /**
      * @brief The x coordinate of the retrieved gridmap in meters
      */
     float real_x;
     /**
      * @brief The y coordinate of the retrieved gridmap in meters
      */
     float real_y;
     /**
      * @brief The width of the retrieved gridmap cell in meters
      */
     int cell_width;
     /**
      * @brief The height of the retrieved gridmap cell in meters
      */
     int cell_height;
 } slamtec_aurora_sdk_2d_gridmap_fetch_info_t;
 
 /**
  * @brief The floor detection description structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The floor detection description structure contains the description of a detected floor
  */
 typedef struct _slamtec_aurora_sdk_floor_detection_desc_t {
     /**
      * @brief The ID of the floor
      * @details The ID of the floor is only to identify a specific floor among the detected floors, it is different from the floor number in real world
      * @details The ID value may change during each detection iteration even for the same logic floor.  
      */
     int floorID;
     /**
      * @brief The typical height of the floor
      */
     float typical_height;
     /**
      * @brief The minimum height of the floor
      */
     float min_height;
     /**
      * @brief The maximum height of the floor
      */
     float max_height;
     /**
      * @brief The confidence of the floor detection
      */
     float confidence;
 } slamtec_aurora_sdk_floor_detection_desc_t;
 
 
 /**
  * @brief The floor detection histogram info structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The floor detection histogram info structure contains the histogram info used by the auto floor detection algorithm
  */
 typedef struct _slamtec_aurora_sdk_floor_detection_histogram_info_t {
     /**
      * @brief The width of the histogram bin in meters
      */
     float bin_width;
     /**
      * @brief The start height of the histogram bin in meters
      */
     float bin_height_start;
     /**
      * @brief The total count of the histogram bin
      */
     int bin_total_count;
 } slamtec_aurora_sdk_floor_detection_histogram_info_t;
 
 
 // -- Map Objects
 
 /**
  * @brief The global map description structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The global map description structure contains the global map description.
  */
 typedef struct _slamtec_aurora_sdk_global_map_desc_t {
     /**
      * @brief The count of the map points to fetch currently in the background thread
      */
     uint64_t lastMPCountToFetch;
     /**
      * @brief The count of the keyframes to fetch currently in the background thread
      */
     uint64_t lastKFCountToFetch;
     /**
      * @brief The count of the maps to fetch currently in the background thread
      */
     uint64_t lastMapCountToFetch;
 
     /**
      * @brief The count of the map points retrieved currently in the background thread
      */
     uint64_t lastMPRetrieved;
     /**
      * @brief The count of the keyframes retrieved currently in the background thread
      */
     uint64_t lastKFRetrieved;
 
     /**
      * @brief The total count of the map points
      */
     uint64_t totalMPCount;
     /**
      * @brief The total count of the keyframes
      */
     uint64_t totalKFCount;
     /**
      * @brief The total count of the maps
      */
     uint64_t totalMapCount;
 
     /**
      * @brief The total count of the map points fetched
      */
     uint64_t totalMPCountFetched;
     /**
      * @brief The total count of the keyframes fetched
      */
     uint64_t totalKFCountFetched;
     /**
      * @brief The total count of the maps fetched
      */
     uint64_t totalMapCountFetched;
 
     /**
      * @brief The current count of the active map points
      */
     uint64_t currentActiveMPCount;
     /**
      * @brief The current count of the active keyframes
      */
     uint64_t currentActiveKFCount;
 
     /**
      * @brief The ID of the active map
      */
     uint32_t activeMapID;
 
     /**
      * @brief The current mapping flags
      */
     slamtec_aurora_sdk_mapping_flag_t mappingFlags;
 
     /**
      * @brief The sliding window start keyframe ID used in the localization mode
      */
     uint64_t slidingWindowStartKFId;
 
 } slamtec_aurora_sdk_global_map_desc_t;
 
 
 enum slamtec_aurora_sdk_map_flags_t {
     /**
      * @brief The none map flag
      */
     SLAMTEC_AURORA_SDK_MAP_FLAG_NONE = 0,
     /**
      * @brief The bad map flag
      */
     SLAMTEC_AURORA_SDK_MAP_FLAG_BAD = (0x1 << 0),
     /**
      * @brief The fixed map flag
      */
     SLAMTEC_AURORA_SDK_MAP_FLAG_FIXED = (0x1 << 1),
 };
 
 
 
 /**
  * @brief The map description structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The map description structure contains the map description.
  */
 typedef struct _slamtec_aurora_sdk_map_desc_t {
     /**
      * @brief The ID of the map
      */
     uint32_t map_id;
     /**
      * @brief The flags of the map, check enum slamtec_aurora_sdk_map_flags_t for more details
      */
     uint32_t map_flags;
     /**
      * @brief The count of the keyframes in the map
      */
     uint64_t keyframe_count;
     /**
      * @brief The count of the map points in the map
      */
     uint64_t map_point_count;
     
     /**
      * @brief The ID of the first keyframe in the map
      */
     uint64_t keyframe_id_start;
     /**
      * @brief The ID of the last keyframe in the map
      */
     uint64_t keyframe_id_end;
 
     /**
      * @brief The ID of the first map point in the map
      */
     uint64_t map_point_id_start;
     /**
      * @brief The ID of the last map point in the map
      */
     uint64_t map_point_id_end;
 
 } slamtec_aurora_sdk_map_desc_t;
 
 
 enum slamtec_aurora_sdk_keyframe_flags_t {
     /**
      * @brief The none keyframe flag
      */
     SLAMTEC_AURORA_SDK_KEYFRAME_FLAG_NONE = 0,
     /**
      * @brief The bad keyframe flag
      */
     SLAMTEC_AURORA_SDK_KEYFRAME_FLAG_BAD = (0x1 << 0),
     /**
      * @brief The fixed keyframe flag
      */
     SLAMTEC_AURORA_SDK_KEYFRAME_FLAG_FIXED = (0x1 << 1),
 };
 
 
 enum slamtec_aurora_sdk_keyframe_fetch_flags_t {
     /**
      * @brief The none keyframe fetch flag
      */
     SLAMTEC_AURORA_SDK_KEYFRAME_FETCH_FLAG_NONE = 0,
     /**
      * @brief The related map point flag
      */
     SLAMTEC_AURORA_SDK_KEYFRAME_FETCH_FLAG_RELATED_MP = (0x1 << 0),
     
 };
 
 
 
 /**
  * @brief The keyframe description structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The keyframe description structure contains the keyframe description.
  */
 typedef struct _slamtec_aurora_sdk_keyframe_desc_t {
     /**
      * @brief The ID of the keyframe
      */
     uint64_t id;
     /**
      * @brief The ID of the parent keyframe
      */
     uint64_t parent_id;
     /**
      * @brief The ID of the map
      */
     uint32_t map_id;
 
     /**
      * @brief The timestamp of the keyframe
      */
     double timestamp;
 
     /**
      * @brief The pose of the keyframe (base to world)
      */
     slamtec_aurora_sdk_pose_se3_t pose_se3;
     slamtec_aurora_sdk_pose_t pose;
 
     /**
      * @brief The count of the looped frames
      */
     size_t looped_frame_count;
     /**
      * @brief The count of the connected frames
      */
     size_t connected_frame_count;
 
 
     /**
      * @brief The count of the related map points
      * @details The related map points are the map points that are observed by the keyframe.
      * @details Users must enable the related map points fetching flag to make SDK synchronize the related map points.
      */
     size_t related_mp_count;
 
 
     /**
      * @brief The flags of the keyframe, check enum slamtec_aurora_sdk_keyframe_flags_t for more details
      */
     uint32_t flags;
 } slamtec_aurora_sdk_keyframe_desc_t;
 
 
 enum slamtec_aurora_sdk_map_point_fetch_flags_t {
     /**
      * @brief The none map point fetch flag
      */
     SLAMTEC_AURORA_SDK_MAP_POINT_FETCH_FLAG_NONE = 0,
     
 };
 
 
 /**
  * @brief The map point description structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The map point description structure contains the map point description.
  */
 typedef struct _slamtec_aurora_sdk_map_point_desc_t {
     /**
      * @brief The ID of the map point
      */
     uint64_t id;
     /**
      * @brief The ID of the map
      */
     uint32_t map_id;
 
     /**
      * @brief The timestamp of the map point
      */
     double timestamp;
 
     /**
      * @brief The position of the map point
      */
     slamtec_aurora_sdk_position3d_t position;
     /**
      * @brief The flags of the map point
      */
     uint32_t flags;
 } slamtec_aurora_sdk_map_point_desc_t;
 
 // -- Calibration Data Types
 
 /**
  * @brief The transform calibration structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The transform calibration structure contains the transform calibration information.
  */
 typedef struct _slamtec_aurora_sdk_transform_calibration_t {
     slamtec_aurora_sdk_pose_se3_t t_base_cam;
     slamtec_aurora_sdk_pose_se3_t t_camera_imu;
 } slamtec_aurora_sdk_transform_calibration_t;
 
 
 enum slamtec_aurora_sdk_len_type {
     SLAMTEC_AURORA_SDK_LEN_TYPE_PINHOLE = 0,
     SLAMTEC_AURORA_SDK_LEN_TYPE_RECTIFIED = 1,
     SLAMTEC_AURORA_SDK_LEN_TYPE_KANNALABRANDT = 2,
 };
 
 typedef uint32_t slamtec_aurora_sdk_len_type_t;
 
 
 enum slamtec_aurora_sdk_camera_color_mode {
     SLAMTEC_AURORA_SDK_CAMERA_COLOR_MODE_RGB = 0,
     SLAMTEC_AURORA_SDK_CAMERA_COLOR_MODE_MONO = 1,
 };
 
 typedef uint32_t slamtec_aurora_sdk_camera_color_mode_t;
 
 typedef struct _slamtec_aurora_sdk_single_camera_calibration_t {
     slamtec_aurora_sdk_len_type_t len_type;
     slamtec_aurora_sdk_camera_color_mode_t color_mode;
     int width;
     int height;
     int fps;
     float intrinsics[4]; // fx, fy, cx, cy
     float distortion[5]; // k1, k2, k3, k4
 } slamtec_aurora_sdk_single_camera_calibration_t;
 
 enum slamtec_aurora_sdk_camera_type {
     SLAMTEC_AURORA_SDK_CAMERA_TYPE_MONO = 0,
     SLAMTEC_AURORA_SDK_CAMERA_TYPE_STEREO = 1,
 };
 
 typedef uint32_t slamtec_aurora_sdk_camera_type_t;
 
 
 typedef struct _slamtec_aurora_sdk_ext_camera_transform_t {
     float t_c2_c1[16]; // 4x4 matrix with row major order
 } slamtec_aurora_sdk_ext_camera_transform_t;
 
 
 /**
  * @brief The camera calibration structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The camera calibration structure contains the camera calibration information.
  */
 typedef struct _slamtec_aurora_sdk_camera_calibration_t {
     slamtec_aurora_sdk_camera_type_t camera_type;
     slamtec_aurora_sdk_single_camera_calibration_t camera_calibration[4];
     slamtec_aurora_sdk_ext_camera_transform_t ext_camera_transform[4];
 } slamtec_aurora_sdk_camera_calibration_t;
 
 // -- Enhanced Imaging Data Types
 // ------------------------------------------------------------------------------------------------
 
 typedef struct _slamtec_aurora_sdk_enhanced_imaging_frame_desc {
     uint64_t timestamp_ns;
     slamtec_aurora_sdk_image_desc_t image_desc;
 } slamtec_aurora_sdk_enhanced_imaging_frame_desc_t;
 
 
 typedef struct _slamtec_aurora_sdk_enhanced_imaging_frame_buffer {
     void * frame_data;
     size_t frame_data_size;
 } slamtec_aurora_sdk_enhanced_imaging_frame_buffer_t;
 
 // -- Depth Camera Data Types
 typedef struct _slamtec_aurora_sdk_depthcam_config_info {
     float fps;
     int frame_skip;
     int image_width;
     int image_height;
 
     int binded_cam_id;
 
 } slamtec_aurora_sdk_depthcam_config_info_t;
 
 
 
 
 enum slamtec_aurora_sdk_depthcam_frame_type {
     SLAMTEC_AURORA_SDK_DEPTHCAM_FRAME_TYPE_DEPTH_MAP = 0,
     SLAMTEC_AURORA_SDK_DEPTHCAM_FRAME_TYPE_POINT3D = 1,
 };
 
 typedef int32_t slamtec_aurora_sdk_depthcam_frame_type_t; 
 
 
 // -- Semantic Segmentation Data Types
 typedef struct _slamtec_aurora_sdk_semantic_segmentation_config_info {
     float fps;
     int frame_skip;
     int image_width;
     int image_height;
     int support_alternative_model;
 
 } slamtec_aurora_sdk_semantic_segmentation_config_info_t;
 
 typedef struct _slamtec_aurora_sdk_semantic_segmentation_label_name {
     char name[64];
 } slamtec_aurora_sdk_semantic_segmentation_label_name_t;
 
 typedef struct _slamtec_aurora_sdk_semantic_segmentation_label_info {
     size_t label_count;
     slamtec_aurora_sdk_semantic_segmentation_label_name_t label_names[256];
 } slamtec_aurora_sdk_semantic_segmentation_label_info_t;
 
 // callbacks
 
 /**
  * @brief The map storage session result callback
  * @ingroup SDK_Callback_Types SDK Callback Types
  * @details The map storage session result callback is the callback for the map storage session result.
  * @param user_data The user data to be passed to the callback
  * @param is_ok Whether the map storage session is ok
  */
 typedef void (*slamtec_aurora_sdk_mapstorage_session_result_callback_t)(void* user_data, int is_ok);
 
 /**
  * @brief The image data callback
  * @ingroup SDK_Callback_Types SDK Callback Types
  * @details The image data callback to receive the raw image data from the device.
  * @param user_data The user data to be passed to the callback
  * @param timestamp_ns The timestamp of the image
  * @param left_desc The description of the left image
  * @param left_data The data of the left image
  * @param right_desc The description of the right image
  * @param right_data The data of the right image
  */
 typedef void (*slamtec_aurora_sdk_on_image_data_callback_t)(void* user_data, uint64_t timestamp_ns, const slamtec_aurora_sdk_image_desc_t* left_desc, const void* left_data, const slamtec_aurora_sdk_image_desc_t* right_desc, const void* right_data);
 
 /**
  * @brief The tracking data callback
  * @ingroup SDK_Callback_Types SDK Callback Types
  * @details The tracking data callback to receive the tracking data from the device.
  * @param user_data The user data to be passed to the callback
  * @param tracking_data The tracking data
  * @param provided_buffer_info The provided buffer of the tracking data like image data and keypoints, this buffer is provided by the SDK, the buffer will be invalidated after the callback returns
  */
 typedef void (*slamtec_aurora_sdk_on_tracking_data_callback_t)(void* user_data, const slamtec_aurora_sdk_tracking_info_t* tracking_data, const slamtec_aurora_sdk_tracking_data_buffer_t* provided_buffer_info);
 
 /**
  * @brief The IMU data callback
  * @ingroup SDK_Callback_Types SDK Callback Types
  * @details The IMU data callback to receive the IMU data from the device.
  * @param user_data The user data to be passed to the callback
  * @param timestamp_ns The timestamp of the IMU data
  * @param status The status of the device
  */
 typedef void (*slamtec_aurora_sdk_on_imu_data_callback_t)(void* user_data, const slamtec_aurora_sdk_imu_data_t* imu_data, size_t imu_data_count);
 
 
 /**
  * @brief The mapping flags callback
  * @ingroup SDK_Callback_Types SDK Callback Types
  * @details The mapping flags callback to receive the mapping flags from the device.
  * @param user_data The user data to be passed to the callback
  * @param mapping_flags The mapping flags
  */
 typedef void (*slamtec_aurora_sdk_on_mapping_flags_callback_t)(void* user_data, slamtec_aurora_sdk_mapping_flag_t mapping_flags);
 
 /**
  * @brief The device status callback
  * @ingroup SDK_Callback_Types SDK Callback Types
  * @details The device status callback to receive the device status from the device.
  * @param user_data The user data to be passed to the callback
  * @param timestamp_ns The timestamp of the device status
  * @param status The status of the device
  */
 typedef void (*slamtec_aurora_sdk_on_device_status_callback_t)(void* user_data, uint64_t timestamp_ns, slamtec_aurora_sdk_device_status_t status);
 
 
 /**
  * @brief The lidar scan callback
  * @ingroup SDK_Callback_Types SDK Callback Types
  * @details The lidar scan callback to receive the lidar scan data from the device.
  * @param user_data The user data to be passed to the callback
  * @param scan_info The scan info of the lidar scan data
  * @param scan_point_buffer The buffer of the lidar scan points, this buffer is provided by the SDK, the buffer will be invalidated after the callback returns. The buffer count is scan_info->scan_count
  */
 typedef void (*slamtec_aurora_sdk_on_lidar_scan_callback_t)(void* user_data, const slamtec_aurora_sdk_lidar_singlelayer_scandata_info_t* scan_info, const slamtec_aurora_sdk_lidar_scan_point_t* scan_point_buffer);
 
 
 /**
  * @brief The camera preview image status callback
  * @ingroup SDK_Callback_Types SDK Callback Types
  * @details The camera preview image status callback to receive the camera preview image status from the device.
  */
 typedef void (*slamtec_aurora_sdk_on_camera_preview_image_callback_t)(void* user_data, uint64_t timestamp_ns, const slamtec_aurora_sdk_image_desc_t* left_desc, const void* left_data, const slamtec_aurora_sdk_image_desc_t* right_desc, const void* right_data);
 
 
 
 /**
  * @brief The connection status callback
  * @ingroup SDK_Callback_Types SDK Callback Types
  * @details The connection status callback to receive the connection status from the device.
  */
 typedef void (*slamtec_aurora_sdk_on_connection_status_callback_t)(void* user_data, slamtec_aurora_sdk_connection_status_t status);
 
 
 /**
  * @brief The depthcam image arrived callback
  * @ingroup SDK_Callback_Types SDK Callback Types
  * @details The depthcam image arrived callback to receive the depthcam image arrived from the device.
  */
 typedef void (*slamtec_aurora_sdk_on_depthcam_image_arrived_callback_t)(void* user_data, uint64_t timestamp_ns);
 
 
 /**
  * @brief The semantic segmentation image arrived callback
  * @ingroup SDK_Callback_Types SDK Callback Types
  * @details The semantic segmentation image arrived callback to receive the semantic segmentation image arrived from the device.
  */
 typedef void (*slamtec_aurora_sdk_on_semantic_segmentation_image_arrived_callback_t)(void* user_data, uint64_t timestamp_ns);
 
 
 /**
  * @brief The listener structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The listener structure contains the listener.
  */
 typedef struct _slamtec_aurora_sdk_listener_t {
     /**
      * @brief The user data to be passed to the callback
      */
     void* user_data;
 
     /**
      * @brief The callback for the raw image data, set to NULL to ignore this callback
      */
     slamtec_aurora_sdk_on_image_data_callback_t on_raw_image_data;
     /**
      * @brief The callback for the tracking data, set to NULL to ignore this callback
      */
     slamtec_aurora_sdk_on_tracking_data_callback_t on_tracking_data;
     /**
      * @brief The callback for the IMU data, set to NULL to ignore this callback
      */
     slamtec_aurora_sdk_on_imu_data_callback_t on_imu_data;
     /**
      * @brief The callback for the mapping flags, set to NULL to ignore this callback
      */
     slamtec_aurora_sdk_on_mapping_flags_callback_t on_mapping_flags;
     /**
      * @brief The callback for the device status, set to NULL to ignore this callback
      */
     slamtec_aurora_sdk_on_device_status_callback_t on_device_status;
     
     /**
      * @brief The callback for the lidar scan data, set to NULL to ignore this callback
      */
     slamtec_aurora_sdk_on_lidar_scan_callback_t on_lidar_scan;
 
 
     /**
      * @brief The callback for the camera preview image status, set to NULL to ignore this callback
      */
     slamtec_aurora_sdk_on_camera_preview_image_callback_t on_camera_preview_image;
 
     /**
      * @brief The callback for the connection status, set to NULL to ignore this callback
      */
     slamtec_aurora_sdk_on_connection_status_callback_t on_connection_status;
 
 
     /**
      * @brief The callback for the depthcam image arrived, set to NULL to ignore this callback
      */
     slamtec_aurora_sdk_on_depthcam_image_arrived_callback_t on_depthcam_image_arrived;
 
     /**
      * @brief The callback for the semantic segmentation image arrived, set to NULL to ignore this callback
      */
     slamtec_aurora_sdk_on_semantic_segmentation_image_arrived_callback_t on_semantic_segmentation_image_arrived;
 
 
 } slamtec_aurora_sdk_listener_t;
 
 
 // map visitor
 
 /**
  * @brief The callback for accessing the keyframe data
  * @ingroup SDK_Callback_Types SDK Callback Types
  * @details The callback to receive the keyframe data locally cached by the SDK
  * @param user_data The user data to be passed to the callback
  * @param keyframe_desc The keyframe description
  * @param looped_frame_ids The looped frame ids
  * @param connected_frame_ids The connected frame ids
  * @param related_mp_ids The related map point ids, this array is only valid when the related map point flag is enabled
  */
 typedef void (*slamtec_aurora_sdk_on_map_keyframe_callback_t)(void* user_data, const slamtec_aurora_sdk_keyframe_desc_t* keyframe_desc, const uint64_t * looped_frame_ids, const uint64_t * connected_frame_ids, const uint64_t * related_mp_ids);
 /**
  * @brief The callback for accessing the map point data
  * @ingroup SDK_Callback_Types SDK Callback Types
  * @details The callback to receive the map point data locally cached by the SDK
  */
 typedef void (*slamtec_aurora_sdk_on_map_point_callback_t)(void* user_data, const slamtec_aurora_sdk_map_point_desc_t* map_point_desc);
 
 /**
  * @brief The callback for accessing the map description
  * @ingroup SDK_Callback_Types SDK Callback Types
  * @details The callback to receive the map description locally cached by the SDK
  */
 typedef void (*slamtec_aurora_sdk_on_map_desc_callback_t)(void* user_data, const slamtec_aurora_sdk_map_desc_t* map_desc);
 
 
 /**
  * @brief The map data visitor structure
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The map data visitor structure contains the map data visitor.
  */
 typedef struct _slamtec_aurora_sdk_map_data_visitor_t {
     /**
      * @brief The user data to be passed to the callback
      */
     void* user_data;
 
     /**
      * @brief The callback for accessing the keyframe data, set to NULL to ignore this callback
      */
     slamtec_aurora_sdk_on_map_keyframe_callback_t on_keyframe;
     /**
      * @brief The callback for accessing the map point data, set to NULL to ignore this callback
      */
     slamtec_aurora_sdk_on_map_point_callback_t on_map_point;
     /**
      * @brief The callback for accessing the map description, set to NULL to ignore this callback
      */
     slamtec_aurora_sdk_on_map_desc_callback_t on_map_desc;
 } slamtec_aurora_sdk_map_data_visitor_t;
 
 
 
 // -- Utility Functions
 /**
  * @brief The data recorder type
  * @ingroup SDK_Basic_Data_Types SDK Basic Data Types
  * @details The data recorder type
  */
 enum slamtec_aurora_sdk_datarecorder_type {
     SLAMTEC_AURORA_DATARECORDER_TYPE_NONE = 0,
     SLAMTEC_AURORA_DATARECORDER_TYPE_RAW_DATASET = 1,
     SLAMTEC_AURORA_DATARECORDER_TYPE_COLMAP_DATASET = 2,
 };
 
 typedef uint32_t slamtec_aurora_sdk_datarecorder_type_t;
 
 /** @} */ // end of SDK_Basic_Data_Types
 