# Aurora Frame Preview Demo

![Frame Preview Demo](../../res/demo_tracking_prev_full.png)

This demo shows how to capture and display tracking frames and raw camera frames from Aurora devices using OpenCV visualization.

## Features

- **Real-time Frame Display**: Shows tracking frames and raw stereo camera images
- **Dual Camera Support**: Displays both left and right camera feeds
- **Tracking Visualization**: Renders tracking features and keypoints
- **Event-driven Processing**: Uses SDK listeners for efficient frame handling

## Requirements

- Aurora device with camera system
- Aurora Remote SDK
- OpenCV 4.2 or higher
- Network connection between host and Aurora device

## Usage

```bash
# Auto-discover and connect
./frame_preview

# Connect to specific device
./frame_preview tcp://192.168.1.100:8090
```

## Key Features

- **Tracking Frame Visualization**: Displays visual features used for SLAM
- **Raw Camera Frames**: Shows unprocessed stereo camera images
- **Real-time Updates**: Frame-rate visualization with minimal latency
- **Thread-safe Processing**: Uses mutex protection for concurrent data access

## Integration Example

```cpp
#include "aurora_pubsdk_inc.h"
#include <opencv2/opencv.hpp>

class FrameListener : public RemoteSDKListener {
public:
    void onTrackingData(const RemoteTrackingFrameInfo& info) override {
        // Process tracking frame data
        std::cout << "Tracking frame received" << std::endl;
    }
    
    void onRawCamImageData(uint64_t timestamp_ns, 
                          const RemoteImageRef& left, 
                          const RemoteImageRef& right) override {
        // Process raw camera images
        cv::Mat leftImage, rightImage;
        left.toMat(leftImage);
        right.toMat(rightImage);
        
        cv::imshow("Left Camera", leftImage);
        cv::imshow("Right Camera", rightImage);
    }
};
```

## Use Cases

- **Camera Calibration**: Visual verification of camera alignment
- **System Debugging**: Real-time monitoring of tracking performance
- **Development**: Visual feedback during SLAM algorithm development
- **Quality Assurance**: Verification of camera functionality