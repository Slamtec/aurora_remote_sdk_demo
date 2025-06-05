# Aurora VSLAM 地图保存/加载演示

本演示展示如何使用 Aurora Remote SDK 从 Aurora 设备保存（下载）和加载（上传）VSLAM 地图。此功能允许您备份地图、在设备间共享地图或恢复先前创建的地图。

## 功能特性

- **地图下载**: 从 Aurora 设备下载 VSLAM 地图到本地文件
- **地图上传**: 将本地 VSLAM 地图文件上传到 Aurora 设备
- **进度监控**: 传输操作期间的实时进度显示
- **设备发现**: 自动发现 Aurora 设备或连接到指定设备
- **灵活的文件命名**: 指定自定义地图文件名或使用默认值
- **会话管理**: 具有中止能力的适当会话处理

## 系统要求

- 具有 VSLAM 映射功能的 Aurora 设备
- Aurora Remote SDK
- 主机与 Aurora 设备之间的网络连接
- 地图文件的充足存储空间（地图可能为几 MB 到 GB）

## 使用方法

### 基本用法

```bash
# 从设备下载地图（默认行为）
./vslam_map_saveload

# 使用自定义文件名下载地图
./vslam_map_saveload -d my_map.stcm

# 上传地图到设备
./vslam_map_saveload -u existing_map.stcm

# 连接到特定设备
./vslam_map_saveload -s tcp://192.168.1.100:8090 -d office_map.stcm
```

### 命令行选项

- `-h, --help`: 显示帮助信息和使用说明
- `-s, --server <locator>`: 连接到特定设备 (例如 `tcp://192.168.1.100:8090`)
- `-d, --download`: 从设备下载地图（如果未指定操作则为默认）
- `-u, --upload`: 上传地图到设备
- `[map_file]`: 指定地图文件名（默认: `auroramap.stcm`）

## 输出示例

### 下载地图
```
Found 1 aurora devices
Device 0
  option 0: tcp://192.168.1.100:8090
Selected first device: 
Connecting to the selected device...
Connected to the selected device
Downloading vslam map to office_map.stcm
Downloading vslam map 25.67%
Downloading vslam map 50.33%
Downloading vslam map 75.89%
Downloading vslam map 100.00%
Downloading vslam map succeeded
```

### 上传地图
```
Connecting to device tcp://192.168.1.100:8090
Connected to the selected device
Uploading vslam map to backup_map.stcm
Uploading vslam map 15.23%
Uploading vslam map 45.67%
Uploading vslam map 78.91%
Uploading vslam map 100.00%
Uploading vslam map succeeded
```

## 关键概念

### VSLAM 地图

VSLAM（视觉同时定位与地图构建）地图包含:
- **3D 点云**: 密集或稀疏的 3D 环境特征
- **关键帧**: 重要的相机姿态和相关的视觉特征
- **地图结构**: 特征和姿态之间的空间关系
- **元数据**: 地图创建时间戳、设备信息等

### 地图文件格式

- **扩展名**: `.stcm`（SLAMTEC 地图格式）
- **内容**: 包含完整地图数据的二进制格式
- **大小**: 根据环境复杂性变化（通常 10MB - 1GB+）
- **兼容性**: 地图在同一产品系列内设备兼容

### 传输操作

1. **下载（设备 → 本地）**:
   - 从设备内存检索活动地图
   - 保存到指定的本地文件
   - 用于备份和存档

2. **上传（本地 → 设备）**:
   - 从本地存储加载地图文件
   - 传输到设备内存
   - 替换当前设备地图

## 技术细节

### 会话管理

- **异步操作**: 地图传输使用异步回调进行非阻塞操作
- **进度监控**: 传输期间的实时进度更新
- **中止能力**: 可以使用 Ctrl+C 中断传输
- **状态查询**: 活动会话期间的连续状态监控

### 错误处理

- 开始传输前的连接验证
- 带有错误检测的进度监控
- 中断时的优雅会话中止
- 全面的错误报告

## 集成示例

在应用程序中进行基本地图保存/加载:

```cpp
#include "aurora_pubsdk_inc.h"

using namespace rp::standalone::aurora;

// 从设备下载地图
bool downloadMap(RemoteSDK* sdk, const std::string& filename) {
    std::promise<bool> resultPromise;
    auto resultFuture = resultPromise.get_future();
    
    auto callback = [](void* userData, int isOK) {
        auto promise = reinterpret_cast<std::promise<bool>*>(userData);
        promise->set_value(isOK != 0);
    };
    
    if (!sdk->mapManager.startDownloadSession(filename.c_str(), callback, &resultPromise)) {
        return false;
    }
    
    // 监控进度
    while (sdk->mapManager.isSessionActive()) {
        slamtec_aurora_sdk_mapstorage_session_status_t status;
        if (sdk->mapManager.querySessionStatus(status)) {
            std::cout << "Progress: " << status.progress << "%" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return resultFuture.get();
}

// 上传地图到设备
bool uploadMap(RemoteSDK* sdk, const std::string& filename) {
    std::promise<bool> resultPromise;
    auto resultFuture = resultPromise.get_future();
    
    auto callback = [](void* userData, int isOK) {
        auto promise = reinterpret_cast<std::promise<bool>*>(userData);
        promise->set_value(isOK != 0);
    };
    
    if (!sdk->mapManager.startUploadSession(filename.c_str(), callback, &resultPromise)) {
        return false;
    }
    
    // 监控进度
    while (sdk->mapManager.isSessionActive()) {
        slamtec_aurora_sdk_mapstorage_session_status_t status;
        if (sdk->mapManager.querySessionStatus(status)) {
            std::cout << "Progress: " << status.progress << "%" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return resultFuture.get();
}
```

### 存储考虑因素

- **磁盘空间**: 确保地图文件有足够的存储空间
- **网络带宽**: 大地图需要稳定的网络连接
- **传输时间**: 在部署规划中考虑传输时间
- **版本控制**: 考虑地图管理的版本控制方案

## 故障排除

### 常见问题

1. **大文件传输**: 确保大地图有稳定的网络连接
2. **存储空间**: 下载前验证足够的磁盘空间
3. **设备内存**: 确保设备有足够的内存存储上传的地图
4. **文件权限**: 检查地图文件的读/写权限

### 最佳实践

- 首先使用小地图测试地图传输
- 监控传输进度并优雅处理中断
- 为失败的传输实现重试机制
- 传输后验证地图完整性
- 保留重要地图的备份副本