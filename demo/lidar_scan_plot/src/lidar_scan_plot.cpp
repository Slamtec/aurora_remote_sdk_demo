/*
 *  SLAMTEC AURORA DEMO
 *  This demo shows how to retrieve the 2D LIDAR Scan data from the aurora device and render it using OpenCV
 */

#include <iostream>

#include "aurora_pubsdk_inc.h"

#include <chrono>
#include <thread>

#include <opencv2/opencv.hpp>
#include <mutex>

#include <signal.h>
using namespace rp::standalone::aurora;


static std::mutex gMutex;
static slamtec_aurora_sdk_lidar_singlelayer_scandata_info_t gLidarScanInfo;
static std::vector<slamtec_aurora_sdk_lidar_scan_point_t> gLidarScanPoints;

static int isCtrlC = 0;

static void onCtrlC(int) {
    std::cout << "Ctrl-C pressed, exiting..." << std::endl;
    isCtrlC = 1;
}


static bool discoverAndSelectAuroraDevice(RemoteSDK* sdk, SDKServerConnectionDesc& selectedDeviceDesc)
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
            auto& connectionOption = serverList[i][j];
            std::cout << "  option " << j << ": " << connectionOption.toLocatorString() << std::endl;
        }
    }

    // select the first device
    selectedDeviceDesc = serverList[0];
    std::cout << "Selected first device: " << std::endl;
    return true;
}


class SDKListener : public RemoteSDKListener {
public:
    virtual void onLIDARScan(const slamtec_aurora_sdk_lidar_singlelayer_scandata_info_t& header, const slamtec_aurora_sdk_lidar_scan_point_t* points)
    {
        std::lock_guard <std::mutex> lock(gMutex);
        gLidarScanInfo = header;
        gLidarScanPoints.reserve(header.scan_count);
        gLidarScanPoints.assign(points, points + header.scan_count);
    }
};


static void plotLIDARScan(cv::Mat& canvas, const slamtec_aurora_sdk_lidar_singlelayer_scandata_info_t& header, const slamtec_aurora_sdk_lidar_scan_point_t* points)
{
    static const int CANVAS_SIZE = 500;
    static const float MAX_DISTANCE = 20.0f;
    canvas = cv::Mat::zeros(CANVAS_SIZE, CANVAS_SIZE, CV_8UC3);
    float scale = CANVAS_SIZE / MAX_DISTANCE;

    for (size_t i = 0; i < header.scan_count; i++) {
        auto& point = points[i];
        if (point.quality > 0) {
            float x = point.dist * cos(point.angle) * scale;
            float y = point.dist * sin(point.angle) * scale;

            cv::Point2f p(x + CANVAS_SIZE / 2, CANVAS_SIZE / 2 - y);
            cv::circle(canvas, p, 1, cv::Scalar(point.quality * 4, 0, 255 - point.quality), 1);
        }
    }

    cv::putText(canvas, "Scan Points: " + std::to_string(header.scan_count), cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
}


int main(int argc, const char* argv[]) {
    // register the ctrl-c signal handler
    signal(SIGINT, onCtrlC);


    const char* connectionString = nullptr;
    if (argc > 1) {
        connectionString = argv[1];
    }
    SDKListener listener;
    RemoteSDK* sdk = RemoteSDK::CreateSession(&listener);
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
    }
    else {
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


    cv::namedWindow("LIDAR Scan Plot", cv::WINDOW_AUTOSIZE);
   
    // Method 1: using callback to get the latest LIDAR scan data
    while (cv::waitKey(30) != 27) {
        if (isCtrlC) {
            break;
        }
        cv::Mat scanPlotMat;

        {
            std::lock_guard <std::mutex> lock(gMutex);
            plotLIDARScan(scanPlotMat, gLidarScanInfo, gLidarScanPoints.data());
        }

        if (!scanPlotMat.empty()) {
            cv::putText(scanPlotMat, "Callback Mode, Press ESC to continue", cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
            cv::imshow("LIDAR Scan Plot", scanPlotMat);
        }
    }


    // Method 2: using the peek polling interface
    while (cv::waitKey(30) != 27) {
        if (isCtrlC) {
            break;
        }
        cv::Mat scanPlotMat;

        SingleLayerLIDARScan scan;
        slamtec_aurora_sdk_pose_se3_t poseSE3;

        if (!sdk->dataProvider.peekRecentLIDARScanSingleLayer(scan, poseSE3)) {
            continue;
        }


        plotLIDARScan(scanPlotMat, scan.info, scan.scanData.data());
        

        if (!scanPlotMat.empty()) {
            cv::putText(scanPlotMat, "Polling Mode, Press ESC to continue", cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
            cv::imshow("LIDAR Scan Plot", scanPlotMat);
        }
    }


    sdk->disconnect();
    sdk->release();

    return 0;

}