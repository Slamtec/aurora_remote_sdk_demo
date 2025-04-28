/* 
 *  SLAMTEC AURORA DEMO
 *  This demo shows how to get the current pose of the aurora device
 *  It will print the current pose in the terminal
 */

#include <iostream>
#include "aurora_pubsdk_inc.h"

#include <thread>
#include <csignal>
using namespace rp::standalone::aurora;

static int isCtrlC = 0;

static void onCtrlC(int) {
    std::cout << "Ctrl-C pressed, exiting..." << std::endl;
    isCtrlC = 1;
}

bool discoverAndSelectAuroraDevice(RemoteSDK * sdk, SDKServerConnectionDesc & selectedDeviceDesc)
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


int main(int argc, char** argv) {

    // register the ctrl-c signal handler
    signal(SIGINT, onCtrlC);

    // print the version info
    slamtec_aurora_sdk_version_info_t versionInfo;
    RemoteSDK::GetSDKInfo(versionInfo);
    std::cout << "Aurora SDK Version: " << versionInfo.sdk_version_string << std::endl;



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


    // if you want to access the map data, you can start the background map data syncing
    // sdk->startBackgroundMapDataSyncing();

    while (!isCtrlC) {

        // get the latest pose
        slamtec_aurora_sdk_pose_t pose;
        
        // Note: it is recommended to get Pose in SE3 format (getCurrentPoseSE3), which is more accurate
        // The following function get pose in euler angle format, which may have gimbal lock issue
        sdk->dataProvider.getCurrentPose(pose);

        std::cout << "Current pose: " << pose.translation.x << ", " << pose.translation.y << ", " << pose.translation.z
                  << " Euler: " << pose.rpy.roll << ", " << pose.rpy.pitch << ", " << pose.rpy.yaw << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    sdk->disconnect();
    sdk->release(); // alternatively, you can release the sdk using RemoteSDK::ReleaseSession()
    return 0;
}