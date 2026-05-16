set(BIN_NAME main)
qt_add_executable(${BIN_NAME}
    main.cpp
)

qt_add_qml_module(${BIN_NAME}
    URI speedshifter
    VERSION 1.0
    QML_FILES
        qml/Main.qml
)

# Use the QtQuick module from Qt 6.
target_link_libraries(${BIN_NAME} PRIVATE Qt6::Quick)

# Install the executable
install(TARGETS ${BIN_NAME} DESTINATION bin)
