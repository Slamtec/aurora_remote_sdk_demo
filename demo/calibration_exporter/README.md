# Aurora Calibration Exporter Demo

This demo demonstrates how to retrieve and export camera calibration and transform calibration parameters from Aurora devices using the Aurora SDK 2.0.

## Features

- **Camera Calibration Export**: Retrieves and exports stereo camera calibration parameters including:
  - Left and right camera intrinsic parameters (focal length, principal point, distortion coefficients)
  - Stereo camera extrinsic parameters (rotation and translation between cameras)
  - Image resolution and baseline information

- **Transform Calibration Export**: Retrieves and exports device transform calibration including:
  - IMU to Camera transformation matrix
  - External Camera transformation matrix

- **Multiple Output Formats**: Supports both XML and YAML formats for OpenCV compatibility

- **Command Line Interface**: Full command-line support with options for output directory and format selection

## Requirements

- Aurora device with firmware supporting Aurora SDK 2.0
- OpenCV 4.2 or higher
- Aurora Remote SDK 2.0

## Usage

### Basic Usage

```bash
# Auto-discover device and save to current directory as XML
./calibration_exporter

# Specify output directory
./calibration_exporter -o ./calibration_data

# Save as YAML format
./calibration_exporter -f yml

# Connect to specific device
./calibration_exporter tcp://192.168.1.100:8090
```

### Command Line Options

- `-h, --help`: Show help message
- `-o, --output <dir>`: Specify output directory (default: current directory)
- `-f, --format <fmt>`: Output format - xml or yml (default: xml)

### Output Files

The demo generates the following calibration files:

1. **left_camera_calibration.xml/yml**: Left camera intrinsic parameters
2. **right_camera_calibration.xml/yml**: Right camera intrinsic parameters  
3. **stereo_calibration.xml/yml**: Complete stereo camera setup parameters
4. **transform_calibration.xml/yml**: Device transform calibration data

## Example Output

```
Aurora Calibration Exporter
Output directory: ./calibration
Output format: xml
Connected to the selected device

=== Camera Calibration Summary ===
Left Camera:
  Resolution: 640x480
  Focal Length: fx=525.67, fy=526.34
  Principal Point: cx=320.12, cy=240.67
Right Camera:
  Resolution: 640x480
  Focal Length: fx=524.89, fy=525.78
  Principal Point: cx=319.45, cy=241.23
Stereo Parameters:
  Baseline: 120.5 mm

=== Transform Calibration Summary ===
IMU to Camera Translation: [0.025, 0.012, -0.008]
External Camera Translation: [0.000, 0.000, 0.000]
===================================

Left camera calibration exported to: ./calibration/left_camera_calibration.xml
Right camera calibration exported to: ./calibration/right_camera_calibration.xml
Stereo calibration exported to: ./calibration/stereo_calibration.xml
Transform calibration exported to: ./calibration/transform_calibration.xml

Calibration export completed successfully!
```

## Integration with OpenCV

The exported calibration files can be directly loaded in OpenCV applications:

```cpp
// Load camera calibration
cv::FileStorage fs("left_camera_calibration.xml", cv::FileStorage::READ);
cv::Mat cameraMatrix, distCoeffs;
fs["camera_matrix"] >> cameraMatrix;
fs["distortion_coefficients"] >> distCoeffs;
int imageWidth, imageHeight;
fs["image_width"] >> imageWidth;
fs["image_height"] >> imageHeight;
fs.release();

// Use for camera rectification, stereo vision, etc.
```

