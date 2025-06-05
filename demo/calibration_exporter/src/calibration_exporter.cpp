/* 
 *  SLAMTEC AURORA DEMO
 *  This demo shows how to retrieve and export camera calibration and transform calibration parameters
 *  Features include:
 *  - Retrieve camera lens calibration parameters from device
 *  - Retrieve transform calibration parameters from device  
 *  - Export calibration data to OpenCV XML/YAML files
 *  - Command-line interface with usage instructions
 */

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <signal.h>
#include <iomanip>

#include "aurora_pubsdk_inc.h"
#include <opencv2/opencv.hpp>

using namespace rp::standalone::aurora;

static int isCtrlC = 0;

static void onCtrlC(int) {
    std::cout << "Ctrl-C pressed, exiting..." << std::endl;
    isCtrlC = 1;
}

static void printUsage(const char* programName) {
    std::cout << "\nUsage: " << programName << " [options] [device_connection_string]" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  -h, --help          Show this help message" << std::endl;
    std::cout << "  -o, --output <dir>  Output directory for calibration files (default: current directory)" << std::endl;
    std::cout << "  -f, --format <fmt>  Output format: xml or yml (default: xml)" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  " << programName << "                    # Auto-discover device, save to current dir as XML" << std::endl;
    std::cout << "  " << programName << " -o ./calibration   # Save to ./calibration directory" << std::endl;
    std::cout << "  " << programName << " -f yml             # Save as YAML format" << std::endl;
    std::cout << "  " << programName << " tcp://192.168.1.100:8090  # Connect to specific device" << std::endl;
    std::cout << std::endl;
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
    std::cout << "Selected first device: " << selectedDeviceDesc[0].toLocatorString() << std::endl;
    return true;
}

