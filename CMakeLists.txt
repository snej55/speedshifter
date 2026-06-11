cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

project(speedshifter VERSION 1.0.0 LANGUAGES CXX)

# Find the Qt Quick library
find_package(Qt6 6.5 REQUIRED COMPONENTS Quick Svg)

qt_policy(SET QTP0004 NEW)
qt_policy(SET QTP0001 NEW)
qt_standard_project_setup()

if (WIN32 OR APPLE OR ANDROID)
    set(AVFORMAT_INCLUDE_DIRS "" CACHE STRING "Injected avformat include path")
    set(AVFORMAT_LIBRARIES "" CACHE STRING "Injected avformat library path")
    
    set(AVCODEC_INCLUDE_DIRS "" CACHE STRING "Injected avcodec include path")
    set(AVCODEC_LIBRARIES "" CACHE STRING "Injected avcodec library path")
    
    set(AVUTIL_INCLUDE_DIRS "" CACHE STRING "Injected avutil include path")
    set(AVUTIL_LIBRARIES "" CACHE STRING "Injected avutil library path")
    
    set(SWRESAMPLE_INCLUDE_DIRS "" CACHE STRING "Injected swresample include path")
    set(SWRESAMPLE_LIBRARIES "" CACHE STRING "Injected swresample library path")
    message(STATUS "Configuring manually injected FFmpeg pathways for Desktop/Mobile fallback architectures")
endif()

add_subdirectory(src)
