/* 
 *  SLAMTEC AURORA DEMO
 *  This demo shows how to capture and visualize semantic segmentation data using Enhanced Imaging API
 *  Features include:
 *  - Real-time segmentation map display with colorization
 *  - Mouse hover object contour detection and label display
 *  - Model switching between default and alternative models
 *  - Camera image overlay functionality
 *  - Depth camera aligned segmentation map display
 */

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <random>
#include <chrono>
#include <thread>
#include <signal.h>
#include <iomanip>

#include "aurora_pubsdk_inc.h"
#include <opencv2/opencv.hpp>

using namespace rp::standalone::aurora;

static int isCtrlC = 0;

// Global variables for current frame data
static RemoteEnhancedImagingFrame currentSegFrame;
static cv::Mat currentCameraImage;
static cv::Mat currentColorizedSegMap;
static cv::Mat currentSegMap;
static cv::Mat currentOverlayBase; // Pre-computed overlay without contours
static cv::Mat currentAlignedSegMap;
static bool hasValidFrames = false;

// Model switching state
static bool isModelSwitching = false;
static std::string modelSwitchStatus = "";

// Segmentation configuration and labels
static slamtec_aurora_sdk_semantic_segmentation_config_info_t segConfig;
static slamtec_aurora_sdk_semantic_segmentation_label_info_t labelInfo;
static std::string labelSetName;
static std::vector<cv::Vec3b> classColors;
static bool isUsingAlternativeModel = false;

// Mouse interaction
static cv::Point lastMousePos(-1, -1);
static std::string hoveredLabel = "";

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

// Generate random colors for each segmentation class
void generateClassColors(int numClasses) {
    classColors.clear();
    classColors.resize(numClasses);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(50, 255);
    
    // Set background (index 0) to black for transparency effect
    classColors[0] = cv::Vec3b(0, 0, 0);
    
    // Generate random colors for other classes
    for (int i = 1; i < numClasses; i++) {
        classColors[i] = cv::Vec3b(dis(gen), dis(gen), dis(gen));
    }
}

// Colorize segmentation map using class colors
cv::Mat colorizeSegmentationMap(const cv::Mat& segMap) {
    cv::Mat colorized(segMap.size(), CV_8UC3);
    
    for (int y = 0; y < segMap.rows; y++) {
        for (int x = 0; x < segMap.cols; x++) {
            uint8_t classId = segMap.at<uint8_t>(y, x);
            if (classId < classColors.size()) {
                colorized.at<cv::Vec3b>(y, x) = classColors[classId];
            } else {
                colorized.at<cv::Vec3b>(y, x) = cv::Vec3b(128, 128, 128); // Gray for unknown classes
            }
        }
    }
    
    return colorized;
}

// Find contours for a specific class at mouse position
bool findContoursAtPosition(const cv::Mat& segMap, cv::Point mousePos, cv::Mat& output, std::string& label) {
    if (mousePos.x < 0 || mousePos.y < 0 || mousePos.x >= segMap.cols || mousePos.y >= segMap.rows) {
        label.clear();
        return false;
    }
    
    uint8_t classId = segMap.at<uint8_t>(mousePos.y, mousePos.x);
    
    // Get class label
    if (classId < labelInfo.label_count) {
        label = std::string(labelInfo.label_names[classId].name);
        
        // Check if this is background (null) - if so, disable contour finding
        if (label == "(null)") {
            label = "Background";
            return false; // Don't draw contours for background
        }
    } else {
        label = "Unknown";
    }
    
    // Only draw contours for non-background classes
    // Create mask for the specific class
    cv::Mat mask = (segMap == classId);
    
    // Find contours with full chain representation to avoid zig-zag patterns
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    // Draw contours using standard OpenCV function
    cv::drawContours(output, contours, -1, cv::Scalar(255, 255, 255), 2);
    
    return true;
}

