/* 
 *  SLAMTEC AURORA DEMO
 *  This demo shows how to access the map data from the aurora device
 *  Notice:
 *     The map data is only for visualization, it is not complete to be used for SLAM operations
 *     If you want to save the map data and reuse it in future navigation task,
 *        the RemoteMapManager::startDownloadSession() can be used to download the map data.
 *     Please refer to the vslam_map_saveload demo for more details.
 *
 *     This demo will render the map data in a vertical view, for better visual effect,
 *        please use the Aurora Remote Application
 */

#include <cmath>
#include <iostream>

#include "aurora_pubsdk_inc.h"
#include "opencv2/core/utility.hpp"

#include <chrono>
#include <opencv2/imgproc.hpp>
#include <thread>

#include <opencv2/opencv.hpp>

#include <signal.h>
#include <vector>


using namespace rp::standalone::aurora;

static int isCtrlC = 0;

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

static void renderTrackingFrame(RemoteSDK * sdk) {
    RemoteTrackingFrameInfo trackingFrame;
    if (!sdk->dataProvider.peekTrackingData(trackingFrame)) {
        return;
    }

    cv::Mat left, right;

    trackingFrame.leftImage.toMat(left);
    trackingFrame.rightImage.toMat(right);

    // convert to BGR
    cv::cvtColor(left, left, cv::COLOR_GRAY2BGR);
    cv::cvtColor(right, right, cv::COLOR_GRAY2BGR);

    // draw keypoints
    for (size_t i = 0; i < trackingFrame.getKeypointsLeftCount(); i++) {
        auto& kp = trackingFrame.getKeypointsLeftBuffer()[i];
        if (kp.flags)
            cv::circle(left, cv::Point(kp.x, kp.y), 2, cv::Scalar(0, 255, 0), 2);
    }
    for (size_t i = 0; i < trackingFrame.getKeypointsRightCount(); i++) {
        auto& kp = trackingFrame.getKeypointsRightBuffer()[i];
        if (kp.flags)
            cv::circle(right, cv::Point(kp.x, kp.y), 2, cv::Scalar(0, 255, 0), 2);
    }


    // place image side-by-side
    cv::Mat merged;
    cv::hconcat(left, right, merged);

    // scale the image to half size
    cv::resize(merged, merged, cv::Size(), 0.5, 0.5);

    cv::imshow("Tracking Frame", merged);
}

