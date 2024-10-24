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

#include "slamtec_remote_objects.hxx"


namespace rp { namespace standalone { namespace aurora { 

class Noncopyable {
protected:
    Noncopyable() = default;
    ~Noncopyable() = default;

    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;

    Noncopyable(Noncopyable&&) = default;
    Noncopyable& operator=(Noncopyable&&) = default;
};

class RemoteSDK;
class RemoteDataProvider;
class RemoteController;
class RemoteMapManager;


class RemoteSDKListener
{
    friend class RemoteSDK;
public:
    RemoteSDKListener() 
        : _listener_obj{ 0 }
    {
        _listener_obj.user_data = this;
        _binding();
    }
    virtual ~RemoteSDKListener() {}

    virtual void onTrackingData(const RemoteTrackingFrameInfo & info) {}
    virtual void onRawCamImageData(uint64_t timestamp_ns, const RemoteImageRef& left, const RemoteImageRef & right) {}
    virtual void onIMUData(const slamtec_aurora_sdk_imu_data_t* imuBuffer, size_t bufferCount) {}
    virtual void onNewMappingFlags(slamtec_aurora_sdk_mapping_flag_t flags) {}
    virtual void onDeviceStatusChanged(uint64_t timestamp_ns, slamtec_aurora_sdk_device_status_t status) {}

private:

    void _binding() {
        _listener_obj.on_raw_image_data = [](void* user_data, uint64_t timestamp_ns, const slamtec_aurora_sdk_image_desc_t* left_desc, const void* left_data, const slamtec_aurora_sdk_image_desc_t* right_desc, const void* right_data) {
            RemoteSDKListener* This = reinterpret_cast<RemoteSDKListener *>(user_data);

            RemoteImageRef left(*left_desc, left_data);
            RemoteImageRef right(*right_desc, right_data);

            This->onRawCamImageData(timestamp_ns, left, right);
        };

        _listener_obj.on_tracking_data = [](void* user_data, const slamtec_aurora_sdk_tracking_info_t* tracking_data, const slamtec_aurora_sdk_tracking_data_buffer_t* provided_buffer_info) {
            RemoteSDKListener* This = reinterpret_cast<RemoteSDKListener*>(user_data);

            RemoteTrackingFrameInfo frameInfo(*tracking_data, *provided_buffer_info);

            This->onTrackingData(frameInfo);
        };

        _listener_obj.on_imu_data = [](void* user_data, const slamtec_aurora_sdk_imu_data_t* imu_data, size_t imu_data_count) {
            RemoteSDKListener* This = reinterpret_cast<RemoteSDKListener*>(user_data);

            This->onIMUData(imu_data, imu_data_count);
        };

        _listener_obj.on_mapping_flags = [](void* user_data, slamtec_aurora_sdk_mapping_flag_t flags) {
            RemoteSDKListener* This = reinterpret_cast<RemoteSDKListener*>(user_data);

            This->onNewMappingFlags(flags);
        };

        _listener_obj.on_device_status = [](void* user_data, uint64_t timestamp_ns,  slamtec_aurora_sdk_device_status_t status) {
            RemoteSDKListener* This = reinterpret_cast<RemoteSDKListener*>(user_data);

            This->onDeviceStatusChanged(timestamp_ns, status);
        };


    }

    slamtec_aurora_sdk_listener_t _listener_obj;
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

    void subscribeMapData(const MapDataCallback& mapDataCallback) {
        _mapDataCallback = (mapDataCallback);

        _visitor_obj.on_map_desc = [](void* user_data, const slamtec_aurora_sdk_map_desc_t* map_data) {
            RemoteMapDataVisitor* This = reinterpret_cast<RemoteMapDataVisitor*>(user_data);
            This->_mapDataCallback(*map_data);
            };
    }

