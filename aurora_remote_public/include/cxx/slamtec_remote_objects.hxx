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

#include <type_traits>

#pragma once

namespace cv {
    class Mat; // in case of opencv
}

namespace rp { namespace standalone { namespace aurora { 

class SDKConnectionInfo : public slamtec_aurora_sdk_connection_info_t
{
public:
    SDKConnectionInfo() : slamtec_aurora_sdk_connection_info_t() {
        memset(this, 0, sizeof(slamtec_aurora_sdk_connection_info_t));
    }

    SDKConnectionInfo(const char* ip, int port = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PORT, const char * proto = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PROTOCOL) : SDKConnectionInfo() {
        snprintf(this->address, sizeof(this->address), "%s", ip);
        this->port = port;
        snprintf(this->protocol_type, sizeof(this->protocol_type), "%s", proto);
    }

    SDKConnectionInfo(const SDKConnectionInfo& other) : SDKConnectionInfo() {
        memcpy(this, &other, sizeof(SDKConnectionInfo));
    }

    SDKConnectionInfo& operator=(const SDKConnectionInfo& other) {
        memcpy(this, &other, sizeof(SDKConnectionInfo));
        return *this;
    }

    SDKConnectionInfo(const slamtec_aurora_sdk_connection_info_t& other) : SDKConnectionInfo() {
        memcpy(this, &other, sizeof(slamtec_aurora_sdk_connection_info_t));
    }


    // with the format "protocol/ip:port"
    std::string toLocatorString() const {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "%s/%s:%d", protocol_type, address, port);
        return std::string(buffer);
    }

    // with the format "[protocol/]ip[:port]"
    bool fromLocatorString(const char* input) {
        std::string protocol;
        std::string ip;
        uint16_t tport = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PORT;

        std::string inputWrapper = input;

        size_t protocol_pos = inputWrapper.find('/');
        if (protocol_pos != std::string::npos) {
            protocol = inputWrapper.substr(0, protocol_pos);
        }
        else {
            protocol = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PROTOCOL;
            protocol_pos = -1;
        }

        size_t port_pos = inputWrapper.rfind(':');
        if (port_pos != std::string::npos) {
            ip = inputWrapper.substr(protocol_pos + 1, port_pos - protocol_pos - 1);
            auto&& portStr = inputWrapper.substr(port_pos + 1);
            tport = (uint16_t)std::stoi(portStr);
        }
        else {
            ip = inputWrapper.substr(protocol_pos + 1);
        }

        snprintf(protocol_type, sizeof(protocol_type), "%s", protocol.c_str());
        snprintf(address, sizeof(address), "%s", ip.c_str());
        this->port = tport;

        return true;
    }
};


class SDKServerConnectionDesc : public slamtec_aurora_sdk_server_connection_info_t
{
public:
    SDKServerConnectionDesc() : slamtec_aurora_sdk_server_connection_info_t() {
        memset(this, 0, sizeof(slamtec_aurora_sdk_server_connection_info_t));
    }

    SDKServerConnectionDesc(const slamtec_aurora_sdk_server_connection_info_t & info)
    {
        memcpy(this, &info, sizeof(slamtec_aurora_sdk_server_connection_info_t));
    }

    SDKServerConnectionDesc(const SDKServerConnectionDesc& other) : SDKServerConnectionDesc() {
        memcpy(this, &other, sizeof(SDKServerConnectionDesc));
    }

    SDKServerConnectionDesc(const std::vector<SDKConnectionInfo>& src) {
        memset(this, 0, sizeof(slamtec_aurora_sdk_server_connection_info_t));
        for (auto&& info : src) {
            push_back(info);
        }
    }


    SDKServerConnectionDesc(const char* ip, int port = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PORT, const char* proto = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PROTOCOL)
    {
        memset(this, 0, sizeof(slamtec_aurora_sdk_server_connection_info_t));
        push_back(SDKConnectionInfo(ip, port, proto));
    }