static void renderMapData(RemoteSDK * sdk) {
    // only cares about the active map
    static const int mapWidth = 1024;
    static const int mapHeight = 1024;

    float scale = 1.0f;


    int activeMapId = -1;
    slamtec_aurora_sdk_map_desc_t activeMapDesc;
    std::vector<slamtec_aurora_sdk_map_point_desc_t> mapPoints;
    std::vector<RemoteKeyFrameData> keyframes;


    slamtec_aurora_sdk_global_map_desc_t globalMapDesc;
    if (!sdk->dataProvider.getGlobalMappingInfo(globalMapDesc)) {
        std::cerr << "Failed to get global mapping info" << std::endl;
        return;
    }


    activeMapId = globalMapDesc.activeMapID;

    // to access the map data, we need to create a map data visitor
    RemoteMapDataVisitor mapDataVisitor;

    // subscribe data type callbacks
    // if you are only interested in the map data, you can subscribe to the map data callback only
    // this will speed up the accessing performance
    mapDataVisitor.subscribeMapData([&](const slamtec_aurora_sdk_map_desc_t& mapDesc) {
        activeMapDesc = mapDesc;
        mapPoints.reserve(mapDesc.map_point_count);
        keyframes.reserve(mapDesc.keyframe_count);
        
    });

    mapDataVisitor.subscribeKeyFrameData([&](const RemoteKeyFrameData& keyframeData) {
        // handle the keyframe data
        keyframes.push_back(keyframeData);
    });

    mapDataVisitor.subscribeMapPointData([&](const slamtec_aurora_sdk_map_point_desc_t& mapPointDesc) {
        // handle the map point data
        mapPoints.push_back(mapPointDesc);
    });


    // start the accessing session, this will block the background syncing thread during the accessing process.
    if (!sdk->dataProvider.accessMapData(mapDataVisitor, {(uint32_t)activeMapId})) {
        std::cerr << "Failed to start the accessing session" << std::endl;
        return;
    }

    // calc the map boundary
    double mapMinX = std::numeric_limits<double>::max();
    double mapMinY = std::numeric_limits<double>::max();
    double mapMaxX = std::numeric_limits<double>::min();
    double mapMaxY = std::numeric_limits<double>::min();

    for (const auto& kf : keyframes) {
        mapMinX = std::min(mapMinX, kf.desc.pose.translation.x);
        mapMinY = std::min(mapMinY, kf.desc.pose.translation.y);
        mapMaxX = std::max(mapMaxX, kf.desc.pose.translation.x);
        mapMaxY = std::max(mapMaxY, kf.desc.pose.translation.y);
    }

    // add margin to the map boundary
    const double margin = 5.0;
    mapMinX -= margin;
    mapMinY -= margin;
    mapMaxX += margin;
    mapMaxY += margin;

    // calc the map center
    double mapCenterX = (mapMinX + mapMaxX) / 2.0;
    double mapCenterY = (mapMinY + mapMaxY) / 2.0;

    // calc the scale
    scale = std::min((mapWidth) / (mapMaxX - mapMinX), (mapHeight) / (mapMaxY - mapMinY));

    cv::Mat mapHeatMap(mapHeight, mapWidth, CV_8UC1);
    mapHeatMap.setTo(0);

    static const int mapPointSize = 2;

    // render map points
    for (const auto& mp : mapPoints) {
        int x = std::round((mp.position.x - mapCenterX) * scale + mapWidth / 2.0);
        int y = std::round(mapHeight / 2.0 - (mp.position.y - mapCenterY) * scale);
        
        for (int yy = y; yy <= y + (mapPointSize-1); ++yy) {
            for (int xx = x ; xx <= x + (mapPointSize - 1); ++xx) {
                if (xx < 0 || xx >= mapWidth || yy < 0 || yy >= mapHeight) {
                    continue;
                }
                auto & heatMapValue = mapHeatMap.at<uchar>(yy, xx);
                int newVal = 0;
                if (heatMapValue == 0) {
                    newVal = 100;
                }
                else {
                    newVal = heatMapValue + 60;
                }
                if (newVal > 255) {
                    newVal = 255;
                }
                heatMapValue = newVal;
            }
        }

    }

    cv::Mat mapHeatMapBGR;


    // make mapHeatMap to be the green channel of mapHeatMapBGR
    cv::Mat mapHeatMapOtherChannels(mapHeight, mapWidth, CV_8UC1);
    mapHeatMapOtherChannels.setTo(0);

    cv::merge(std::vector<cv::Mat>{mapHeatMapOtherChannels, mapHeatMap, mapHeatMapOtherChannels}, mapHeatMapBGR);


    // render keyframe trajectory
    for (size_t i = 1; i < keyframes.size(); i++) {
        const auto& kf = keyframes[i];
        const auto& kfPrev = keyframes[i - 1];

        int x = std::round((kf.desc.pose.translation.x - mapCenterX) * scale + mapWidth / 2.0);
        int y = std::round(mapHeight / 2.0 - (kf.desc.pose.translation.y - mapCenterY) * scale);

        int xPrev = std::round((kfPrev.desc.pose.translation.x - mapCenterX) * scale + mapWidth / 2.0);
        int yPrev = std::round(mapHeight / 2.0 - (kfPrev.desc.pose.translation.y - mapCenterY) * scale);

        cv::line(mapHeatMapBGR, cv::Point(xPrev, yPrev), cv::Point(x, y), cv::Scalar(0, 0, 100), 1);

        cv::circle(mapHeatMapBGR, cv::Point(x, y), 2, cv::Scalar(0, 0, 100), 1);
    }


    slamtec_aurora_sdk_pose_t pose;
    sdk->dataProvider.getCurrentPose(pose);

    // draw the current pose
    cv::circle(mapHeatMapBGR, cv::Point(std::round((pose.translation.x - mapCenterX) * scale + mapWidth / 2.0), std::round(mapHeight / 2.0 - (pose.translation.y - mapCenterY) * scale)), 3, cv::Scalar(0, 0, 200), 2);

    cv::putText(mapHeatMapBGR, "Map Points: " + std::to_string(mapPoints.size()), cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
    cv::putText(mapHeatMapBGR, "Keyframes: " + std::to_string(keyframes.size()), cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
    cv::putText(mapHeatMapBGR, "Active Map ID: " + std::to_string(activeMapId), cv::Point(10, 90), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
    cv::imshow("Vertical View Map", mapHeatMapBGR);
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
    

    // enable the map data syncing in background
    sdk->controller.setMapDataSyncing(true);

    cv::namedWindow("Tracking Frame", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Vertical View Map", cv::WINDOW_AUTOSIZE);

    
    auto lastRefreshTime = std::chrono::steady_clock::now();
    while (cv::waitKey(30) != 27) {
        if (isCtrlC) {
            break;
        }

        renderTrackingFrame(sdk);
        renderMapData(sdk);


        int refreshInterval = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastRefreshTime).count();
        if (refreshInterval > 10*1000) {
            lastRefreshTime = std::chrono::steady_clock::now();
            // force refresh the map data
            sdk->controller.resyncMapData();
        }
    }

    

    sdk->disconnect();
    sdk->release();

    return 0;

}
