import QtQuick 1.0

Rectangle  {

    id: welcomeRectangle
    width: 640
    height: 480
    SystemPalette { id: activePalette; colorGroup: SystemPalette.Active }
    color: activePalette.base

    property string selectedModule: ""

    ListView {
        id: welcomeListView
        anchors.right: parent.right
        anchors.rightMargin: 2 * Math.floor(parent.width / 3) // 1/3 from the left -> 2/3 from the left
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        anchors.top: parent.top
        anchors.topMargin: 5

        focus: true
        highlightFollowsCurrentItem: true
        spacing: 10

        model: ListModel {
            id: welcomeScreenModel

            ListElement {
                name: "Interactive Segment Tubes"
                module: "InteractiveSegmentTubes"
                imageSource: "InteractiveSegmentTubesIcon.png"
            }
            ListElement {
                name: "Workflow"
                module: "Workflow"
                imageSource: "Workflow.png"
            }
            ListElement {
                name: "Tortuosity"
                module: "Tortuosity"
                imageSource: "Workflow.png"
            }
            ListElement {
                name: "Spatial Objects Display"
                module: "SpatialObjectsDisplay"
                imageSource: "Workflow.png"
            }
        }
        delegate: Rectangle {

            function elementHeight(listView)
            {
                return Math.floor( (listView.height - (listView.count - 1)*listView.spacing) / listView.count )
            }

            id: elementItem
            height: elementHeight( welcomeListView )
            width: welcomeListView.width
            color: activePalette.button
            border.color: activePalette.dark
            radius: 5

            Image {
                id: elementImage
                width: elementItem.width
                height: elementItem.height
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                fillMode: Image.PreserveAspectFit
                source: imageSource
                }

            Text {
                id: elementText
                anchors.bottom: elementItem.bottom
                anchors.bottomMargin: 5
                anchors.horizontalCenter: elementItem.horizontalCenter
                text: name
                color: activePalette.text
                font.pixelSize: 20
                z: 1
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    selectedModule = module;
                z: 2
                }
            }
            states: [
                State {
                    name: "selected"
                    when: (module==selectedModule)
                    PropertyChanges {target: elementItem; color: activePalette.highlight;}
                }
            ]
        }

    }

    Text { // Debug text
        text: "selectedModule: " + selectedModule + "\n "/* +
              "welcomeListView.height: " + welcomeListView.height + "\n " +
              "welcomeListView.currentItem: " + welcomeListView.currentItem + "\n " +
              "welcomeListView.currentItem.height: " + welcomeListView.currentItem.height + "\n "*/
    }
}
