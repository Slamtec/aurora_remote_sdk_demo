# Aurora Relocalization Demo

This demo demonstrates how to trigger relocalization on an Aurora device using the Aurora Remote SDK. Relocalization allows the device to re-establish its position within a previously mapped environment.

## Features

- **Device Discovery**: Auto-discovers Aurora devices on the network or connects to a specific device
- **Relocalization Trigger**: Initiates the relocalization process on the connected device
- **Status Feedback**: Reports the success or failure of the relocalization attempt
- **Simple Interface**: Minimal command-line interface for triggering relocalization

## Requirements

- Aurora device with an existing map loaded
- Aurora Remote SDK
- Network connection between host and Aurora device
- Pre-existing map data on the device (relocalization requires a reference map)

## Usage

### Basic Usage

```bash
# Auto-discover and connect to first available device
./relocalization

# Connect to specific device
./relocalization tcp://192.168.1.100:8090
```

### Command Line Arguments

- No arguments: Auto-discovers Aurora devices and connects to the first one found
- `<connection_string>`: Direct connection to specific device (e.g., `tcp://192.168.1.100:8090`)

## Example Output

### Successful Relocalization
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

### Failed Relocalization
```
Aurora SDK Version: 2.0.0-alpha
Connected to the selected device
Failed to relocalization
```

## Key Concepts

### Relocalization

Relocalization is the process by which the Aurora device re-establishes its position within a known map when:
- The device is placed in a previously mapped environment
- Tracking has been lost due to environmental changes
- The device needs to recover from a positioning failure
- The device is restarted in a known location

### When to Use Relocalization

- **Map Loading**: After loading a previously saved map
- **Position Recovery**: When the device loses tracking and needs to recover
- **Startup**: When placing the device in a known mapped environment

## Technical Details

### Relocalization Process

1. **Request Initiation**: The demo calls `requireRelocalization()` on the controller
2. **Environment Analysis**: The device analyzes current sensor data against the loaded map
3. **Position Estimation**: Attempts to match current observations with map features
4. **Success/Failure**: Returns whether relocalization was successful

### Prerequisites for Success

- **Existing Map**: A valid map must be loaded on the device
- **Feature Rich Environment**: Sufficient visual or geometric features for matching
- **Proper Positioning**: Device should be placed within the mapped area
- **Environmental Consistency**: Environment should be reasonably similar to when the map was created

## Integration Example

Basic relocalization in your application:

```cpp
#include "aurora_pubsdk_inc.h"

using namespace rp::standalone::aurora;

// Create SDK session and connect to device
RemoteSDK* sdk = RemoteSDK::CreateSession();
SDKServerConnectionDesc deviceDesc("tcp://192.168.1.100:8090");

if (sdk->connect(deviceDesc)) {
    // Trigger relocalization
    if (sdk->controller.requireRelocalization()) {
        std::cout << "Relocalization successful" << std::endl;
        
        // Now you can safely get current pose
        slamtec_aurora_sdk_pose_t pose;
        sdk->dataProvider.getCurrentPose(pose);
        // Use pose data...
        
    } else {
        std::cout << "Relocalization failed" << std::endl;
        // Handle failure case
    }
}

sdk->disconnect();
sdk->release();
```

### Best Practices

- Load the appropriate map before attempting relocalization
- Place the device in a feature-rich area of the mapped environment
- Allow sufficient time for the relocalization process to complete
- Consider multiple relocalization attempts if the first attempt fails

## Use Cases

- **Autonomous Navigation**: Establishing initial position for navigation tasks
- **Mapping Continuation**: Resuming mapping operations in a known environment  
- **Position Recovery**: Recovering from tracking failures during operation
- **System Initialization**: Setting up the device position at startup