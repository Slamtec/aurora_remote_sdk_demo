# 语义分割演示

![Semantic Segmentation Demo](../../res/demo_sematic_seg.gif)

本演示展示了 Aurora SDK 2.0 的增强成像功能，特别是语义分割功能。它演示了如何从配备基于 DNN 语义分割模型的 Aurora 设备捕获、可视化和交互语义分割数据。

## 功能特性

- **实时语义分割**：显示具有不同对象类别彩色可视化的分割图
- **交互式对象检测**：鼠标悬停显示对象轮廓和类别标签
- **模型切换**：在默认模型（80类）和候选模型（18类）之间切换
- **相机图像叠加**：在相机图像上叠加分割结果以提供空间上下文
- **深度相机对齐**：显示与深度相机坐标系对齐的分割图
- **自动设备发现**：自动发现并连接 Aurora 设备
- **帧率监控**：实时 FPS 显示以进行性能监控

## 系统要求

- 支持语义分割的 Aurora 设备
- OpenCV 4.2 或更高版本
- Aurora Remote SDK 2.0+
- 与 Aurora 设备的稳定网络连接（推荐使用以太网以获得最佳性能）

## 编译

当 OpenCV 可用时，演示程序会自动编译：

```bash
cd build
cmake ..
make semantic_segmentation
```

## 使用方法

### 基本使用

```bash
./semantic_segmentation [连接字符串]
```

- 如果未提供连接字符串，演示程序将自动发现 Aurora 设备
- 演示程序将连接到第一个发现的设备

### 手动设备连接

```bash
./semantic_segmentation "tcp://192.168.1.100:1445"
```

请将 IP 地址替换为您的 Aurora 设备的 IP 地址。

### 控制方式

- **ESC**：退出应用程序
- **'m'**：在默认模型和替代模型之间切换
- **鼠标悬停**：在分割图中显示对象轮廓和类别标签
- **Ctrl+C**：优雅关闭

## 输出

### 可视化显示

演示程序打开两个窗口：

1. **分割与相机叠加**：合并显示分割和相机数据
   - 相机图像背景与彩色分割叠加
   - 背景区域保持纯相机图像（透明分割）
   - 鼠标悬停以白色显示对象轮廓，响应速度极快
   - 悬停对象类别标签显示在顶部
   - 模型切换状态以彩色编码消息显示
   - 屏幕显示控制说明："'m' - Switch between default and alternative model"
   - 帧数和模型信息显示在底部

2. **深度对齐分割**：与深度相机坐标系对齐的分割图
   - 仅在支持深度相机的设备上可用
   - 对 3D 场景理解应用很有用

### 控制台输出

演示程序提供全面的控制台信息：

- 设备发现和连接状态
- 语义分割配置（FPS、帧跳过、图像大小）
- 标签集信息和类别名称
- 当前模型状态（默认/替代）
- 帧率监控
- 模型切换确认

## 技术细节

### 分割模型

Aurora 设备支持两种语义分割模型：

1. **默认模型**（通用室内应用）：
   - 80 个对象类别
   - 针对室内环境优化
   - 包括家具、电子设备、人员等

2. **替代模型**（户外应用）：
   - 18 个对象类别
   - 针对户外环境优化
   - 包括车辆、路标、建筑物等

### 对象类别

类别信息从设备动态检索：
- 类别 0 通常是背景（标记为"(null)"或"背景"）
- 类别名称和索引依赖于模型
- 完整类别列表在启动和模型切换时打印到控制台

### 交互功能

#### 鼠标悬停检测
- 将鼠标移动到合并的分割与相机叠加窗口上以突出显示对象
- 对象轮廓使用线段手动绘制，提高可靠性
- 背景区域（null类别）悬停时不显示轮廓
- 类别标签出现在窗口顶部，带有颜色编码（对象为黄色，背景为灰色）
- 实时对象识别，具有稳健的轮廓渲染

#### 模型切换
- 按 'm' 在模型之间切换
- 模型切换进度在 OpenCV 窗口中可视化显示
- 切换进度显示："Switching to [Model] model..."（黄色文本）
- 成功确认："Model switch completed successfully!"（绿色文本）
- 错误通知："Failed to switch model!"（红色文本）
- 模型切换可能需要几秒钟
- 新标签信息会自动检索和显示
- 为新模型的类别重新生成颜色

### 数据处理

- **优化渲染**：预计算叠加基础图像，实现快速鼠标交互
- **彩色化**：为每个类别生成随机颜色（背景为黑色以实现透明效果）
- **背景处理**：背景（null类别）渲染为黑色，在叠加中作为透明处理
- **轮廓检测**：手动轮廓绘制，具有边界检查，实现可靠的对象高亮
- **图像对齐**：SDK 提供的深度相机对齐功能
- **向量化混合**：使用掩码操作的 OpenCV 优化混合，获得更好性能

## 错误处理

演示程序包含全面的错误处理：

- 设备连接失败
- 语义分割支持验证
- 帧获取超时
- 模型切换失败
- 无效帧数据检测
- 中断时的优雅关闭


## 故障排除

### 常见问题

1. **"不支持语义分割"**
   - 确保您的 Aurora 设备具有语义分割功能
   - 检查固件版本兼容性

2. **"订阅语义分割失败"**
   - 验证网络连接稳定性
   - 检查是否有其他应用程序正在使用设备

3. **"模型切换失败"**
   - 确保设备支持两种模型
   - 等待当前帧处理完成

4. **分割质量差**
   - 确保充足的照明条件
   - 检查相机镜头是否有遮挡
   - 验证场景在模型训练域内（室内 vs 户外）

### 调试信息

演示程序提供控制台输出：
- 设备发现和连接状态
- 分割配置参数
- 模型切换进度
- 帧获取率（FPS）
- 标签集和类别信息
- 带有特定错误代码的错误消息

## 示例输出

```
Device connection string not provided, try to discover aurora devices...
Waiting for aurora devices...
Found 1 aurora devices
Device 0
  option 0: tcp/[fe80::ad94:89de:cef2:dcb4]:7447
  option 1: tcp/192.168.1.212:7447
Selected first device: 
Connecting to the selected device...
Connected to the selected device
Semantic segmentation is supported
Semantic Segmentation Config:
  FPS: 7.5
  Frame Skip: 1
  Image Size: 640x480
Waiting for semantic segmentation to be ready...

=== Semantic Segmentation Label Information ===
Label Set: coco80
Total Classes: 81
Classes:
  [ 0] Background
  [ 1] person
...
  [79] hair drier
  [80] toothbrush
================================================

Current model: Default

Controls:
  ESC - Exit
  'm' - Switch between default and alternative model
  Mouse hover - Show object contours and labels (in merged segmentation & camera overlay)
Segmentation FPS: 7.46269 | Frame: 30
Segmentation FPS: 7.51691 | Frame: 60
Segmentation FPS: 7.4664 | Frame: 90
Segmentation FPS: 7.55097 | Frame: 120
Segmentation FPS: 7.42758 | Frame: 150
Segmentation FPS: 7.57576 | Frame: 180
```

## 集成说明

此演示可作为以下应用的基础：
- 对象检测和跟踪应用程序
- 自主导航系统
- 增强现实应用程序
- 场景理解项目
- 智能家居/办公自动化
- 机器人感知系统

代码演示了 Aurora SDK 2.0 增强成像使用的最佳实践，可根据特定应用需求进行调整。
