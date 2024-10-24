/* 
 *  SLAMTEC AURORA DEMO
 *  This demo shows how to get the imu data from the aurora device
 *  It will print the imu data in the terminal
 */

#include <cstddef>
#include <cstdio>
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


    uint64_t lastTimestamp = 0;
    while (!isCtrlC) {

        // get the imu data
        // the sdk will cache the imu data, so the peekIMUData() is non-blocking
        // alternatively, you can use a listener to get the imu data
        std::vector<slamtec_aurora_sdk_imu_data_t> imuData;
        if (sdk->dataProvider.peekIMUData(imuData)) {
            for (auto& imu : imuData) {
               if (imu.timestamp_ns <= lastTimestamp) {
                    // ignore the old data that has been fetched before
                    continue;
               }
               lastTimestamp = imu.timestamp_ns;
               std::cout << "IMU Data: Accel: " << imu.acc[0] << ", " << imu.acc[1] << ", " << imu.acc[2] << " Gyro: " << imu.gyro[0] << ", " << imu.gyro[1] << ", " << imu.gyro[2] << std::endl;
            }
        } else {
            std::cerr << "Failed to get imu data" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    sdk->disconnect();
    sdk->release(); // alternatively, you can release the sdk suing RemoteSDK::ReleaseSession()
    return 0;
}