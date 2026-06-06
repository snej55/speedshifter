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

    Shortcut {
        sequence: "Space"
        onActivated: {
            if (musicPath.text) {
                playButton.clicked();
            }
        }
    }

    Shortcut {
        sequence: "K"
        onActivated: {
            if (musicPath.text) {
                playButton.clicked();
            }
        }
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

                readonly property real itemWidth: 4
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
                        height: Math.max(4, modelData * parent.height * 0.8 * Math.min(1.0, Math.max(0.0, 1.0 - Math.pow((index * waveListView.itemWidth - (waveListView.posX + waveListView.width / 2)) / (waveListView.width / 2), 49.0))))
                        Behavior on height {
                            NumberAnimation {
                                duration: 50
                                easing.type: Easing.InOutQuad
                            }
                        }
                        color: (index * waveListView.itemWidth) <= (waveListView.posX + waveListView.width / 2) ? root.palette.highlight : root.palette.placeholderText

                        Behavior on color {
                            ColorAnimation {
                                duration: 130
                                easing.type: Easing.Linear
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
                    else {
                        if (player.position == player.duration) {
                            player.position = 0;
                        }
                        player.play();
                    }
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

                Binding on value {
                    value: Math.round(player.speed * 100)
                }
                onValueModified: player.speed = value / 100

                validator: IntValidator {
                    bottom: speedSpinBox.from
                    top: speedSpinBox.to
                }

                textFromValue: function (value, locale) {
                    return Number(value / 100).toLocaleString(locale, 'f', 2) + "x";
                }

                valueFromText: function (text, locale) {
                    let clean = text.replace(/[^0-9.,-]/g, "");
                    let num = Number.fromLocalString(locale, clean);
                    if (isNaN(num)) {
                        return speedSpinBox.value;
                    }

                    return Math.round(num * 100);
                }

                Layout.preferredWidth: 120
                Layout.alignment: Qt.AlignVCenter
            }
        }
    }
}
