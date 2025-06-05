# Aurora 标定参数导出演示

本演示展示如何使用 Aurora SDK 2.0 从 Aurora 设备中检索和导出相机标定和变换标定参数。

## 功能特性

- **相机标定导出**: 检索和导出立体相机标定参数，包括：
  - 左右相机内参（焦距、主点、畸变系数）
  - 立体相机外参（相机间旋转和平移）
  - 图像分辨率和基线信息

- **变换标定导出**: 检索和导出设备变换标定，包括：
  - IMU 到相机的变换矩阵
  - 外部相机变换矩阵

- **多种输出格式**: 支持 XML 和 YAML 格式，兼容 OpenCV

- **命令行界面**: 完整的命令行支持，包含输出目录和格式选择选项

## 系统要求

- 支持 Aurora SDK 2.0 的 Aurora 设备
- OpenCV 4.2 或更高版本
- Aurora Remote SDK 2.0

## 使用方法

### 基本用法

```bash
# 自动发现设备并保存到当前目录为 XML 格式
./calibration_exporter

# 指定输出目录
./calibration_exporter -o ./calibration_data

# 保存为 YAML 格式
./calibration_exporter -f yml

# 连接到指定设备
./calibration_exporter tcp://192.168.1.100:8090
```

### 命令行选项

- `-h, --help`: 显示帮助信息
- `-o, --output <dir>`: 指定输出目录（默认：当前目录）
- `-f, --format <fmt>`: 输出格式 - xml 或 yml（默认：xml）

### 输出文件

演示生成以下标定文件：

1. **left_camera_calibration.xml/yml**: 左相机内参
2. **right_camera_calibration.xml/yml**: 右相机内参
3. **stereo_calibration.xml/yml**: 完整立体相机设置参数
4. **transform_calibration.xml/yml**: 设备变换标定数据

## 输出示例

```
Aurora Calibration Exporter
Output directory: ./calibration
Output format: xml
Connected to the selected device

=== Camera Calibration Summary ===
Left Camera:
  Resolution: 640x480
  Focal Length: fx=525.67, fy=526.34
  Principal Point: cx=320.12, cy=240.67
Right Camera:
  Resolution: 640x480
  Focal Length: fx=524.89, fy=525.78
  Principal Point: cx=319.45, cy=241.23
Stereo Parameters:
  Baseline: 120.5 mm

=== Transform Calibration Summary ===
IMU to Camera Translation: [0.025, 0.012, -0.008]
External Camera Translation: [0.000, 0.000, 0.000]
===================================

Left camera calibration exported to: ./calibration/left_camera_calibration.xml
Right camera calibration exported to: ./calibration/right_camera_calibration.xml
Stereo calibration exported to: ./calibration/stereo_calibration.xml
Transform calibration exported to: ./calibration/transform_calibration.xml

Calibration export completed successfully!
```

## 与 OpenCV 集成

导出的标定文件可以直接在 OpenCV 应用中加载：

```cpp
// 加载相机标定
cv::FileStorage fs("left_camera_calibration.xml", cv::FileStorage::READ);
cv::Mat cameraMatrix, distCoeffs;
fs["camera_matrix"] >> cameraMatrix;
fs["distortion_coefficients"] >> distCoeffs;
int imageWidth, imageHeight;
fs["image_width"] >> imageWidth;
fs["image_height"] >> imageHeight;
fs.release();

// 用于相机矫正、立体视觉等
```
