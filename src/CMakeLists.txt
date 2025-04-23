add_executable(speedshifter)

ecm_add_qml_module(speedshifter
    URI
    org.speedshifter
)

set(CMAKE_AUTORCC ON)

qt_add_resources(SOURCES resources.qrc)

target_sources(speedshifter
    PRIVATE
    main.cpp
    player.h
    player.cpp
    timer.h
    timer.cpp
)

ecm_target_qml_sources(speedshifter
    SOURCES
    Main.qml
    resources.qrc
)

if (CMAKE_SYSTEM MATCHES Linux)
    target_link_libraries(speedshifter
        PRIVATE
        Qt6::Core
        Qt6::Quick
        Qt6::Qml
        Qt6::Gui
        Qt6::QuickControls2
        Qt6::Widgets
        Qt6::Multimedia
        KF6::I18n
        KF6::CoreAddons
        KF6::IconThemes
        ${GSTREAMER_LIBRARIES}
    )
elseif (CMAKE_SYSTEM MATCHES Windows)
    target_link_libraries(speedshifter
        PRIVATE
        Qt6::Core
        Qt6::Quick
        Qt6::Qml
        Qt6::Gui
        Qt6::QuickControls2
        Qt6::Widgets
        Qt6::Multimedia
        KF6::I18n
        KF6::CoreAddons
        KF6::IconThemes
        gstreamer-1.0
        gobject-2.0
        glib-2.0
    )
endif()

install(TARGETS speedshifter ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
