// include relevant modules
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
// import QtQuick.Dialogs As Dialogs
import QtQuick.Dialogs
import org.kde.kirigami as Kirigami

import org.kde.speedshifter 1.0

// basic features required for kirigami application
Kirigami.ApplicationWindow {
    // unique identifier to reference object
    id: root

    width: 500
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
            },

            Kirigami.Action {
                text: i18n("About")
                icon.name: "help-about"
                onTriggered: pageStack.layers.push(aboutPage)
            }
        ]
    }

    Component {
        id: aboutPage

        Kirigami.AboutPage {
            aboutData: About
        }
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
            Player.filePath = selectedFile.toString()
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
            enabled: musicPath.text ? true : false
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
                onMoved: {
                    Player.playbackSpeed = speedSlider.value / 100
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
                    icon.name: Player.playing ? "media-playback-pause" : "media-playback-start"
                    onClicked: {
                        if (Player.playing)
                            Player.pause()
                        else
                            Player.play()
                    }
                }

                Controls.Button {
                    id: stopButton
                    Layout.alignment: Qt.AlignHCenter
                    icon.name: "media-playback-stop"

                    onClicked: {
                        console.log("stopping!")
                        Player.stop()
                    }
                }
            }

            Controls.Slider {
                id: playPosition
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                value: Player.timeElapsed
                to: Player.duration

                onMoved: {
                    Player.seek(value)
                }
            }
        }
    }
}
