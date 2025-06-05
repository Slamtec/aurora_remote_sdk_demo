# 深度相机视图演示

![depthcam_view](../../res/demo_depthcam.gif)

本演示展示了 Aurora SDK 2.0 的增强成像功能，特别是深度相机功能。它演示了如何从配备深度相机的 Aurora 设备捕获、可视化和保存 3D 点云数据。

## 功能特性

- **实时深度可视化**：使用颜色映射显示深度图，提供更好的可视化效果
- **纹理叠加**：显示深度数据与校正后相机图像的叠加效果
- **3D 点云导出**：将当前帧保存为彩色 3D 点云的 PLY 格式文件
- **自动设备发现**：自动发现并连接 Aurora 设备
- **帧率监控**：实时 FPS 显示以进行性能监控

## 系统要求

- 支持深度相机的 Aurora 设备
- OpenCV 4.2 或更高版本
- Aurora Remote SDK 2.0+
- 与 Aurora 设备的稳定网络连接（推荐使用以太网以获得最佳性能）

## 编译

当 OpenCV 可用时，演示程序会自动编译：

```bash
cd build
cmake ..
make depthcam_view
```

## 使用方法

### 基本使用

```bash
./depthcam_view [连接字符串]
```

- 如果未提供连接字符串，演示程序将自动发现 Aurora 设备
- 演示程序将连接到第一个发现的设备

### 手动设备连接

```bash
./depthcam_view "tcp://192.168.1.100:1445"
```

请将 IP 地址替换为您的 Aurora 设备的 IP 地址。

### 控制方式

- **ESC**：退出应用程序
- **'s'**：将当前帧保存为 3D 点云（PLY 格式）
- **Ctrl+C**：优雅关闭

## 输出

### 可视化显示

演示程序打开两个窗口：

1. **深度图**：使用 COLORMAP_JET 的颜色映射深度可视化
   - 蓝色/紫色：近距离物体
   - 绿色/黄色：中等距离
   - 红色：远距离物体

2. **深度图（叠加）**：深度数据与校正后相机图像的叠加
   - 为深度信息提供空间上下文
   - 有助于理解场景几何结构

### 点云文件

按下 's' 键时，演示程序保存 3D 点云文件：

- **格式**：PLY（多边形文件格式）- ASCII 编码
- **文件名**：`pointcloud_YYYYMMDD_HHMMSS.ply`
- **内容**：带有 RGB 颜色信息的 3D 坐标（X, Y, Z）
- **坐标系**：相机坐标系（Z 向前，X 向右，Y 向下）

#### PLY 文件结构

```
ply
format ascii 1.0
element vertex [点数量]
property float x
property float y
property float z
property uchar red
property uchar green
property uchar blue
end_header
[点数据...]
```

## 查看点云

生成的 PLY 文件可以使用各种 3D 可视化软件查看：

- **MeshLab**：免费的跨平台网格处理工具
- **CloudCompare**：点云处理软件
- **PCL Viewer**：点云库查看器
- **Blender**：3D 建模软件（导入 PLY 文件）
- **ParaView**：科学数据可视化软件

## 技术细节

### 深度相机帧类型

演示程序使用两种增强成像帧类型：

1. **SLAMTEC_AURORA_SDK_DEPTHCAM_FRAME_TYPE_DEPTH_MAP**：
   - 用于可视化的原始深度值
   - 用于颜色映射深度显示

2. **SLAMTEC_AURORA_SDK_DEPTHCAM_FRAME_TYPE_POINT3D**：
   - 使用设备校准预计算的 3D 点
   - 用于点云生成和导出

### 点云生成

演示程序提供两种实现方法（通过预处理器指令选择）：

1. **基于 OpenCV**（默认，`#else` 分支）：
   - 使用 `RemoteImageRef::toMat()` 获取 OpenCV Mat
   - 遍历 3D 点的 2D 数组
   - 对 OpenCV 用户更熟悉

2. **直接 SDK 访问**（可选，`#if 0` 分支）：
   - 使用 `RemoteImageRef::toPoint3D()` 进行直接数组访问
   - 对于大型点云可能更高效
   - 更低级的 SDK 访问

### 数据处理

- **无效点过滤**：移除 NaN、无穷大和零深度点
- **颜色映射**：将纹理图像颜色映射到 3D 点
- **距离过滤**：可自定义过滤超过特定距离的点
- **内存优化**：高效的点云生成和存储

## 错误处理

演示程序包含全面的错误处理：

- 设备连接失败
- 帧获取超时
- 无效深度数据检测
- 点云保存的文件 I/O 错误
- 中断时的优雅关闭

## 性能考虑

- **网络带宽**：深度相机数据可能消耗大量带宽
- **处理能力**：实时深度处理需要足够的 CPU
- **内存使用**：大型点云可能需要大量 RAM
- **帧率**：典型运行在 10-30 FPS，取决于硬件

## 故障排除

### 常见问题

1. **"不支持深度相机"**
   - 确保您的 Aurora 设备具有深度相机功能
   - 检查固件版本兼容性

2. **"订阅深度相机帧失败"**
   - 验证网络连接稳定性
   - 检查是否有其他应用程序正在使用设备

3. **"当前帧中未找到有效点"**
   - 确保充足的照明条件
   - 检查物体是否在深度相机范围内
   - 验证设备定位和校准

4. **点云质量差**
   - 改善照明条件
   - 确保物体有足够的纹理
   - 检查设备校准状态

### 调试信息

演示程序提供控制台输出：
- 设备发现和连接状态
- 帧获取率（FPS）
- 点云生成进度
- 文件保存确认
- 带有特定错误代码的错误消息

## 示例输出

```
Device connection string not provided, try to discover aurora devices...
Waiting for aurora devices...
Found 1 aurora devices
Device 0
  option 0: tcp://192.168.1.100:1445
Selected first device:
Connecting to the selected device...
Connected to the selected device
Depth camera is supported
Controls: ESC to exit, 's' to save current frame as point cloud
Depth camera FPS: 14.9701 | Frame: 30
Generating point cloud...
Point cloud saved to pointcloud_20231225_143052.ply (45234 points)
```

## 集成说明

此演示可作为以下应用的基础：
- 3D 扫描应用程序
- 机器人导航系统
- 增强现实应用程序
- 物体识别和测量
- 场景重建项目

代码演示了 Aurora SDK 2.0 增强成像使用的最佳实践，可根据特定应用需求进行调整。
