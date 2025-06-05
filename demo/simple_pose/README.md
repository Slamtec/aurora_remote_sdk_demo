# Aurora Simple Pose Demo

This demo demonstrates how to retrieve and display the current pose (position and orientation) of an Aurora device using the Aurora Remote SDK.

## Features

- **Real-time Pose Retrieval**: Continuously fetches the current device pose at 10Hz
- **Device Discovery**: Auto-discovers Aurora devices on the network or connects to a specific device
- **Position & Orientation**: Displays 3D position (x, y, z) and Euler angles (roll, pitch, yaw)
- **Simple Interface**: Minimal command-line interface for easy understanding

## Requirements

- Aurora device with active positioning system
- Aurora Remote SDK
- Network connection between host and Aurora device

## Usage

### Basic Usage

```bash
# Auto-discover and connect to first available device
./simple_pose

# Connect to specific device
./simple_pose tcp://192.168.1.100:8090
```

### Command Line Arguments

- No arguments: Auto-discovers Aurora devices and connects to the first one found
- `<connection_string>`: Direct connection to specific device (e.g., `tcp://192.168.1.100:8090`)

## Example Output

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

## Key Concepts

### Pose Representation

The demo retrieves pose in Euler angle format:
- **Position**: 3D coordinates (x, y, z) in meters relative to the map origin
- **Orientation**: Euler angles (roll, pitch, yaw) in radians

### Important Notes

- The demo uses `getCurrentPose()` which returns Euler angles
- For production applications, consider using `getCurrentPoseSE3()` which provides quaternion-based rotation (more numerically stable)
- Euler angles may suffer from gimbal lock in certain orientations

## Technical Details

### Update Rate
- Pose is retrieved at 10Hz (100ms intervals)
- Real-time performance suitable for monitoring and debugging

### Coordinate System
- Position coordinates are relative to the map coordinate frame
- Orientation follows standard robotics conventions (roll-pitch-yaw)

### Error Handling
- Graceful handling of connection failures
- Automatic device discovery with fallback options
- Ctrl+C interrupt support for clean shutdown

## Integration Example

Basic pose retrieval in your application:

```cpp
#include "aurora_pubsdk_inc.h"

using namespace rp::standalone::aurora;

// Create SDK session
RemoteSDK* sdk = RemoteSDK::CreateSession();

// Connect to device (discovery or direct connection)
SDKServerConnectionDesc deviceDesc("tcp://192.168.1.100:8090");
if (sdk->connect(deviceDesc)) {
    // Get current pose
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

## Advanced Usage

For more robust pose estimation, consider using the SE3 pose format:

```cpp
slamtec_aurora_sdk_pose_se3_t pose_se3;
sdk->dataProvider.getCurrentPoseSE3(pose_se3);

// pose_se3.translation contains x, y, z
// pose_se3.quaternion contains x, y, z, w quaternion components
```

## Use Cases

- **Robot Navigation**: Real-time position feedback for autonomous navigation
- **Mapping Verification**: Monitor device position during mapping operations  
- **System Integration**: Simple pose monitoring for larger robotic systems
- **Development & Testing**: Basic pose verification during application development