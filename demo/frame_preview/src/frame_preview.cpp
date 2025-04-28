/* 
 *  SLAMTEC AURORA DEMO
 *  This demo shows how to capture the tracking frame and raw frame and display them using OpenCV
 *  A stable connection to the Aurora device is required if you want to see the raw frame data
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
static size_t gFramecount = 0;
static RemoteTrackingFrameInfo gTrackingFrame;
static cv::Mat rawFrameL, rawFrameR;

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


class SDKListener : public RemoteSDKListener {
public:
    virtual void onTrackingData(const RemoteTrackingFrameInfo& info) {
        std::lock_guard <std::mutex> lock(gMutex);
        gTrackingFrame = info;
        ++gFramecount;
        
    }

    virtual void onRawCamImageData(uint64_t timestamp_ns, const RemoteImageRef& left, const RemoteImageRef& right) {
        std::lock_guard <std::mutex> lock(gMutex);
        left.toMat(rawFrameL);
        right.toMat(rawFrameR);

        rawFrameL = rawFrameL.clone();
        rawFrameR = rawFrameR.clone();
    }

};


int main(int argc, const char* argv[]) {
    // register the ctrl-c signal handler
    signal(SIGINT, onCtrlC);


    const char* connectionString = nullptr;
    if (argc > 1) {
        connectionString = argv[1];
    }
    SDKListener listener;
    RemoteSDK * sdk = RemoteSDK::CreateSession(&listener);
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
    

    cv::namedWindow("Tracking Frame", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Raw Frame", cv::WINDOW_AUTOSIZE);


    // Start Raw Image Subscription
    // This will consumes extra up to 100Mbps bandwidth, a ethernet connection is recommended
    sdk->controller.setRawDataSubscription(true);


    // Method 1: using callback to get the tracking frame and raw frame
    while (cv::waitKey(30) != 27) {
        if (isCtrlC) {
            break;
        }
        cv::Mat left, right;
        {
            std::lock_guard <std::mutex> lock(gMutex);
            gTrackingFrame.leftImage.toMat(left);
            gTrackingFrame.rightImage.toMat(right);
        }

        cv::Mat merged;
        if (!(left.empty() || right.empty())) {
           
            // convert to BGR
            cv::cvtColor(left, left, cv::COLOR_GRAY2BGR);
            cv::cvtColor(right, right, cv::COLOR_GRAY2BGR);

            // draw keypoints
            for (size_t i = 0; i < gTrackingFrame.getKeypointsLeftCount(); i++) {
                auto& kp = gTrackingFrame.getKeypointsLeftBuffer()[i];
                if (kp.flags)
                    cv::circle(left, cv::Point(kp.x, kp.y), 2, cv::Scalar(0, 255, 0), 2);
            }
            for (size_t i = 0; i < gTrackingFrame.getKeypointsRightCount(); i++) {
                auto& kp = gTrackingFrame.getKeypointsRightBuffer()[i];
                if (kp.flags)
                    cv::circle(right, cv::Point(kp.x, kp.y), 2, cv::Scalar(0, 255, 0), 2);
            }

            // place image side-by-side
            
            cv::hconcat(left, right, merged);


            cv::putText(merged, "callback test, ESC to next test. Frame: " + std::to_string(gFramecount), cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
            cv::imshow("Tracking Frame", merged);
        }
        {
            std::lock_guard <std::mutex> lock(gMutex);
            if (!rawFrameL.empty() && !rawFrameR.empty()) {
                cv::hconcat(rawFrameL, rawFrameR, merged);

            }
        }
        if (!merged.empty())
            cv::imshow("Raw Frame", merged);
    }
    sdk->controller.setRawDataSubscription(false);
    cv::destroyWindow("Raw Frame");


    // Method 2: using peek to get the tracking frame
    size_t peekedFrame = 0;
    while (cv::waitKey(30) != 27) {
        if (isCtrlC) {
            break;
        }

        RemoteTrackingFrameInfo trackingFrame;
        if (!sdk->dataProvider.peekTrackingData(trackingFrame)) {
            continue;
        }
        ++peekedFrame;
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


        cv::putText(merged, "peek test, ESC to exit. Frame: " + std::to_string(peekedFrame), cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
        cv::imshow("Tracking Frame", merged);
    }

    

    sdk->disconnect();
    sdk->release();

    return 0;

}