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
    title: i18nc("@title:window", "Speed Shifter")

    globalDrawer: Kirigami.GlobalDrawer {
        isMenu: true
        actions: [
            Kirigami.Action {
                text: i18n("Quit")
                icon.name: "application-exit-symbolic"
                shortcut: StandardKey.Quit
                onTriggered: Qt.quit()
            }
        ]
    }

    // set first page that will be loaded when app opens
    pageStack.initialPage: Kirigami.ScrollablePage {
        title: i18nc("@title", "Speed Shifter")

        ColumnLayout {
            anchors.fill: parent
            Controls.Dial {
                id: speedSlider
                Layout.alignment: Qt.AlignCenter
                Layout.fillWidth: true
                width: maximumWidth
                snapMode: Controls.Dial.SnapAlways
                value: 100
                to: 200
                stepSize: 4.0
                Controls.Label {
                    Layout.alignment: Qt.AlignHCenter
                    anchors.centerIn: parent
                    text: speedSlider.value
                }
            }


        }
    }
}
