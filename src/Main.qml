// include relevant modules
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

// basic features required for kirigami application
Kirigami.ApplicationWindow {
    // unique identifier to reference object
    id: root

    width: 400
    height: 300

    // i18nc() makes string translateable
    // and provides context
    title: i18nc("@title:window", "Hello World!")

    // set first page that will be loaded when app opens
    pageStack.initialPage: Kirigami.Page {
        Controls.Label {
            anchors.centerIn: parent
            text: i18n("Hello World!")
        }
    }
}
