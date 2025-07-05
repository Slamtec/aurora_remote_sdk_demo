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
#include <sstream>
#include <optional>

#pragma once

namespace cv {
    class Mat; // in case of opencv
}

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

/**
 * @brief The connection info class
 * @details This class is used to store the connection information.
 * @ingroup Cxx_Controller_Operations Controller Operations
 */
class SDKConnectionInfo : public slamtec_aurora_sdk_connection_info_t
{
public:
    SDKConnectionInfo() : slamtec_aurora_sdk_connection_info_t() {
        memset(this, 0, sizeof(slamtec_aurora_sdk_connection_info_t));
    }

    /**
     * @brief The constructor with IP address, port and protocol
     * @details This constructor is used to create a connection info with IP address, port and protocol.
     * @param[in] ip The IP address
     * @param[in] port The port
     * @param[in] proto The protocol
     */
    SDKConnectionInfo(const char* ip, int port = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PORT, const char * proto = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PROTOCOL) : SDKConnectionInfo() {
        snprintf(this->address, sizeof(this->address), "%s", ip);
        this->port = port;
        snprintf(this->protocol_type, sizeof(this->protocol_type), "%s", proto);
    }

    SDKConnectionInfo(const SDKConnectionInfo& other) {
        memcpy(this, &other, sizeof(SDKConnectionInfo));
    }

    SDKConnectionInfo& operator=(const SDKConnectionInfo& other) {
        memcpy(this, &other, sizeof(SDKConnectionInfo));
        return *this;
    }

    SDKConnectionInfo(const slamtec_aurora_sdk_connection_info_t& other) {
        memcpy(this, &other, sizeof(slamtec_aurora_sdk_connection_info_t));
    }


    /**
     * @brief Convert the connection info to a locator string
     * @details This function is used to convert the connection info to a locator string.
     * @details The format of the locator string is "[protocol/]ip[:port]".
     * @return The locator string
     */
    std::string toLocatorString() const {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "%s/%s:%d", protocol_type, address, port);
        return std::string(buffer);
    }

    /**
     * @brief Parse the connection info from a locator string
     * @details This function is used to parse the connection info from a locator string.
     * @details The format of the locator string is "[protocol/]ip[:port]".
     * @param[in] input The locator string
     * @return True if the parsing is successful, false otherwise
     */
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


/**
 * @brief The server connection description class
 * @details This class is used to store the server connection information.
 * @ingroup Cxx_Controller_Operations Session Management
 */
class SDKServerConnectionDesc : public slamtec_aurora_sdk_server_connection_info_t
{
public:
    SDKServerConnectionDesc() : slamtec_aurora_sdk_server_connection_info_t() {
        memset(this, 0, sizeof(slamtec_aurora_sdk_server_connection_info_t));
    }

    /**
     * @brief Create the server connection description with a server connection info structure
     * @details This constructor is used to create a server connection description with a server connection info structure.
     * @param[in] info The server connection info
     */
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

    /**
     * @brief Create the server connection description with one connection option with IP address, port and protocol
     * @details This constructor is used to create a server connection description with IP address, port and protocol.
     * @param[in] ip The IP address
     * @param[in] port The port
     * @param[in] proto The protocol
     */
    SDKServerConnectionDesc(const char* ip, int port = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PORT, const char* proto = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PROTOCOL)
    {
        memset(this, 0, sizeof(slamtec_aurora_sdk_server_connection_info_t));
        push_back(SDKConnectionInfo(ip, port, proto));
    }

    SDKServerConnectionDesc& operator=(const SDKServerConnectionDesc& other) {
        memcpy(this, &other, sizeof(SDKServerConnectionDesc));
        return *this;
    }

    /**
     * @brief Convert the server connection description to a vector of connection info
     * @details This function is used to convert the server connection description to a vector of connection info.
     * @return The vector of connection info
     */
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

    /**
     * @brief Get the count of the server connection description
     * @details This function is used to get the count of the server connection description.
     * @return The count of the server connection description
     */
    size_t size() const {
        return connection_count;
    }

