import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs

import speedshifter

ApplicationWindow {
    id: root

    width: 640
    height: 480
    minimumWidth: 360
    minimumHeight: 300
    visible: true
    title: qsTr("Speed Shifter")

    Player {
        id: player
    }

    function basename(str) {
        return str.slice(str.lastIndexOf("/") + 1);
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

        /*Item {
            Layout.fillHeight: true
        }*/

        Item {
            id: waveContainer
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: waveListView
                anchors.fill: parent
                orientation: ListView.Horizontal
                interactive: false

                model: player.displayBuffer

                readonly property real itemWidth: 2
                property real posX: player.duration > 0 ? (player.position / player.duration) * count * itemWidth - (width / 2) : 0
                contentX: posX

                Behavior on contentX {
                    NumberAnimation {
                        duration: 200 / player.speed
                        easing.type: Easing.OutCubic
                    }
                }

                // currentIndex: player.duration > 0 ? Math.floor((player.position / player.duration) * waveListView.count) : 0

                delegate: Item {
                    width: waveListView.itemWidth - waveListView.spacing
                    height: waveListView.height

                    Rectangle {
                        anchors.centerIn: parent
                        width: parent.width
                        height: Math.max(4, modelData * parent.height * 0.8)
                        color: (index * waveListView.itemWidth) <= (waveListView.posX + waveListView.width / 2) ? root.palette.highlight : root.palette.placeholderText

                        Behavior on color {
                            ColorAnimation {
                                duration: 130
                                easing.type: Easing.InOutQuad
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            id: divider
            Layout.fillWidth: true
            height: 1
            color: root.palette.buttonText
            opacity: 0.15
            Layout.bottomMargin: 5
        }

        Label {
            id: musicPath
            text: basename(player.filePath)
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: 5
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

        RowLayout {
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

            Slider {
                id: speedSlider
                from: player.minSpeed * 100
                to: player.maxSpeed * 100
                stepSize: 1
                snapMode: Slider.NoSnap

                value: player.speed * 100
                onMoved: player.speed = value / 100

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
            }

            SpinBox {
                id: speedSpinBox
                from: speedSlider.from
                to: speedSlider.to
                stepSize: 1

                value: player.speed * 100
                onValueModified: player.speed = value / 100

                validator: DoubleValidator {
                    bottom: speedSpinBox.from / 100
                    top: speedSpinBox.to / 100
                }

                textFromValue: function (value, locale) {
                    return Number(value / 100).toLocaleString(locale, 'f', 2) + "x";
                }

                valueFromText: function (text, locale) {
                    return Number.fromLocaleString(locale, text.replace("x", "")) * 100;
                }

                Layout.preferredWidth: 120
                Layout.alignment: Qt.AlignVCenter
            }
        }
    }
}
