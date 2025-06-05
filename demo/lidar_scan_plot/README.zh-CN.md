# Aurora LiDAR 扫描绘图演示

![LiDAR Scan Plot Demo](../../res/demo.lidar.scan.rendering.gif)

本演示展示如何使用 OpenCV 绘图从 Aurora 设备检索和可视化 LiDAR 扫描数据。

## 功能特性

- **实时 LiDAR 可视化**: 显示实时 LiDAR 扫描点
- **2D 绘图渲染**: 以俯视图显示 LiDAR 数据
- **距离测量**: 彩色编码的距离可视化
- **扫描速率监控**: 显示扫描频率和统计信息

## 系统要求

- 带有 LiDAR 传感器的 Aurora 设备
- Aurora Remote SDK
- OpenCV 4.2 或更高版本
- 网络连接

## 使用方法

```bash
# 自动发现并绘制 LiDAR 扫描
./lidar_scan_plot

# 连接到特定设备
./lidar_scan_plot tcp://192.168.1.100:8090
```

## 关键功能

- **点云显示**: 实时 LiDAR 点可视化
- **范围信息**: 距离和角度数据
- **颜色映射**: 基于距离的颜色编码
- **扫描统计**: 帧率和点数监控

## 使用场景

- **传感器验证**: LiDAR 功能测试
- **环境分析**: 实时障碍物检测
- **开发**: LiDAR 数据调试
- **校准**: 传感器对齐验证