    void subscribeKeyFrameData(const KeyFrameDataCallback& keyFrameDataCallback) {
        _keyFrameDataCallback = (keyFrameDataCallback);
        _visitor_obj.on_keyframe = [](void* user_data, const slamtec_aurora_sdk_keyframe_desc_t* keyframe_data, const uint64_t * lcIDs, const uint64_t * connIDs) {
            RemoteMapDataVisitor* This = reinterpret_cast<RemoteMapDataVisitor*>(user_data);
            RemoteKeyFrameData kframeData(*keyframe_data, lcIDs, connIDs);
            
            This->_keyFrameDataCallback(kframeData);
            };
    }

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


class SDKConfig : public slamtec_aurora_sdk_session_config_t
{
    
public:
    SDKConfig() : slamtec_aurora_sdk_session_config_t() {
        memset(this, 0, sizeof(slamtec_aurora_sdk_session_config_t));
    }
};




class RemoteController : public Noncopyable {
    friend class RemoteSDK;
public:
    size_t getDiscoveredServers(std::vector<SDKServerConnectionDesc>& serverList, size_t maxFetchCount = 32)
    {
        serverList.resize(maxFetchCount);

        auto count = slamtec_aurora_sdk_controller_get_discovered_servers(_sdk, serverList.data(), maxFetchCount);
        serverList.resize(count);
        return count;
    }

