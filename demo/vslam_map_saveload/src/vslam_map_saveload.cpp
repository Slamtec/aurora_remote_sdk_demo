/* 
 *  SLAMTEC AURORA DEMO
 *  This demo shows how to download or upload the vslam map from the aurora device
 */

#include <iostream>
#include "aurora_pubsdk_inc.h"

#include <thread>
#include <csignal>
#include <future>
#include <iomanip>
using namespace rp::standalone::aurora;
static int isCtrlC = 0;

static void onCtrlC(int sig) {
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


void showHelp() {
    std::cout << "Usage: vslam_map_saveload [options] [-s <server_locator>] [map_file]" << std::endl;
    std::cout << "  -h, --help            Show this help message" << std::endl;
    std::cout << "  -s, --server          The server locator of the aurora device" << std::endl;
    std::cout << "                        If not specified, the demo will try to discover and select the aurora device automatically" << std::endl;
    std::cout << "  -d, --download        Download the vslam map from the aurora device" << std::endl;
    std::cout << "  -u, --upload          Upload the vslam map to the aurora device" << std::endl;
    std::cout << "  [map_file]            The map file to be downloaded or uploaded" << std::endl;
    std::cout << "                        Default: auroramap.stcm" << std::endl;
}


struct Options {
    std::string serverLocator;
    bool download = false;
    bool upload = false;
    bool autoSelectDevice = true;
    std::string mapFile = "auroramap.stcm";
};


bool downloadVSLAMMap(RemoteSDK * sdk, const std::string & mapFile) {
    std::cout << "Downloading vslam map to " << mapFile << std::endl;

    std::promise<bool> resultPromise;
    auto resultFuture = resultPromise.get_future();

    auto resultCallBack = [](void* userData, int isOK) {
        auto promise = reinterpret_cast<std::promise<bool>*>(userData);
        promise->set_value(isOK != 0);
    };

    if (!sdk->mapManager.startDownloadSession(mapFile.c_str(), resultCallBack, &resultPromise)) {
        std::cerr << "Failed to start download session" << std::endl;
        return false;
    }

    
    while (sdk->mapManager.isSessionActive()) {
        slamtec_aurora_sdk_mapstorage_session_status_t status;
        if (!sdk->mapManager.querySessionStatus(status)) {
            std::cerr << "Failed to query session status" << std::endl;
            return false;
        }
        std::cout << "Downloading vslam map " << std::fixed << std::setprecision(2) << status.progress << "%\r" << std::flush;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (isCtrlC) {
            std::cout << "Aborting download session..." << std::endl;

            sdk->mapManager.abortSession();
            return false;
        }
    }

    bool result = resultFuture.get();

    std::cout << "Downloading vslam map " << (result ? "succeeded" : "failed") << std::endl;

    return result;
}

bool uploadVSLAMMap(RemoteSDK * sdk, const std::string & mapFile) {
    std::cout << "Uploading vslam map to " << mapFile << std::endl;

    std::promise<bool> resultPromise;
    auto resultFuture = resultPromise.get_future(); 

    auto resultCallBack = [](void* userData, int isOK) {
        auto promise = reinterpret_cast<std::promise<bool>*>(userData);
        promise->set_value(isOK != 0);
    };

    if (!sdk->mapManager.startUploadSession(mapFile.c_str(), resultCallBack, &resultPromise)) {
        std::cerr << "Failed to start upload session" << std::endl;
        return false;
    }

    while (sdk->mapManager.isSessionActive()) {
        slamtec_aurora_sdk_mapstorage_session_status_t status;
        if (!sdk->mapManager.querySessionStatus(status)) {
            std::cerr << "Failed to query session status" << std::endl;
            return false;
        }

        std::cout << "Uploading vslam map " << std::fixed << std::setprecision(2) << status.progress << "%\r" << std::flush;
        
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (isCtrlC) {
            std::cout << "Aborting upload session..." << std::endl;
            sdk->mapManager.abortSession();
            return false;
        }
    }

    bool result = resultFuture.get();

    std::cout << "Uploading vslam map " << (result ? "succeeded" : "failed") << std::endl;

    return result;
}

bool parseOptions(int argc, char** argv, Options & options) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            showHelp();
            return false;
        } else if (arg == "-s" || arg == "--server") {
            if (i + 1 >= argc) {
                std::cerr << "Error: --server option requires a server locator" << std::endl;
                return false;
            }
            options.autoSelectDevice = false;
            options.serverLocator = argv[++i];
        } else if (arg == "-d" || arg == "--download") {
            options.download = true;
        } else if (arg == "-u" || arg == "--upload") {
            options.upload = true;
        }  else {
            options.mapFile = arg;
        }
    }
    return true;
}

int main(int argc, char** argv) {


    signal(SIGINT, onCtrlC);

    Options options;
    if (!parseOptions(argc, argv, options)) {
        showHelp();
        return -1;
    }

    auto sdk = RemoteSDK::CreateSession();
    if (!sdk) {
        std::cerr << "Failed to create SDK session" << std::endl;
        return -1;
    }

    if (options.download && options.upload) {
        std::cerr << "Error: --download and --upload cannot be specified at the same time" << std::endl;
        return -1;
    }

    if (!options.download && !options.upload) {
        // default to download
        options.download = true;
        std::cout << "Defaulting to download" << std::endl;
    }

    if (options.autoSelectDevice) {
        SDKServerConnectionDesc selectedDeviceDesc;
        std::cout << "Trying to discover and select aurora device..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));

        if (!discoverAndSelectAuroraDevice(sdk, selectedDeviceDesc)) {
            std::cerr << "Failed to discover and select aurora device" << std::endl;
            return -1;
        }

        if (!sdk->connect(selectedDeviceDesc)) {
            std::cerr << "Failed to connect to aurora device" << std::endl;
            return -1;
        }
    } else {
        std::cout << "Connecting to aurora device: " << options.serverLocator << std::endl;
        if (!sdk->connect(SDKServerConnectionDesc(options.serverLocator.c_str()))) {
            std::cerr << "Failed to connect to aurora device" << std::endl;
            return -1;
        }
    }


    if (options.download) {
        downloadVSLAMMap(sdk, options.mapFile);
    } else if (options.upload) {
        uploadVSLAMMap(sdk, options.mapFile);
    }

    sdk->disconnect();
    sdk->release();

    std::cout << "Done" << std::endl;
    return 0;
}