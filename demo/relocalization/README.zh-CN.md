# Aurora 重定位演示

本演示展示如何使用 Aurora Remote SDK 在 Aurora 设备上触发重定位。重定位允许设备在先前映射的环境中重新建立其位置。

## 功能特性

- **设备发现**: 自动发现网络上的 Aurora 设备或连接到指定设备
- **重定位触发**: 在连接的设备上启动重定位过程
- **状态反馈**: 报告重定位尝试的成功或失败
- **简单界面**: 用于触发重定位的最小化命令行界面

## 系统要求

- 已加载现有地图的 Aurora 设备
- Aurora Remote SDK
- 主机与 Aurora 设备之间的网络连接
- 设备上的预先存在的地图数据（重定位需要参考地图）

## 使用方法

### 基本用法

```bash
# 自动发现并连接到第一个可用设备
./relocalization

# 连接到特定设备
./relocalization tcp://192.168.1.100:8090
```

### 命令行参数

- 无参数: 自动发现 Aurora 设备并连接到发现的第一个设备
- `<connection_string>`: 直接连接到特定设备 (例如 `tcp://192.168.1.100:8090`)

## 输出示例

### 成功重定位
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
Relocalization success
```

### 失败重定位
```
Aurora SDK Version: 2.0.0-alpha
Connected to the selected device
Failed to relocalization
```

## 关键概念

### 重定位

重定位是 Aurora 设备在以下情况下重新建立其在已知地图中位置的过程:
- 设备被放置在先前映射的环境中
- 由于环境变化导致跟踪丢失
- 设备需要从定位故障中恢复
- 设备在已知位置重新启动

### 何时使用重定位

- **地图加载**: 加载先前保存的地图后
- **位置恢复**: 当设备失去跟踪并需要恢复时
- **启动**: 将设备放置在已知映射环境中时
- **回环闭合**: 通过识别先前访问的区域来提高映射精度

## 技术细节

### 重定位过程

1. **请求启动**: 演示在控制器上调用 `requireRelocalization()`
2. **环境分析**: 设备根据加载的地图分析当前传感器数据
3. **位置估计**: 尝试将当前观察与地图特征匹配
4. **成功/失败**: 返回重定位是否成功

### 成功的先决条件

- **现有地图**: 设备上必须加载有效地图
- **特征丰富的环境**: 用于匹配的充足视觉或几何特征
- **正确定位**: 设备应放置在映射区域内
- **环境一致性**: 环境应与创建地图时合理相似

## 集成示例

在应用程序中进行基本重定位:

```cpp
#include "aurora_pubsdk_inc.h"

using namespace rp::standalone::aurora;

// 创建 SDK 会话并连接到设备
RemoteSDK* sdk = RemoteSDK::CreateSession();
SDKServerConnectionDesc deviceDesc("tcp://192.168.1.100:8090");

if (sdk->connect(deviceDesc)) {
    // 触发重定位
    if (sdk->controller.requireRelocalization()) {
        std::cout << "Relocalization successful" << std::endl;
        
        // 现在可以安全地获取当前姿态
        slamtec_aurora_sdk_pose_t pose;
        sdk->dataProvider.getCurrentPose(pose);
        // 使用姿态数据...
        
    } else {
        std::cout << "Relocalization failed" << std::endl;
        // 处理失败情况
    }
}

sdk->disconnect();
sdk->release();
```


## 故障排除

### 常见重定位失败

1. **未加载地图**: 确保在尝试重定位之前加载有效地图
2. **环境变化**: 环境的重大变化可能阻止成功重定位
3. **特征可见性差**: 确保设备能清楚看到独特的环境特征
4. **位置错误**: 设备可能在映射区域之外

### 最佳实践

- 在尝试重定位之前加载适当的地图
- 将设备放置在映射环境的特征丰富区域
- 为重定位过程留出充足的完成时间
- 如果第一次尝试失败，考虑多次重定位尝试

## 使用场景

- **自主导航**: 为导航任务建立初始位置
- **地图继续**: 在已知环境中恢复地图操作
- **位置恢复**: 从操作期间的跟踪故障中恢复
- **系统初始化**: 在启动时设置设备位置