    /**
     * @brief Get the capacity of the connection description this object can hold
     * @details This function is used to get the capacity of the connection description this object can hold.
     * @return The capacity of the connection description
     */
    size_t capacity() const {
        return sizeof(connection_info) / sizeof(connection_info[0]);
    }

    /**
     * @brief Clear the connection description
     * @details This function is used to clear the connection description.
     */
    void clear() {
        connection_count = 0;
    }

    /**
     * @brief Push a connection info to the server connection description
     * @details This function is used to push a connection info to the server connection description.
     * @param[in] info The connection info
     * @return True if the push is successful, false otherwise
     */
    bool push_back(const slamtec_aurora_sdk_connection_info_t& info) {
        if (connection_count >= capacity()) {
            return false;
        }

        connection_info[connection_count++] = info;
        return true;
    }

    /**
     * @brief Pop a connection info from the server connection description
     * @details This function is used to pop a connection info from the server connection description.
     */
    void pop_back() {
        if (connection_count > 0) {
            connection_count--;
        }
    }

    /**
     * @brief Get the connection info at the specified index
     * @details This function is used to get the connection info at the specified index.
     * @param[in] index The index
     * @return The connection info
     */
    const SDKConnectionInfo& operator[](size_t index) const {
        return *(const SDKConnectionInfo *) ( & connection_info[index]);
    }

    /**
     * @brief Get the connection info at the specified index
     * @details This function is used to get the connection info at the specified index.
     * @param[in] index The index
     * @return The connection info
     */
    const SDKConnectionInfo& at(size_t index) const {
        return *(const SDKConnectionInfo*)(&connection_info[index]);
    }


};


class RemoteDeviceBasicInfo : public slamtec_aurora_sdk_device_basic_info_t {
public:
    RemoteDeviceBasicInfo() : slamtec_aurora_sdk_device_basic_info_t() {
        memset(this, 0, sizeof(slamtec_aurora_sdk_device_basic_info_t));
    }

    RemoteDeviceBasicInfo(const slamtec_aurora_sdk_device_basic_info_t& other) {
        memcpy(this, &other, sizeof(slamtec_aurora_sdk_device_basic_info_t));
    }

    RemoteDeviceBasicInfo(const RemoteDeviceBasicInfo& other) {
        memcpy(this, &other, sizeof(slamtec_aurora_sdk_device_basic_info_t));
    }

    RemoteDeviceBasicInfo& operator=(const slamtec_aurora_sdk_device_basic_info_t& other) {
        memcpy(this, &other, sizeof(slamtec_aurora_sdk_device_basic_info_t));
        return *this;
    }

    RemoteDeviceBasicInfo& operator=(const RemoteDeviceBasicInfo& other) {
        memcpy(this, &other, sizeof(slamtec_aurora_sdk_device_basic_info_t));
        return *this;
    }

    std::string getDeviceSerialNumberString() const {
        std::string sn;
        sn.resize(32);

        for (int i = 0; i < 16; ++i)
        {
            snprintf(&sn[i * 2], 3, "%02X", device_sn[i]);
        }

        return sn;
    }

    std::string getDeviceModelString() const {
        std::stringstream ss;
        if (model_major == 0 && model_sub == 0) {
            ss << "A1M1";
        }
        else {
            
            ss << "A" << std::to_string(model_major) << "M" << std::to_string(model_sub);
        }

        if (model_revision) {
            ss << "-r" << std::to_string(model_revision);
        }
        return ss.str();
    }


    bool isSupportDepthCamera() const {
        return sensing_feature_bitmaps & SLAMTEC_AURORA_SDK_SENSING_FEATURE_BIT_STEREO_DENSE_DISPARITY;
    }

    bool isSupportSemanticSegmentation() const {
        return sensing_feature_bitmaps & SLAMTEC_AURORA_SDK_SENSING_FEATURE_BIT_SEMANTIC_SEGMENTATION;
    }

    bool isSupportCameraPreviewStream() const {
        return swfeature_bitmaps & SLAMTEC_AURORA_SDK_SW_FEATURE_BIT_CAMREA_PREVIEW_STREAM;
    }
    
};


