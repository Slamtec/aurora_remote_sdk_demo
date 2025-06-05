# Aurora 设备信息监控演示

本演示展示如何使用 Aurora SDK 2.0 连续监控和显示 Aurora 设备基本信息。

## 功能特性

- **实时设备监控**: 以 1 秒间隔连续获取和显示设备信息

- **全面的设备信息**: 显示以下设备信息：
  - 设备名称
  - 设备序列号
  - 设备型号
  - 固件版本
  - 硬件版本
  - 功能集
  - 设备运行时间（格式化为天、小时、分钟、秒）

- **清晰的控制台界面**: 清晰、格式化的输出，带有时间戳和更新计数器

- **功能集解释**: 自动将设备功能标志解释为可读的功能

## 系统要求

- 支持 Aurora SDK 2.0 的 Aurora 设备
- Aurora Remote SDK 2.0
- 支持 ANSI 转义码的终端或控制台（用于屏幕清除）

## 使用方法

### 基本用法

```bash
# 自动发现设备并开始监控
./device_info_monitor

# 连接到指定设备
./device_info_monitor tcp://192.168.1.100:8090

# 显示帮助
./device_info_monitor --help
```

### 命令行选项

- `-h, --help`: 显示帮助信息和使用示例

## 输出示例

```
==================== AURORA DEVICE MONITOR ====================
Timestamp: 2025-05-25 12:07:17.055 | Update #33
=================================================================
Device Name:        Aurora
Serial Number:      156F806B5F93F596AD4775291B6583C7
Device Model:       A1M1
Firmware Version:   2.0.0-alpha
Firmware Build:     2025-05-25 10:28:38
HW Features:        17
Sensing Features:   196639
SW Features:        196615
Device Uptime:      7d 21h 52m 58s
=================================================================
Press Ctrl+C to exit...
```
