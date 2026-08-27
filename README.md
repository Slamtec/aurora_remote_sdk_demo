# SLAMTEC Aurora Remote SDK and Demo

([中文版点此](README.zh-CN.md))

![Aurora](res/aurora.device.png)


This repository contains the Demo code and project skeleton for using the SLAMTEC Aurora Remote SDK.


## Prerequisites
- SLAMTEC Aurora Device
- WiFi or Ethernet connection between the device and the host machine

## Prerequisites for Building
- CMake 3.12 or above
- C++14 Compiler (gcc 7.5 or above, MSVC 2019 or above, clang 8 or above)
- Pure C can be compiled with any C compiler that supports C11 (but C++ wrapper is not supported)
- OpenCV 4.2 or above (if you want to compile the demo that uses OpenCV)

## ROS Integration
- SLAMTEC provides Aurora ROS wrapper nodes for [ROS](https://github.com/Slamtec/aurora_ros) and [ROS2](https://github.com/Slamtec/aurora_ros/tree/ros2).
- You can find the wrapper nodes on SLAMTEC Aurora Website. ([ROS Wrapper Nodes](https://developer.slamtec.com/docs/slamware/aurora-ros2-sdk-en/))
- Some Aurora specific features are not supported in the ROS wrapper nodes, such as the auto floor detection.
- If you want to use the Aurora specific features, you need to use the Remote SDK.


## Have you tried the SLAMTEC Official Tool?
We are highly recommend you to use the SLAMTEC Official Tool first for better evaluation and development experience.
- Aurora Remote App
![Aurora Remote](res/aurora.remote.png)

They can be downloaded from the following links:
- [SLAMTEC Aurora Website](https://www.slamtec.com/en/Aurora) / ([中文版点此](https://www.slamtec.com/cn/Aurora))

## API Reference of the Remote SDK
- [Remote SDK API Reference](doc/html/index.html)
This is the API reference for the Remote SDK. It contains the function prototypes, parameter descriptions, and return values for all the functions in the Remote SDK.


## Steps to build the Demos
1. Clone the repository and its submodules:
    ```
    git clone --recurse-submodules https://github.com/Slamtec/aurora_remote_sdk_demo.git
   
2. (optional) Install the dependencies for the demo that uses OpenCV:
  
   e.g. on Ubuntu:
   ```
   sudo apt-get install -y libopencv-dev
   ```

3. Build the demos with CMake.

    ```
    # navigate to source directory
    mkdir build
    cd build
    cmake ..
    make
    ```
4. Run the demo.


## How to deploy the SDK on target machines
- The precompiled libraries only depend on the C++ standard library, so it can be deployed on any machine that supports C++14.
- For Linux platforms, also make sure the glibc version is 2.31 or above.
- For MacOS platforms, only apple silicon based machines (M1/M2/M3 etc.) are supported.

## About the Demos

### Core Functionality Demos

#### simple_pose
- Basic demo showing how to retrieve device pose (position and orientation).
- [📖 Detailed README](demo/simple_pose/README.md)

#### relocalization  
- Demonstrates how to trigger relocalization to establish device position in a known map.
- [📖 Detailed README](demo/relocalization/README.md)

#### pure_c_demo
- Pure C implementation without C++ wrapper dependencies.
- [📖 Detailed README](demo/pure_c_demo/README.md)

### Enhanced Imaging Demos (SDK 2.0)

#### depthcam_view
![depthcam_view](res/demo_depthcam.gif)
- Depth camera visualization with 3D point cloud export capabilities.
- Requires OpenCV.
- [📖 Detailed README](demo/depthcam_view/README.md)

#### semantic_segmentation
![Semantic Segmentation Demo](res/demo_sematic_seg.gif)
- Real-time semantic segmentation with interactive visualization and model switching.
- Requires OpenCV.
- [📖 Detailed README](demo/semantic_segmentation/README.md)

### Sensor Data Demos

#### imu_fetcher
- Real-time IMU (accelerometer and gyroscope) data retrieval and display.
- [📖 Detailed README](demo/imu_fetcher/README.md)

#### frame_preview
![frame_preview](res/demo_tracking_prev_full.png)
- Captures and displays tracking frames and raw stereo camera images.
- Requires OpenCV.
- [📖 Detailed README](demo/frame_preview/README.md)

#### lidar_scan_plot
![lidar_scan_plot](res/demo.lidar.scan.rendering.gif)  
- Real-time LiDAR scan visualization with distance and quality data.
- Requires OpenCV.
- [📖 Detailed README](demo/lidar_scan_plot/README.md)

### Mapping and Visualization Demos

#### map_render
![map_render](res/demo_vertical_map.gif)
- Renders VSLAM map data including keyframes and map points.
- Requires OpenCV.
- [📖 Detailed README](demo/map_render/README.md)

#### lidar_2dmap_render
![lidar_2dmap_render](res/demo_lidar_2dmap.gif)
- Generates and visualizes 2D occupancy grid maps from LiDAR data.
- Requires OpenCV.
- [📖 Detailed README](demo/lidar_2dmap_render/README.md)

#### vslam_map_saveload
- Command-line tool for downloading/uploading VSLAM maps to/from devices.
- [📖 Detailed README](demo/vslam_map_saveload/README.md)



### Calibration and Device Info Demos (SDK 2.0)

#### calibration_exporter
- Command-line tool for exporting camera and transform calibration data.
- Requires OpenCV.
- [📖 Detailed README](demo/calibration_exporter/README.md)

#### device_info_monitor
- Real-time monitoring of device basic information and status.
- [📖 Detailed README](demo/device_info_monitor/README.md)

### Dataset Recording Demos (SDK 2.0)

#### colmap_recorder
- Records COLMAP-compatible datasets for Structure-from-Motion and 3D reconstruction.
- Configurable recording options including image quality, file formats, and processing parameters.
- Supports stereo recording, image undistortion, and various output formats.
- [📖 Detailed README](demo/colmap_recorder/README.md)

### Advanced Features

#### time_sync
- Software-based time synchronization between Aurora device and client system.
- Sub-millisecond accuracy for accurate timestamp translation.
- Essential for correlating Aurora data with external sensors or events.
- [📖 Detailed README](demo/time_sync/README.md) | [📚 Tutorial](tutorials/TimeSync_Tutorial_EN.md)

#### pose_augmentation
- High-frequency pose output (up to 200+ Hz) using IMU data integration.
- Increases pose update rate from typical 10-15 Hz to 200+ Hz.
- Ideal for robotics navigation and AR/VR applications requiring low-latency pose estimates.
- [📖 Detailed README](demo/pose_augmentation/README.md) | [📚 Tutorial](tutorials/PoseAugmentation_Tutorial_EN.md)

#### pose_covariance
- Retrieve pose uncertainty estimates (position and orientation covariance).
- Assess localization confidence and quality in real-time.
- Useful for adaptive navigation, quality monitoring, and multi-sensor fusion.
- [📖 Detailed README](demo/pose_covariance/README.md) | [📚 Tutorial](tutorials/PoseCovariance_Tutorial_EN.md)

### Device Configuration and Management (SDK 2.1.1)

#### persistent_config
- Manage device configuration entries via JSON-based API.
- Enumerate, get, set, and reset device configurations.
- Supports saving/loading configuration to/from files.
- [📖 Detailed README](demo/persistent_config/README.md)

#### transform_manager
- Manage coordinate frame transforms (SE3 poses) on the device.
- List, get, set, and reset transforms between coordinate frames.
- Useful for calibration adjustments and custom coordinate frame definitions.
- [📖 Detailed README](demo/transform_manager/README.md)

#### camera_mask
- Manage static camera masks for excluding regions from SLAM processing.
- Enable/disable masking, upload/download mask images.
- Useful for filtering out static obstacles, reflective surfaces, or moving objects.
- Requires OpenCV for image I/O operations.
- [📖 Detailed README](demo/camera_mask/README.md)

#### dashcam_recorder (Datalogger)
- Interactive console for monitoring and controlling the device datalogger.
- Real-time dashboard with recording status, storage info, and session management.
- Enable/disable recording, set size limits, and manage recording sessions.
- [📖 Detailed README](demo/dashcam_recorder/README.md)

#### system_power
- Control Aurora device power operations (reboot and shutdown).
- Display device status before power operations.
- Safety confirmation required before executing power commands.
- [📖 Detailed README](demo/system_power/README.md)