/* 
 *  SLAMTEC AURORA DEMO
 *  This demo shows how to capture the depth camera frames using Enhanced Imaging API
 *  and display them using OpenCV with color mapping for better visualization
 *  A stable connection to the Aurora device with depth camera support is required
 */

#include <iostream>

#include "aurora_pubsdk_inc.h"

#include <chrono>
#include <thread>

#include <opencv2/opencv.hpp>

#include <signal.h>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <sstream>
using namespace rp::standalone::aurora;

static int isCtrlC = 0;

// Structure to hold 3D point data
struct Point3D {
    float x, y, z;
    uint8_t r, g, b;
};


static void onCtrlC(int) {
    std::cout << "Ctrl-C pressed, exiting..." << std::endl;
    isCtrlC = 1;
}

static bool discoverAndSelectAuroraDevice(RemoteSDK * sdk, SDKServerConnectionDesc & selectedDeviceDesc)
{
    std::vector<SDKServerConnectionDesc> serverList;
    size_t count = sdk->getDiscoveredServers(serverList, 32);
    if (count == 0) {
        std::cerr << "No aurora devices found" << std::endl;
        return false;
    }

    // print the server list
    std::cout << "Found " << count << " aurora devices" << std::endl;
    for (size_t i = 0; i < count; i++) {
        std::cout << "Device " << i << std::endl;
        for (size_t j = 0; j < serverList[i].size(); ++j)
        {
            auto & connectionOption = serverList[i][j];
            std::cout << "  option " << j << ": " << connectionOption.toLocatorString() << std::endl;
        }
    }

    // select the first device
    selectedDeviceDesc = serverList[0];
    std::cout << "Selected first device: " << std::endl;
    return true;
}

// Generate 3D point cloud from SDK point cloud data and texture image
std::vector<Point3D> generatePointCloud(const RemoteEnhancedImagingFrame& point3dFrame, const cv::Mat& textureMat) {
    std::vector<Point3D> pointCloud;
#if 0
    // using non-opencv implementation to retrieve the point cloud data
    
    // Get the 3D points directly from the SDK
    const auto* points3d = point3dFrame.image.toPoint3D();
    if (!points3d) {
        std::cerr << "Failed to get 3D points from frame" << std::endl;
        return pointCloud;
    }
    
    size_t pointCount = point3dFrame.image.getPointCount();
    int width = point3dFrame.desc.image_desc.width;
    int height = point3dFrame.desc.image_desc.height;
    
    // Reserve space for efficiency
    pointCloud.reserve(pointCount);
    
    for (size_t i = 0; i < pointCount; i++) {
        const auto& point = points3d[i];
        
        // Skip invalid points (NaN or points too far away)
        if (std::isnan(point[0]) || std::isnan(point[1]) || std::isnan(point[2]) || 
            point[2] <= 0.0f) {
            continue;
        }
        
        Point3D cloudPoint;
        cloudPoint.x = point[0];
        cloudPoint.y = point[1];
        cloudPoint.z = point[2];
        
        // Calculate pixel coordinates for texture mapping
        int u = i % width;
        int v = i / width;
        
        // Get color from texture image
        if (!textureMat.empty() && u < textureMat.cols && v < textureMat.rows) {
            cv::Vec3b color = textureMat.at<cv::Vec3b>(v, u);
            cloudPoint.b = color[0];  // BGR to RGB
            cloudPoint.g = color[1];
            cloudPoint.r = color[2];
        } else {
            // Default gray color if texture is not available
            cloudPoint.r = cloudPoint.g = cloudPoint.b = 128;
        }
        
        pointCloud.push_back(cloudPoint);
    }
#else
    // using opencv implementation to retrieve the point cloud data
    cv::Mat pointCloudMat;
    point3dFrame.image.toMat(pointCloudMat);
    
    for (int y = 0; y < pointCloudMat.rows; y++) {
        for (int x = 0; x < pointCloudMat.cols; x++) {
            const auto& point = pointCloudMat.at<cv::Vec3f>(y, x);
            if (std::isnan(point[0]) || std::isnan(point[1]) || std::isnan(point[2]) || 
                std::isinf(point[0]) || std::isinf(point[1]) || std::isinf(point[2]) ||
                point[2] <= 0.0f) {
                continue;
            }
        
            Point3D cloudPoint;
            cloudPoint.x = point[0];
            cloudPoint.y = point[1];
            cloudPoint.z = point[2];

            if (textureMat.channels() == 1) {
                cloudPoint.b = cloudPoint.g = cloudPoint.r = textureMat.at<uint8_t>(y, x);
            } else {
                const auto& color = textureMat.at<cv::Vec3b>(y, x);
                cloudPoint.b = color[0];
                cloudPoint.g = color[1];
                cloudPoint.r = color[2];
            }

            pointCloud.push_back(cloudPoint);
        }
    }


#endif

    return pointCloud;
}