// Create optimized camera overlay (pre-computed for speed)
cv::Mat createCameraOverlay(const cv::Mat& cameraImage, const cv::Mat& colorizedSegMap) {
    if (cameraImage.empty() || colorizedSegMap.empty()) {
        return colorizedSegMap.clone();
    }
    
    cv::Mat overlay = cameraImage.clone();
    
    // Optimized blending using OpenCV vectorized operations
    cv::Mat mask;
    cv::inRange(colorizedSegMap, cv::Scalar(1, 1, 1), cv::Scalar(255, 255, 255), mask);
    
    // Blend only non-black pixels
    cv::Mat blendedSeg;
    cv::addWeighted(cameraImage, 0.6, colorizedSegMap, 0.4, 0, blendedSeg);
    blendedSeg.copyTo(overlay, mask);
    
    return overlay;
}

// Mouse callback function with immediate display update
void onMouse(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_MOUSEMOVE && hasValidFrames) {
        lastMousePos = cv::Point(x, y);
        
        // Immediate update for responsive feedback
        cv::Mat displayMap = currentOverlayBase.clone();
        std::string label;
        
        if (findContoursAtPosition(currentSegMap, lastMousePos, displayMap, label)) {
            // Display hovered label
            if (!label.empty()) {
                cv::putText(displayMap, label, cv::Point(10, 30), 
                           cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 255), 2);
            }
        } else if (!label.empty()) {
            // Still show label even if no contours (for background)
            cv::putText(displayMap, label, cv::Point(10, 30), 
                       cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(128, 128, 128), 2);
        }
        
        // Display model switching status if active
        if (isModelSwitching && !modelSwitchStatus.empty()) {
            cv::putText(displayMap, modelSwitchStatus, cv::Point(10, 70), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 0), 2);
        }
        
        cv::imshow("Segmentation & Camera Overlay", displayMap);
    }
}