    SDKServerConnectionDesc& operator=(const SDKServerConnectionDesc& other) {
        memcpy(this, &other, sizeof(SDKServerConnectionDesc));
        return *this;
    }

    std::vector<SDKConnectionInfo> toVector() const {
        std::vector<SDKConnectionInfo> result;
        for (size_t i = 0; i < connection_count; i++) {
            result.push_back(connection_info[i]);
        }
        return result;
    }

    SDKServerConnectionDesc& operator=(const std::vector<SDKConnectionInfo>& src) {
        memset(this, 0, sizeof(slamtec_aurora_sdk_server_connection_info_t));
        for (auto&& info : src) {
            push_back(info);
        }
        return *this;
    }


    size_t size() const {
        return connection_count;
    }

    size_t capacity() const {
        return sizeof(connection_info) / sizeof(connection_info[0]);
    }

    void clear() {
        connection_count = 0;
    }

    bool push_back(const slamtec_aurora_sdk_connection_info_t& info) {
        if (connection_count >= capacity()) {
            return false;
        }

        connection_info[connection_count++] = info;
        return true;
    }

    void pop_back() {
        if (connection_count > 0) {
            connection_count--;
        }
    }

    const SDKConnectionInfo& operator[](size_t index) const {
        return *(const SDKConnectionInfo *) ( & connection_info[index]);
    }


    const SDKConnectionInfo& at(size_t index) const {
        return *(const SDKConnectionInfo*)(&connection_info[index]);
    }


};

class RemoteImageRef {
public:
    
    RemoteImageRef(const slamtec_aurora_sdk_image_desc_t& desc, const void * data)
        : _desc(desc), _data(data)
    {
    }

    const void* _data;
    const slamtec_aurora_sdk_image_desc_t & _desc;


public:

    template <typename T>
    typename std::enable_if<std::is_same<T, cv::Mat>::value, bool>::type
    toMat(T &mat)const {
        switch (_desc.format)
        {
        case 0: // mono
            mat = T(_desc.height, _desc.width, 0, (void*)_data, _desc.stride);
            break;
        case 1: // bgr
            mat = T(_desc.height, _desc.width, (2<<3), (void*)_data, _desc.stride);
            break;
        case 2: // rgba
            mat = T(_desc.height, _desc.width, (3<<3), (void*)_data, _desc.stride);
            break;
        default:
            mat = T();
            return false;
        }

        return true;
    }

};


class RemoteTrackingFrameInfo {
public:
    RemoteTrackingFrameInfo()
        : leftImage(trackingInfo.left_image_desc, nullptr)
        , rightImage(trackingInfo.right_image_desc, nullptr)
        , _keypoints_left(_keypoints_buffer_left.data())
        , _keypoints_right(_keypoints_buffer_rightf.data())
    {
        memset(&trackingInfo, 0, sizeof(slamtec_aurora_sdk_tracking_info_t));
    }

    RemoteTrackingFrameInfo(const slamtec_aurora_sdk_tracking_info_t& info, const slamtec_aurora_sdk_tracking_data_buffer_t & buffer)
        : trackingInfo(info)
        , leftImage(info.left_image_desc, buffer.imgdata_left)
        , rightImage(info.right_image_desc, buffer.imgdata_right)
        , _keypoints_left(buffer.keypoints_left)
        , _keypoints_right(buffer.keypoints_right)
    {
    }

    RemoteTrackingFrameInfo(const slamtec_aurora_sdk_tracking_info_t& info,
        std::vector<uint8_t>&& imgbuffer_left,
        std::vector<uint8_t>&& imgbuffer_right,
        std::vector< slamtec_aurora_sdk_keypoint_t>&& keypoints_buffer_left,
        std::vector< slamtec_aurora_sdk_keypoint_t>&& keypoints_buffer_right)
        : trackingInfo(info)
        , leftImage(info.left_image_desc, imgbuffer_left.data())
        , rightImage(info.right_image_desc, imgbuffer_right.data())
        , _keypoints_left(keypoints_buffer_left.data())
        , _keypoints_right(keypoints_buffer_right.data())
        , _imgbuffer_left(std::move(imgbuffer_left))
        , _imgbuffer_right(std::move(imgbuffer_right))
        , _keypoints_buffer_left(std::move(keypoints_buffer_left))
        , _keypoints_buffer_rightf(std::move(keypoints_buffer_right))
    {}

