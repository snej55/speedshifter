set(BIN_NAME main)

include(FetchContent)

FetchContent_Declare(
    miniaudio
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG 0.11.25
    GIT_SHALLOW ON
)

FetchContent_MakeAvailable(miniaudio)

FetchContent_Declare(
    signalsmith_stretch_download
    GIT_REPOSITORY https://github.com/Signalsmith-Audio/signalsmith-stretch.git
    GIT_TAG 1.1.0
    GIT_SHALLOW ON
)

FetchContent_Declare(
    signalsmith_linear_download
    GIT_REPOSITORY https://github.com/Signalsmith-Audio/linear.git
    GIT_TAG        main
)

FetchContent_MakeAvailable(signalsmith_stretch_download signalsmith_linear_download)

add_library(signalsmith-stretch INTERFACE)

target_include_directories(signalsmith-stretch INTERFACE
    ${signalsmith_stretch_download_SOURCE_DIR}
    ${signalsmith_linear_download_SOURCE_DIR}
)

if (NOT WIN32 AND NOT APPLE AND NOT ANDROID)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(AVFORMAT REQUIRED libavformat)
    pkg_check_modules(AVCODEC REQUIRED libavcodec)
    pkg_check_modules(AVUTIL REQUIRED libavutil)
    pkg_check_modules(SWRESAMPLE REQUIRED libswresample)
endif()

qt_add_executable(${BIN_NAME}
    main.cpp
)

qt_add_qml_module(${BIN_NAME}
    URI speedshifter
    VERSION 1.0

    QML_FILES
        qml/Main.qml

    SOURCES
        player.h
        player.cpp
)
target_include_directories(${BIN_NAME} PRIVATE
    .
    ${AVFORMAT_INCLUDE_DIRS}
    ${AVCODEC_INCLUDE_DIRS}
    ${AVUTIL_INCLUDE_DIRS}
    ${SWRESAMPLE_INCLUDE_DIRS}
)

# Use the QtQuick module from Qt 6.
target_link_libraries(${BIN_NAME} PRIVATE Qt6::Quick miniaudio signalsmith-stretch ${AVFORMAT_LIBRARIES}
    ${AVCODEC_LIBRARIES}
    ${AVUTIL_LIBRARIES}
    ${SWRESAMPLE_LIBRARIES})

# Install the executable
install(TARGETS ${BIN_NAME} DESTINATION bin)