/**
 * @brief The image reference class wraps the image description and data
 * @details This class is used to wrap the image description and data.
 * @ingroup Cxx_DataProvider_Operations Data Provider Operations
 */
class RemoteImageRef {
public:
    
    RemoteImageRef(const slamtec_aurora_sdk_image_desc_t& desc, const void * data)
        : _desc(desc), _data(data)
    {
    }

    /**
     * @brief The data pointer of the image
     */
    const void* _data;

    /*
    * @brief The description of the image
    */
    const slamtec_aurora_sdk_image_desc_t & _desc;


public:
    /**
     * @brief Convert the image to a cv::Mat object
     * @details This function is used to convert the image to a cv::Mat object. 
     * @details This function is only available when OpenCV headers are included.
     * @param[out] mat The cv::Mat object
     * @return True if the conversion is successful, false otherwise
     */
    template <typename T>
    typename std::enable_if<std::is_same<T, cv::Mat>::value, bool>::type
    toMat(T &mat)const {
        if (isEmpty()) {
            mat = T();
            return false;
        }

        switch (_desc.format)
        {
        case 0: // mono
            mat = T(_desc.height, _desc.width, 0, (void*)_data, _desc.stride);
            break;
        case 1: // bgr
            mat = T(_desc.height, _desc.width, 0x10, (void*)_data, _desc.stride);
            break;
        case 2: // rgba
            mat = T(_desc.height, _desc.width, 0x18, (void*)_data, _desc.stride);
            break;
        case 3: // depth (float32)
            mat = T(_desc.height, _desc.width, 5, (void*)_data, _desc.stride);
            break;
        case 4: // point3D (floatx3 XYZ)
            mat = T(_desc.height, _desc.width, 0x15, (void*)_data, _desc.stride);
            break;

            
        default:
            mat = T();
            return false;
        }

        return true;
    }

    /**
     * @brief Convert the image to a point cloud array
     * @details If the image format is not point3D, the function will return nullptr.
     * @return The point cloud array
     */
    const std::array<float, 3>* toPoint3D() const {
        if (_desc.format != 4) {
            return nullptr;
        }

        return reinterpret_cast<const std::array<float,3>*>(_data);
    }



    size_t getPointCount() const {
        return _desc.height * _desc.width;
    }


    bool isEmpty() const {
        return (_data == nullptr || _desc.height == 0 || _desc.width == 0);
    }

};

/**
 * @brief The tracking frame info class wraps the tracking information and its data
 * @details This class is used to wrap the tracking information and its data.
 * @ingroup Cxx_DataProvider_Operations Data Provider Operations
 */
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

    /**
     * @brief Create the tracking frame info with the tracking information and the tracking data buffer
     * @details This constructor is used to create the tracking frame info with the tracking information and the tracking data buffer.
     * @param[in] info The tracking information
     * @param[in] buffer The tracking data buffer
     */
    RemoteTrackingFrameInfo(const slamtec_aurora_sdk_tracking_info_t& info, const slamtec_aurora_sdk_tracking_data_buffer_t & buffer)
        : trackingInfo(info)
        , leftImage(info.left_image_desc, buffer.imgdata_left)
        , rightImage(info.right_image_desc, buffer.imgdata_right)
        , _keypoints_left(buffer.keypoints_left)
        , _keypoints_right(buffer.keypoints_right)
    {
    }

    /**
     * @brief Create the tracking frame info with the tracking information and the image data buffer
     * @details This constructor is used to create the tracking frame info with the tracking information and the image data buffer.
     * @details The image data buffer and the keypoints data buffer are moved from the input parameters.
     * @param[in] info The tracking information
     * @param[in] imgbuffer_left The left image data buffer
     * @param[in] imgbuffer_right The right image data buffer
     * @param[in] keypoints_buffer_left The left keypoints data buffer
     * @param[in] keypoints_buffer_right The right keypoints data buffer
     */
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

    /**
     * @brief Get the left keypoints buffer
     * @details This function is used to get the left keypoints buffer.
     * @return The left keypoints buffer
     */
    const slamtec_aurora_sdk_keypoint_t* getKeypointsLeftBuffer() const {
        return _keypoints_left;
    }

    /**
     * @brief Get the right keypoints buffer
     * @details This function is used to get the right keypoints buffer.
     * @return The right keypoints buffer
     */
    const slamtec_aurora_sdk_keypoint_t* getKeypointsRightBuffer() const {
        return _keypoints_right;
    }

    /**
     * @brief Get the left keypoints count
     * @details This function is used to get the left keypoints count.
     * @return The left keypoints count
     */
    size_t getKeypointsLeftCount() const {
        return trackingInfo.keypoints_left_count;
    }

    /**
     * @brief Get the right keypoints count
     * @details This function is used to get the right keypoints count.
     * @return The right keypoints count
     */
    size_t getKeypointsRightCount() const {
        return trackingInfo.keypoints_right_count;
    }

    void reset() {
        memset(&trackingInfo, 0, sizeof(slamtec_aurora_sdk_tracking_info_t));
        leftImage._data = nullptr;
        rightImage._data = nullptr;
        _keypoints_left = nullptr;
        _keypoints_right = nullptr;
        _imgbuffer_left.clear();
        _imgbuffer_right.clear();
        _keypoints_buffer_left.clear();
        _keypoints_buffer_rightf.clear();
    }



