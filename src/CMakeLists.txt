qt_add_executable(speedshifter
    main.cpp
)

qt_add_qml_module(speedshifter
    URI speedshifter
    VERSION 1.0
    QML_FILES
        qml/Main.qml
)

# Use the QtQuick module from Qt 6.
target_link_libraries(speedshifter PRIVATE Qt6::Quick)

# Install the executable
install(TARGETS speedshifter DESTINATION bin)
