# Aurora IMU 数据获取演示

本演示展示如何使用 Aurora Remote SDK 从 Aurora 设备检索和显示 IMU（惯性测量单元）数据。IMU 提供运动跟踪和传感器融合所必需的加速度和角速度测量。

## 功能特性

- **实时 IMU 数据**: 高频率连续获取加速度计和陀螺仪数据
- **设备发现**: 自动发现网络上的 Aurora 设备或连接到指定设备
- **数据过滤**: 使用时间戳比较过滤重复数据
- **非阻塞访问**: 使用缓存的 IMU 数据进行高效的非阻塞数据检索
- **全面输出**: 显示 3 轴加速度和角速度测量值

## 系统要求

- 带有 IMU 传感器的 Aurora 设备
- Aurora Remote SDK
- 主机与 Aurora 设备之间的网络连接

## 使用方法

### 基本用法

```bash
# 自动发现并连接到第一个可用设备
./imu_fetcher

# 连接到特定设备
./imu_fetcher tcp://192.168.1.100:8090
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
IMU Data: Accel: -0.123, 0.456, 9.789 Gyro: 0.001, -0.002, 0.003
IMU Data: Accel: -0.124, 0.457, 9.791 Gyro: 0.002, -0.001, 0.003
IMU Data: Accel: -0.125, 0.458, 9.788 Gyro: 0.001, -0.003, 0.004
...
```

## 关键概念

### IMU 数据结构

演示检索包含以下内容的 `slamtec_aurora_sdk_imu_data_t` 结构:
- **加速度** (`acc[3]`): 3 轴线性加速度，单位 m/s²
  - acc[0]: X 轴加速度
  - acc[1]: Y 轴加速度
  - acc[2]: Z 轴加速度（通常包括重力 ~9.8 m/s²）
- **角速度** (`gyro[3]`): 3 轴角速度，单位 rad/s
  - gyro[0]: 滚转角速度（绕 X 轴旋转）
  - gyro[1]: 俯仰角速度（绕 Y 轴旋转）
  - gyro[2]: 偏航角速度（绕 Z 轴旋转）
- **时间戳**: 纳秒精度的高精度时间戳

### 数据流

1. **IMU 采样**: 设备高频率连续采样 IMU 数据
2. **SDK 缓存**: SDK 缓存 IMU 数据以便高效访问
3. **非阻塞检索**: `peekIMUData()` 提供对缓存数据的即时访问
4. **时间戳过滤**: 仅处理新数据（较新的时间戳）
5. **实时显示**: IMU 值连续显示


## 集成示例

在应用程序中进行基本 IMU 数据检索:

```cpp
#include "aurora_pubsdk_inc.h"

using namespace rp::standalone::aurora;

// 创建 SDK 会话并连接
RemoteSDK* sdk = RemoteSDK::CreateSession();
SDKServerConnectionDesc deviceDesc("tcp://192.168.1.100:8090");

if (sdk->connect(deviceDesc)) {
    uint64_t lastTimestamp = 0;
    
    // 连续检索 IMU 数据
    std::vector<slamtec_aurora_sdk_imu_data_t> imuData;
    if (sdk->dataProvider.peekIMUData(imuData)) {
        for (const auto& imu : imuData) {
            if (imu.timestamp_ns > lastTimestamp) {
                lastTimestamp = imu.timestamp_ns;
                
                // 处理加速度数据
                double accel_x = imu.acc[0];
                double accel_y = imu.acc[1]; 
                double accel_z = imu.acc[2];
                
                // 处理陀螺仪数据
                double gyro_x = imu.gyro[0];
                double gyro_y = imu.gyro[1];
                double gyro_z = imu.gyro[2];
                
                // 您的处理逻辑在这里...
            }
        }
    }
}

sdk->disconnect();
sdk->release();
```

### 替代方案: 事件驱动的 IMU 数据

对于更高级的应用程序，考虑使用监听器:

```cpp
// 为事件驱动的 IMU 数据访问设置监听器
class IMUListener : public RemoteSDKListener {
public:
    void onIMUDataUpdated(const std::vector<slamtec_aurora_sdk_imu_data_t>& imuData) override {
        // 在数据到达时处理 IMU 数据
        for (const auto& imu : imuData) {
            // 处理每个 IMU 样本...
        }
    }
};

IMUListener listener;
sdk->setListener(&listener);
```

## 高级用法

### IMU 数据分析

常见的 IMU 处理任务:

```cpp
// 计算加速度幅值
double accel_magnitude = sqrt(imu.acc[0]*imu.acc[0] + 
                             imu.acc[1]*imu.acc[1] + 
                             imu.acc[2]*imu.acc[2]);

// 检测运动
bool isMoving = accel_magnitude > 9.8 + threshold; // 超过重力 + 阈值

// 计算旋转幅值
double gyro_magnitude = sqrt(imu.gyro[0]*imu.gyro[0] + 
                            imu.gyro[1]*imu.gyro[1] + 
                            imu.gyro[2]*imu.gyro[2]);

// 检测旋转
bool isRotating = gyro_magnitude > rotation_threshold;
```

## 使用场景

- **运动检测**: 检测设备何时移动或静止
- **方向跟踪**: 监控设备方向变化
- **振动分析**: 分析振动和机械干扰
- **传感器融合**: 将 IMU 数据与其他传感器结合以增强跟踪
- **跌倒检测**: 检测加速度模式的突然变化
- **导航**: 惯性导航和航位推算
- **校准**: IMU 校准和偏差估计

## 性能说明

- **高频数据**: IMU 产生高频数据；考虑处理要求
- **时间戳管理**: 始终使用时间戳确保正确的数据排序
- **内存效率**: 及时处理数据以避免过度内存使用
- **实时处理**: IMU 数据对时间敏感；最小化处理延迟