# Aurora 简单姿态演示

本演示展示如何使用 Aurora Remote SDK 检索和显示 Aurora 设备的当前姿态（位置和方向）。

## 功能特性

- **实时姿态检索**: 以 10Hz 频率连续获取当前设备姿态
- **设备发现**: 自动发现网络上的 Aurora 设备或连接到指定设备
- **位置和方向**: 显示 3D 位置 (x, y, z) 和欧拉角 (roll, pitch, yaw)
- **简单界面**: 最小化命令行界面，易于理解

## 系统要求

- 具有活动定位系统的 Aurora 设备
- Aurora Remote SDK
- 主机与 Aurora 设备之间的网络连接

## 使用方法

### 基本用法

```bash
# 自动发现并连接到第一个可用设备
./simple_pose

# 连接到特定设备
./simple_pose tcp://192.168.1.100:8090
```

### 命令行参数

- 无参数: 自动发现 Aurora 设备并连接到发现的第一个设备
- `<connection_string>`: 直接连接到特定设备 (例如 `tcp://192.168.1.100:8090`)

## 输出示例

```
Aurora SDK Version: 2.0.0-alpha
Device connection string not provided, try to discover aurora devices...
Waiting for aurora devices...
Found 1 aurora devices
Device 0
  option 0: tcp://192.168.1.100:8090
Selected first device: 
Connecting to the selected device...
Connected to the selected device
Current pose: 1.234, 2.567, 0.089 Euler: 0.012, -0.003, 1.567
Current pose: 1.235, 2.568, 0.089 Euler: 0.012, -0.003, 1.568
Current pose: 1.236, 2.569, 0.089 Euler: 0.013, -0.003, 1.569
...
```

## 关键概念

### 姿态表示

演示以欧拉角格式检索姿态:
- **位置**: 相对于地图原点的 3D 坐标 (x, y, z)，单位为米
- **方向**: 欧拉角 (roll, pitch, yaw)，单位为弧度

### 重要说明

- 演示使用 `getCurrentPose()` 返回欧拉角
- 对于生产应用，建议使用 `getCurrentPoseSE3()` 提供基于四元数的旋转（数值更稳定）
- 欧拉角在某些方向可能出现万向锁问题

## 技术细节

### 更新频率
- 姿态以 10Hz 频率检索（100ms 间隔）
- 适合监控和调试的实时性能

### 坐标系
- 位置坐标相对于地图坐标系
- 方向遵循标准机器人学约定（roll-pitch-yaw）

### 错误处理
- 优雅处理连接失败
- 带有备用选项的自动设备发现
- 支持 Ctrl+C 中断以进行干净关闭

## 集成示例

在应用程序中进行基本姿态检索:

```cpp
#include "aurora_pubsdk_inc.h"

using namespace rp::standalone::aurora;

// 创建 SDK 会话
RemoteSDK* sdk = RemoteSDK::CreateSession();

// 连接到设备（发现或直接连接）
SDKServerConnectionDesc deviceDesc("tcp://192.168.1.100:8090");
if (sdk->connect(deviceDesc)) {
    // 获取当前姿态
    slamtec_aurora_sdk_pose_t pose;
    sdk->dataProvider.getCurrentPose(pose);
    
    std::cout << "Position: " << pose.translation.x << ", " 
              << pose.translation.y << ", " << pose.translation.z << std::endl;
    std::cout << "Orientation: " << pose.rpy.roll << ", " 
              << pose.rpy.pitch << ", " << pose.rpy.yaw << std::endl;
}

sdk->disconnect();
sdk->release();
```

## 高级用法

对于更稳健的姿态估计，考虑使用 SE3 姿态格式:

```cpp
slamtec_aurora_sdk_pose_se3_t pose_se3;
sdk->dataProvider.getCurrentPoseSE3(pose_se3);

// pose_se3.translation 包含 x, y, z
// pose_se3.quaternion 包含 x, y, z, w 四元数分量
```

## 使用场景

- **机器人导航**: 自主导航的实时位置反馈
- **地图验证**: 在地图操作期间监控设备位置
- **系统集成**: 大型机器人系统的简单姿态监控
- **开发和测试**: 应用程序开发期间的基本姿态验证