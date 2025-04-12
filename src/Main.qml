// include relevant modules
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
// import QtQuick.Dialogs As Dialogs
import QtQuick.Dialogs
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

    function basename(str)
    {
        return (str.slice(str.lastIndexOf("/")+1))
    }

    FileDialog {
        id: musicSelect
        nameFilters: ["Audio file (*.mp3 *.wma *.wav *.ogg *.flac)"]
        onAccepted: {
            musicPath.text = basename(selectedFile.toString())
        }
    }

    // set first page that will be loaded when app opens
    pageStack.initialPage: Kirigami.ScrollablePage {
        title: i18nc("@title", "Speed Shifter")

        actions: [
            Kirigami.Action {
                text: i18n("Open file...")
                icon.name: "document-open"
                shortcut: StandardKey.Open
                onTriggered: musicSelect.open()
            }
        ]

        ColumnLayout {
            anchors.fill: parent
            Controls.Dial {
                id: speedSlider
                Layout.alignment: Qt.AlignCenter
                Layout.fillWidth: true
                snapMode: Controls.Dial.SnapAlways
                value: 100
                to: 200
                stepSize: 2.0
                Controls.Label {
                    Layout.alignment: Qt.AlignHCenter
                    anchors.centerIn: parent
                    text: (speedSlider.value > 100 ? '+' : '') + (speedSlider.value - 100) + '%'
                }
            }

            Controls.Label {
                id: musicPath
                Layout.alignment: Qt.AlignHCenter
            }

            GridLayout {
                Layout.alignment: Qt.AlignHCenter
                Controls.Button {
                    id: playButton
                    Layout.alignment: Qt.AlignHCenter
                    icon.name: "currenttrack_play"
                }

                Controls.Button {
                    id: stopButton
                    Layout.alignment: Qt.AlignHCenter
                    icon.name: "media-playback-stop"
                }
            }

            Controls.Slider {
                id: playPosition
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                value: 0
                to: 100
            }

            // Controls.Button {
            //     id: myButton
            //     text: i18n("Select file")
            //     onClicked: {
            //         musicSelect.open()
            //     }
            // }
        }
    }
}