// Save point cloud to PLY format (simple text format)
bool savePointCloudToPLY(const std::vector<Point3D>& pointCloud, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }
    
    // Write PLY header
    file << "ply\n";
    file << "format ascii 1.0\n";
    file << "element vertex " << pointCloud.size() << "\n";
    file << "property float x\n";
    file << "property float y\n";
    file << "property float z\n";
    file << "property uchar red\n";
    file << "property uchar green\n";
    file << "property uchar blue\n";
    file << "end_header\n";
    
    // Write point data
    for (const auto& point : pointCloud) {
        file << std::fixed << std::setprecision(6) 
             << point.x << " " << point.y << " " << point.z << " "
             << static_cast<int>(point.r) << " " 
             << static_cast<int>(point.g) << " " 
             << static_cast<int>(point.b) << "\n";
    }
    
    file.close();
    return true;
}

int main(int argc, const char* argv[]) {
    // register the ctrl-c signal handler
    signal(SIGINT, onCtrlC);

    const char* connectionString = nullptr;
    if (argc > 1) {
        connectionString = argv[1];
    }

    RemoteSDK * sdk = RemoteSDK::CreateSession();
    if (sdk == nullptr) {
        std::cerr << "Failed to create session" << std::endl;
        return 1;
    }

    // wait for the sdk to detect aurora devices
    SDKServerConnectionDesc selectedDeviceDesc;
    if (connectionString == nullptr) {
        std::cout << "Device connection string not provided, try to discover aurora devices..." << std::endl;
        std::cout << "Waiting for aurora devices..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));

        if (!discoverAndSelectAuroraDevice(sdk, selectedDeviceDesc)) {
            std::cerr << "Failed to discover aurora devices" << std::endl;
            return 1;
        }
    } else {
        selectedDeviceDesc = SDKServerConnectionDesc(connectionString);
        std::cout << "Selected device: " << selectedDeviceDesc[0].toLocatorString() << std::endl;
    }

    // connect to the selected device
    std::cout << "Connecting to the selected device..." << std::endl;
    if (!sdk->connect(selectedDeviceDesc)) {
        std::cerr << "Failed to connect to the selected device" << std::endl;
        return 1;
    }
    std::cout << "Connected to the selected device" << std::endl;

    // Check if depth camera is supported
    if (!sdk->enhancedImaging.isDepthCameraSupported()) {
        std::cerr << "Depth camera is not supported by this device" << std::endl;
        sdk->disconnect();
        sdk->release();
        return 1;
    }
    std::cout << "Depth camera is supported" << std::endl;

    // subscribe to the depth camera frame first, otherwise the depth data won't be transferred to the client
    if (!sdk->controller.setEnhancedImagingSubscription(SLAMTEC_AURORA_SDK_ENHANCED_IMAGE_TYPE_DEPTH, true)) {
        std::cerr << "Failed to subscribe to depth camera frame" << std::endl;
        sdk->disconnect();
        sdk->release();
        return 1;
    }

    cv::namedWindow("Depth Map", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Depth Map (Overlay)", cv::WINDOW_AUTOSIZE);

    size_t frameCount = 0;
    auto startTime = std::chrono::steady_clock::now();

    // Main loop: peek depth camera frames and display them
    std::cout << "Controls: ESC to exit, 's' to save current frame as point cloud" << std::endl;
    
    int key;
    while ((key = cv::waitKey(10)) != 27) {  // ESC key to exit
        if (isCtrlC) {
            break;
        }

        RemoteEnhancedImagingFrame depthFrame;
        slamtec_aurora_sdk_errorcode_t errorCode;
        
        if (!sdk->enhancedImaging.waitDepthCameraNextFrame(1000)) {
            continue;
        }

        // Peek depth map frame (using DEPTH_MAP type for visualization)
        if (!sdk->enhancedImaging.peekDepthCameraFrame(depthFrame, SLAMTEC_AURORA_SDK_DEPTHCAM_FRAME_TYPE_DEPTH_MAP, &errorCode)) {
            if (errorCode != SLAMTEC_AURORA_SDK_ERRORCODE_NOT_READY) {
                std::cerr << "Failed to peek depth camera frame, error code: " << errorCode << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            continue;
        }
        
        // Also peek the 3D point cloud for saving functionality
        RemoteEnhancedImagingFrame point3dFrame;
        if (!sdk->enhancedImaging.peekDepthCameraFrame(point3dFrame, SLAMTEC_AURORA_SDK_DEPTHCAM_FRAME_TYPE_POINT3D, &errorCode)) {
            std::cerr << "Failed to peek point3d frame, error code: " << errorCode << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            continue;
        }
        
        // Peek for the related texture frame
        RemoteEnhancedImagingFrame textureFrame;
        if (!sdk->enhancedImaging.peekDepthCameraRelatedRectifiedImage(textureFrame, depthFrame.desc.timestamp_ns, &errorCode)) {
            // No texture frame is available, you may need to wait for the next frame
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
        
        frameCount++;

        // Convert to OpenCV Mat
        cv::Mat depthMat;
        depthFrame.image.toMat(depthMat);

        if (!depthMat.empty()) {
            cv::Mat heatMap;
            depthMat.convertTo(heatMap, CV_8UC1, -255.0 / 5.0f, 255);


            cv::Mat imgColor;
            cv::applyColorMap(heatMap, imgColor, cv::COLORMAP_JET);

            // Add frame information
            std::string frameInfo = "ESC to exit, 's' to save point cloud";


            cv::putText(imgColor, frameInfo, cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 255), 1);
            cv::imshow("Depth Map", imgColor);


            cv::Mat textureMatBGR;
            textureFrame.image.toMat(textureMatBGR);

            if ( (!textureMatBGR.empty()) && textureMatBGR.channels() == 1) {
                cv::cvtColor(textureMatBGR, textureMatBGR, cv::COLOR_GRAY2BGR);
            }

            cv::addWeighted(imgColor, 0.5, textureMatBGR, 0.5, 0, imgColor);
            cv::imshow("Depth Map (Overlay)", imgColor);
            
            // Handle 's' key press to save point cloud
            if (key == 's' || key == 'S') {
                if (!textureMatBGR.empty()) {
                    std::cout << "Generating point cloud..." << std::endl;
                    auto pointCloud = generatePointCloud(point3dFrame, textureMatBGR);
                    
                    if (!pointCloud.empty()) {
                        // Generate filename with timestamp
                        auto now = std::chrono::system_clock::now();
                        auto time_t = std::chrono::system_clock::to_time_t(now);
                        std::stringstream ss;
                        ss << "pointcloud_" << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S") << ".ply";
                        std::string filename = ss.str();
                        
                        if (savePointCloudToPLY(pointCloud, filename)) {
                            std::cout << "Point cloud saved to " << filename 
                                      << " (" << pointCloud.size() << " points)" << std::endl;
                        } else {
                            std::cerr << "Failed to save point cloud to " << filename << std::endl;
                        }
                    } else {
                        std::cout << "No valid points found in current frame" << std::endl;
                    }
                } else {
                    std::cout << "No valid frame data available for point cloud generation" << std::endl;
                }
            }

            // Calculate and display FPS every 30 frames
            if (frameCount % 30 == 0) {
                auto currentTime = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
                double fps = 30000.0 / duration.count();
                std::cout << "Depth camera FPS: " << fps << " | Frame: " << frameCount << std::endl;
                startTime = currentTime;
            }
        }
    }

    cv::destroyAllWindows();
    sdk->disconnect();
    sdk->release();

    std::cout << "Depth camera demo completed. Total frames processed: " << frameCount << std::endl;
    return 0;
}