    RemoteTrackingFrameInfo(const RemoteTrackingFrameInfo& other) 
        : trackingInfo(other.trackingInfo)
        , leftImage(trackingInfo.left_image_desc, nullptr)
        , rightImage(trackingInfo.right_image_desc, nullptr)
        , _keypoints_left(nullptr)
        , _keypoints_right(nullptr)
    {
        _copyFrom(other);
    }

    RemoteTrackingFrameInfo(RemoteTrackingFrameInfo&& other)
        : trackingInfo(other.trackingInfo)
        , leftImage(trackingInfo.left_image_desc, nullptr)
        , rightImage(trackingInfo.right_image_desc, nullptr)
        , _keypoints_left(nullptr)
        , _keypoints_right(nullptr)
    {
        if (!other._isOwnBuffer()) {
            _copyFrom(other);
        }
        else {
            _moveFrom(other);
        }
    }


    RemoteTrackingFrameInfo& operator=(const RemoteTrackingFrameInfo& other) {
        trackingInfo = other.trackingInfo;
        _copyFrom(other);
        return *this;
    }

    RemoteTrackingFrameInfo& operator=(RemoteTrackingFrameInfo&& other) {
        trackingInfo = other.trackingInfo;
        if (!other._isOwnBuffer()) {
            _copyFrom(other);
        }
        else {
            _moveFrom(other);
        }
        return *this;
    }

    
    const slamtec_aurora_sdk_keypoint_t* getKeypointsLeftBuffer() const {
        return _keypoints_left;
    }

    const slamtec_aurora_sdk_keypoint_t* getKeypointsRightBuffer() const {
        return _keypoints_right;
    }

    size_t getKeypointsLeftCount() const {
        return trackingInfo.keypoints_left_count;
    }

    size_t getKeypointsRightCount() const {
        return trackingInfo.keypoints_right_count;
    }


public: 
    RemoteImageRef leftImage;
    RemoteImageRef rightImage;
  
    slamtec_aurora_sdk_tracking_info_t trackingInfo;
    
protected:
    bool _isOwnBuffer() const {
        return (_keypoints_left == _keypoints_buffer_left.data());
    }

    void _moveFrom(RemoteTrackingFrameInfo& other) {
        _imgbuffer_left = std::move(other._imgbuffer_left);
        _imgbuffer_right = std::move(other._imgbuffer_right);
        _keypoints_buffer_left = std::move(other._keypoints_buffer_left);
        _keypoints_buffer_rightf = std::move(other._keypoints_buffer_rightf);

        leftImage._data = _imgbuffer_left.data();
        rightImage._data = _imgbuffer_right.data();
        _keypoints_left = _keypoints_buffer_left.data();
        _keypoints_right = _keypoints_buffer_rightf.data();

    }

