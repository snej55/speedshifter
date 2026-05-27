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

    function basename(str) {
        return (str.slice(str.lastIndexOf("/") + 1));
    }

    FileDialog {
        id: musicSelect
        nameFilters: ["Audio file (*.mp3 *.wma *.wav *.ogg *.flac)"]
        onAccepted: {
            player.position = 0;
            player.pause();
            player.filePath = selectedFile.toString();
            player.loadFile(selectedFile);
        }
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            Action {
                text: qsTr("&Open...")
                shortcut: "Ctrl+O"
                onTriggered: musicSelect.open()
            }
            MenuSeparator {}
            Action {
                text: qsTr("&Quit")
                shortcut: "Ctrl+Q"
                onTriggered: Qt.quit()
            }
        }
    }

    ColumnLayout {
        enabled: musicPath.text ? true : false
        anchors.fill: parent

        Item {
            Layout.fillHeight: true
        }

        Slider {
            id: playbackSlider
            from: 0
            handle: null
            to: player.duration
            value: playbackSlider.pressed ? value : player.position

            Layout.leftMargin: 20
            Layout.rightMargin: 20
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignBottom
            //Layout.bottomMargin: 10

            transform: Scale {
                yScale: 1.8
                origin.y: playbackSlider.height / 2
            }

            onMoved: {
                player.position = value;
            }

            onPressedChanged: {
                if (!pressed) {
                    player.position = value;
                }
            }

            TapHandler {
                id: sliderTapHandler
                onTapped: eventPoint => {
                    let ratio = eventPoint.position.x / playbackSlider.width;
                    let target = playbackSlider.from + ratio * (playbackSlider.to - playbackSlider.from);
                    player.position = Math.max(playbackSlider.from, Math.min(target, playbackSlider.to));
                }
            }
        }

        GridLayout {
            Layout.alignment: Qt.AlignBottom
            Layout.bottomMargin: 20
            Layout.leftMargin: 20
            Layout.rightMargin: 20
            Layout.topMargin: 10

            Button {
                id: playButton
                icon.name: player.playing ? "media-playback-pause" : "media-playback-start"
                onClicked: {
                    if (player.playing)
                        player.pause();
                    else
                        player.play();
                }
            }

            Button {
                id: stopButton
                icon.name: "media-playback-stop"

                onClicked: {
                    player.position = 0;
                    player.pause();
                }
            }

            Label {
                id: musicPath
                text: basename(player.filePath)
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }
}