public: 

    /**
     * @brief The left image reference
     */
    RemoteImageRef leftImage;

    /**
     * @brief The right image reference
     */
    RemoteImageRef rightImage;
  
    /**
     * @brief The tracking information
     */
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


struct RemoteStereoImagePair {

public:
    RemoteStereoImagePair() 
        : leftImage(desc.left_image_desc, nullptr)
        , rightImage(desc.right_image_desc, nullptr) 
    {
        memset(&desc, 0, sizeof(slamtec_aurora_sdk_stereo_image_pair_desc_t));
    }

    RemoteStereoImagePair(const slamtec_aurora_sdk_stereo_image_pair_desc_t& desc, const slamtec_aurora_sdk_stereo_image_pair_buffer_t& buffer)
        : desc(desc)
        , leftImage(desc.left_image_desc, buffer.imgdata_left)
        , rightImage(desc.right_image_desc, buffer.imgdata_right)
    {
    }

    RemoteStereoImagePair(const slamtec_aurora_sdk_stereo_image_pair_desc_t & info,
        std::vector<uint8_t> && left_buffer,
        std::vector<uint8_t> && right_buffer
    )
        : desc(info)
        , leftImage(info.left_image_desc, left_buffer.data())
        , rightImage(info.right_image_desc, right_buffer.data())
        , _imgbuffer_left(std::move(left_buffer))
        , _imgbuffer_right(std::move(right_buffer))
    {
    }

    RemoteStereoImagePair(const RemoteStereoImagePair& other)
        : desc(other.desc)
        , leftImage(desc.left_image_desc, nullptr)
        , rightImage(desc.right_image_desc, nullptr)
    {
        _copyFrom(other);
    }

    RemoteStereoImagePair(RemoteStereoImagePair && other)
        : desc(other.desc)
        , leftImage(desc.left_image_desc, nullptr)
        , rightImage(desc.right_image_desc, nullptr)
    {
        if (!other._isOwnBuffer()) {
            _copyFrom(other);
        } else {
            _moveFrom(other);
        }
    }

    RemoteStereoImagePair& operator=(const RemoteStereoImagePair& other) {
        desc = other.desc;
        _copyFrom(other);
        return *this;
    }

    RemoteStereoImagePair& operator=(RemoteStereoImagePair && other) {
        desc = other.desc;
        if (!other._isOwnBuffer()) {
            _copyFrom(other);
        } else {
            _moveFrom(other);
        }
        return *this;
    }

public:
    RemoteImageRef leftImage;
    RemoteImageRef rightImage;
    slamtec_aurora_sdk_stereo_image_pair_desc_t desc;


protected:
    bool _isOwnBuffer() const {
        return (_imgbuffer_left.data() == leftImage._data);
    }

