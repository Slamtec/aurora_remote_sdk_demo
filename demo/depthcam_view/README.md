# Depth Camera View Demo

![depthcam_view](../../res/demo_depthcam.gif)

This demo showcases the Enhanced Imaging capabilities of Aurora SDK 2.0, specifically the depth camera functionality. It demonstrates how to capture, visualize, and save 3D point cloud data from Aurora devices equipped with depth cameras.

## Features

- **Real-time Depth Visualization**: Displays depth maps with color mapping for better visualization
- **Texture Overlay**: Shows depth data overlaid with rectified camera images
- **3D Point Cloud Export**: Save current frame as colored 3D point cloud in PLY format
- **Automatic Device Discovery**: Discovers and connects to Aurora devices automatically
- **Frame Rate Monitoring**: Real-time FPS display for performance monitoring

## Requirements

- Aurora device with depth camera support
- OpenCV 4.2 or later
- Aurora Remote SDK 2.0+
- Stable network connection to Aurora device (Ethernet recommended for best performance)

## Building

The demo is built automatically when OpenCV is available:

```bash
cd build
cmake ..
make depthcam_view
```

## Usage

### Basic Usage

```bash
./depthcam_view [connection_string]
```

- If no connection string is provided, the demo will automatically discover Aurora devices
- The demo will connect to the first discovered device

### Manual Device Connection

```bash
./depthcam_view "tcp://192.168.1.100:1445"
```

Replace the IP address with your Aurora device's IP address.

### Controls

- **ESC**: Exit the application
- **'s'**: Save current frame as 3D point cloud (PLY format)
- **Ctrl+C**: Graceful shutdown

## Output

### Visual Display

The demo opens two windows:

1. **Depth Map**: Color-mapped depth visualization using COLORMAP_JET
   - Blue/Purple: Close objects
   - Green/Yellow: Medium distance
   - Red: Far objects

2. **Depth Map (Overlay)**: Depth data overlaid with texture from rectified camera image
   - Provides spatial context for depth information
   - Useful for understanding scene geometry

### Point Cloud Files

When 's' key is pressed, the demo saves a 3D point cloud file:

- **Format**: PLY (Polygon File Format) - ASCII encoding
- **Filename**: `pointcloud_YYYYMMDD_HHMMSS.ply`
- **Content**: 3D coordinates (X, Y, Z) with RGB color information
- **Coordinate System**: Camera coordinate system (Z forward, X right, Y down)

#### PLY File Structure

```
ply
format ascii 1.0
element vertex [number_of_points]
property float x
property float y
property float z
property uchar red
property uchar green
property uchar blue
end_header
[point data...]
```

## Viewing Point Clouds

The generated PLY files can be viewed with various 3D visualization software:

- **MeshLab**: Free, cross-platform mesh processing tool
- **CloudCompare**: Point cloud processing software
- **PCL Viewer**: Point Cloud Library viewer
- **Blender**: 3D modeling software (import PLY files)
- **ParaView**: Scientific data visualization

## Technical Details

### Depth Camera Frame Types

The demo uses two Enhanced Imaging frame types:

1. **SLAMTEC_AURORA_SDK_DEPTHCAM_FRAME_TYPE_DEPTH_MAP**: 
   - Raw depth values for visualization
   - Used for color-mapped depth

2. **SLAMTEC_AURORA_SDK_DEPTHCAM_FRAME_TYPE_POINT3D**:
   - Pre-computed 3D points using device calibration
   - Used for point cloud generation and export

### Point Cloud Generation

The demo provides two implementation approaches (selectable via preprocessor directive):

1. **OpenCV-based** (default, `#else` branch):
   - Uses `RemoteImageRef::toMat()` to get OpenCV Mat
   - Iterates through 2D array of 3D points
   - More familiar for OpenCV users

2. **Direct SDK access** (optional, `#if 0` branch):
   - Uses `RemoteImageRef::toPoint3D()` for direct array access
   - Potentially more efficient for large point clouds
   - Lower-level SDK access

### Data Processing

- **Invalid Point Filtering**: Removes NaN, infinite, and zero-depth points
- **Color Mapping**: Maps texture image colors to 3D points
- **Distance Filtering**: Can be customized to filter points beyond certain distances
- **Memory Optimization**: Efficient point cloud generation and storage

## Error Handling

The demo includes comprehensive error handling:

- Device connection failures
- Frame acquisition timeouts
- Invalid depth data detection
- File I/O errors for point cloud saving
- Graceful shutdown on interruption

## Performance Considerations

- **Network Bandwidth**: Depth camera data can consume significant bandwidth
- **Processing Power**: Real-time depth processing requires adequate CPU
- **Memory Usage**: Large point clouds may require substantial RAM
- **Frame Rate**: Typical operation at 10-30 FPS depending on hardware

## Troubleshooting

### Common Issues

1. **"Depth camera is not supported"**
   - Ensure your Aurora device has depth camera capability
   - Check firmware version compatibility

2. **"Failed to subscribe to depth camera frame"**
   - Verify network connection stability
   - Check if another application is using the device

3. **"No valid points found in current frame"**
   - Ensure adequate lighting conditions
   - Check if objects are within depth camera range
   - Verify device positioning and calibration

4. **Poor point cloud quality**
   - Improve lighting conditions
   - Ensure objects have sufficient texture
   - Check device calibration status

### Debug Information

The demo provides console output for:
- Device discovery and connection status
- Frame acquisition rates (FPS)
- Point cloud generation progress
- File save confirmations
- Error messages with specific error codes

## Example Output

```
Device connection string not provided, try to discover aurora devices...
Waiting for aurora devices...
Found 1 aurora devices
Device 0
  option 0: tcp://192.168.1.100:1445
Selected first device:
Connecting to the selected device...
Connected to the selected device
Depth camera is supported
Controls: ESC to exit, 's' to save current frame as point cloud
Depth camera FPS: 14.9701 | Frame: 30
Generating point cloud...
Point cloud saved to pointcloud_20231225_143052.ply (45234 points)
```

## Integration Notes

This demo can serve as a foundation for:
- 3D scanning applications
- Robotics navigation systems
- Augmented reality applications
- Object recognition and measurement
- Scene reconstruction projects

The code demonstrates best practices for Aurora SDK 2.0 Enhanced Imaging usage and can be adapted for specific application requirements.