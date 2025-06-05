# Aurora VSLAM Map Save/Load Demo

This demo demonstrates how to save (download) and load (upload) VSLAM maps from Aurora devices using the Aurora Remote SDK. This functionality allows you to backup maps, share maps between devices, or restore previously created maps.

## Features

- **Map Download**: Download VSLAM maps from Aurora device to local files
- **Map Upload**: Upload local VSLAM map files to Aurora device
- **Progress Monitoring**: Real-time progress display during transfer operations
- **Device Discovery**: Auto-discovers Aurora devices or connects to specific device
- **Flexible File Naming**: Specify custom map file names or use defaults
- **Session Management**: Proper session handling with abort capabilities

## Requirements

- Aurora device with VSLAM mapping capability
- Aurora Remote SDK
- Network connection between host and Aurora device
- Sufficient storage space for map files (maps can be several MB to GB)

## Usage

### Basic Usage

```bash
# Download map from device (default behavior)
./vslam_map_saveload

# Download map with custom filename
./vslam_map_saveload -d my_map.stcm

# Upload map to device
./vslam_map_saveload -u existing_map.stcm

# Connect to specific device
./vslam_map_saveload -s tcp://192.168.1.100:8090 -d office_map.stcm
```

### Command Line Options

- `-h, --help`: Show help message and usage information
- `-s, --server <locator>`: Connect to specific device (e.g., `tcp://192.168.1.100:8090`)
- `-d, --download`: Download map from device (default if no operation specified)
- `-u, --upload`: Upload map to device
- `[map_file]`: Specify map filename (default: `auroramap.stcm`)

## Example Output

### Downloading a Map
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

### Uploading a Map
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

## Key Concepts

### VSLAM Maps

VSLAM (Visual Simultaneous Localization and Mapping) maps contain:
- **3D Point Cloud**: Dense or sparse 3D environmental features
- **Keyframes**: Important camera poses and associated visual features
- **Map Structure**: Spatial relationships between features and poses
- **Metadata**: Map creation timestamps, device information, etc.

### Map File Format

- **Extension**: `.stcm` (SLAMTEC Map format)
- **Content**: Binary format containing complete map data
- **Size**: Varies based on environment complexity (typically 10MB - 1GB+)
- **Compatibility**: Maps are device-compatible within the same product family

### Transfer Operations

1. **Download (Device → Local)**:
   - Retrieves active map from device memory
   - Saves to specified local file
   - Useful for backup and archival

2. **Upload (Local → Device)**:
   - Loads map file from local storage
   - Transfers to device memory
   - Replaces current device map

## Technical Details

### Session Management

- **Asynchronous Operations**: Map transfers use async callbacks for non-blocking operation
- **Progress Monitoring**: Real-time progress updates during transfer
- **Abort Capability**: Transfers can be interrupted with Ctrl+C
- **Status Queries**: Continuous status monitoring during active sessions

### Error Handling

- Connection validation before starting transfers
- Progress monitoring with error detection
- Graceful session abort on interruption
- Comprehensive error reporting

## Integration Example

Basic map save/load in your application:

```cpp
#include "aurora_pubsdk_inc.h"

using namespace rp::standalone::aurora;

// Download map from device
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
    
    // Monitor progress
    while (sdk->mapManager.isSessionActive()) {
        slamtec_aurora_sdk_mapstorage_session_status_t status;
        if (sdk->mapManager.querySessionStatus(status)) {
            std::cout << "Progress: " << status.progress << "%" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return resultFuture.get();
}

// Upload map to device
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
    
    // Monitor progress
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



### Storage Considerations

- **Disk Space**: Ensure adequate storage for map files
- **Network Bandwidth**: Large maps require stable network connections
- **Transfer Time**: Factor in transfer time for deployment planning
- **Version Control**: Consider versioning schemes for map management

## Troubleshooting

### Common Issues

1. **Large File Transfers**: Ensure stable network connection for large maps
2. **Storage Space**: Verify sufficient disk space before download
3. **Device Memory**: Ensure device has adequate memory for uploaded maps
4. **File Permissions**: Check read/write permissions for map files

### Best Practices

- Test map transfers with small maps first
- Monitor transfer progress and handle interruptions gracefully
- Implement retry mechanisms for failed transfers
- Validate map integrity after transfers
- Keep backup copies of important maps