static bool exportCameraCalibration(const slamtec_aurora_sdk_camera_calibration_t& cameraCalib, 
                                  const std::string& outputDir, const std::string& format) {
    std::string extension = (format == "yml") ? ".yml" : ".xml";
    
    // Check if we have at least one camera
    if (cameraCalib.camera_type == 0) { // MONO camera
        std::cout << "Detected MONO camera setup" << std::endl;
    } else if (cameraCalib.camera_type == 1) { // STEREO camera
        std::cout << "Detected STEREO camera setup" << std::endl;
    }
    
    // Export first camera (left camera for stereo, main camera for mono)
    std::string leftCalibFile = outputDir + "/camera_0_calibration" + extension;
    cv::FileStorage leftFs(leftCalibFile, cv::FileStorage::WRITE);
    if (!leftFs.isOpened()) {
        std::cerr << "Failed to open " << leftCalibFile << " for writing" << std::endl;
        return false;
    }
    
    // Convert calibration data to OpenCV matrices
    const auto& leftCam = cameraCalib.camera_calibration[0];
    cv::Mat leftCameraMatrix = (cv::Mat_<double>(3, 3) << 
        leftCam.intrinsics[0], 0, leftCam.intrinsics[2],  // fx, 0, cx
        0, leftCam.intrinsics[1], leftCam.intrinsics[3],  // 0, fy, cy
        0, 0, 1);
    
    cv::Mat leftDistCoeffs = (cv::Mat_<double>(1, 4) << 
        leftCam.distortion[0], leftCam.distortion[1], 
        leftCam.distortion[2], leftCam.distortion[3]);
    
    leftFs << "camera_matrix" << leftCameraMatrix;
    leftFs << "distortion_coefficients" << leftDistCoeffs;
    leftFs << "image_width" << leftCam.width;
    leftFs << "image_height" << leftCam.height;
    leftFs << "fps" << leftCam.fps;
    leftFs.release();
    
    std::cout << "Camera 0 calibration exported to: " << leftCalibFile << std::endl;
    
    // Export second camera if stereo
    if (cameraCalib.camera_type == 1) { // STEREO
        std::string rightCalibFile = outputDir + "/camera_1_calibration" + extension;
        cv::FileStorage rightFs(rightCalibFile, cv::FileStorage::WRITE);
        if (!rightFs.isOpened()) {
            std::cerr << "Failed to open " << rightCalibFile << " for writing" << std::endl;
            return false;
        }
        
        const auto& rightCam = cameraCalib.camera_calibration[1];
        cv::Mat rightCameraMatrix = (cv::Mat_<double>(3, 3) << 
            rightCam.intrinsics[0], 0, rightCam.intrinsics[2],  // fx, 0, cx
            0, rightCam.intrinsics[1], rightCam.intrinsics[3],  // 0, fy, cy
            0, 0, 1);
        
        cv::Mat rightDistCoeffs = (cv::Mat_<double>(1, 4) << 
            rightCam.distortion[0], rightCam.distortion[1], 
            rightCam.distortion[2], rightCam.distortion[3]);
        
        rightFs << "camera_matrix" << rightCameraMatrix;
        rightFs << "distortion_coefficients" << rightDistCoeffs;
        rightFs << "image_width" << rightCam.width;
        rightFs << "image_height" << rightCam.height;
        rightFs << "fps" << rightCam.fps;
        rightFs.release();
        
        std::cout << "Camera 1 calibration exported to: " << rightCalibFile << std::endl;
        
        // Export stereo calibration parameters
        std::string stereoCalibFile = outputDir + "/stereo_calibration" + extension;
        cv::FileStorage stereoFs(stereoCalibFile, cv::FileStorage::WRITE);
        if (!stereoFs.isOpened()) {
            std::cerr << "Failed to open " << stereoCalibFile << " for writing" << std::endl;
            return false;
        }
        
        // Extract 4x4 transformation matrix from first external camera transform
        const float* t_matrix = cameraCalib.ext_camera_transform[0].t_c2_c1;
        
        // Extract rotation (3x3) and translation (3x1) from 4x4 transformation matrix
        cv::Mat R = (cv::Mat_<double>(3, 3) << 
            t_matrix[0], t_matrix[1], t_matrix[2],
            t_matrix[4], t_matrix[5], t_matrix[6],
            t_matrix[8], t_matrix[9], t_matrix[10]);
        
        cv::Mat T = (cv::Mat_<double>(3, 1) << 
            t_matrix[3], t_matrix[7], t_matrix[11]);
        
        stereoFs << "camera_0_matrix" << leftCameraMatrix;
        stereoFs << "camera_1_matrix" << rightCameraMatrix;
        stereoFs << "camera_0_distortion" << leftDistCoeffs;
        stereoFs << "camera_1_distortion" << rightDistCoeffs;
        stereoFs << "rotation_matrix" << R;
        stereoFs << "translation_vector" << T;
        stereoFs << "transform_4x4" << (cv::Mat_<double>(4, 4) << 
            t_matrix[0], t_matrix[1], t_matrix[2], t_matrix[3],
            t_matrix[4], t_matrix[5], t_matrix[6], t_matrix[7],
            t_matrix[8], t_matrix[9], t_matrix[10], t_matrix[11],
            t_matrix[12], t_matrix[13], t_matrix[14], t_matrix[15]);
        stereoFs.release();
        
        std::cout << "Stereo calibration exported to: " << stereoCalibFile << std::endl;
    }
    
    return true;
}