// Print label information
void printLabelInfo(const std::string& labelSetName, const slamtec_aurora_sdk_semantic_segmentation_label_info_t& labelInfo) {
    std::cout << "\n=== Semantic Segmentation Label Information ===" << std::endl;
    std::cout << "Label Set: " << labelSetName << std::endl;
    std::cout << "Total Classes: " << labelInfo.label_count << std::endl;
    std::cout << "Classes:" << std::endl;
    
    for (int i = 0; i < labelInfo.label_count; i++) {
        std::string labelName = std::string(labelInfo.label_names[i].name);
        if (labelName == "(null)") {
            labelName = "Background";
        }
        std::cout << "  [" << std::setw(2) << i << "] " << labelName << std::endl;
    }
    std::cout << "================================================\n" << std::endl;
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

    // Check if semantic segmentation is supported
    if (!sdk->enhancedImaging.isSemanticSegmentationSupported()) {
        std::cerr << "Semantic segmentation is not supported by this device" << std::endl;
        sdk->disconnect();
        sdk->release();
        return 1;
    }
    std::cout << "Semantic segmentation is supported" << std::endl;

    // Get semantic segmentation configuration
    if (!sdk->enhancedImaging.getSemanticSegmentationConfig(segConfig)) {
        std::cerr << "Failed to get semantic segmentation config" << std::endl;
        sdk->disconnect();
        sdk->release();
        return 1;
    }

    std::cout << "Semantic Segmentation Config:" << std::endl;
    std::cout << "  FPS: " << segConfig.fps << std::endl;
    std::cout << "  Frame Skip: " << segConfig.frame_skip << std::endl;
    std::cout << "  Image Size: " << segConfig.image_width << "x" << segConfig.image_height << std::endl;

    // Subscribe to enhanced imaging
    if (!sdk->controller.setEnhancedImagingSubscription(SLAMTEC_AURORA_SDK_ENHANCED_IMAGE_TYPE_SEMANTIC, true)) {
        std::cerr << "Failed to subscribe to semantic segmentation" << std::endl;
        sdk->disconnect();
        sdk->release();
        return 1;
    }

    // Wait for semantic segmentation to be ready
    std::cout << "Waiting for semantic segmentation to be ready..." << std::endl;
    while (!sdk->enhancedImaging.isSemanticSegmentationReady()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (isCtrlC) {
            sdk->disconnect();
            sdk->release();
            return 0;
        }
    }

    // Get label information
    if (!sdk->enhancedImaging.getSemanticSegmentationLabelSetName(labelSetName)) {
        std::cerr << "Failed to get label set name" << std::endl;
    }

    if (!sdk->enhancedImaging.getSemanticSegmentationLabels(labelInfo)) {
        std::cerr << "Failed to get label information" << std::endl;
        sdk->disconnect();
        sdk->release();
        return 1;
    }

    // Print label information
    printLabelInfo(labelSetName, labelInfo);

    // Generate colors for classes
    generateClassColors(labelInfo.label_count);

    // Check current model
    isUsingAlternativeModel = sdk->enhancedImaging.isSemanticSegmentationAlternativeModel();
    std::cout << "Current model: " << (isUsingAlternativeModel ? "Alternative" : "Default") << std::endl;

    // Create windows
    cv::namedWindow("Segmentation & Camera Overlay", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Depth Aligned Segmentation", cv::WINDOW_AUTOSIZE);

    // Set mouse callback
    cv::setMouseCallback("Segmentation & Camera Overlay", onMouse, nullptr);

    std::cout << "\nControls:" << std::endl;
    std::cout << "  ESC - Exit" << std::endl;
    std::cout << "  'm' - Switch between default and alternative model" << std::endl;
    std::cout << "  Mouse hover - Show object contours and labels (in merged segmentation & camera overlay)" << std::endl;

    size_t frameCount = 0;
    auto startTime = std::chrono::steady_clock::now();

    // Main loop with faster response time for mouse interaction
    int key;
    while ((key = cv::waitKey(10)) != 27) {  // ESC key to exit
        if (isCtrlC) {
            break;
        }

        // Handle model switching
        if (key == 'm' || key == 'M') {
            bool newModel = !isUsingAlternativeModel;
            std::cout << "Switching to " << (newModel ? "Alternative" : "Default") << " model..." << std::endl;
            
            // Set model switching status for display
            isModelSwitching = true;
            modelSwitchStatus = "Switching to " + std::string(newModel ? "Alternative" : "Default") + " model...";
            
            if (sdk->controller.requireSemanticSegmentationAlternativeModel(newModel)) {
                // Wait for model switch to complete
                while (sdk->enhancedImaging.isSemanticSegmentationAlternativeModel() != newModel) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    
                    // Update display during waiting
                    if (hasValidFrames) {
                        cv::Mat displayMap = currentOverlayBase.clone();
                        cv::putText(displayMap, modelSwitchStatus, cv::Point(10, 70), 
                                   cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 0), 2);
                        cv::imshow("Segmentation & Camera Overlay", displayMap);
                        cv::waitKey(1); // Allow OpenCV to update display
                    }
                }
                
                isUsingAlternativeModel = newModel;
                
                // Update status message
                modelSwitchStatus = "Model switched to " + std::string(newModel ? "Alternative" : "Default") + " - Updating labels...";
                
                // Update label information after model switch
                if (sdk->enhancedImaging.getSemanticSegmentationLabelSetName(labelSetName) &&
                    sdk->enhancedImaging.getSemanticSegmentationLabels(labelInfo)) {
                    printLabelInfo(labelSetName, labelInfo);
                    generateClassColors(labelInfo.label_count);
                }
                
                // Show success message briefly
                modelSwitchStatus = "Model switch completed successfully!";
                if (hasValidFrames) {
                    cv::Mat displayMap = currentOverlayBase.clone();
                    cv::putText(displayMap, modelSwitchStatus, cv::Point(10, 70), 
                               cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
                    cv::imshow("Segmentation & Camera Overlay", displayMap);
                    cv::waitKey(1000); // Show success message for 1 second
                }
                
                std::cout << "Model switched successfully" << std::endl;
            } else {
                modelSwitchStatus = "Failed to switch model!";
                if (hasValidFrames) {
                    cv::Mat displayMap = currentOverlayBase.clone();
                    cv::putText(displayMap, modelSwitchStatus, cv::Point(10, 70), 
                               cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);
                    cv::imshow("Segmentation & Camera Overlay", displayMap);
                    cv::waitKey(1000); // Show error message for 1 second
                }
                std::cerr << "Failed to switch model" << std::endl;
            }
            
            // Clear model switching status
            isModelSwitching = false;
            modelSwitchStatus = "";
        }

        // Wait for next frame
        if (!sdk->enhancedImaging.waitSemanticSegmentationNextFrame(1000)) {
            continue;
        }

        // Peek segmentation frame
        RemoteEnhancedImagingFrame segFrame;
        slamtec_aurora_sdk_errorcode_t errorCode;
        if (!sdk->enhancedImaging.peekSemanticSegmentationFrame(segFrame, &errorCode)) {
            if (errorCode != SLAMTEC_AURORA_SDK_ERRORCODE_NOT_READY) {
                std::cerr << "Failed to peek segmentation frame, error code: " << errorCode << std::endl;
            }
            continue;
        }

        // Peek camera preview image for overlay
        RemoteStereoImagePair cameraImagePair;
        if (sdk->dataProvider.peekCameraPreviewImage(cameraImagePair, segFrame.desc.timestamp_ns, true)) {
            cameraImagePair.leftImage.toMat(currentCameraImage);
            if (currentCameraImage.channels() == 1) {
                cv::cvtColor(currentCameraImage, currentCameraImage, cv::COLOR_GRAY2BGR);
            }
        }

        frameCount++;
        currentSegFrame = segFrame;
        hasValidFrames = true;

        // Convert segmentation map to OpenCV Mat
        cv::Mat localSegMap;
        segFrame.image.toMat(localSegMap);
        currentSegMap = localSegMap.clone(); // the localSegMap uses the same memory as segFrame.image, so we need to clone it
        if (!currentSegMap.empty()) {
            // Colorize segmentation map
            currentColorizedSegMap = colorizeSegmentationMap(currentSegMap);
            
            // Create optimized camera overlay (pre-computed base)
            currentOverlayBase = createCameraOverlay(currentCameraImage, currentColorizedSegMap);
            
            // Add frame info to base overlay
            std::string frameInfo = "Frame: " + std::to_string(frameCount) + 
                                  " | Model: " + (isUsingAlternativeModel ? "Alt" : "Default");
            cv::putText(currentOverlayBase, frameInfo, cv::Point(10, currentOverlayBase.rows - 10), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
            
            // Add control instructions
            std::string controlText = "'m' - Switch between default and alternative model";
            cv::putText(currentOverlayBase, controlText, cv::Point(10, currentOverlayBase.rows - 30), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);

            // Initial display (mouse callback will handle updates)
            cv::Mat displayMap = currentOverlayBase.clone();
            
            // Handle current mouse position if valid
            if (lastMousePos.x >= 0 && lastMousePos.y >= 0) {
                std::string label;
                if (findContoursAtPosition(currentSegMap, lastMousePos, displayMap, label)) {
                    if (!label.empty()) {
                        cv::putText(displayMap, label, cv::Point(10, 30), 
                                   cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 255), 2);
                    }
                } else if (!label.empty()) {
                    cv::putText(displayMap, label, cv::Point(10, 30), 
                               cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(128, 128, 128), 2);
                }
            }
            
            // Display model switching status if active
            if (isModelSwitching && !modelSwitchStatus.empty()) {
                cv::putText(displayMap, modelSwitchStatus, cv::Point(10, 70), 
                           cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 0), 2);
            }

            cv::imshow("Segmentation & Camera Overlay", displayMap);

            // Depth aligned segmentation
            if (sdk->enhancedImaging.isDepthCameraSupported()) {
                RemoteEnhancedImagingFrame alignedFrame;
                if (sdk->enhancedImaging.calcDepthCameraAlignedSegmentationMap(segFrame.image, alignedFrame)) {
                    cv::Mat alignedSegMap;
                    alignedFrame.image.toMat(alignedSegMap);
                    cv::Mat alignedColorized = colorizeSegmentationMap(alignedSegMap);
                    cv::imshow("Depth Aligned Segmentation", alignedColorized);
                }
            }

            // Calculate and display FPS every 30 frames
            if (frameCount % 30 == 0) {
                auto currentTime = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
                double fps = 30000.0 / duration.count();
                std::cout << "Segmentation FPS: " << fps << " | Frame: " << frameCount << std::endl;
                startTime = currentTime;
            }
        }
    }

    cv::destroyAllWindows();
    sdk->disconnect();
    sdk->release();

    std::cout << "Semantic segmentation demo completed. Total frames processed: " << frameCount << std::endl;
    return 0;
}