    bool connect(const SDKServerConnectionDesc& serverDesc, slamtec_aurora_sdk_errorcode_t * errCode = nullptr) {
       

        auto result = slamtec_aurora_sdk_controller_connect(_sdk, &serverDesc);
        if (errCode) {
            *errCode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    void disconnect() {
        slamtec_aurora_sdk_controller_disconnect(_sdk);
    }

    bool isConnected() {
        return slamtec_aurora_sdk_controller_is_connected(_sdk) != 0;
    }

    void setLowRateMode(bool enable) {
        slamtec_aurora_sdk_controller_set_low_rate_mode(_sdk, enable ? 1 : 0);
    }

    void setMapDataSyncing(bool enable) {
        slamtec_aurora_sdk_controller_set_map_data_syncing(_sdk, enable ? 1 : 0);
    }

    void resyncMapData(bool invalidateCache = false) {
        slamtec_aurora_sdk_controller_resync_map_data(_sdk, invalidateCache ? 1 : 0);
    }

    void setRawDataSubscription(bool enable) {
        slamtec_aurora_sdk_controller_set_raw_data_subscription(_sdk, enable ? 1 : 0);
    }

    bool isRawDataSubscribed() {
        return slamtec_aurora_sdk_controller_is_raw_data_subscribed(_sdk) != 0;
    }

    bool requireMapReset(uint64_t timeout_ms = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_TIMEOUT, slamtec_aurora_sdk_errorcode_t * errcode = nullptr) {
        auto result =  slamtec_aurora_sdk_controller_require_map_reset(_sdk, timeout_ms);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    bool requirePureLocalizationMode(uint64_t timeout_ms = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_TIMEOUT, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        auto result = slamtec_aurora_sdk_controller_require_pure_localization_mode(_sdk, timeout_ms);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    bool requireMappingMode(uint64_t timeout_ms = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_TIMEOUT, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        auto result = slamtec_aurora_sdk_controller_require_mapping_mode(_sdk, timeout_ms);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }


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


class RemoteMapManager : public Noncopyable {
    friend class RemoteSDK;
public:
    // NOTE: the sdk will enter low rate mode during the working session to reduce the data traffic
    //       the low rate mode will be automatically disabled after the map streaming operation is done


    bool startUploadSession(const char* mapfilePath, slamtec_aurora_sdk_mapstorage_session_result_callback_t resultCallBack = nullptr, void * userData = nullptr,  slamtec_aurora_sdk_errorcode_t* errCode = nullptr) {
        return startSession(mapfilePath, SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_TYPE_UPLOAD, resultCallBack, userData, errCode);
    }

    bool startDownloadSession(const char* mapfilePath, slamtec_aurora_sdk_mapstorage_session_result_callback_t resultCallBack = nullptr, void* userData = nullptr, slamtec_aurora_sdk_errorcode_t* errCode = nullptr) {
        return startSession(mapfilePath, SLAMTEC_AURORA_SDK_MAPSTORAGE_SESSION_TYPE_DOWNLOAD, resultCallBack, userData, errCode);
    }

    bool startSession(const char* mapfilePath, slamtec_aurora_sdk_mapstorage_session_type_t sessionType, slamtec_aurora_sdk_mapstorage_session_result_callback_t resultCallBack = nullptr, void* userData = nullptr, slamtec_aurora_sdk_errorcode_t* errCode = nullptr) {
        auto result = slamtec_aurora_sdk_mapmanager_start_storage_session(_sdk, mapfilePath, sessionType, resultCallBack, userData);
        if (errCode) {
            *errCode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    bool isSessionActive() {
        return slamtec_aurora_sdk_mapmanager_is_storage_session_active(_sdk) != 0;
    }

    void abortSession() {
        slamtec_aurora_sdk_mapmanager_is_storage_abort_session(_sdk);
    }

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



class RemoteDataProvider : public Noncopyable {
    friend class RemoteSDK;
public:

    bool getCurrentPoseSE3(slamtec_aurora_sdk_pose_se3_t& poseOut, slamtec_aurora_sdk_errorcode_t * errcode = nullptr) {
        auto result = slamtec_aurora_sdk_dataprovider_get_current_pose_se3(_sdk, &poseOut);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    bool getCurrentPose(slamtec_aurora_sdk_pose_t& poseOut, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        auto result = slamtec_aurora_sdk_dataprovider_get_current_pose(_sdk, &poseOut);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    bool getMappingFlags(slamtec_aurora_sdk_mapping_flag_t& flagsOut, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        auto result = slamtec_aurora_sdk_dataprovider_get_mapping_flags(_sdk, &flagsOut);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    bool getLastDeviceStatus(slamtec_aurora_sdk_device_status_t& statusOut, uint64_t & timestamp_ns, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        auto result = slamtec_aurora_sdk_dataprovider_get_last_device_status(_sdk, &statusOut, &timestamp_ns);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

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

    bool getIMUInfo(slamtec_aurora_sdk_imu_info_t& infoOut, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        auto result = slamtec_aurora_sdk_dataprovider_get_imu_info(_sdk, &infoOut);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    bool getGlobalMappingInfo(slamtec_aurora_sdk_global_map_desc_t& descOut, slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        auto result = slamtec_aurora_sdk_dataprovider_get_global_mapping_info(_sdk, &descOut);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

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

    bool accessMapData(const RemoteMapDataVisitor& visitor, std::vector<uint32_t> mapIDs = std::vector<uint32_t>() , slamtec_aurora_sdk_errorcode_t* errcode = nullptr) {
        return accessMapData(visitor._visitor_obj, mapIDs, errcode);
    }

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

class RemoteSDK : public Noncopyable
{
public:
    static bool GetSDKInfo(slamtec_aurora_sdk_version_info_t & info_out, slamtec_aurora_sdk_errorcode_t * errcode = nullptr) {
        auto result =  slamtec_aurora_sdk_get_version_info(&info_out);
        if (errcode) {
            *errcode = result;
        }
        return result == SLAMTEC_AURORA_SDK_ERRORCODE_OK;
    }

    static RemoteSDK * CreateSession(const RemoteSDKListener * listener = nullptr, const SDKConfig & config = SDKConfig(), slamtec_aurora_sdk_errorcode_t* error_code = nullptr) {
        
        const slamtec_aurora_sdk_listener_t* rawListener = nullptr;
        if (listener) {
            rawListener = &listener->_listener_obj;
        }
        auto && handle = slamtec_aurora_sdk_create_session(&config, sizeof(config), rawListener, error_code);

        if (slamtec_aurora_sdk_is_valid_handle(handle)) {
            return new RemoteSDK(handle);
        }
        else {
            return nullptr;
        }
    }

    static void DestroySession(RemoteSDK* session) {
        delete session;
    }
    
public:
    size_t getDiscoveredServers(std::vector<SDKServerConnectionDesc>& serverList, size_t maxFetchCount = 32) {
        return controller.getDiscoveredServers(serverList, maxFetchCount);
    }

    // helper functions
    bool connect(const SDKServerConnectionDesc& serverDesc, slamtec_aurora_sdk_errorcode_t * errCode = nullptr) {
        return controller.connect(serverDesc, errCode);
    }

    void disconnect() {
        controller.disconnect();
    }

    bool isConnected() {
        return controller.isConnected();
    }

    void startBackgroundMapDataSyncing() {
        return controller.setMapDataSyncing(true);
    }
    
    void stopBackgroundMapDataSyncing() {
        return controller.setMapDataSyncing(false);
    }

public:

    ~RemoteSDK() {
        slamtec_aurora_sdk_release_session(handle);
    }


    void release() {
        delete this;
    }

    RemoteDataProvider dataProvider;
    RemoteController   controller;
    RemoteMapManager   mapManager;
protected:


    slamtec_aurora_sdk_session_handle_t handle;
    

    RemoteSDK(slamtec_aurora_sdk_session_handle_t & obj)
        : handle(obj)
        , dataProvider(obj)
        , controller(obj)
        , mapManager(obj)
    {}


};

}}} // namespace rp::standalone::aurora