static bool exportTransformCalibration(const slamtec_aurora_sdk_transform_calibration_t& transformCalib,
                                     const std::string& outputDir, const std::string& format) {
    std::string extension = (format == "yml") ? ".yml" : ".xml";
    std::string transformCalibFile = outputDir + "/transform_calibration" + extension;
    
    cv::FileStorage fs(transformCalibFile, cv::FileStorage::WRITE);
    if (!fs.isOpened()) {
        std::cerr << "Failed to open " << transformCalibFile << " for writing" << std::endl;
        return false;
    }
    
    // Extract base to camera transform (t_base_cam)
    cv::Mat baseToCameraPose = (cv::Mat_<double>(7, 1) << 
        transformCalib.t_base_cam.translation.x, transformCalib.t_base_cam.translation.y, transformCalib.t_base_cam.translation.z,
        transformCalib.t_base_cam.quaternion.x, transformCalib.t_base_cam.quaternion.y, 
        transformCalib.t_base_cam.quaternion.z, transformCalib.t_base_cam.quaternion.w);
    
    // Extract camera to IMU transform (t_camera_imu) 
    cv::Mat cameraToImuPose = (cv::Mat_<double>(7, 1) << 
        transformCalib.t_camera_imu.translation.x, transformCalib.t_camera_imu.translation.y, transformCalib.t_camera_imu.translation.z,
        transformCalib.t_camera_imu.quaternion.x, transformCalib.t_camera_imu.quaternion.y, 
        transformCalib.t_camera_imu.quaternion.z, transformCalib.t_camera_imu.quaternion.w);
    
    fs << "base_to_camera_pose" << baseToCameraPose;  // [x, y, z, qx, qy, qz, qw]
    fs << "camera_to_imu_pose" << cameraToImuPose;   // [x, y, z, qx, qy, qz, qw]
    
    // Add separate rotation matrices for convenience (converted from quaternions)
    // Note: For full rotation matrix conversion, would need quaternion to rotation matrix function
    fs << "base_to_camera_translation" << (cv::Mat_<double>(3, 1) << 
        transformCalib.t_base_cam.translation.x, transformCalib.t_base_cam.translation.y, transformCalib.t_base_cam.translation.z);
    fs << "base_to_camera_quaternion" << (cv::Mat_<double>(4, 1) << 
        transformCalib.t_base_cam.quaternion.x, transformCalib.t_base_cam.quaternion.y, 
        transformCalib.t_base_cam.quaternion.z, transformCalib.t_base_cam.quaternion.w);
        
    fs << "camera_to_imu_translation" << (cv::Mat_<double>(3, 1) << 
        transformCalib.t_camera_imu.translation.x, transformCalib.t_camera_imu.translation.y, transformCalib.t_camera_imu.translation.z);
    fs << "camera_to_imu_quaternion" << (cv::Mat_<double>(4, 1) << 
        transformCalib.t_camera_imu.quaternion.x, transformCalib.t_camera_imu.quaternion.y, 
        transformCalib.t_camera_imu.quaternion.z, transformCalib.t_camera_imu.quaternion.w);
    
    fs.release();
    
    std::cout << "Transform calibration exported to: " << transformCalibFile << std::endl;
    
    return true;
}

static void printCalibrationSummary(const slamtec_aurora_sdk_camera_calibration_t& cameraCalib,
                                   const slamtec_aurora_sdk_transform_calibration_t& transformCalib) {
    std::cout << "\n=== Camera Calibration Summary ===" << std::endl;
    
    std::string cameraTypeStr = (cameraCalib.camera_type == 0) ? "MONO" : 
                               (cameraCalib.camera_type == 1) ? "STEREO" : "UNKNOWN";
    std::cout << "Camera Type: " << cameraTypeStr << std::endl;
    
    std::cout << "Camera 0:" << std::endl;
    const auto& cam0 = cameraCalib.camera_calibration[0];
    std::cout << "  Resolution: " << cam0.width << "x" << cam0.height << std::endl;
    std::cout << "  Focal Length: fx=" << std::fixed << std::setprecision(2) << cam0.intrinsics[0] 
              << ", fy=" << cam0.intrinsics[1] << std::endl;
    std::cout << "  Principal Point: cx=" << cam0.intrinsics[2] << ", cy=" << cam0.intrinsics[3] << std::endl;
    std::cout << "  FPS: " << cam0.fps << std::endl;
    
    if (cameraCalib.camera_type == 1) { // STEREO
        std::cout << "Camera 1:" << std::endl;
        const auto& cam1 = cameraCalib.camera_calibration[1];
        std::cout << "  Resolution: " << cam1.width << "x" << cam1.height << std::endl;
        std::cout << "  Focal Length: fx=" << cam1.intrinsics[0] 
                  << ", fy=" << cam1.intrinsics[1] << std::endl;
        std::cout << "  Principal Point: cx=" << cam1.intrinsics[2] << ", cy=" << cam1.intrinsics[3] << std::endl;
        std::cout << "  FPS: " << cam1.fps << std::endl;
    }
    
    std::cout << "\n=== Transform Calibration Summary ===" << std::endl;
    std::cout << "Base to Camera Translation: [" 
              << std::fixed << std::setprecision(4)
              << transformCalib.t_base_cam.translation.x << ", " 
              << transformCalib.t_base_cam.translation.y << ", " 
              << transformCalib.t_base_cam.translation.z << "]" << std::endl;
    std::cout << "Base to Camera Quaternion: [" 
              << transformCalib.t_base_cam.quaternion.x << ", " 
              << transformCalib.t_base_cam.quaternion.y << ", " 
              << transformCalib.t_base_cam.quaternion.z << ", " 
              << transformCalib.t_base_cam.quaternion.w << "]" << std::endl;
    
    std::cout << "Camera to IMU Translation: [" 
              << transformCalib.t_camera_imu.translation.x << ", " 
              << transformCalib.t_camera_imu.translation.y << ", " 
              << transformCalib.t_camera_imu.translation.z << "]" << std::endl;
    std::cout << "Camera to IMU Quaternion: [" 
              << transformCalib.t_camera_imu.quaternion.x << ", " 
              << transformCalib.t_camera_imu.quaternion.y << ", " 
              << transformCalib.t_camera_imu.quaternion.z << ", " 
              << transformCalib.t_camera_imu.quaternion.w << "]" << std::endl;
    std::cout << "===================================" << std::endl;
}

