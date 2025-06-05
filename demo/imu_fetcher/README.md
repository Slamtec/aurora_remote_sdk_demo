# Aurora IMU Data Fetcher Demo

This demo demonstrates how to retrieve and display IMU (Inertial Measurement Unit) data from an Aurora device using the Aurora Remote SDK. The IMU provides acceleration and angular velocity measurements essential for motion tracking and sensor fusion.

## Features

- **Real-time IMU Data**: Continuously fetches accelerometer and gyroscope data at high frequency
- **Device Discovery**: Auto-discovers Aurora devices on the network or connects to a specific device
- **Data Filtering**: Filters out duplicate data using timestamp comparison
- **Non-blocking Access**: Uses cached IMU data for efficient, non-blocking data retrieval
- **Comprehensive Output**: Displays 3-axis acceleration and angular velocity measurements

## Requirements

- Aurora device with IMU sensor
- Aurora Remote SDK
- Network connection between host and Aurora device

## Usage

### Basic Usage

```bash
# Auto-discover and connect to first available device
./imu_fetcher

# Connect to specific device
./imu_fetcher tcp://192.168.1.100:8090
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
IMU Data: Accel: -0.123, 0.456, 9.789 Gyro: 0.001, -0.002, 0.003
IMU Data: Accel: -0.124, 0.457, 9.791 Gyro: 0.002, -0.001, 0.003
IMU Data: Accel: -0.125, 0.458, 9.788 Gyro: 0.001, -0.003, 0.004
...
```

## Key Concepts

### IMU Data Structure

The demo retrieves `slamtec_aurora_sdk_imu_data_t` structures containing:
- **Acceleration** (`acc[3]`): 3-axis linear acceleration in m/s²
  - acc[0]: X-axis acceleration
  - acc[1]: Y-axis acceleration  
  - acc[2]: Z-axis acceleration (typically includes gravity ~9.8 m/s²)
- **Angular Velocity** (`gyro[3]`): 3-axis angular velocity in rad/s
  - gyro[0]: Roll rate (rotation around X-axis)
  - gyro[1]: Pitch rate (rotation around Y-axis)
  - gyro[2]: Yaw rate (rotation around Z-axis)
- **Timestamp**: High-precision timestamp in nanoseconds

### Data Flow

1. **IMU Sampling**: The device continuously samples IMU data at high frequency
2. **SDK Caching**: The SDK caches IMU data for efficient access
3. **Non-blocking Retrieval**: `peekIMUData()` provides immediate access to cached data
4. **Timestamp Filtering**: Only new data (newer timestamps) is processed
5. **Real-time Display**: IMU values are continuously displayed


## Integration Example

Basic IMU data retrieval in your application:

```cpp
#include "aurora_pubsdk_inc.h"

using namespace rp::standalone::aurora;

// Create SDK session and connect
RemoteSDK* sdk = RemoteSDK::CreateSession();
SDKServerConnectionDesc deviceDesc("tcp://192.168.1.100:8090");

if (sdk->connect(deviceDesc)) {
    uint64_t lastTimestamp = 0;
    
    // Retrieve IMU data continuously
    std::vector<slamtec_aurora_sdk_imu_data_t> imuData;
    if (sdk->dataProvider.peekIMUData(imuData)) {
        for (const auto& imu : imuData) {
            if (imu.timestamp_ns > lastTimestamp) {
                lastTimestamp = imu.timestamp_ns;
                
                // Process acceleration data
                double accel_x = imu.acc[0];
                double accel_y = imu.acc[1]; 
                double accel_z = imu.acc[2];
                
                // Process gyroscope data
                double gyro_x = imu.gyro[0];
                double gyro_y = imu.gyro[1];
                double gyro_z = imu.gyro[2];
                
                // Your processing logic here...
            }
        }
    }
}

sdk->disconnect();
sdk->release();
```

### Alternative: Event-Driven IMU Data

For more advanced applications, consider using listeners:

```cpp
// Set up listener for event-driven IMU data access
class IMUListener : public RemoteSDKListener {
public:
    void onIMUDataUpdated(const std::vector<slamtec_aurora_sdk_imu_data_t>& imuData) override {
        // Process IMU data as it arrives
        for (const auto& imu : imuData) {
            // Handle each IMU sample...
        }
    }
};

IMUListener listener;
sdk->setListener(&listener);
```

## Advanced Usage

### IMU Data Analysis

Common IMU processing tasks:

```cpp
// Calculate magnitude of acceleration
double accel_magnitude = sqrt(imu.acc[0]*imu.acc[0] + 
                             imu.acc[1]*imu.acc[1] + 
                             imu.acc[2]*imu.acc[2]);

// Detect motion
bool isMoving = accel_magnitude > 9.8 + threshold; // Above gravity + threshold

// Calculate rotation magnitude  
double gyro_magnitude = sqrt(imu.gyro[0]*imu.gyro[0] + 
                            imu.gyro[1]*imu.gyro[1] + 
                            imu.gyro[2]*imu.gyro[2]);

// Detect rotation
bool isRotating = gyro_magnitude > rotation_threshold;
```

## Use Cases

- **Motion Detection**: Detect when the device is moving or stationary
- **Orientation Tracking**: Monitor device orientation changes
- **Vibration Analysis**: Analyze vibrations and mechanical disturbances
- **Sensor Fusion**: Combine IMU data with other sensors for enhanced tracking
- **Fall Detection**: Detect sudden changes in acceleration patterns
- **Navigation**: Inertial navigation and dead reckoning
- **Calibration**: IMU calibration and bias estimation

## Performance Notes

- **High Frequency Data**: IMU generates high-frequency data; consider processing requirements
- **Timestamp Management**: Always use timestamps to ensure proper data ordering
- **Memory Efficiency**: Process data promptly to avoid excessive memory usage
- **Real-time Processing**: IMU data is time-sensitive; minimize processing delays