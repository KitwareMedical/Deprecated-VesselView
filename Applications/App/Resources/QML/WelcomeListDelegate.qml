import QtQuick 1.0

Component {
    id: welcomeListDelegate

    Rectangle {
        id: elementItem
        height: elementHeight
        width: welcomeListWidth
        color: activePalette.button
        border.color: activePalette.dark
        radius: generalMargin

        Image {
            id: elementImage
            width: elementItem.width
            height: elementItem.height - elementText.height - 2*generalMargin
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: generalMargin
            fillMode: Image.PreserveAspectFit
            source: imageSource
            }

        Text {
            id: elementText
            anchors.bottom: elementItem.bottom
            anchors.bottomMargin: generalMargin
            anchors.horizontalCenter: elementItem.horizontalCenter
            text: name
            color: activePalette.text
            font.pixelSize: 16
            z: 1
            clip: true
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                elementItem.ListView.view.currentIndex = index

            }
            onDoubleClicked: {
                welcomeScreen.loadModule(selectedModule, selectedLayout)
            }
            z: 2
        }

        states: [
            State {
                name: "selected"
                when: elementItem.ListView.view.currentIndex == index
                PropertyChanges {target: elementItem; color: activePalette.dark;}
            }
        ]
    }
}

