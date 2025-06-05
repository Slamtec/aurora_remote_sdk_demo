# Aurora 纯 C 语言演示

本演示展示如何使用纯 C API（不使用 C++ 包装器）的 Aurora Remote SDK 来检索设备姿态信息。此示例非常适合需要 C 兼容性或无法使用 C++ 功能的项目。

## 功能特性

- **纯 C 实现**: 仅使用 C API 函数，无 C++ 包装器依赖
- **设备发现**: 自动发现 Aurora 设备或连接到指定设备
- **实时姿态**: 连续显示设备位置（x, y, z 坐标）
- **跨平台**: 兼容 Windows 和类 Unix 系统
- **最小依赖**: 仅需要 C11 编译器支持

## 系统要求

- 具有活动定位系统的 Aurora 设备
- Aurora Remote SDK
- C11 兼容编译器（gcc, clang, MSVC）
- 主机与 Aurora 设备之间的网络连接

## 使用方法

### 基本用法

```bash
# 自动发现并连接到第一个可用设备
./pure_c_demo

# 通过 IP 地址连接到特定设备
./pure_c_demo 192.168.1.100
```

### 命令行参数

- 无参数: 自动发现 Aurora 设备并连接到发现的第一个设备
- `<ip_address>`: 直接连接到指定 IP 地址的设备（使用默认端口 8090）

## 输出示例

### 自动发现模式
```
Aurora SDK Version: 2.0.0-alpha
Searching for aurora devices...
Connecting to the first server: 192.168.1.100
Current pose: 1.234000, 2.567000, 0.089000
Current pose: 1.235000, 2.568000, 0.089000
Current pose: 1.236000, 2.569000, 0.089000
...
```

### 直接连接模式
```
Aurora SDK Version: 2.0.0-alpha
Using connection string: 192.168.1.100
Current pose: 1.234000, 2.567000, 0.089000
Current pose: 1.235000, 2.568000, 0.089000
...
```

## 关键概念

### 纯 C API 使用

本演示展示了核心 C API 函数:

- **会话管理**:
  - `slamtec_aurora_sdk_create_session()`: 创建 SDK 会话
  - `slamtec_aurora_sdk_release_session()`: 释放会话资源

- **设备发现**:
  - `slamtec_aurora_sdk_controller_get_discovered_servers()`: 发现可用设备
  
- **连接管理**:
  - `slamtec_aurora_sdk_controller_connect()`: 连接到设备
  - `slamtec_aurora_sdk_controller_disconnect()`: 断开设备连接

- **数据检索**:
  - `slamtec_aurora_sdk_dataprovider_get_current_pose()`: 获取当前设备姿态

### 数据结构

演示使用 SDK 中定义的 C 结构:
- `slamtec_aurora_sdk_pose_t`: 包含平移（x, y, z）和旋转分量
- `slamtec_aurora_sdk_server_connection_info_t`: 服务器连接信息
- `slamtec_aurora_sdk_version_info_t`: SDK 版本信息

## 技术细节

### 跨平台睡眠
- Windows: 使用 `Sleep(毫秒)`
- Unix/Linux: 使用 `sleep(秒)`

### 错误处理
- 函数返回 `slamtec_aurora_sdk_errorcode_t` 值
- `SLAMTEC_AURORA_SDK_ERRORCODE_OK` 表示成功
- 通过会话释放进行适当清理

### 内存管理
- 基本数据类型的自动内存管理
- 需要手动会话清理以进行适当的资源管理

## 集成示例

项目中的基本 C 集成:

```c
#include "aurora_pubsdk_inc.h"
#include <stdio.h>

int main() {
    // 创建会话
    slamtec_aurora_sdk_session_handle_t session = 
        slamtec_aurora_sdk_create_session(NULL, 0, NULL, NULL);
    
    if (session == NULL) {
        printf("Failed to create session\n");
        return -1;
    }
    
    // 设置连接信息
    slamtec_aurora_sdk_server_connection_info_t info;
    memset(&info, 0, sizeof(info));
    strncpy(info.connection_info[0].address, "192.168.1.100", 
           sizeof(info.connection_info[0].address));
    strncpy(info.connection_info[0].protocol_type, 
           SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PROTOCOL,
           sizeof(info.connection_info[0].protocol_type));
    info.connection_info[0].port = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PORT;
    info.connection_count = 1;
    
    // 连接
    if (slamtec_aurora_sdk_controller_connect(session, &info) == 
        SLAMTEC_AURORA_SDK_ERRORCODE_OK) {
        
        // 获取姿态
        slamtec_aurora_sdk_pose_t pose;
        slamtec_aurora_sdk_dataprovider_get_current_pose(session, &pose);
        
        printf("Position: %f, %f, %f\n", 
               pose.translation.x, pose.translation.y, pose.translation.z);
        
        // 清理
        slamtec_aurora_sdk_controller_disconnect(session);
    }
    
    slamtec_aurora_sdk_release_session(session);
    return 0;
}
```

## 编译

### GCC/Clang
```bash
gcc -o pure_c_demo pure_c_demo.c -lslamtec_aurora_remote_sdk
```

### MSVC
```cmd
cl pure_c_demo.c slamtec_aurora_remote_sdk.lib
```

### CMake
```cmake
add_executable(pure_c_demo pure_c_demo.c)
target_link_libraries(pure_c_demo slamtec_aurora_remote_sdk)
```

## 高级功能

### 多设备处理
```c
slamtec_aurora_sdk_server_connection_info_t servers[32];
int count = slamtec_aurora_sdk_controller_get_discovered_servers(session, servers, 32);

for (int i = 0; i < count; i++) {
    printf("Device %d: %s\n", i, servers[i].connection_info[0].address);
}
```

### 错误检查
```c
slamtec_aurora_sdk_errorcode_t result = 
    slamtec_aurora_sdk_controller_connect(session, &info);

switch (result) {
    case SLAMTEC_AURORA_SDK_ERRORCODE_OK:
        printf("Connection successful\n");
        break;
    case SLAMTEC_AURORA_SDK_ERRORCODE_CONNECTION_LOST:
        printf("Connection failed\n");
        break;
    default:
        printf("Unknown error: %d\n", result);
        break;
}
```

## 使用场景

- **嵌入式系统**: 无 C++ 支持的纯 C 环境
- **遗留集成**: 与现有 C 代码库的集成
- **最小占用**: 需要最小依赖的应用程序
- **实时系统**: 具有可预测行为的低级控制
- **跨语言绑定**: 为其他语言创建绑定的基础

## 限制

- 与 C++ 包装器相比功能较少
- 复杂操作需要手动内存管理
- 无自动资源管理（RAII）
- 更冗长的错误处理

## 最佳实践

- 始终检查返回值以获取错误代码
- 正确释放会话并断开设备连接
- 为跨平台兼容性使用适当的睡眠函数
- 使用前用 `memset()` 初始化结构
- 优雅地处理连接失败