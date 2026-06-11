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

if (WIN32)
    set_target_properties(${BIN_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if (ANDROID)
    set_target_properties(${BIN_NAME} PROPERTIES
        QT_ANDROID_APP_ICON "${CMAKE_CURRENT_SOURCE_DIR}/../icon.png"
    )
endif()

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/../icon.png PROPERTIES
    QT_RESOURCE_ALIAS "icon.png"
)

if (APPLE)
    set_target_properties(${BIN_NAME} PROPERTIES
        MACOSX_BUNDLE_ICON_FILE "icon.icns"
    )

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/../icon.icns PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources
    )

    target_sources(${BIN_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../icon.icns
    )
endif()

if (WIN32)
    target_sources(${BIN_NAME} PRIVATE
        appicon.rc
    )
endif()

qt_add_qml_module(${BIN_NAME}
    URI speedshifter
    VERSION 1.0

    QML_FILES
        qml/Main.qml

    SOURCES
        player.h
        player.cpp
)

qt_add_resources(${BIN_NAME} speedshifter_icons
    PREFIX "/"
    FILES
        qtquickcontrols2.conf
        ${CMAKE_CURRENT_SOURCE_DIR}/../icon.png
        icons/play.svg
        icons/pause.svg
        icons/stop.svg
)

target_include_directories(${BIN_NAME} PRIVATE
    .
    ${AVFORMAT_INCLUDE_DIRS}
    ${AVCODEC_INCLUDE_DIRS}
    ${AVUTIL_INCLUDE_DIRS}
    ${SWRESAMPLE_INCLUDE_DIRS}
)

# Use the QtQuick module from Qt 6.
target_link_libraries(${BIN_NAME} PRIVATE Qt6::Quick Qt6::Svg miniaudio signalsmith-stretch ${AVFORMAT_LIBRARIES}
    ${AVCODEC_LIBRARIES}
    ${AVUTIL_LIBRARIES}
    ${SWRESAMPLE_LIBRARIES})

# Install the executable
install(TARGETS ${BIN_NAME} DESTINATION bin)
