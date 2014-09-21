import QtQuick 1.0

Rectangle  {
    id: welcomeRectangle
    width: 640; height: 480
    SystemPalette { id: activePalette }

    ListView {
        id: menuListView
        width: 200

        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 0

        model: ListModel {
            id: welcomeScreenModel

            ListElement {
                name: "Interactive Segment Tubes"
            }
            ListElement {
                name: "Workflow"
            }
            ListElement {
                name: "Tortuosity"
            }
            ListElement {
                name: "Spatial Objects Display"
            }
        }

        delegate: Text { text: name }

        highlight: Rectangle { color: activePalette.highlightedText; radius: 5 }
    }
}
