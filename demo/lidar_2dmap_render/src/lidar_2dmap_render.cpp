/*
 *  SLAMTEC AURORA DEMO
 *  This demo shows how to render the LIDAR 2D Grid Preview Map on-the-fly during SLAM mapping process
 */

#include <iostream>

#include "aurora_pubsdk_inc.h"

#include <chrono>
#include <thread>

#include <opencv2/opencv.hpp>
#include <mutex>

#include <signal.h>
using namespace rp::standalone::aurora;


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





int main(int argc, const char* argv[]) {
    // register the ctrl-c signal handler
    signal(SIGINT, onCtrlC);


    const char* connectionString = nullptr;
    if (argc > 1) {
        connectionString = argv[1];
    }
  
    RemoteSDK* sdk = RemoteSDK::CreateSession();
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


    // map data syncing must be enabled first
    sdk->controller.setMapDataSyncing(true);

    do {
        // start the preview map background update
        LIDAR2DGridMapGenerationOptions genOption;
        genOption.map_canvas_height = 150;
        genOption.map_canvas_width = 150;
        genOption.resolution = 0.05f;

        bool result = sdk->lidar2DMapBuilder.startPreviewMapBackgroundUpdate(genOption);

        if (!result) {
            std::cerr << "Failed to start the LIDAR 2D preview map updating" << std::endl;
            break;
        }

        // enable auto floor detection to render the map of the current floor
        sdk->lidar2DMapBuilder.setPreviewMapAutoFloorDetection(true);

        cv::namedWindow("LIDAR 2D Grid Map", cv::WINDOW_AUTOSIZE);
        cv::namedWindow("Floor Detection Histogram", cv::WINDOW_AUTOSIZE);
        cv::Mat canvas;
        bool redraw = true;

        while (1) {
            if (redraw) {
                sdk->lidar2DMapBuilder.requireRedrawPreviewMap();
                redraw = false;
            }

            slamtec_aurora_sdk_rect_t dirtyRect;
            bool mapChange;

            sdk->lidar2DMapBuilder.getAndResetPreviewMapDirtyRect(dirtyRect, mapChange);

            if (dirtyRect.height > 0 && dirtyRect.width > 0) {
                auto& prevMap = sdk->lidar2DMapBuilder.getPreviewMap();
                slamtec_aurora_sdk_2dmap_dimension_t mapDim;
                std::vector<uint8_t> mapData;
                prevMap.getMapDimension(mapDim);

                slamtec_aurora_sdk_rect_t fetchRect;
                fetchRect.x = mapDim.min_x;
                fetchRect.y = mapDim.min_y;
                fetchRect.width = mapDim.max_x - mapDim.min_x;
                fetchRect.height = mapDim.max_y - mapDim.min_y;

                slamtec_aurora_sdk_2d_gridmap_fetch_info_t info;

                prevMap.readCellData(fetchRect, info, mapData);

                canvas = cv::Mat(info.cell_height, info.cell_width, CV_8UC1, mapData.data());

                cv::putText(canvas, "Press 'r' to redraw the map", cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255), 1);
                cv::putText(canvas, "Press 'q' to exit", cv::Point(10, 40), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255), 1);

                cv::imshow("LIDAR 2D Grid Map", canvas);
            }

            int key = cv::waitKey(500);
            if (key == 'q') {
                break;
            }
            else if (key == 'r') {
                redraw = true;
            }



            FloorDetectionHistogram floorHist;
            std::vector<slamtec_aurora_sdk_floor_detection_desc_t> floorDesc;
            int currentFloorID;

            sdk->floorDetector.getDetectionHistogram(floorHist);
            sdk->floorDetector.getAllDetectionDesc(floorDesc, currentFloorID);

            cv::Mat histImg(200, 200, CV_8UC3, cv::Scalar(0, 0, 0));

            float maxValue = 1;
            for (size_t i = 0; i < floorHist.info.bin_total_count; i++) {
                if (floorHist.histogramData[i] > maxValue) {
                    maxValue = floorHist.histogramData[i];
                }
            }
            float scale = 200 / maxValue;
            const int binWidth = 10;
            for (size_t i = 0; i < floorHist.info.bin_total_count; i++) {
                cv::rectangle(histImg, cv::Rect(i * binWidth, 200 - floorHist.histogramData[i] * scale, binWidth, floorHist.histogramData[i] * scale), cv::Scalar(0, 255, 0), 1);
            }

            cv::imshow("Floor Detection Histogram", histImg);

            std::cout << "Current Floor ID: " << currentFloorID << std::endl;
            for (auto& desc : floorDesc) {
                std::cout << "Floor ID: " << desc.floorID << " Floor Height: " << desc.typical_height << " Confidence: " << desc.confidence << std::endl;
            }
        }

        sdk->lidar2DMapBuilder.stopPreviewMapBackgroundUpdate();

    } while (0);



    sdk->disconnect();
    sdk->release();

    return 0;

}