    void _moveFrom(RemoteStereoImagePair& other) {
        _imgbuffer_left = std::move(other._imgbuffer_left);
        _imgbuffer_right = std::move(other._imgbuffer_right);


        leftImage._data = _imgbuffer_left.data();
        rightImage._data = _imgbuffer_right.data();

    }

    void _copyFrom(const RemoteStereoImagePair& other) {
        if (other.leftImage._data) {
            _imgbuffer_left.resize(other.desc.left_image_desc.data_size);
            memcpy(_imgbuffer_left.data(), other.leftImage._data, other.desc.left_image_desc.data_size);
            leftImage._data = _imgbuffer_left.data();
        }
        else {
            leftImage._data = nullptr;
            _imgbuffer_left.clear();
        }

        if (other.rightImage._data) {
            _imgbuffer_right.resize(other.desc.right_image_desc.data_size);
            memcpy(_imgbuffer_right.data(), other.rightImage._data, other.desc.right_image_desc.data_size);
            rightImage._data = _imgbuffer_right.data();
        }
        else {
            rightImage._data = nullptr;
            _imgbuffer_right.clear();
        }
    }

protected:
    std::vector<uint8_t> _imgbuffer_left;
    std::vector<uint8_t> _imgbuffer_right;
};


struct RemoteEnhancedImagingFrame {

public:
    RemoteEnhancedImagingFrame() 
        : image(desc.image_desc, nullptr)
    {
        memset(&desc, 0, sizeof(slamtec_aurora_sdk_enhanced_imaging_frame_desc_t));
    }

    RemoteEnhancedImagingFrame(const slamtec_aurora_sdk_enhanced_imaging_frame_desc_t& desc, const slamtec_aurora_sdk_enhanced_imaging_frame_buffer_t& buffer)
        : desc(desc)
        , image(desc.image_desc, buffer.frame_data)
    {
    }

    RemoteEnhancedImagingFrame(const slamtec_aurora_sdk_enhanced_imaging_frame_desc_t & info,
        std::vector<uint8_t> && buffer
    )
        : desc(info)
        , image(info.image_desc, buffer.data())
        , _imgbuffer(std::move(buffer))
    {
    }

    RemoteEnhancedImagingFrame(const RemoteEnhancedImagingFrame& other)
        : desc(other.desc)
        , image(desc.image_desc, nullptr)
    {
        _copyFrom(other);
    }

    RemoteEnhancedImagingFrame(RemoteEnhancedImagingFrame && other)
        : desc(other.desc)
        , image(desc.image_desc, nullptr)
    {
        if (!other._isOwnBuffer()) {
            _copyFrom(other);
        } else {
            _moveFrom(other);
        }
    }

    RemoteEnhancedImagingFrame& operator=(const RemoteEnhancedImagingFrame& other) {
        desc = other.desc;
        _copyFrom(other);
        return *this;
    }

    RemoteEnhancedImagingFrame& operator=(RemoteEnhancedImagingFrame && other) {
        desc = other.desc;
        if (!other._isOwnBuffer()) {
            _copyFrom(other);
        } else {
            _moveFrom(other);
        }
        return *this;
    }

public:
    RemoteImageRef image;
    slamtec_aurora_sdk_enhanced_imaging_frame_desc_t desc;


protected:
    bool _isOwnBuffer() const {
        return (_imgbuffer.data() == image._data);
    }

    void _moveFrom(RemoteEnhancedImagingFrame& other) {
        _imgbuffer = std::move(other._imgbuffer);


        image._data = _imgbuffer.data();

    }

    void _copyFrom(const RemoteEnhancedImagingFrame& other) {
        if (other.image._data) {
            _imgbuffer.resize(other.desc.image_desc.data_size);
            memcpy(_imgbuffer.data(), other.image._data, other.desc.image_desc.data_size);
            image._data = _imgbuffer.data();
        }
        else {
            image._data = nullptr;
            _imgbuffer.clear();
        }

        if (other.image._data) {
            _imgbuffer.resize(other.desc.image_desc.data_size);
            memcpy(_imgbuffer.data(), other.image._data, other.desc.image_desc.data_size);
            image._data = _imgbuffer.data();
        }
        else {
            image._data = nullptr;
            _imgbuffer.clear();
        }
    }

protected:
    std::vector<uint8_t> _imgbuffer;
};