    void _copyFrom(const RemoteTrackingFrameInfo& other) {
        if (other.leftImage._data) {
            _imgbuffer_left.resize(other.trackingInfo.left_image_desc.data_size);
            memcpy(_imgbuffer_left.data(), other.leftImage._data, other.trackingInfo.left_image_desc.data_size);
            leftImage._data = _imgbuffer_left.data();
        }
        else {
            leftImage._data = nullptr;
            _imgbuffer_left.clear();
        }

        if (other.rightImage._data) {
            _imgbuffer_right.resize(other.trackingInfo.right_image_desc.data_size);
            memcpy(_imgbuffer_right.data(), other.rightImage._data, other.trackingInfo.right_image_desc.data_size);
            rightImage._data = _imgbuffer_right.data();
        }
        else {
            rightImage._data = nullptr;
            _imgbuffer_right.clear();
        }

        if (other._keypoints_left) {
            _keypoints_buffer_left.resize(other.trackingInfo.keypoints_left_count);
            memcpy(_keypoints_buffer_left.data(), other._keypoints_left, other.trackingInfo.keypoints_left_count * sizeof(slamtec_aurora_sdk_keypoint_t));
            _keypoints_left = _keypoints_buffer_left.data();
        }
        else {
            _keypoints_left = nullptr;
            _keypoints_buffer_left.clear();
        }

        if (other._keypoints_right) {
            _keypoints_buffer_rightf.resize(other.trackingInfo.keypoints_right_count);
            memcpy(_keypoints_buffer_rightf.data(), other._keypoints_right, other.trackingInfo.keypoints_right_count * sizeof(slamtec_aurora_sdk_keypoint_t));
            _keypoints_right = _keypoints_buffer_rightf.data();
        }
        else {
            _keypoints_right = nullptr;
            _keypoints_buffer_rightf.clear();
        }
    }


    const slamtec_aurora_sdk_keypoint_t* _keypoints_left;
    const slamtec_aurora_sdk_keypoint_t* _keypoints_right;


    std::vector<uint8_t> _imgbuffer_left;
    std::vector<uint8_t> _imgbuffer_right;
    std::vector< slamtec_aurora_sdk_keypoint_t> _keypoints_buffer_left;
    std::vector< slamtec_aurora_sdk_keypoint_t> _keypoints_buffer_rightf;

};


class RemoteKeyFrameData {
public:
    RemoteKeyFrameData() : desc{ 0 } {
    }

    RemoteKeyFrameData(const slamtec_aurora_sdk_keyframe_desc_t& desc, const uint64_t * lcIDs, const uint64_t * connIDs)
        : desc(desc)
    {
        if (lcIDs && desc.looped_frame_count) {
            loopedKeyFrameIDs.reserve(desc.looped_frame_count);
            loopedKeyFrameIDs.insert(loopedKeyFrameIDs.end(), lcIDs, lcIDs + desc.looped_frame_count);
        }

        if (connIDs && desc.connected_frame_count) {
            connectedKeyFrameIDs.reserve(desc.connected_frame_count);
            connectedKeyFrameIDs.insert(connectedKeyFrameIDs.end(), connIDs, connIDs + desc.connected_frame_count);
        }
    }

    RemoteKeyFrameData(const RemoteKeyFrameData& other) : desc(other.desc), loopedKeyFrameIDs(other.loopedKeyFrameIDs), connectedKeyFrameIDs(other.connectedKeyFrameIDs) {
    }

    RemoteKeyFrameData& operator=(const RemoteKeyFrameData& other) {
        desc = other.desc;
        loopedKeyFrameIDs = other.loopedKeyFrameIDs;
        connectedKeyFrameIDs = other.connectedKeyFrameIDs;
        return *this;
    }

    RemoteKeyFrameData(RemoteKeyFrameData&& other) : desc(other.desc), loopedKeyFrameIDs(std::move(other.loopedKeyFrameIDs)), connectedKeyFrameIDs(std::move(other.connectedKeyFrameIDs)) {
    }

    RemoteKeyFrameData& operator=(RemoteKeyFrameData&& other) {
        desc = other.desc;
        loopedKeyFrameIDs = std::move(other.loopedKeyFrameIDs);
        connectedKeyFrameIDs = std::move(other.connectedKeyFrameIDs);
        return *this;
    }

public:
    slamtec_aurora_sdk_keyframe_desc_t desc;
    std::vector<uint64_t> loopedKeyFrameIDs;
    std::vector<uint64_t> connectedKeyFrameIDs;
};




}}} // namespace rp::standalone::aurora