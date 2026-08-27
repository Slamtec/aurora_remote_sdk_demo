# SLAMTEC Aurora Remote SDK and Demo

[English Version](README.md)

![Aurora](res/aurora.device.png)

本代码库包含使用SLAMTEC Aurora Remote SDK的演示代码和项目框架。

## 先决条件
- SLAMTEC Aurora 设备
- 设备与主机之间的 WiFi 或以太网连接

## 构建先决条件
- CMake 3.12 或更高版本
- C++14 编译器 (gcc 7.5 或更高版本, MSVC 2019 或更高版本, clang 8 或更高版本)
- 纯 C 可以用任何支持 C11 的 C 编译器编译 (但无法使用SDK中C++的功能)
- OpenCV 4.2 或更高版本 (如果您想编译使用 OpenCV 的例子程序)


## 用于ROS
- SLAMTEC官方提供Aurora的ROS包装节点，支持[ROS](https://github.com/Slamtec/aurora_ros) 和 [ROS2](https://github.com/Slamtec/aurora_ros/tree/ros2)。
- 您可以在SLAMTEC Aurora网站上找到这些包装节点。([ROS Wrapper Nodes](https://developer.slamtec.com/docs/slamware/aurora-ros2-sdk/))
- 一些Aurora特有的功能在ROS包装节点中不受支持，例如自动楼层检测。
- 如果您想使用Aurora特有的功能，您需要使用Remote SDK。


## 您是否尝试过 SLAMTEC 官方工具?
我们强烈建议您首先使用 SLAMTEC 官方工具以获得更好的评估和开发体验。
- Aurora Remote App
![Aurora Remote](res/aurora.remote.png)

它们可以从以下链接下载:
- [SLAMTEC Aurora 网站](https://www.slamtec.com/cn/Aurora) 

## Remote SDK API 参考文档
- [Remote SDK API Reference](doc/html/index.html)
这是Remote SDK的API参考。它包含所有函数在Remote SDK中的函数原型、参数描述和返回值。


## 构建演示的步骤
1. 克隆仓库及其子模块：
    ```
    git clone --recurse-submodules https://github.com/Slamtec/aurora_remote_sdk_demo.git
   
2. （可选）安装使用OpenCV的演示所需的依赖：
  
   例如，在Ubuntu上：
   ```
   sudo apt-get install -y libopencv-dev
   ```

3. 使用CMake构建演示。

    ```
    # 导航到源代码目录
    mkdir build
    cd build
    cmake ..
    make
    ```
4. 运行演示。


## 如何在目标机器上部署SDK
- 预编译的库仅依赖于C++标准库，因此可以部署在任何支持C++14的机器上。
- 对于Linux平台，还需确保glibc版本为2.31或更高。
- 对于MacOS平台，仅支持M系列芯片的机器（M1/M2/M3等）。

## 关于演示程序

### 核心功能演示

#### simple_pose
- 基础演示，展示如何获取设备姿态（位置和方向）。
- [📖 详细说明](demo/simple_pose/README.zh-CN.md)

#### relocalization  
- 演示如何触发重定位以在已知地图中建立设备位置。
- [📖 详细说明](demo/relocalization/README.zh-CN.md)

#### pure_c_demo
- 不使用 C++ 包装器依赖的纯 C 实现。
- [📖 详细说明](demo/pure_c_demo/README.zh-CN.md)


### 增强成像演示（SDK 2.0）

#### depthcam_view
- 深度相机可视化，具有 3D 点云导出功能。
- 需要 OpenCV。
- [📖 详细说明](demo/depthcam_view/README.zh-CN.md)

#### semantic_segmentation
![Semantic Segmentation Demo](res/demo_sematic_seg.gif)
- 实时语义分割，具有交互式可视化和模型切换。
- 需要 OpenCV。
- [📖 详细说明](demo/semantic_segmentation/README.zh-CN.md)


### 传感器数据演示

#### imu_fetcher
- 实时 IMU（加速度计和陀螺仪）数据检索和显示。
- [📖 详细说明](demo/imu_fetcher/README.zh-CN.md)

#### frame_preview
![frame_preview](res/demo_tracking_prev_full.png)
- 捕获和显示跟踪帧和原始立体相机图像。
- 需要 OpenCV。
- [📖 详细说明](demo/frame_preview/README.zh-CN.md)

#### lidar_scan_plot
![lidar_scan_plot](res/demo.lidar.scan.rendering.gif)  
- 实时 LiDAR 扫描可视化，包含距离和质量数据。
- 需要 OpenCV。
- [📖 详细说明](demo/lidar_scan_plot/README.zh-CN.md)

### 地图和可视化演示

#### map_render
![map_render](res/demo_vertical_map.gif)
- 渲染 VSLAM 地图数据，包括关键帧和地图点。
- 需要 OpenCV。
- [📖 详细说明](demo/map_render/README.zh-CN.md)

#### lidar_2dmap_render
![lidar_2dmap_render](res/demo_lidar_2dmap.gif)
- 从 LiDAR 数据生成和可视化 2D 占用栅格地图。
- 需要 OpenCV。
- [📖 详细说明](demo/lidar_2dmap_render/README.zh-CN.md)

#### vslam_map_saveload
- 用于下载/上传 VSLAM 地图到/从设备的命令行工具。
- [📖 详细说明](demo/vslam_map_saveload/README.zh-CN.md)


### 校准和设备信息演示（SDK 2.0）

#### calibration_exporter
- 用于导出相机和变换校准数据的命令行工具。
- 需要 OpenCV。
- [📖 详细说明](demo/calibration_exporter/README.zh-CN.md)

#### device_info_monitor
- 设备基本信息和状态的实时监控。
- [📖 详细说明](demo/device_info_monitor/README.zh-CN.md)

### 数据集记录演示（SDK 2.0）

#### colmap_recorder
- 记录 COLMAP 兼容数据集用于运动结构恢复和 3D 重建。
- 可配置记录选项，包括图像质量、文件格式和处理参数。
- 支持立体录制、图像去畸变和各种输出格式。
- [📖 详细说明](demo/colmap_recorder/README.zh-CN.md)

### 高级功能（SDK 2.1）

#### time_sync
- Aurora 设备与客户端系统之间的软件时间同步。
- 亚毫秒级精度，用于准确的时间戳转换。
- 对于将 Aurora 数据与外部传感器或事件关联至关重要。
- [📖 详细说明](demo/time_sync/README.zh-CN.md) | [📚 教程](tutorials/TimeSync_Tutorial_CN.md)

#### pose_augmentation
- 使用 IMU 数据集成实现高频位姿输出（高达 200+ Hz）。
- 将位姿更新速率从典型的 10-15 Hz 提高到 200+ Hz。
- 适用于需要低延迟位姿估计的机器人导航和 AR/VR 应用。
- [📖 详细说明](demo/pose_augmentation/README.zh-CN.md) | [📚 教程](tutorials/PoseAugmentation_Tutorial_CN.md)

#### pose_covariance
- 检索位姿不确定性估计（位置和方向协方差）。
- 实时评估定位置信度和质量。
- 适用于自适应导航、质量监控和多传感器融合。
- [📖 详细说明](demo/pose_covariance/README.zh-CN.md) | [📚 教程](tutorials/PoseCovariance_Tutorial_CN.md)

### 设备配置和管理（SDK 2.1.1）

#### persistent_config
- 通过基于 JSON 的 API 管理设备配置条目。
- 枚举、获取、设置和重置设备配置。
- 支持将配置保存到文件或从文件加载。
- [📖 详细说明](demo/persistent_config/README.zh-CN.md)

#### transform_manager
- 管理设备上的坐标系变换（SE3 位姿）。
- 列出、获取、设置和重置坐标系之间的变换。
- 适用于校准调整和自定义坐标系定义。
- [📖 详细说明](demo/transform_manager/README.zh-CN.md)

#### camera_mask
- 管理静态相机遮罩，用于从 SLAM 处理中排除区域。
- 启用/禁用遮罩，上传/下载遮罩图像。
- 适用于过滤静态障碍物、反射表面或移动物体。
- 图像 I/O 操作需要 OpenCV。
- [📖 详细说明](demo/camera_mask/README.zh-CN.md)

#### dashcam_recorder（数据记录仪）
- 用于监控和控制设备数据记录仪的交互式控制台。
- 实时仪表板，显示录制状态、存储信息和会话管理。
- 启用/禁用录制、设置大小限制和管理录制会话。
- [📖 详细说明](demo/dashcam_recorder/README.zh-CN.md)

#### system_power（系统电源管理）
- 控制 Aurora 设备电源操作（重启和关机）。
- 执行电源操作前显示设备状态。
- 执行电源命令前需要安全确认。
- [📖 详细说明](demo/system_power/README.zh-CN.md)
