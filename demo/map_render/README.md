# Aurora Map Render Demo

![Map Render Demo](../../res/demo_vertical_map.gif)

This demo demonstrates how to retrieve and visualize map data from Aurora devices using OpenCV rendering.

## Features

- **2D Map Visualization**: Renders overview mode point cloud maps in real-time
- **Interactive Display**: Navigate and zoom through map data
- **Multiple Map Types**: Supports different map representations
- **Real-time Updates**: Shows live map updates during mapping

## Requirements

- Aurora device with mapping capability
- Aurora Remote SDK  
- OpenCV 4.2 or higher
- Network connection

## Usage

```bash
# Auto-discover and render map
./map_render

# Connect to specific device
./map_render tcp://192.168.1.100:8090
```

## Key Features

- **Point Cloud Rendering**: Visualizes 3D environmental structure
- **Map Navigation**: Interactive viewing controls
- **Color Coding**: Different colors for map features
- **Scale Adjustment**: Zoom and pan capabilities

## Use Cases

- **Map Verification**: Visual verification of mapping quality
- **Development**: Real-time mapping feedback
- **Debugging**: Map structure analysis
- **Presentation**: Demonstration of mapping capabilities