/**
 * @brief The keyframe data class wraps the keyframe description and its data
 * @details This class is used to wrap the keyframe description and its data.
 * @ingroup Cxx_DataProvider_Operations Data Provider Operations
 */
class RemoteKeyFrameData {
public:
    RemoteKeyFrameData() : desc{ 0 } {
    }

    RemoteKeyFrameData(const slamtec_aurora_sdk_keyframe_desc_t& desc, const uint64_t * lcIDs, const uint64_t * connIDs, const uint64_t * relatedMPIDs)
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

        if (relatedMPIDs && desc.related_mp_count) {
            relatedMapPointIDs.reserve(desc.related_mp_count);
            relatedMapPointIDs.insert(relatedMapPointIDs.end(), relatedMPIDs, relatedMPIDs + desc.related_mp_count);
        }
    }

    RemoteKeyFrameData(const RemoteKeyFrameData& other) : desc(other.desc), loopedKeyFrameIDs(other.loopedKeyFrameIDs), connectedKeyFrameIDs(other.connectedKeyFrameIDs), relatedMapPointIDs(other.relatedMapPointIDs) {
    }

    RemoteKeyFrameData& operator=(const RemoteKeyFrameData& other) {
        desc = other.desc;
        loopedKeyFrameIDs = other.loopedKeyFrameIDs;
        connectedKeyFrameIDs = other.connectedKeyFrameIDs;
        relatedMapPointIDs = other.relatedMapPointIDs;
        return *this;
    }

    RemoteKeyFrameData(RemoteKeyFrameData&& other) : desc(other.desc), loopedKeyFrameIDs(std::move(other.loopedKeyFrameIDs)), connectedKeyFrameIDs(std::move(other.connectedKeyFrameIDs)), relatedMapPointIDs(std::move(other.relatedMapPointIDs)) {
    }

    RemoteKeyFrameData& operator=(RemoteKeyFrameData&& other) {
        desc = other.desc;
        loopedKeyFrameIDs = std::move(other.loopedKeyFrameIDs);
        connectedKeyFrameIDs = std::move(other.connectedKeyFrameIDs);
        relatedMapPointIDs = std::move(other.relatedMapPointIDs);
        return *this;
    }

public:
    /**
     * @brief The keyframe description
     */
    slamtec_aurora_sdk_keyframe_desc_t desc;

    /**
     * @brief The looped keyframe IDs
     */
    std::vector<uint64_t> loopedKeyFrameIDs;

    /**
     * @brief The connected keyframe IDs
     */
    std::vector<uint64_t> connectedKeyFrameIDs;

    /**
     * @brief The related map point IDs
     */
    std::vector<uint64_t> relatedMapPointIDs;
};


/**
 * @brief The single layer LIDAR scan data class wraps the LIDAR scan data and its description
 * @details This class is used to wrap the LIDAR scan data and its description.
 * @ingroup Cxx_DataProvider_Operations Data Provider Operations
 */
class SingleLayerLIDARScan {
public:
    SingleLayerLIDARScan()  {
        memset(&info, 0, sizeof(slamtec_aurora_sdk_lidar_singlelayer_scandata_info_t));
    }

    SingleLayerLIDARScan(const SingleLayerLIDARScan& other) : info(other.info), scanData(other.scanData) {
    }

    SingleLayerLIDARScan& operator=(const SingleLayerLIDARScan& other) {
        info = other.info;
        scanData = other.scanData;
        return *this;
    }

    SingleLayerLIDARScan(SingleLayerLIDARScan&& other) : info(other.info), scanData(std::move(other.scanData)) {
    }

    SingleLayerLIDARScan& operator=(SingleLayerLIDARScan&& other) {
        info = other.info;
        scanData = std::move(other.scanData);
        return *this;
    }

public:
    /**
     * @brief The LIDAR scan data info
     */
    slamtec_aurora_sdk_lidar_singlelayer_scandata_info_t info;

