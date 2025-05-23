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

if (CMAKE_SYSTEM MATCHES Linux)
    # using pkg-config to getting Gstreamer
# set(PKG_CONFIG_PATH )
    pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)

    #including GStreamer header files directory
    include_directories(
            ${GLIB_INCLUDE_DIRS}
            ${GSTREAMER_INCLUDE_DIRS}
    )

    #linking GStreamer library directory
    link_directories(
            ${GLIB_LIBRARY_DIRS}
            ${GSTREAMER_LIBRARY_DIRS}
    )
elseif(CMAKE_SYSTEM MATCHES Windows)
    include_directories(include)
    include_directories(include/glib-2.0)
    include_directories(lib/glib-2.0/include)
    include_directories(include/gstreamer-1.0)
    link_directories(lib)
    link_directories(lib/gio)
    link_directories(lib/graphene-1.0)
    link_directories(lib/gstreamer-1.0)
    link_directories(lib/glib-2.0)
endif()

ecm_find_qmlmodule(org.kde.kirigami REQUIRED)

add_subdirectory(src)

install(PROGRAMS org.speedshifter.desktop DESTINATION ${KDE_INSTALL_APPDIR})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
