/*
 *  SLAMTEC Aurora
 *  Copyright 2013 - 2024 SLAMTEC Co., Ltd.
 *
 *  http://www.slamtec.com
 *
 *  Aurora Remote SDK
 *  C++ Wrapper Header of the SDK
 *  
 *  At lease C++ 14 is required
 */


#pragma once


#include <cstdlib>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <functional>
#include <memory>

#include "slamtec_remote_objects.hxx"


/**
 * @defgroup SDK_Cxx_Wrapper Aurora Remote SDK C++ Interface
 * @brief The C++ wrapper for the Aurora Remote SDK
 * @details The C++ wrapper for the Aurora Remote SDK, providing a more convenient and type-safe interface for the SDK.
 * @details At least C++ 14 is required.
 * 
 * @{
 */

/**
 * @defgroup SDK_Callback_Types SDK Callback Types
 * @brief The callback types for the SDK
 * @ingroup SDK_Cxx_Wrapper SDK C++ Wrapper
 */

/**
 * @defgroup Cxx_Session_Management Session Management
 * @brief The session management classes
 */

/**
 * @defgroup Cxx_Controller_Operations Controller Operations
 * @brief The controller classes
 */

/**
 * @defgroup Cxx_Map_Management Map Management
 * @brief The map management classes
 */

/**
 * @defgroup Cxx_DataProvider_Operations Data Provider Operations
 * @brief The data provider classes
 */

/**
 * @defgroup Cxx_LIDAR_2DMap_Operations LIDAR 2D GridMap Operations
 * @brief The LIDAR 2D GridMap classes
 */

/**
 * @defgroup Cxx_Auto_Floor_Detection_Operations LIDAR Auto Floor Detection Operations
 * @brief The Auto Floor Detection classes
 */

/** @} */ // end of SDK_Cxx_Wrapper


namespace rp { namespace standalone { namespace aurora { 



class RemoteSDK;
class RemoteDataProvider;
class RemoteController;
class RemoteMapManager;
class LIDAR2DMapBuilder;

/**
 * @brief The listener base class for receiving the data stream from the remote device
 * @details The caller should inherit this class and implement the virtual functions to handle the data stream
 * @ingroup SDK_Callback_Types SDK Callback Types
 */
class RemoteSDKListener
{
    friend class RemoteSDK;
public:
    RemoteSDKListener() 
        : _sdk_listener_obj{ 0 }
    {
        _sdk_listener_obj.user_data = this;
        _binding();
    }
    virtual ~RemoteSDKListener() {}

    /**
     * @brief The callback for the tracking data
     * @ingroup SDK_Callback_Types SDK Callback Types
     * @details The callback to receive the tracking data from the remote device
     */
    virtual void onTrackingData(const RemoteTrackingFrameInfo & info) {}

    /**
     * @brief The callback for the raw image data
     * @ingroup SDK_Callback_Types SDK Callback Types
     * @details The callback to receive the raw image data from the remote device
     */
    virtual void onRawCamImageData(uint64_t timestamp_ns, const RemoteImageRef& left, const RemoteImageRef & right) {}

    /**
     * @brief The callback for the IMU data
     * @ingroup SDK_Callback_Types SDK Callback Types
     * @details The callback to receive the IMU data from the remote device
     */
    virtual void onIMUData(const slamtec_aurora_sdk_imu_data_t* imuBuffer, size_t bufferCount) {}

    /**
     * @brief The callback for the mapping flags
     * @ingroup SDK_Callback_Types SDK Callback Types
     * @details The callback to receive the mapping flags from the remote device
     */
    virtual void onNewMappingFlags(slamtec_aurora_sdk_mapping_flag_t flags) {}

    /**
     * @brief The callback for the device status
     * @ingroup SDK_Callback_Types SDK Callback Types
     * @details The callback to receive the device status from the remote device
     */
    virtual void onDeviceStatusChanged(uint64_t timestamp_ns, slamtec_aurora_sdk_device_status_t status) {}


    /**
     * @brief The callback for the LIDAR scan data
     * @ingroup SDK_Callback_Types SDK Callback Types
     * @details The callback to receive the LIDAR scan data from the remote device
     */
    virtual void onLIDARScan(const slamtec_aurora_sdk_lidar_singlelayer_scandata_info_t& header, const slamtec_aurora_sdk_lidar_scan_point_t* points) {}

private:

    void _binding() {
        _sdk_listener_obj.on_raw_image_data = [](void* user_data, uint64_t timestamp_ns, const slamtec_aurora_sdk_image_desc_t* left_desc, const void* left_data, const slamtec_aurora_sdk_image_desc_t* right_desc, const void* right_data) {
            RemoteSDKListener* This = reinterpret_cast<RemoteSDKListener *>(user_data);

            RemoteImageRef left(*left_desc, left_data);
            RemoteImageRef right(*right_desc, right_data);

            This->onRawCamImageData(timestamp_ns, left, right);
        };

        _sdk_listener_obj.on_tracking_data = [](void* user_data, const slamtec_aurora_sdk_tracking_info_t* tracking_data, const slamtec_aurora_sdk_tracking_data_buffer_t* provided_buffer_info) {
            RemoteSDKListener* This = reinterpret_cast<RemoteSDKListener*>(user_data);

            RemoteTrackingFrameInfo frameInfo(*tracking_data, *provided_buffer_info);

            This->onTrackingData(frameInfo);
        };

        _sdk_listener_obj.on_imu_data = [](void* user_data, const slamtec_aurora_sdk_imu_data_t* imu_data, size_t imu_data_count) {
            RemoteSDKListener* This = reinterpret_cast<RemoteSDKListener*>(user_data);

            This->onIMUData(imu_data, imu_data_count);
        };

        _sdk_listener_obj.on_mapping_flags = [](void* user_data, slamtec_aurora_sdk_mapping_flag_t flags) {
            RemoteSDKListener* This = reinterpret_cast<RemoteSDKListener*>(user_data);

            This->onNewMappingFlags(flags);
        };

        _sdk_listener_obj.on_device_status = [](void* user_data, uint64_t timestamp_ns,  slamtec_aurora_sdk_device_status_t status) {
            RemoteSDKListener* This = reinterpret_cast<RemoteSDKListener*>(user_data);

            This->onDeviceStatusChanged(timestamp_ns, status);
        };

        _sdk_listener_obj.on_lidar_scan = [](void* user_data, const slamtec_aurora_sdk_lidar_singlelayer_scandata_info_t* header, const slamtec_aurora_sdk_lidar_scan_point_t* points) {
            RemoteSDKListener* This = reinterpret_cast<RemoteSDKListener*>(user_data);
            This->onLIDARScan(*header, points);
        };

    }

protected:
    slamtec_aurora_sdk_listener_t _sdk_listener_obj;
};
#if 0
class RemoteMapDataVisitor {
public:
    friend class RemoteDataProvider;

    virtual ~RemoteMapDataVisitor() {}
    RemoteMapDataVisitor()
    {
        memset(&_visitor_obj, 0, sizeof(_visitor_obj));
        _visitor_obj.user_data = this;
        binding();
    }

