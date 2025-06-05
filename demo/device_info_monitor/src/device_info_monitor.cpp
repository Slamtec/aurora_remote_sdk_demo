/* 
 *  SLAMTEC AURORA DEMO
 *  This demo shows how to continuously monitor device basic information
 *  Features include:
 *  - Retrieve device name, serial number, firmware/hardware versions
 *  - Monitor device model, feature set, and uptime
 *  - Continuous real-time updates in console
 *  - Clean formatted output with timestamps
 */

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <signal.h>
#include <iomanip>
#include <sstream>
#include <cstring>

#include "aurora_pubsdk_inc.h"

using namespace rp::standalone::aurora;

static int isCtrlC = 0;

static void onCtrlC(int) {
    std::cout << "\nCtrl-C pressed, exiting..." << std::endl;
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
    std::cout << "Selected first device: " << selectedDeviceDesc[0].toLocatorString() << std::endl;
    return true;
}

static std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

static std::string formatUptime(uint64_t uptimeUs) {
    uint64_t seconds = uptimeUs / 1000000;  // Convert microseconds to seconds
    uint64_t minutes = seconds / 60;
    uint64_t hours = minutes / 60;
    uint64_t days = hours / 24;
    
    seconds %= 60;
    minutes %= 60;
    hours %= 24;
    
    std::stringstream ss;
    if (days > 0) {
        ss << days << "d ";
    }
    if (hours > 0 || days > 0) {
        ss << hours << "h ";
    }
    if (minutes > 0 || hours > 0 || days > 0) {
        ss << minutes << "m ";
    }
    ss << seconds << "s";
    
    return ss.str();
}




static void printDeviceInfo(const rp::standalone::aurora::RemoteDeviceBasicInfo& deviceInfo, int updateCount) {
    // Clear screen for clean output (works on most terminals)
    std::cout << "\033[2J\033[H";
    
    std::cout << "==================== AURORA DEVICE MONITOR ====================" << std::endl;
    std::cout << "Timestamp: " << getCurrentTimestamp() << " | Update #" << updateCount << std::endl;
    std::cout << "=================================================================" << std::endl;
    
    std::cout << std::left << std::setw(20) << "Device Name:" 
              << std::string(deviceInfo.device_name) << std::endl;
    
    std::cout << std::left << std::setw(20) << "Serial Number:" 
              << deviceInfo.getDeviceSerialNumberString() << std::endl;
    
    std::cout << std::left << std::setw(20) << "Device Model:" 
              << deviceInfo.getDeviceModelString() << std::endl;
    
    std::cout << std::left << std::setw(20) << "Firmware Version:" 
              << std::string(deviceInfo.firmware_version_string) << std::endl;
    
    std::cout << std::left << std::setw(20) << "Firmware Build:" 
              << std::string(deviceInfo.firmware_build_date) << " " << std::string(deviceInfo.firmware_build_time) << std::endl;
    
    std::cout << std::left << std::setw(20) << "HW Features:" 
              << deviceInfo.hwfeature_bitmaps << std::endl;
    
    std::cout << std::left << std::setw(20) << "Sensing Features:" 
              << deviceInfo.sensing_feature_bitmaps << std::endl;
    
    std::cout << std::left << std::setw(20) << "SW Features:" 
              << deviceInfo.swfeature_bitmaps << std::endl;
    
    std::cout << std::left << std::setw(20) << "Device Uptime:" 
              << formatUptime(deviceInfo.device_uptime_us) << std::endl;
    
    std::cout << "=================================================================" << std::endl;
    std::cout << "Press Ctrl+C to exit..." << std::endl;
}

static void printUsage(const char* programName) {
    std::cout << "\nUsage: " << programName << " [device_connection_string]" << std::endl;
    std::cout << "\nThis demo continuously monitors Aurora device basic information." << std::endl;
    std::cout << "If no connection string is provided, it will auto-discover devices." << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  " << programName << "                    # Auto-discover device" << std::endl;
    std::cout << "  " << programName << " tcp://192.168.1.100:8090  # Connect to specific device" << std::endl;
    std::cout << std::endl;
}

int main(int argc, const char* argv[]) {
    // register the ctrl-c signal handler
    signal(SIGINT, onCtrlC);

    const char* connectionString = nullptr;
    
    // Parse command line arguments
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg.find("://") != std::string::npos) {
            connectionString = argv[1];
        } else {
            std::cerr << "Error: Invalid argument '" << arg << "'" << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }

    std::cout << "Aurora Device Info Monitor" << std::endl;
    std::cout << "==========================" << std::endl;

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
    std::cout << "Starting device info monitoring..." << std::endl;
    
    // Give the device a moment to initialize
    std::this_thread::sleep_for(std::chrono::seconds(2));

    int updateCount = 0;
    uint64_t timestamp_ns;
    
    // Main monitoring loop
    while (!isCtrlC) {
        // Retrieve current device basic info using the C++ wrapper
        rp::standalone::aurora::RemoteDeviceBasicInfo deviceInfoWrapper;
        if (sdk->dataProvider.getLastDeviceBasicInfo(deviceInfoWrapper, timestamp_ns)) {
            updateCount++;
            
            // The wrapper actually contains the same structure, so we can use it directly
            printDeviceInfo(deviceInfoWrapper, updateCount);
        } else {
            std::cerr << "Failed to retrieve device basic info (update #" << updateCount + 1 << ")" << std::endl;
        }
        
        // Wait for 1 second before next update
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "\nStopping device info monitoring..." << std::endl;
    sdk->disconnect();
    sdk->release();

    std::cout << "Device info monitor completed. Total updates: " << updateCount << std::endl;
    return 0;
}