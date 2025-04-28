/*
 * SLAMTEC AURORA DEMO
 * This demo shows how to get the current pose of the aurora device using pure C API
 * 
 */

#include <stdio.h>
#include "aurora_pubsdk_inc.h"

#include <signal.h>
#include <string.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

static int isCtrlC = 0;

static void onCtrlC(int sig) {
    printf("Ctrl-C pressed, exiting...\n");
    isCtrlC = 1;
}

int main(int argc, char** argv) {

    // register the ctrl-c signal handler
    signal(SIGINT, onCtrlC);


    const char * connectionString = NULL;
    if (argc > 1) {
        connectionString = argv[1];
    }

    slamtec_aurora_sdk_version_info_t versionInfo;
    slamtec_aurora_sdk_get_version_info(&versionInfo);
    printf("Aurora SDK Version: %s\n", versionInfo.sdk_version_string);

    slamtec_aurora_sdk_session_handle_t sdkSession = slamtec_aurora_sdk_create_session(NULL, 0, NULL, NULL);

    if (sdkSession == NULL) {
        printf("Failed to create SDK session\n");
        return -1;
    }


    if (connectionString == NULL) {
        printf("Searching for aurora devices...\n");
        // wait for 5 seconds to discover aurora devices
#if defined(_WIN32)
        Sleep(5000);
#else
        sleep(5);
#endif

        slamtec_aurora_sdk_server_connection_info_t serverConnInfo[32];
        int serverCount = slamtec_aurora_sdk_controller_get_discovered_servers(sdkSession, serverConnInfo, 32);
        if (serverCount <= 0) {
            printf("No servers discovered\n");
            return -1;
        }


        // connect to the first server
        printf("Connecting to the first server: %s\n", serverConnInfo[0].connection_info[0].address);

        if (slamtec_aurora_sdk_controller_connect(sdkSession, &serverConnInfo[0]) != SLAMTEC_AURORA_SDK_ERRORCODE_OK) {
            printf("Failed to connect to the server\n");
            return -1;
        }
    } else {
        printf("Using connection string: %s\n", connectionString);

        slamtec_aurora_sdk_server_connection_info_t info;
        memset(&info, 0, sizeof(info));
        strncpy(info.connection_info[0].address, connectionString, sizeof(info.connection_info[0].address));
        strncpy(info.connection_info[0].protocol_type, SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PROTOCOL, sizeof(info.connection_info[0].protocol_type));
        info.connection_info[0].port = SLAMTEC_AURORA_SDK_REMOTE_SERVER_DEFAULT_PORT;
        info.connection_count = 1;


        if (slamtec_aurora_sdk_controller_connect(sdkSession, &info) != SLAMTEC_AURORA_SDK_ERRORCODE_OK) {
            printf("Failed to connect to the server\n");
            return -1;
        }
    }

    // get the current pose
    while (!isCtrlC) {
        slamtec_aurora_sdk_pose_t pose;
        slamtec_aurora_sdk_dataprovider_get_current_pose(sdkSession, &pose);
        printf("Current pose: %f, %f, %f\n", pose.translation.x, pose.translation.y, pose.translation.z);
    }

    slamtec_aurora_sdk_controller_disconnect(sdkSession);
    slamtec_aurora_sdk_release_session(sdkSession);

    return 0;
}