    virtual void onMapData(const slamtec_aurora_sdk_map_desc_t* mapData) {}
    virtual void onKeyFrameData(const slamtec_aurora_sdk_keyframe_desc_t* keyFrameData) {}
    virtual void onMapPointData(const slamtec_aurora_sdk_map_point_desc_t* mpData) {}


protected:
    void binding() {
        _visitor_obj.on_map_desc = [](void* user_data, const slamtec_aurora_sdk_map_desc_t* map_data) {
            RemoteMapDataVisitor* This = reinterpret_cast<RemoteMapDataVisitor*>(user_data);
            This->onMapData(map_data);
            };

        _visitor_obj.on_keyframe = [](void* user_data, const slamtec_aurora_sdk_keyframe_desc_t* keyframe_data) {
            RemoteMapDataVisitor* This = reinterpret_cast<RemoteMapDataVisitor*>(user_data);
            This->onKeyFrameData(keyframe_data);
            };

        _visitor_obj.on_map_point = [](void* user_data, const slamtec_aurora_sdk_map_point_desc_t* mp_data) {
            RemoteMapDataVisitor* This = reinterpret_cast<RemoteMapDataVisitor*>(user_data);
            This->onMapPointData(mp_data);
            };
    }


    slamtec_aurora_sdk_map_data_visitor_t _visitor_obj;
    
};
#endif

/**
 * @brief The map data visitor class for accessing the map data from the remote device
 * @details The caller must invoke the subscribe functions to register the callbacks to access the map data
 * @ingroup Cxx_DataProvider_Operations Data Provider Operations
 */
class RemoteMapDataVisitor {
public:
    friend class RemoteDataProvider;

    using MapDataCallback = std::function<const void(const slamtec_aurora_sdk_map_desc_t&)>;
    using KeyFrameDataCallback = std::function<const void(const RemoteKeyFrameData &)>;
    using MapPointDataCallback = std::function<const void(const slamtec_aurora_sdk_map_point_desc_t&)>;

    RemoteMapDataVisitor() 
        : _visitor_obj{0}
    {
        _visitor_obj.user_data = this;
    }

    /**
     * @brief Subscribe the map data callback
     */
    void subscribeMapData(const MapDataCallback& mapDataCallback) {
        _mapDataCallback = (mapDataCallback);

        _visitor_obj.on_map_desc = [](void* user_data, const slamtec_aurora_sdk_map_desc_t* map_data) {
            RemoteMapDataVisitor* This = reinterpret_cast<RemoteMapDataVisitor*>(user_data);
            This->_mapDataCallback(*map_data);
            };
    }

    /**
     * @brief Subscribe the keyframe data callback
     */
    void subscribeKeyFrameData(const KeyFrameDataCallback& keyFrameDataCallback) {
        _keyFrameDataCallback = (keyFrameDataCallback);
        _visitor_obj.on_keyframe = [](void* user_data, const slamtec_aurora_sdk_keyframe_desc_t* keyframe_data, const uint64_t * lcIDs, const uint64_t * connIDs) {
            RemoteMapDataVisitor* This = reinterpret_cast<RemoteMapDataVisitor*>(user_data);
            RemoteKeyFrameData kframeData(*keyframe_data, lcIDs, connIDs);
            
            This->_keyFrameDataCallback(kframeData);
            };
    }

    /**
     * @brief Subscribe the map point data callback
     */
    void subscribeMapPointData(const MapPointDataCallback& mapPointDataCallback) {
        _mapPointDataCallback = (mapPointDataCallback);
        _visitor_obj.on_map_point = [](void* user_data, const slamtec_aurora_sdk_map_point_desc_t* mp_data) {
            RemoteMapDataVisitor* This = reinterpret_cast<RemoteMapDataVisitor*>(user_data);
            This->_mapPointDataCallback(*mp_data);
            };
    }

protected:
    MapDataCallback _mapDataCallback;
    KeyFrameDataCallback _keyFrameDataCallback;
    MapPointDataCallback _mapPointDataCallback;

    slamtec_aurora_sdk_map_data_visitor_t _visitor_obj;
};


/**
 * @brief The SDK configuration class
 * @ingroup Cxx_Session_Management Session Management
 */
class SDKConfig : public slamtec_aurora_sdk_session_config_t
{
    
public:
    SDKConfig() : slamtec_aurora_sdk_session_config_t() {
        memset(this, 0, sizeof(slamtec_aurora_sdk_session_config_t));
    }
};



/**
 * @brief The controller class for communicating with the remote device
 * @ingroup Cxx_Controller_Operations Controller Operations
 */
class RemoteController : public Noncopyable {
    friend class RemoteSDK;
public:

    /**
     * @brief Get the discovered server list
     * @details Get the discovered remote devices, the SDK continuously performs the discovery process to update the server list in the background
     * @param[out] serverList The output server list to store the discovered servers
     * @param[in] maxFetchCount The maximum number of servers to fetch, default is 32
     * @return The number of servers fetched
     */
    size_t getDiscoveredServers(std::vector<SDKServerConnectionDesc>& serverList, size_t maxFetchCount = 32)
    {
        serverList.resize(maxFetchCount);

        auto count = slamtec_aurora_sdk_controller_get_discovered_servers(_sdk, serverList.data(), maxFetchCount);
        serverList.resize(count);
        return count;
    }

