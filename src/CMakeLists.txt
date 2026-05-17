set(BIN_NAME main)

include(FetchContent)

FetchContent_Declare(
    miniaudio
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG 0.11.25
    GIT_SHALLOW ON
)

FetchContent_MakeAvailable(miniaudio)
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

# Use the QtQuick module from Qt 6.
target_link_libraries(${BIN_NAME} PRIVATE Qt6::Quick miniaudio)

# Install the executable
install(TARGETS ${BIN_NAME} DESTINATION bin)
