# Semantic Segmentation Demo

![Semantic Segmentation Demo](../../res/demo_sematic_seg.gif)

This demo showcases the Enhanced Imaging capabilities of Aurora SDK 2.0, specifically the semantic segmentation functionality. It demonstrates how to capture, visualize, and interact with semantic segmentation data from Aurora devices equipped with DNN-based semantic segmentation models.

## Features

- **Real-time Semantic Segmentation**: Displays segmentation maps with colorized visualization for different object classes
- **Interactive Object Detection**: Mouse hover shows object contours and class labels
- **Model Switching**: Toggle between default (80 classes) and alternative (18 classes) models
- **Camera Image Overlay**: Overlay segmentation results on camera images for spatial context
- **Depth Camera Alignment**: Display segmentation maps aligned with depth camera coordinate system
- **Automatic Device Discovery**: Discovers and connects to Aurora devices automatically
- **Frame Rate Monitoring**: Real-time FPS display for performance monitoring

## Requirements

- Aurora device with semantic segmentation support
- OpenCV 4.2 or later
- Aurora Remote SDK 2.0+
- Stable network connection to Aurora device (Ethernet recommended for best performance)

## Building

The demo is built automatically when OpenCV is available:

```bash
cd build
cmake ..
make semantic_segmentation
```

## Usage

### Basic Usage

```bash
./semantic_segmentation [connection_string]
```

- If no connection string is provided, the demo will automatically discover Aurora devices
- The demo will connect to the first discovered device

### Manual Device Connection

```bash
./semantic_segmentation "tcp://192.168.1.100:1445"
```

Replace the IP address with your Aurora device's IP address.

### Controls

- **ESC**: Exit the application
- **'m'**: Switch between default and alternative models
- **Mouse hover**: Show object contours and class labels in segmentation map
- **Ctrl+C**: Graceful shutdown

## Output

### Visual Display

The demo opens two windows:

1. **Segmentation & Camera Overlay**: Merged display combining segmentation and camera data
   - Camera image background with colorized segmentation overlay
   - Background areas remain as pure camera image (transparent segmentation)
   - Mouse hover shows object contours in white with instant response
   - Hovered object class label is displayed at the top
   - Model switching status shown with color-coded messages
   - Control instructions displayed on screen: "'m' - Switch between default and alternative model"
   - Frame count and model information shown at the bottom

2. **Depth Aligned Segmentation**: Segmentation map aligned to depth camera coordinate system
   - Only available on devices with depth camera support
   - Useful for 3D scene understanding applications

### Console Output

The demo provides comprehensive console information:

- Device discovery and connection status
- Semantic segmentation configuration (FPS, frame skip, image size)
- Label set information and class names
- Current model status (Default/Alternative)
- Frame rate monitoring
- Model switching confirmations

## Technical Details

### Segmentation Models

Aurora devices support two semantic segmentation models:

1. **Default Model** (General Indoor Applications):
   - 80 object classes
   - Optimized for indoor environments
   - Includes furniture, electronics, people, etc.

2. **Alternative Model** (Outdoor Applications):
   - 18 object classes  
   - Optimized for outdoor environments
   - Includes vehicles, road signs, buildings, etc.

### Object Classes

Class information is retrieved dynamically from the device:
- Class 0 is typically background (labeled as "(null)" or "Background")
- Class names and indices are model-dependent
- Full class list is printed to console during startup and model switches

### Interactive Features

#### Mouse Hover Detection
- Move mouse over the merged segmentation & camera overlay to highlight objects
- Object contours are drawn manually using line segments for reliability
- Background areas (null class) do not show contours when hovered
- Class label appears at the top of the window with color coding (yellow for objects, gray for background)
- Real-time object identification with robust contour rendering

#### Model Switching
- Press 'm' to toggle between models
- Model switching progress is displayed visually in the OpenCV window
- Switch progress shows: "Switching to [Model] model..." (yellow text)
- Success confirmation: "Model switch completed successfully!" (green text)
- Error notification: "Failed to switch model!" (red text)
- Model switching may take several seconds
- New label information is automatically retrieved and displayed
- Colors are regenerated for the new model's classes

### Data Processing

- **Optimized Rendering**: Pre-computed overlay base for fast mouse interaction
- **Colorization**: Random colors generated for each class (background is black for transparency)
- **Background Handling**: Background (null class) is rendered as black and treated as transparent in overlays
- **Contour Detection**: Manual contour drawing with bounds checking for reliable object highlighting
- **Image Alignment**: SDK-provided depth camera alignment functionality
- **Vectorized Blending**: OpenCV-optimized blending using mask operations for better performance

## Error Handling

The demo includes comprehensive error handling:

- Device connection failures
- Semantic segmentation support verification
- Frame acquisition timeouts
- Model switching failures
- Invalid frame data detection
- Graceful shutdown on interruption


## Troubleshooting

### Common Issues

1. **"Semantic segmentation is not supported"**
   - Ensure your Aurora device has semantic segmentation capability
   - Check firmware version compatibility

2. **"Failed to subscribe to semantic segmentation"**
   - Verify network connection stability
   - Check if another application is using the device

3. **"Failed to switch model"**
   - Ensure device supports both models
   - Wait for current frame processing to complete

4. **Poor segmentation quality**
   - Ensure adequate lighting conditions
   - Check camera lens for obstruction
   - Verify scene is within model's training domain (indoor vs outdoor)

### Debug Information

The demo provides console output for:
- Device discovery and connection status
- Segmentation configuration parameters
- Model switching progress
- Frame acquisition rates (FPS)
- Label set and class information
- Error messages with specific error codes

## Example Output

```
Device connection string not provided, try to discover aurora devices...
Waiting for aurora devices...
Found 1 aurora devices
Device 0
  option 0: tcp/[fe80::ad94:89de:cef2:dcb4]:7447
  option 1: tcp/192.168.1.212:7447
Selected first device: 
Connecting to the selected device...
Connected to the selected device
Semantic segmentation is supported
Semantic Segmentation Config:
  FPS: 7.5
  Frame Skip: 1
  Image Size: 640x480
Waiting for semantic segmentation to be ready...

=== Semantic Segmentation Label Information ===
Label Set: coco80
Total Classes: 81
Classes:
  [ 0] Background
  [ 1] person
...
  [79] hair drier
  [80] toothbrush
================================================

Current model: Default

Controls:
  ESC - Exit
  'm' - Switch between default and alternative model
  Mouse hover - Show object contours and labels (in merged segmentation & camera overlay)
Segmentation FPS: 7.46269 | Frame: 30
Segmentation FPS: 7.51691 | Frame: 60
Segmentation FPS: 7.4664 | Frame: 90
Segmentation FPS: 7.55097 | Frame: 120
Segmentation FPS: 7.42758 | Frame: 150
Segmentation FPS: 7.57576 | Frame: 180
```

## Integration Notes

This demo can serve as a foundation for:
- Object detection and tracking applications
- Autonomous navigation systems
- Augmented reality applications
- Scene understanding projects
- Smart home/office automation
- Robotics perception systems

The code demonstrates best practices for Aurora SDK 2.0 Enhanced Imaging usage and can be adapted for specific application requirements.

