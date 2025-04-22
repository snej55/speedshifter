cmake_minimum_required(VERSION 3.20)
project(speedshifter)

set(CMAKE_CXX_STANDARD 17)

find_package(ECM 6.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMFindQmlModule)
include(ECMQmlModule)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Quick
    Test
    Gui
    QuickControls2
    Widgets
    Qml
    Multimedia
)

find_package(KF6 REQUIRED COMPONENTS
    Kirigami
    I18n
    CoreAddons
    QQC2DesktopStyle
    IconThemes
)

find_package(PkgConfig) #finding pkg-config is a helper tool

#using pkg-config to getting Gstreamer
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)

message("GSTREAMER_LIBRARIES: ${GSTREAMER_LIBRARIES}") # gstreamer-1.0;gobject-2.0;glib-2.0
message("GLIB_INCLUDE_DIRS: ${GLIB_INCLUDE_DIRS}")
message("GSTREAMER_INCLUDE_DIRS: ${GSTREAMER_INCLUDE_DIRS}")
#including GStreamer header files directory
include_directories(
        ${GLIB_INCLUDE_DIRS}
        ${GSTREAMER_INCLUDE_DIRS}
)

message("GLIB_LIBRARY_DIRS: ${GLIB_LIBRARY_DIRS}")
message("GSTREAMER_LIBRARY_DIRS: ${GSTREAMER_LIBRARY_DIRS}")
#linking GStreamer library directory
link_directories(
        ${GLIB_LIBRARY_DIRS}
        ${GSTREAMER_LIBRARY_DIRS}
)

ecm_find_qmlmodule(org.kde.kirigami REQUIRED)

add_subdirectory(src)

install(PROGRAMS org.speedshifter.desktop DESTINATION ${KDE_INSTALL_APPDIR})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
