# Aurora Device Info Monitor Demo

This demo demonstrates how to continuously monitor and display Aurora device basic information using the Aurora SDK 2.0.

## Features

- **Real-time Device Monitoring**: Continuously fetches and displays device information with 1-second intervals

- **Comprehensive Device Info**: Displays the following device information:
  - Device Name
  - Device Serial Number
  - Device Model
  - Firmware Version
  - Hardware Version
  - Feature Set
  - Device Uptime (formatted as days, hours, minutes, seconds)

- **Clean Console Interface**: Clear, formatted output with timestamps and update counters

- **Feature Set Interpretation**: Automatically interprets device feature flags into readable capabilities

## Requirements

- Aurora device with firmware supporting Aurora SDK 2.0
- Aurora Remote SDK 2.0
- Terminal or console with ANSI escape code support (for screen clearing)

## Usage

### Basic Usage

```bash
# Auto-discover device and start monitoring
./device_info_monitor

# Connect to specific device
./device_info_monitor tcp://192.168.1.100:8090

# Show help
./device_info_monitor --help
```

### Command Line Options

- `-h, --help`: Show help message and usage examples

## Example Output

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


