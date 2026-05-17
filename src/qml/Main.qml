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
            player.filePath = selectedFile.toString()
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
    }

    Button {
        id: musicOpen
        icon.name: "document-open"
        onClicked: musicSelect.open()
    }
}