    /**
     * @brief Connect to the remote device
     * @param[in] serverDesc The server description to connect to
     * @param[out] errCode The error code, set to nullptr if not interested
     * @return True if the connection is successful, false otherwise
     */
    bool connect(const SDKServerConnectionDesc& serverDesc, slamtec_aurora_sdk_errorcode_t * errCode = nullptr) {
        auto result = slamtec_aurora_sdk_controller_connect(_sdk, &serverDesc);
        if (errCode) {
            *errCode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    /**
     * @brief Disconnect from the remote device
     */
    void disconnect() {
        slamtec_aurora_sdk_controller_disconnect(_sdk);
    }

    /**
     * @brief Check if the controller is connected to the remote device
     * @return True if the controller is connected, false otherwise
     */
    bool isConnected() {
        return slamtec_aurora_sdk_controller_is_connected(_sdk) != 0;
    }

    /**
     * @brief Set the low rate mode
     * @details Once the low rate mode is enabled, the SDK will disable the IMU data and Raw image data subscription to reduce the data traffic
     * @details Some SDK operations like map streaming will automatically set the low rate mode to true, and the mode will be automatically disabled after the map streaming operation is done
     * @param[in] enable True to enable the low rate mode, false to disable
     */
    void setLowRateMode(bool enable) {
        slamtec_aurora_sdk_controller_set_low_rate_mode(_sdk, enable ? 1 : 0);
    }

    /**
     * @brief Set the map data syncing mode
     * @details Once the map data syncing is enabled, the SDK will start a background thread to sync the map data from the remote device
     * @details The synced map data will be cached in the local device.
     * @details The map data can be accessed via the map data access function
     * @details NOTICE: the synced map data is only for visualization, it should not be used for localization or other high-precision requirements
     * @param[in] enable True to enable the map data syncing, false to disable
     */
    void setMapDataSyncing(bool enable) {
        slamtec_aurora_sdk_controller_set_map_data_syncing(_sdk, enable ? 1 : 0);
    }

    /**
     * @brief Resync the map data
     * @details Ask the SDK to refetch all the map data from the remote device
     * @param[in] invalidateCache True to invalidate the cached map data, false to keep the cached map data
     */
    void resyncMapData(bool invalidateCache = false) {
        slamtec_aurora_sdk_controller_resync_map_data(_sdk, invalidateCache ? 1 : 0);
    }

    /**
     * @brief Set the raw data subscription mode
     * @details Once the raw data subscription is enabled, the SDK will subscribe the raw image data 
     * @param[in] enable True to enable the raw data subscription, false to disable
     */
    void setRawDataSubscription(bool enable) {
        slamtec_aurora_sdk_controller_set_raw_data_subscription(_sdk, enable ? 1 : 0);
    }

    /**
     * @brief Check if the raw data subscription is enabled
     * @return True if the raw data subscription is enabled, false otherwise
     */
    bool isRawDataSubscribed() {
        return slamtec_aurora_sdk_controller_is_raw_data_subscribed(_sdk) != 0;
    }

    /**
     * @brief Require the remote device to perform the map reset
     * @param[in] timeout_ms The timeout in milliseconds, default is 5000ms
     * @param[out] errcode The error code, set to nullptr if not interested
     * @return True if the map reset request is sent and performed successfully, false otherwise
     */
    bool requireMapReset(uint64_t timeout_ms = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_TIMEOUT, slamtec_aurora_sdk_errorcode_t * errcode = nullptr) {
        auto result =  slamtec_aurora_sdk_controller_require_map_reset(_sdk, timeout_ms);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    /**
     * @brief Require the remote device to enter the pure localization mode
     * @param[in] timeout_ms The timeout in milliseconds, default is 5000ms
     * @param[out] errcode The error code, set to nullptr if not interested
     * @return True if the pure localization request is sent and performed successfully, false otherwise
     */
    bool requirePureLocalizationMode(uint64_t timeout_ms = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_TIMEOUT, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        auto result = slamtec_aurora_sdk_controller_require_pure_localization_mode(_sdk, timeout_ms);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    /**
     * @brief Require the remote device to enter the mapping mode
     * @param[in] timeout_ms The timeout in milliseconds, default is 5000ms
     * @param[out] errcode The error code, set to nullptr if not interested
     * @return True if the mapping request is sent and performed successfully, false otherwise
     */
    bool requireMappingMode(uint64_t timeout_ms = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_TIMEOUT, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        auto result = slamtec_aurora_sdk_controller_require_mapping_mode(_sdk, timeout_ms);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    /**
    * @brief Require the remote device to perform the relocalization
    * @param[in] timeout_ms The timeout in milliseconds, default is 5000ms
    * @param[out] errcode The error code, set to nullptr if not interested
    * @return True if the relocalization request is sent and performed successfully, false otherwise
    */

    bool requireRelocalization(uint64_t timeout_ms = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_TIMEOUT, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        auto result = slamtec_aurora_sdk_controller_require_relocalization(_sdk, timeout_ms);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    /**
    * @brief Cancel the relocalization
    * @param[in] timeout_ms The timeout in milliseconds, default is 5000ms
    * @param[out] errcode The error code, set to nullptr if not interested
    * @return True if the relocalization is canceled successfully, false otherwise
    */
    bool cancelRelocalization(uint64_t timeout_ms = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_TIMEOUT, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        auto result = slamtec_aurora_sdk_controller_cancel_relocalization(_sdk, timeout_ms);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    /**
     * @brief Set the loop closure flag
     * @param[in] enable True to enable the loop closure, false to disable
     * @param[in] timeout_ms The timeout in milliseconds, default is 5000ms
     * @param[out] errcode The error code, set to nullptr if not interested
     * @return True if the loop closure is set successfully, false otherwise
     */
    bool setLoopClosure(bool enable, uint64_t timeout_ms = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_TIMEOUT, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        auto result = slamtec_aurora_sdk_controller_set_loop_closure(_sdk, enable ? 1 : 0, timeout_ms);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    /**
     * @brief Send a custom command to the remote device
     * @param[in] timeout_ms The timeout in milliseconds, default is 5000ms
     * @param[in] cmd The command to send
     * @param[in] data The data to send, set to nullptr if not interested
     * @param[in] data_size The size of the data to send, default is 0
     * @param[out] response The response from the remote device, set to nullptr if not interested
     * @param[in] response_buffer_size The size of the response buffer, default is 0
     * @param[out] response_retrieved_size The size of the response retrieved, set to nullptr if not interested
     * @param[out] errcode The error code, set to nullptr if not interested
     * @return True if the custom command is sent and performed successfully, false otherwise
     */
    bool sendCustomCommand(uint64_t timeout_ms, uint64_t cmd, const void* data = nullptr, size_t data_size = 0, void* response = nullptr, size_t response_buffer_size = 0, size_t* response_retrieved_size = nullptr, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        auto result = slamtec_aurora_sdk_controller_send_custom_command(_sdk, timeout_ms, cmd, data, data_size, response, response_buffer_size, response_retrieved_size);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }



protected:

    RemoteController(slamtec_aurora_sdk_session_handle_t& sdk)
        : _sdk(sdk)
    {}

    slamtec_aurora_sdk_session_handle_t _sdk;
};


/**
 * @brief The map manager class for managing the map data
 * @details Use this class to upload or download the map to the remote device
 * @details The map data accessing provided by the @ref RemoteDataProvider class is only for visualization
 * @ingroup Cxx_Map_Management Map Management
 */
class RemoteMapManager : public Noncopyable {
    friend class RemoteSDK;
public:

    /**
     * @brief Start an upload session
     * @details Start an upload session to upload the map file to the remote device
     * @details It the device has already working on a map streaming session, the new upload session will be failed
     * @details The SDK will enter the low rate mode during the upload session to reduce the data traffic
     * @details The low rate mode will be automatically disabled after the upload session is done
     * @param[in] mapfilePath The path to the map file to upload
     * @param[in] resultCallBack The callback function to receive the session result, set to nullptr if not interested
     * @param[in] userData The user data to pass to the callback function, set to nullptr if not interested
     * @param[out] errCode The error code, set to nullptr if not interested
     * @return True if the upload session is started successfully, false otherwise
     */
    bool startUploadSession(const char* mapfilePath, slamtec_aurora_sdk_mapstorage_session_result_callback_t resultCallBack = nullptr, void * userData = nullptr,  slamtec_aurora_sdk_errorcode_t* errCode = nullptr) {
        return startSession(mapfilePath, SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_TYPE_UPLOAD, resultCallBack, userData, errCode);
    }

    /**
     * @brief Start a download session
     * @details Start a download session to download the map file from the remote device
     * @param[in] mapfilePath The path to the map file to download
     * @param[in] resultCallBack The callback function to receive the session result, set to nullptr if not interested
     * @param[in] userData The user data to pass to the callback function, set to nullptr if not interested
     * @param[out] errCode The error code, set to nullptr if not interested
     * @return True if the download session is started successfully, false otherwise
     */
    bool startDownloadSession(const char* mapfilePath, slamtec_aurora_sdk_mapstorage_session_result_callback_t resultCallBack = nullptr, void* userData = nullptr, slamtec_aurora_sdk_errorcode_t* errCode = nullptr) {
        return startSession(mapfilePath, SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_TYPE_DOWNLOAD, resultCallBack, userData, errCode);
    }

    /**
     * @brief Start a map streaming session
     * @details Start a map streaming session to stream the map data from the remote device
     * @param[in] mapfilePath The path to the map file to stream
     * @param[in] sessionType The type of the session to start
     * @param[in] resultCallBack The callback function to receive the session result, set to nullptr if not interested
     * @param[in] userData The user data to pass to the callback function, set to nullptr if not interested
     * @param[out] errCode The error code, set to nullptr if not interested
     * @return True if the map streaming session is started successfully, false otherwise
     */
    bool startSession(const char* mapfilePath, slamtec_aurora_sdk_mapstorage_session_type_t sessionType, slamtec_aurora_sdk_mapstorage_session_result_callback_t resultCallBack = nullptr, void* userData = nullptr, slamtec_aurora_sdk_errorcode_t* errCode = nullptr) {
        auto result = slamtec_aurora_sdk_mapmanager_start_storage_session(_sdk, mapfilePath, sessionType, resultCallBack, userData);
        if (errCode) {
            *errCode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    /**
     * @brief Check if the map streaming session is active
     * @return True if the map streaming session is active, false otherwise
     */
    bool isSessionActive() {
        return slamtec_aurora_sdk_mapmanager_is_storage_session_active(_sdk) != 0;
    }

    /**
     * @brief Abort the map streaming session
     * @details Abort the current map streaming session
     * @details If the session is created by other client, the abort operation will be rejected
     */
    void abortSession() {
        slamtec_aurora_sdk_mapmanager_abort_session(_sdk);
    }

    /**
     * @brief Query the status of the map streaming session
     * @param[out] progressOut The progress of the map streaming session
     * @param[out] errCode The error code, set to nullptr if not interested
     * @return True if the status is queried successfully, false otherwise
     */
    bool querySessionStatus(slamtec_aurora_sdk_mapstorage_session_status_t& progressOut, slamtec_aurora_sdk_errorcode_t* errCode = nullptr) {
        auto result = slamtec_aurora_sdk_mapmanager_query_storage_status(_sdk, &progressOut);
        if (errCode) {
            *errCode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }



protected:
    RemoteMapManager(slamtec_aurora_sdk_session_handle_t& sdk)
        : _sdk(sdk)
    {}

    slamtec_aurora_sdk_session_handle_t _sdk;
};



/**
 * @brief The data provider class for accessing the data retrieved from the remote device
 * @details Use this class to access the data retrieved from the remote device
 * @ingroup Cxx_DataProvider_Operations Data Provider Operations
 */
class RemoteDataProvider : public Noncopyable {
    friend class RemoteSDK;
public:
    /**
     * @brief Get the current device pose in SE3 format
     * @details Caller can use this function to get the current pose in SE3 format.
     * @details The pose data retrieved is the cached data from previous fetched by the background data sync thread.
     * @details The pose data may be outdated. If caller needs the latest pose data, it should using the SDK listener to get the pose update event.
     * @param[out] poseOut The current pose in SE3 format
     * @param[out] errcode The error code, set to nullptr if not interested
     * @return True if the pose is retrieved successfully, false otherwise
     */
    bool getCurrentPoseSE3(slamtec_aurora_sdk_pose_se3_t& poseOut, slamtec_aurora_sdk_errorcode_t * errcode = nullptr) {
        auto result = slamtec_aurora_sdk_dataprovider_get_current_pose_se3(_sdk, &poseOut);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    /**
     * @brief Get the current device pose in Euler angles (Roll-Pitch-Yaw  RPY order) format
     * @details Caller can use this function to get the current pose in Euler angles format.
     * @details The pose data retrieved is the cached data from previous fetched by the background data sync thread.
     * @details The pose data may be outdated. If caller needs the latest pose data, it should using the SDK listener to get the pose update event.
     * @details WARNING: gimbal lock may happen, please use the SE3 pose if possible.
     * @param[out] poseOut The current pose in Euler angles format
     * @param[out] errcode The error code, set to nullptr if not interested
     * @return True if the pose is retrieved successfully, false otherwise
     */
    bool getCurrentPose(slamtec_aurora_sdk_pose_t& poseOut, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        auto result = slamtec_aurora_sdk_dataprovider_get_current_pose(_sdk, &poseOut);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    /**
     * @brief Get the current mapping flags
     * @details Caller can use this function to get the current SLAM working flags, e.g. whether Loop Closure is disabled or not
     * @param[out] flagsOut The current mapping flags
     * @param[out] errcode The error code, set to nullptr if not interested
     * @return True if the flags are retrieved successfully, false otherwise
     */
    bool getMappingFlags(slamtec_aurora_sdk_mapping_flag_t& flagsOut, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        auto result = slamtec_aurora_sdk_dataprovider_get_mapping_flags(_sdk, &flagsOut);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    /**
     * @brief Get the last device status
     * @details Caller can use this function to get the last device status.
     * @param[out] statusOut The last device status
     * @param[out] timestamp_ns The timestamp of the last device status
     * @param[out] errcode The error code, set to nullptr if not interested
     * @return True if the status is retrieved successfully, false otherwise
     */
    bool getLastDeviceStatus(slamtec_aurora_sdk_device_status_t& statusOut, uint64_t & timestamp_ns, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        auto result = slamtec_aurora_sdk_dataprovider_get_last_device_status(_sdk, &statusOut, &timestamp_ns);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    /**
     * @brief Peek the tracking data
     * @details Caller can use this function to peek the tracking data
     * @details The tracking data is the cached data from previous fetched by the background data sync thread.
     * @details The tracking data may be outdated. If caller needs the latest tracking data, it should using the SDK listener to get the tracking update event.
     * @param[out] infoOut The tracking data
     * @param[out] errcode The error code, set to nullptr if not interested
     * @return True if the tracking data is retrieved successfully, false otherwise
     */
    bool peekTrackingData(RemoteTrackingFrameInfo& infoOut, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        slamtec_aurora_sdk_tracking_info_t trackingInfo;
        slamtec_aurora_sdk_tracking_data_buffer_t bufferInfo;

        

        std::vector<uint8_t> imgbufferLeft, imgbufferRight;
        std::vector< slamtec_aurora_sdk_keypoint_t> keypointBufferLeft, keypointBufferRight;


        // fetch the image and kp count
        memset(&bufferInfo, 0, sizeof(bufferInfo));
        auto result = slamtec_aurora_sdk_dataprovider_peek_tracking_data(_sdk, &trackingInfo, &bufferInfo);
        
        if (result != SLAMTEC_AURORA_SDK_ERRORCODE_OK) {
            if (errcode) {
                *errcode = result;
            }
            return false;
        }


        // allocate buffer
        imgbufferLeft.resize(trackingInfo.left_image_desc.data_size);
        imgbufferRight.resize(trackingInfo.right_image_desc.data_size);
        keypointBufferLeft.resize(trackingInfo.keypoints_left_count);
        keypointBufferRight.resize(trackingInfo.keypoints_right_count);

        bufferInfo.imgdata_left = imgbufferLeft.data();
        bufferInfo.imgdata_right = imgbufferRight.data();
        bufferInfo.keypoints_left = keypointBufferLeft.data();
        bufferInfo.keypoints_right = keypointBufferRight.data();

        bufferInfo.imgdata_left_size = imgbufferLeft.size();
        bufferInfo.imgdata_right_size = imgbufferRight.size();
        bufferInfo.keypoints_left_buffer_count = keypointBufferLeft.size();
        bufferInfo.keypoints_right_buffer_count = keypointBufferRight.size();


        result = slamtec_aurora_sdk_dataprovider_peek_tracking_data(_sdk, &trackingInfo, &bufferInfo);
        if (errcode) {
            *errcode = result;
        }

        if (result == SLAMTEC_AURORA_SDK_ERRORCODE_OK) {
            infoOut = std::move(RemoteTrackingFrameInfo(trackingInfo, 
                std::move(imgbufferLeft), std::move(imgbufferRight),
                std::move(keypointBufferLeft), std::move(keypointBufferRight)));
        }

        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    //peek peek VSLAM System Status
    bool peekVSLAMSystemStatus(slamtec_aurora_sdk_device_status_desc_t& statusOut, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        auto result = slamtec_aurora_sdk_dataprovider_get_last_device_status(_sdk, &statusOut.status, &statusOut.timestamp_ns);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    bool peekRelocalizationStatus(slamtec_aurora_sdk_relocalization_status_t & statusOut, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        auto result = slamtec_aurora_sdk_dataprovider_get_relocalization_status(_sdk, &statusOut.status, &statusOut.timestamp_ns);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    /**
     * @brief Peek the recent LIDAR scan data in single layer mode
     * @details Caller can use this function to peek the recent LIDAR scan data along with its pose.
     * @details The pose is estimated by the tracking pose at the time of the scan.
     * @details The LIDAR scan data is the cached data from previous fetched by the background data sync thread.
     * @details The LIDAR scan data may be outdated. If caller needs the latest LIDAR scan data, it should using the SDK listener to get the LIDAR scan update event.
     * @param[out] scanData The LIDAR scan data
     * @param[out] scanPoseSE3 The pose of the LIDAR scan data
     * @param[in] forceLatest Whether to force the latest LIDAR scan data, set to false to use the cached data
     * @param[out] errcode The error code, set to nullptr if not interested
     * @return True if the LIDAR scan data is retrieved successfully, false otherwise
     */
    bool peekRecentLIDARScanSingleLayer(SingleLayerLIDARScan& scanData, slamtec_aurora_sdk_pose_se3_t & scanPoseSE3, bool forceLatest = false,  slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        // pre-allocate the buffer
        std::vector< slamtec_aurora_sdk_lidar_scan_point_t> localdata;
        
        size_t newSize = 4096;

        do {
            localdata.resize(newSize);
            auto result = slamtec_aurora_sdk_dataprovider_peek_recent_lidar_scan_singlelayer(_sdk, &scanData.info, localdata.data(), localdata.size(), &scanPoseSE3, forceLatest ? 1 : 0);

            if (result != SLAMTEC_AURORA_SDK_ERRORCODE_OK) {
                scanData.scanData.clear();
                if (errcode) {
                    *errcode = result;
                }
                return false;
            }


            // not enough buffer? less likely to happen
            if (scanData.info.scan_count > localdata.size()) {
                newSize = scanData.info.scan_count;
                continue;
            }


            localdata.resize(scanData.info.scan_count);
            scanData.scanData = std::move(localdata);

            return true;

        } while (1);

        return false;
    }   

    /**
     * @brief Peek the IMU data
     * @details Caller can use this function to peek the IMU data
     * @details The IMU data is the cached data from previous fetched by the background data sync thread.
     * @details The IMU data may be outdated. If caller needs the latest IMU data, it should using the SDK listener to get the IMU update event.
     * @param[out] imuDataOut The IMU data
     * @param[out] errcode The error code, set to nullptr if not interested
     * @return True if the IMU data is retrieved successfully, false otherwise
     */
    bool peekIMUData(std::vector<slamtec_aurora_sdk_imu_data_t>& imuDataOut, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        size_t bufferCount = 0;
        imuDataOut.resize(4096);
        auto result = slamtec_aurora_sdk_dataprovider_peek_imu_data(_sdk, imuDataOut.data(), imuDataOut.size(), &bufferCount);
        if (errcode) {
            *errcode = result;
        }
        if (result == SLAMTEC_AURORA_SDK_ERRORCODE_OK) {
            imuDataOut.resize(bufferCount);
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    /**
     * @brief Get the IMU info
     * @details Caller can use this function to get the IMU info.
     * @param[out] infoOut The IMU info
     * @param[out] errcode The error code, set to nullptr if not interested
     * @return True if the IMU info is retrieved successfully, false otherwise
     */
    bool getIMUInfo(slamtec_aurora_sdk_imu_info_t& infoOut, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        auto result = slamtec_aurora_sdk_dataprovider_get_imu_info(_sdk, &infoOut);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    /**
     * @brief Get the global mapping info
     * @details Caller can use this function to get the global mapping info.
     * @param[out] descOut The global mapping info
     * @param[out] errcode The error code, set to nullptr if not interested
     * @return True if the global mapping info is retrieved successfully, false otherwise
     */
    bool getGlobalMappingInfo(slamtec_aurora_sdk_global_map_desc_t& descOut, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        auto result = slamtec_aurora_sdk_dataprovider_get_global_mapping_info(_sdk, &descOut);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    /**
     * @brief Get all map info
     * @details Caller can use this function to get all map description info.
     * @param[out] descBuffer The buffer to store the map info
     * @param[out] errcode The error code, set to nullptr if not interested
     * @return True if the map info is retrieved successfully, false otherwise
     */
    bool getAllMapInfo(std::vector<slamtec_aurora_sdk_map_desc_t>& descBuffer, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        size_t mapCount;
        auto result = slamtec_aurora_sdk_dataprovider_get_all_map_info(_sdk, nullptr, 0, &mapCount);

        if (result != SLAMTEC_AURORA_SDK_ERRORCODE_OK) {
            if (errcode) {
                *errcode = result;
            }
            return false;
        }
        
        descBuffer.resize(mapCount*2);
        size_t actualCount = 0;
        result = slamtec_aurora_sdk_dataprovider_get_all_map_info(_sdk, descBuffer.data(), descBuffer.size(), &actualCount);
        if (errcode) {
            *errcode = result;
        }
        if (result != SLAMTEC_AURORA_SDK_ERRORCODE_OK) {
            descBuffer.clear();
            return false;
        }
        descBuffer.resize(actualCount);
        return true;
    }

    /**
     * @brief Access the map data like keyframe and map points data 
     * @details Caller can use this function to access the map data like keyframe and map points data.
     * @details A visitor object contains data callback listeners must be provided
     * @details those callbacks set to NULL will be ignored
     * @details the SDK will enter stall state during the data accessing,
     * @details i.e. the background data sync will  paused
     * @details if all map data should be accessed, simply pass an empty vector to the map_ids
     * @param[in] visitor The visitor to visit the map data
     * @param[in] mapIDs The map IDs to access, set to empty vector if access all maps
     * @param[out] errcode The error code, set to nullptr if not interested
     * @return True if the map data is accessed successfully, false otherwise
     */
    bool accessMapData(const RemoteMapDataVisitor& visitor, std::vector<uint32_t> mapIDs = std::vector<uint32_t>() , slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        return accessMapData(visitor._visitor_obj, mapIDs, errcode);
    }

    /**
     * @brief Access the map data like keyframe and map points data 
     * @details Caller can use this function to access the map data like keyframe and map points data.
     * @details A visitor object contains data callback listeners must be provided
     * @details those callbacks set to NULL will be ignored
     * @details the SDK will enter stall state during the data accessing,
     * @details i.e. the background data sync will  paused
     * @details if all map data should be accessed, simply pass an empty vector to the map_ids
     * @param[in] visitor The visitor to visit the map data
     * @param[in] mapIDs The map IDs to access, set to empty vector if access all maps
     * @param[out] errcode The error code, set to nullptr if not interested
     * @return True if the map data is accessed successfully, false otherwise
     */
    bool accessMapData(const slamtec_aurora_sdk_map_data_visitor_t& visitor, std::vector<uint32_t> mapIDs = std::vector<uint32_t>(), slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        uint32_t* mapIDsBuffer = nullptr;
        size_t mapCount = 0;
        if (!mapIDs.empty()) {
            mapIDsBuffer = mapIDs.data();
            mapCount = mapIDs.size();
        }


        auto result = slamtec_aurora_sdk_dataprovider_access_map_data(_sdk, &visitor, mapIDsBuffer, mapCount);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

protected:
    RemoteDataProvider(slamtec_aurora_sdk_session_handle_t& sdk)
        : _sdk(sdk)
    {}

    slamtec_aurora_sdk_session_handle_t _sdk;
};

/**
 * @brief The 2D gridmap reference class wraps the 2D gridmap handle
 * @details This class is used to access a 2D gridmap data.
 * @ingroup Cxx_LIDAR_2DMap_Operations LIDAR 2D GridMap Operations
 */
class OccupancyGridMap2DRef : public Noncopyable {
    friend class LIDAR2DMapBuilder;
public:
    OccupancyGridMap2DRef(const slamtec_aurora_sdk_occupancy_grid_2d_handle_t handle, bool ownBuffer)
        : _handle(handle), _ownBuffer(ownBuffer)
    {
    }


    ~OccupancyGridMap2DRef() {
        if (_ownBuffer) {
            slamtec_aurora_sdk_lidar2dmap_gridmap_release(_handle);
        }
    }

    /**
     * @brief Get the resolution of the 2D gridmap
     * @return The resolution of the 2D gridmap
     */
    float getResolution() const {
        float resolution;
        if (slamtec_aurora_sdk_lidar2dmap_gridmap_get_resolution(_handle, &resolution) != SLAMTEC_AURORA_SDK_ERRORCODE_OK) {
            return 0;
        }
        return resolution;
    }

    /**
     * @brief Get the dimension of the 2D gridmap
     * @param[out] dimensionOut The dimension of the 2D gridmap
     */
    void getMapDimension(slamtec_aurora_sdk_2dmap_dimension_t& dimensionOut) const {
        slamtec_aurora_sdk_lidar2dmap_gridmap_get_dimension(_handle, &dimensionOut, 0);
    }

    /**
     * @brief Get the maximum capacity dimension of the 2D gridmap
     * @param[out] dimensionOut The maximum capacity dimension of the 2D gridmap
     */
    void getMaxMapCapacityDimension(slamtec_aurora_sdk_2dmap_dimension_t& dimensionOut) const {
        slamtec_aurora_sdk_lidar2dmap_gridmap_get_dimension(_handle, &dimensionOut, 1);
    }

    /**
     * @brief Read the cell data of the 2D gridmap
     * @param[in] rect The rectangle area to read
     * @param[out] fetchInfoOut The fetch info of the actual area of the cell data
     * @param[out] dataOut The data of the cell
     * @param[in] l2pMapping Whether to perform log odd to logic (0-255) mapping to each cell. For visualization purpose, set to true.
     * @return True if the cell data is read successfully, false otherwise
     */
    bool readCellData(const slamtec_aurora_sdk_rect_t& rect, slamtec_aurora_sdk_2d_gridmap_fetch_info_t& fetchInfoOut, std::vector<uint8_t>& dataOut, bool l2pMapping = true) const {
        int bL2p = (l2pMapping ? 1 : 0);
        auto result = slamtec_aurora_sdk_lidar2dmap_gridmap_read_cell_data(_handle, &rect, &fetchInfoOut, nullptr, 0, bL2p);
        if (result != SLAMTEC_AURORA_SDK_ERRORCODE_OK) {
            return false;
        }

        size_t cellSize = fetchInfoOut.cell_height * fetchInfoOut.cell_width;
        if (cellSize) {
            dataOut.resize(cellSize);
            result = slamtec_aurora_sdk_lidar2dmap_gridmap_read_cell_data(_handle, &rect, &fetchInfoOut, dataOut.data(), dataOut.size(), bL2p);
        }

        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    /**
     * @brief Read the cell data of the 2D gridmap
     * @param[in] rect The rectangle area to read
     * @param[out] fetchInfoOut The fetch info of the actual area of the cell data
     * @param[out] dataBuffer The buffer to store the cell data, set to nullptr to only get the fetch info
     * @param[in] dataBufferSize The size of the buffer, set to 0 to only get the fetch info
     * @param[in] l2pMapping Whether to perform log odd to logic (0-255) mapping to each cell. For visualization purpose, set to true.
     * @return True if the cell data is read successfully, false otherwise
     */
    bool readCellData(const slamtec_aurora_sdk_rect_t& rect, slamtec_aurora_sdk_2d_gridmap_fetch_info_t& fetchInfoOut, uint8_t* dataBuffer, size_t dataBufferSize, bool l2pMapping = true) const {
        return slamtec_aurora_sdk_lidar2dmap_gridmap_read_cell_data(_handle, &rect, &fetchInfoOut, dataBuffer, dataBufferSize, l2pMapping?1:0); 
    }

    /**
     * @brief Get the handle of the 2D gridmap
     * @return The handle of the 2D gridmap
     */
    slamtec_aurora_sdk_occupancy_grid_2d_handle_t getHandle() const {
        return _handle;
    }

protected:
    slamtec_aurora_sdk_occupancy_grid_2d_handle_t _handle;
    bool _ownBuffer;
};


/**
 * @brief The 2D gridmap builder class
 * @details This class is used to build a 2D gridmap.
 * @ingroup Cxx_LIDAR_2DMap_Operations LIDAR 2D GridMap Operations
 */
class LIDAR2DMapBuilder : public Noncopyable {
    friend class RemoteSDK;

public:

    /**
     * @brief Get the supported resolution range of the 2D gridmap
     * @return The supported resolution range of the 2D gridmap [min, max]
     */
    std::tuple<float, float> getSupportedResolutionRange() const {
        float minRes, maxRes;
        if (slamtec_aurora_sdk_lidar2dmap_get_supported_grid_resultion_range(_sdk, &minRes, &maxRes) != SLAMTEC_AURORA_SDK_ERRORCODE_OK) {
            return std::make_tuple(0.0f, 0.0f);
        }
        return std::make_tuple(minRes, maxRes);
    }

    /**
     * @brief Get the maximum grid cell count of the 2D gridmap
     * @details Caller can use this function to get the supported maximum grid cell count. Each cell is stored as a byte. 
 * @details For example, for a map with 100 meters width and 100 meters height, if the resolution is 0.1 meter, the maximum cell count will be (100/0.1) * (100/0.1) =  1,000,000 .
     * @details If the cell count is larger than the supported maximum, the map will not be generated.
     * @return The maximum grid cell count of the 2D gridmap
     */
    size_t getMaxGridCellCount() const {
        size_t maxCount;
        if (slamtec_aurora_sdk_lidar2dmap_get_supported_max_grid_cell_count(_sdk, &maxCount) != SLAMTEC_AURORA_SDK_ERRORCODE_OK) {
            return 0;
        }
        return maxCount;
    }


    /**
     * @brief Require the preview map to be redrawn
     * @details The request will be queued and processed by the background thread.
     * @return True if the preview map is redrawn requested successfully, false otherwise
     */
    bool requireRedrawPreviewMap() {
        return slamtec_aurora_sdk_lidar2dmap_previewmap_require_redraw(_sdk) == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    /**
     * @brief Start the preview map background update
     * @param[in] options The generation options of the preview map
     * @return True if the preview map background update is started successfully, false otherwise
     */
    bool startPreviewMapBackgroundUpdate(const LIDAR2DGridMapGenerationOptions & options) {
        auto result = slamtec_aurora_sdk_lidar2dmap_previewmap_start_background_update(_sdk, &options);
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    /**
     * @brief Stop the preview map background update
     */
    void stopPreviewMapBackgroundUpdate() {
        slamtec_aurora_sdk_lidar2dmap_previewmap_stop_background_update(_sdk);
    }

    /**
     * @brief Check if the preview map background update is active
     * @return True if the preview map background update is active, false otherwise
     */
    bool isPreviewMapBackgroundUpdateActive() const {
        return slamtec_aurora_sdk_lidar2dmap_previewmap_is_background_updating(_sdk) != 0;
    }

    /**
     * @brief Get the dirty rectangle of the preview map and reset the dirty rectangle
     * @param[out] rectOut The dirty rectangle of the preview map
     * @param[out] mapBigChange the map big change flag will be stored in this pointer. If there is a big change, the map will be redrawn and this flag will be set to true. It commonly happens when there is a loop closure or a new map has been loaded.
     */
    void getAndResetPreviewMapDirtyRect(slamtec_aurora_sdk_rect_t& rectOut, bool & mapBigChange) {
        int mpChange;
        if (slamtec_aurora_sdk_lidar2dmap_previewmap_get_and_reset_update_dirty_rect(_sdk, &rectOut, &mpChange) != SLAMTEC_AURORA_SDK_ERRORCODE_OK) {
            mapBigChange = false;
            memset(&rectOut, 0, sizeof(rectOut));
        }
        else {
            mapBigChange = (mpChange != 0);
        }
    }

    /**
     * @brief Get the generation options of the preview map
     * @details Caller can use this function to get the current generation options of the LIDAR 2D preview map.
     * @details If the auto floor detection is enabled, caller can use this function to get the current height range used to generate the preview map.
     * @param[out] optionsOut The generation options of the preview map
     * @return The error code
     */ 
    void getPreviewMapGenerationOptions(LIDAR2DGridMapGenerationOptions& optionsOut) const {
        slamtec_aurora_sdk_lidar2dmap_previewmap_get_generation_options(_sdk, &optionsOut);
    }

    /**
     * @brief Set the auto floor detection for the preview map
     * @details Caller can use this function to set the auto floor detection for the LIDAR 2D preview map.
     * @details If the auto floor detection is enabled, the detector will update the height range used to generate the preview map.
     * @details The height range is based on the current floor description.
     * @details Auto floor detection is useful for multiple floor scenarios. 
     * @param[in] enable Whether to enable the auto floor detection
     */
    void setPreviewMapAutoFloorDetection(bool enable) {
        slamtec_aurora_sdk_lidar2dmap_previewmap_set_auto_floor_detection(_sdk, enable?1:0);
    }

    /**
     * @brief Check if the auto floor detection is enabled for the preview map
     * @return True if the auto floor detection is enabled, false otherwise
     */
    bool isPreviewMapAutoFloorDetectionEnabled() const {
        return slamtec_aurora_sdk_lidar2dmap_previewmap_is_auto_floor_detection(_sdk) != 0;
    }

    /**
     * @brief Get the preview map to access the map data
     * @return The preview map
     */
    const OccupancyGridMap2DRef& getPreviewMap() const {
        _cachedPreviewMap._handle = slamtec_aurora_sdk_lidar2dmap_previewmap_get_gridmap_handle(_sdk);
        return _cachedPreviewMap;
    }

    /**
     * @brief Generate the full map
     * @details Caller can use this function to generate the full map on demand and return the handle of the generated map.
     * @details The caller thread will be blocked until the map is generated or the timeout occurs.
     * @param[in] options The generation options of the full map
     * @param[in] waitForDataSync Whether to wait for the data sync to complete
     * @param[in] timeout_ms The timeout in milliseconds
     * @return The full map
     */
    std::shared_ptr<OccupancyGridMap2DRef> generateFullMap(const LIDAR2DGridMapGenerationOptions& options, bool waitForDataSync = true, uint64_t timeout_ms = 30000) {
        slamtec_aurora_sdk_occupancy_grid_2d_handle_t handleOut;
        auto result = slamtec_aurora_sdk_lidar2dmap_generate_fullmap(_sdk, &handleOut, &options, (waitForDataSync?1:0), timeout_ms);
    
        if (result != SLAMTEC_AURORA_SDK_ERRORCODE_OK) {
            return nullptr;
        }

        return std::make_shared<OccupancyGridMap2DRef>(handleOut, true);
    }

protected:
    LIDAR2DMapBuilder(slamtec_aurora_sdk_session_handle_t& sdk)
        : _sdk(sdk)
        , _cachedPreviewMap(nullptr, false)
    {
    }

    mutable OccupancyGridMap2DRef _cachedPreviewMap;

    slamtec_aurora_sdk_session_handle_t _sdk;
};


/**
 * @brief The floor detector class
 * @ingroup Cxx_Auto_Floor_Detection_Operations LIDAR Auto Floor Detection Operations
 */
class FloorDetector : public Noncopyable {
    friend class RemoteSDK;


public:
    /**
     * @brief Get the detected floor description of the current detected floor
     * @param[out] descOut The current detected floor description
     * @return True if the current detected floor description is retrieved successfully, false otherwise
     */
    bool getCurrentDetectedFloorDesc(slamtec_aurora_sdk_floor_detection_desc_t& descOut) {
        return slamtec_aurora_sdk_autofloordetection_get_current_detection_desc(_sdk, &descOut) == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    /**
     * @brief Get all the detected floor descriptions
     * @param[out] descBuffer The buffer to store the detected floor descriptions
     * @param[out] currentFloorID The ID of the current detected floor
     * @return True if the detected floor descriptions are retrieved successfully, false otherwise
     */
    bool getAllDetectionDesc(std::vector<slamtec_aurora_sdk_floor_detection_desc_t>& descBuffer, int & currentFloorID) {
        
        descBuffer.clear();

        do {
            size_t actualCount = 0;
            auto result = slamtec_aurora_sdk_autofloordetection_get_all_detection_info(_sdk, descBuffer.data(), descBuffer.size(), &actualCount, &currentFloorID);

            if (result != SLAMTEC_AURORA_SDK_ERRORCODE_OK) {
                descBuffer.clear();
                return false;
            }

            if (actualCount > descBuffer.size()) {
                descBuffer.resize(actualCount);
                continue;
            }
            
            descBuffer.resize(actualCount);
            break;
        } while (1);
        return true;
      
    }

    /**
     * @brief Get the detection histogram of the floor detection
     * @param[out] histogramOut The histogram of the floor detection
     * @return True if the histogram is retrieved successfully, false otherwise
     */
    bool getDetectionHistogram(FloorDetectionHistogram& histogramOut) {
        size_t histogramSize = 100;

        do {
            histogramOut.histogramData.resize(histogramSize);

            auto result = slamtec_aurora_sdk_autofloordetection_get_detection_histogram(_sdk, &histogramOut.info, histogramOut.histogramData.data(), histogramOut.histogramData.size());


            if (result != SLAMTEC_AURORA_SDK_ERRORCODE_OK) {
                histogramOut.histogramData.clear();
                return false;
            }

            if (histogramOut.info.bin_total_count > histogramOut.histogramData.size()) {
                histogramSize = histogramOut.info.bin_total_count;
                continue;
            }

            histogramOut.histogramData.resize(histogramOut.info.bin_total_count);
            break;
        } while (1);
        return true;
    }


protected:
    FloorDetector(slamtec_aurora_sdk_session_handle_t& sdk)
        : _sdk(sdk)
    {
    }


    slamtec_aurora_sdk_session_handle_t _sdk;
};
/**
 * @brief The main class for the remote SDK
 * @details Caller can use this class to create a session and access the data from the remote device
 * @ingroup Session_Management Session Management
 */
class RemoteSDK : public Noncopyable
{
public:
    /**
     * @brief Get the SDK version info
     * @details Caller can use this function to get the SDK version info.
     * @param[out] info_out The SDK version info
     * @param[out] errcode The error code, set to nullptr if not interested
     * @return True if the version info is retrieved successfully, false otherwise
     */
    static bool GetSDKInfo(slamtec_aurora_sdk_version_info_t & info_out, slamtec_aurora_sdk_errorcode_t * errcode = nullptr) {
        auto result =  slamtec_aurora_sdk_get_version_info(&info_out);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    /**
     * @brief Create a session
     * @details Caller can use this function to create a session.
     * @param[in] listener The listener to listen the SDK events, set to nullptr if not interested
     * @param[in] config The SDK configuration, set to default if using default configuration
     * @param[out] error_code The error code, set to nullptr if not interested
     * @return The pointer to the created session, nullptr if failed
     */
    static RemoteSDK * CreateSession(const RemoteSDKListener * listener = nullptr, const SDKConfig & config = SDKConfig(), slamtec_aurora_sdk_errorcode_t* error_code = nullptr) {
        
        const slamtec_aurora_sdk_listener_t* rawListener = nullptr;
        if (listener) {
            rawListener = &listener->_sdk_listener_obj;
        }
        auto && handle = slamtec_aurora_sdk_create_session(&config, sizeof(config), rawListener, error_code);

        if (slamtec_aurora_sdk_is_valid_handle(handle)) {
            return new RemoteSDK(handle);
        }
        else {
            return nullptr;
        }
    }

    /**
     * @brief Destroy a session
     * @details Caller can use this function to destroy a session.
     * @param[in] session The pointer to the session to be destroyed
     */
    static void DestroySession(RemoteSDK* session) {
        delete session;
    }
    

public:
    /**
     * @brief Get the discovered servers
     * @details Caller can use this function to get the discovered servers. This is an alias of the getDiscoveredServers function in the controller.
     * @param[out] serverList The buffer to store the server list
     * @param[in] maxFetchCount The maximum number of servers to fetch
     * @return The number of discovered servers
     */
    size_t getDiscoveredServers(std::vector<SDKServerConnectionDesc>& serverList, size_t maxFetchCount = 32) {
        return controller.getDiscoveredServers(serverList, maxFetchCount);
    }

    // helper functions
    /**
     * @brief Connect to a server
     * @details Caller can use this function to connect to a server. This is an alias of the connect function in the controller.
     * @param[in] serverDesc The server description
     * @param[out] errCode The error code, set to nullptr if not interested
     * @return True if the connection is established successfully, false otherwise
     */
    bool connect(const SDKServerConnectionDesc& serverDesc, slamtec_aurora_sdk_errorcode_t * errCode = nullptr) {
        return controller.connect(serverDesc, errCode);
    }

    /**
     * @brief Disconnect from the server
     * @details Caller can use this function to disconnect from the server. This is an alias of the disconnect function in the controller.
     */
    void disconnect() {
        controller.disconnect();
    }

    /**
     * @brief Check if the session is connected
     * @details Caller can use this function to check if the session is connected. This is an alias of the isConnected function in the controller.
     * @return True if the session is connected, false otherwise
     */
    bool isConnected() {
        return controller.isConnected();
    }

    /**
     * @brief Start the background map data syncing
     * @details Caller can use this function to start the background map data syncing. This is an alias of the setMapDataSyncing function in the controller.
     */
    void startBackgroundMapDataSyncing() {
        return controller.setMapDataSyncing(true);
    }
    
    /**
     * @brief Stop the background map data syncing
     * @details Caller can use this function to stop the background map data syncing. This is an alias of the setMapDataSyncing function in the controller.
     */
    void stopBackgroundMapDataSyncing() {
        return controller.setMapDataSyncing(false);
    }

public:

    ~RemoteSDK() {
        slamtec_aurora_sdk_release_session(handle);
    }

    /**
     * @brief Release the session
     * @details Caller can use this function to release the session.
     */
    void release() {
        delete this;
    }

    /**
     * @brief The data provider class object
     */
    RemoteDataProvider dataProvider;
    /**
     * @brief The controller class object
     */
    RemoteController   controller;
    /**
     * @brief The map manager class object
     */
    RemoteMapManager   mapManager;


    /**
     * @brief The LIDAR 2D map builder class object
     */
    LIDAR2DMapBuilder lidar2DMapBuilder;


    /**
     * @brief The floor detector class object
     */
    FloorDetector floorDetector;
protected:


    slamtec_aurora_sdk_session_handle_t handle;
    

    RemoteSDK(slamtec_aurora_sdk_session_handle_t & obj)
        : handle(obj)
        , dataProvider(obj)
        , controller(obj)
        , mapManager(obj)
        , lidar2DMapBuilder(obj)
        , floorDetector(obj)
    {}


};

}}} // namespace rp::standalone::aurora

