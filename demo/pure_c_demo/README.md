# Aurora Pure C Demo

This demo demonstrates how to use the Aurora Remote SDK with pure C API (without C++ wrapper) to retrieve device pose information. This example is ideal for projects that require C compatibility or cannot use C++ features.

## Features

- **Pure C Implementation**: Uses only C API functions, no C++ wrapper dependencies
- **Device Discovery**: Auto-discovers Aurora devices or connects to specific device
- **Real-time Pose**: Continuously displays device position (x, y, z coordinates)
- **Cross-platform**: Compatible with both Windows and Unix-like systems
- **Minimal Dependencies**: Only requires C11 compiler support

## Requirements

- Aurora device with active positioning system
- Aurora Remote SDK
- C11-compatible compiler (gcc, clang, MSVC)
- Network connection between host and Aurora device

## Usage

### Basic Usage

```bash
# Auto-discover and connect to first available device
./pure_c_demo

# Connect to specific device by IP address
./pure_c_demo 192.168.1.100
```

### Command Line Arguments

- No arguments: Auto-discovers Aurora devices and connects to the first one found
- `<ip_address>`: Direct connection to device at specified IP address (uses default port 8090)

## Example Output

### Auto-discovery Mode
```
Aurora SDK Version: 2.0.0-alpha
Searching for aurora devices...
Connecting to the first server: 192.168.1.100
Current pose: 1.234000, 2.567000, 0.089000
Current pose: 1.235000, 2.568000, 0.089000
Current pose: 1.236000, 2.569000, 0.089000
...
```

### Direct Connection Mode
```
Aurora SDK Version: 2.0.0-alpha
Using connection string: 192.168.1.100
Current pose: 1.234000, 2.567000, 0.089000
Current pose: 1.235000, 2.568000, 0.089000
...
```

## Key Concepts

### Pure C API Usage

This demo demonstrates the core C API functions:

- **Session Management**:
  - `slamtec_aurora_sdk_create_session()`: Creates an SDK session
  - `slamtec_aurora_sdk_release_session()`: Releases session resources

- **Device Discovery**:
  - `slamtec_aurora_sdk_controller_get_discovered_servers()`: Discovers available devices
  
- **Connection Management**:
  - `slamtec_aurora_sdk_controller_connect()`: Connects to a device
  - `slamtec_aurora_sdk_controller_disconnect()`: Disconnects from device

- **Data Retrieval**:
  - `slamtec_aurora_sdk_dataprovider_get_current_pose()`: Gets current device pose

### Data Structures

The demo uses C structures defined in the SDK:
- `slamtec_aurora_sdk_pose_t`: Contains translation (x, y, z) and rotation components
- `slamtec_aurora_sdk_server_connection_info_t`: Server connection information
- `slamtec_aurora_sdk_version_info_t`: SDK version information

## Technical Details

### Cross-platform Sleep
- Windows: Uses `Sleep(milliseconds)`
- Unix/Linux: Uses `sleep(seconds)`

### Error Handling
- Functions return `slamtec_aurora_sdk_errorcode_t` values
- `SLAMTEC_AURORA_SDK_ERRORCODE_OK` indicates success
- Proper cleanup with session release

### Memory Management
- Automatic memory management for basic data types
- Manual session cleanup required for proper resource management

## Integration Example

Basic C integration in your project:

```c
#include "aurora_pubsdk_inc.h"
#include <stdio.h>

int main() {
    // Create session
    slamtec_aurora_sdk_session_handle_t session = 
        slamtec_aurora_sdk_create_session(NULL, 0, NULL, NULL);
    
    if (session == NULL) {
        printf("Failed to create session\n");
        return -1;
    }
    
    // Setup connection info
    slamtec_aurora_sdk_server_connection_info_t info;
    memset(&info, 0, sizeof(info));
    strncpy(info.connection_info[0].address, "192.168.1.100", 
           sizeof(info.connection_info[0].address));
    strncpy(info.connection_info[0].protocol_type, 
           SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PROTOCOL,
           sizeof(info.connection_info[0].protocol_type));
    info.connection_info[0].port = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PORT;
    info.connection_count = 1;
    
    // Connect
    if (slamtec_aurora_sdk_controller_connect(session, &info) == 
        SLAMTEC_AURORA_SDK_ERRORCODE_OK) {
        
        // Get pose
        slamtec_aurora_sdk_pose_t pose;
        slamtec_aurora_sdk_dataprovider_get_current_pose(session, &pose);
        
        printf("Position: %f, %f, %f\n", 
               pose.translation.x, pose.translation.y, pose.translation.z);
        
        // Cleanup
        slamtec_aurora_sdk_controller_disconnect(session);
    }
    
    slamtec_aurora_sdk_release_session(session);
    return 0;
}
```

## Compilation

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

## Advanced Features

### Multiple Device Handling
```c
slamtec_aurora_sdk_server_connection_info_t servers[32];
int count = slamtec_aurora_sdk_controller_get_discovered_servers(session, servers, 32);

for (int i = 0; i < count; i++) {
    printf("Device %d: %s\n", i, servers[i].connection_info[0].address);
}
```

### Error Checking
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

## Use Cases

- **Embedded Systems**: C-only environments without C++ support
- **Legacy Integration**: Integration with existing C codebases
- **Minimal Footprint**: Applications requiring minimal dependencies
- **Real-time Systems**: Low-level control with predictable behavior
- **Cross-language Bindings**: Base for creating bindings in other languages

## Limitations

- Less feature-rich compared to C++ wrapper
- Manual memory management required for complex operations
- No automatic resource management (RAII)
- More verbose error handling

## Best Practices

- Always check return values for error codes
- Properly release sessions and disconnect from devices
- Use appropriate sleep functions for cross-platform compatibility
- Initialize structures with `memset()` before use
- Handle connection failures gracefully