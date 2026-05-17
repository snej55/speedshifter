import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs

import speedshifter

ApplicationWindow {
    id: root

    width: 640
    height: 480
    visible: true
    title: qsTr("speedshifter")

    Player {
        id: player
    }

    function basename(str)
    {
        return (str.slice(str.lastIndexOf("/") + 1))
    }

    FileDialog {
        id: musicSelect
        nameFilters : ["Audio file (*.mp3 *.wma *.wav *.ogg *.flac)"]
        onAccepted: {
            player.position = 0
            player.pause()
            player.filePath = selectedFile.toString()
            player.loadFile(selectedFile)
        }
    }

    ColumnLayout {
        enabled: musicPath.text ? true : false
        anchors.fill: parent

        Label {
            id: musicPath
            text: basename(player.filePath)
            Layout.alignment: Qt.AlignHCenter
        }

        Slider
        {
            Layout.alignment: Qt.AlignHCenter
            id: playbackSlider
            from: 0
            to: player.duration
            value: pressed ? value : player.position
            live: true

            onMoved: {
                player.position = value
            }
        }

        GridLayout {
            Layout.alignment: Qt.AlignHCenter
            Button {
                id: playButton
                icon.name: player.playing ? "media-playback-pause" : "media-playback-start"
                onClicked: {
                    if (player.playing)
                        player.pause()
                    else
                        player.play()
                }
            }

            Button {
                id: stopButton
                icon.name: "media-playback-stop"

                onClicked: {
                    player.position = 0
                    player.pause()
                }
            }
        }
    }

    Button {
        id: musicOpen
        icon.name: "document-open"
        onClicked: musicSelect.open()
    }
}
