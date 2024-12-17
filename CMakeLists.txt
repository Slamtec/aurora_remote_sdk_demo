cmake_minimum_required(VERSION 3.16)
project(Aurora_Remote_SDK_Demo)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF()

MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})


# Check C++14 support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
if(COMPILER_SUPPORTS_CXX14)

else()
   if (NOT WIN32)
       message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
   else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++14")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
   endif()
endif()


option(ENABLE_DEMOS_WITH_OPENCV "Build Demo with OpenCV" ON)
if(ENABLE_DEMOS_WITH_OPENCV)
    find_package(OpenCV 4.2 QUIET)
    if(OpenCV_FOUND)
        MESSAGE("OPENCV VERSION:")
        MESSAGE(${OpenCV_VERSION})
    else()
        MESSAGE(WARNING "OpenCV > 4.2 not found. Demos that use OpenCV will not be compiled.")
        set(ENABLE_DEMOS_WITH_OPENCV OFF)
    endif()
else()
    MESSAGE("OpenCV support is disabled.")
endif()




# Determine platform and architecture
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(AURORA_LIB_PATH "${PROJECT_SOURCE_DIR}/aurora_remote_public/lib/win64")
    else()
        message(FATAL_ERROR "Unsupported architecture for Windows. Only x64 is supported.")
    endif()
elseif(UNIX AND NOT APPLE)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
        set(AURORA_LIB_PATH "${PROJECT_SOURCE_DIR}/aurora_remote_public/lib/linux_aarch64")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
        set(AURORA_LIB_PATH "${PROJECT_SOURCE_DIR}/aurora_remote_public/lib/linux_x86_64")
    else()
        message(FATAL_ERROR "Unsupported architecture for Linux. Only aarch64 and x86_64 are supported.")
    endif()
else()
    message(FATAL_ERROR "Unsupported platform. Only Windows and Linux are supported.")
endif()

# Set the library name
set(AURORA_LIB_NAME slamtec_aurora_remote_sdk)

# Add the determined library path to the linker search path
include_directories(${PROJECT_SOURCE_DIR}/aurora_remote_public/include)
link_directories(${AURORA_LIB_PATH})




message(STATUS "Aurora SDK library path: ${AURORA_LIB_PATH}")




# Build Demos

## simple_pose
add_executable(simple_pose demo/simple_pose/src/simple_pose.cpp)
target_link_libraries(simple_pose ${AURORA_LIB_NAME})
# copy the .so or .dll with ${AURORA_LIB_NAME} to the build directory
add_custom_command(TARGET simple_pose POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory  
    ${AURORA_LIB_PATH}
    $<TARGET_FILE_DIR:simple_pose>)

## relocalization
add_executable(relocalization demo/relocalization/src/relocalization.cpp)
target_link_libraries(relocalization ${AURORA_LIB_NAME})
# copy the .so or .dll with ${AURORA_LIB_NAME} to the build directory
add_custom_command(TARGET relocalization POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory  
    ${AURORA_LIB_PATH}
    $<TARGET_FILE_DIR:relocalization>)

## pure_c_demo
add_executable(pure_c_demo demo/pure_c_demo/src/pure_c_demo.c)
target_link_libraries(pure_c_demo ${AURORA_LIB_NAME})


## imu_fetcher
add_executable(imu_fetcher demo/imu_fetcher/src/imu_fetcher.cpp)
target_link_libraries(imu_fetcher ${AURORA_LIB_NAME})


## vslam_map_saveload
add_executable(vslam_map_saveload demo/vslam_map_saveload/src/vslam_map_saveload.cpp)
target_link_libraries(vslam_map_saveload ${AURORA_LIB_NAME})

## frame preview, only enabled when ENABLE_DEMOS_WITH_OPENCV is ON
if(ENABLE_DEMOS_WITH_OPENCV)

## frame preview
add_executable(frame_preview demo/frame_preview/src/frame_preview.cpp)
target_include_directories(frame_preview PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(frame_preview ${AURORA_LIB_NAME} ${OpenCV_LIBS})

## map render
add_executable(map_render demo/map_render/src/map_render.cpp)
target_include_directories(map_render PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(map_render ${AURORA_LIB_NAME}  ${OpenCV_LIBS})
    

endif()