int main(int argc, const char* argv[]) {
    // register the ctrl-c signal handler
    signal(SIGINT, onCtrlC);

    // Parse command line arguments
    std::string outputDir = ".";
    std::string format = "xml";
    const char* connectionString = nullptr;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                outputDir = argv[++i];
            } else {
                std::cerr << "Error: " << arg << " requires an argument" << std::endl;
                printUsage(argv[0]);
                return 1;
            }
        } else if (arg == "-f" || arg == "--format") {
            if (i + 1 < argc) {
                format = argv[++i];
                if (format != "xml" && format != "yml") {
                    std::cerr << "Error: Unsupported format '" << format << "'. Use 'xml' or 'yml'" << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "Error: " << arg << " requires an argument" << std::endl;
                printUsage(argv[0]);
                return 1;
            }
        } else if (arg.find("://") != std::string::npos) {
            connectionString = argv[i];
        } else {
            std::cerr << "Error: Unknown argument '" << arg << "'" << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }

    std::cout << "Aurora Calibration Exporter" << std::endl;
    std::cout << "Output directory: " << outputDir << std::endl;
    std::cout << "Output format: " << format << std::endl;

    // Create output directory if it doesn't exist
    std::string createDirCmd = "mkdir -p \"" + outputDir + "\"";
    if (system(createDirCmd.c_str()) != 0) {
        std::cerr << "Failed to create output directory: " << outputDir << std::endl;
        return 1;
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
            sdk->release();
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
        sdk->release();
        return 1;
    }
    std::cout << "Connected to the selected device" << std::endl;

    // Retrieve camera calibration
    std::cout << "\nRetrieving camera calibration..." << std::endl;
    slamtec_aurora_sdk_camera_calibration_t cameraCalibration;
    if (!sdk->dataProvider.getCameraCalibration(cameraCalibration)) {
        std::cerr << "Failed to retrieve camera calibration" << std::endl;
        sdk->disconnect();
        sdk->release();
        return 1;
    }
    std::cout << "Camera calibration retrieved successfully" << std::endl;

    // Retrieve transform calibration
    std::cout << "Retrieving transform calibration..." << std::endl;
    slamtec_aurora_sdk_transform_calibration_t transformCalibration;
    if (!sdk->dataProvider.getTransformCalibration(transformCalibration)) {
        std::cerr << "Failed to retrieve transform calibration" << std::endl;
        sdk->disconnect();
        sdk->release();
        return 1;
    }
    std::cout << "Transform calibration retrieved successfully" << std::endl;

    // Print calibration summary
    printCalibrationSummary(cameraCalibration, transformCalibration);

    // Export calibration data
    std::cout << "\nExporting calibration data..." << std::endl;
    
    bool cameraExportSuccess = exportCameraCalibration(cameraCalibration, outputDir, format);
    bool transformExportSuccess = exportTransformCalibration(transformCalibration, outputDir, format);
    
    if (cameraExportSuccess && transformExportSuccess) {
        std::cout << "\nCalibration export completed successfully!" << std::endl;
        std::cout << "Files saved to: " << outputDir << std::endl;
    } else {
        std::cerr << "\nCalibration export failed!" << std::endl;
        sdk->disconnect();
        sdk->release();
        return 1;
    }

    sdk->disconnect();
    sdk->release();

    return 0;
}