    /**
     * @brief The LIDAR scan data
     */
    std::vector< slamtec_aurora_sdk_lidar_scan_point_t> scanData;

};

/**
 * @brief The 2D gridmap generation options class wraps the 2D gridmap generation options
 * @details This class is used to wrap the 2D gridmap generation options.
 * @ingroup Cxx_LIDAR_2DMap_Operations LIDAR 2D GridMap Operations
 */
class LIDAR2DGridMapGenerationOptions : public slamtec_aurora_sdk_2d_gridmap_generation_options_t {
public:
    LIDAR2DGridMapGenerationOptions()
    {
        memset(this, 0, sizeof(slamtec_aurora_sdk_2d_gridmap_generation_options_t));
        loadDefaults();
    }


    LIDAR2DGridMapGenerationOptions(const slamtec_aurora_sdk_2d_gridmap_generation_options_t& other) {
        memcpy(this, &other, sizeof(slamtec_aurora_sdk_2d_gridmap_generation_options_t));
    }



    LIDAR2DGridMapGenerationOptions(const LIDAR2DGridMapGenerationOptions& other) {
        memcpy(this, &other, sizeof(LIDAR2DGridMapGenerationOptions));
    }

    LIDAR2DGridMapGenerationOptions& operator=(const LIDAR2DGridMapGenerationOptions& other) {
        memcpy(this, &other, sizeof(LIDAR2DGridMapGenerationOptions));
        return *this;
    }


    
    /**
     * @brief Load the default 2D gridmap generation options
     */
    void loadDefaults() {
        this->resolution = SLAMTEC_AURORA_SDK_LIDAR_2D_GRIDMAP_DEFAULT_RESOLUTION;
        this->map_canvas_width = SLAMTEC_AURORA_SDK_LIDAR_2D_GRIDMAP_DEFAULT_WIDTH;
        this->map_canvas_height = SLAMTEC_AURORA_SDK_LIDAR_2D_GRIDMAP_DEFAULT_HEIGHT;
        this->active_map_only = 1;
        this->height_range_specified = 0;
    }

    /**
     * @brief Set the height range for the 2D gridmap generation
     * @param[in] minHeight The minimum height
     * @param[in] maxHeight The maximum height
     */
    void setHeightRange(float minHeight, float maxHeight) {
        this->height_range_specified = 1;
        this->min_height = minHeight;
        this->max_height = maxHeight;
    }

    /**
     * @brief Clear the height range for the 2D gridmap generation
     */
    void clearHeightRange() {
        this->height_range_specified = 0;
        this->min_height = 0;
        this->max_height = 0;
    }


};

/**
 * @brief The floor detection histogram class wraps the floor detection histogram data and its description
 * @details This class is used to wrap the floor detection histogram data and its description.
 * @ingroup Cxx_Auto_Floor_Detection_Operations LIDAR Auto Floor Detection Operations
 */
class FloorDetectionHistogram {
public:
    FloorDetectionHistogram() {
        memset(&info, 0, sizeof(slamtec_aurora_sdk_floor_detection_histogram_info_t));
    }

    FloorDetectionHistogram(const FloorDetectionHistogram& other) : info(other.info), histogramData(other.histogramData) {
    }

    FloorDetectionHistogram& operator=(const FloorDetectionHistogram& other) {
        info = other.info;
        histogramData = other.histogramData;
        return *this;
    }

    FloorDetectionHistogram(FloorDetectionHistogram&& other) : info(other.info), histogramData(std::move(other.histogramData)) {
    }

    FloorDetectionHistogram& operator=(FloorDetectionHistogram&& other)  {
        info = other.info;
        histogramData = std::move(other.histogramData);
        return *this;
    }

public:
    /**
     * @brief The floor detection histogram info
     */
    slamtec_aurora_sdk_floor_detection_histogram_info_t info;

    /**
     * @brief The floor detection histogram data
     */
    std::vector<float> histogramData;
};


}}} // namespace rp::standalone::aurora