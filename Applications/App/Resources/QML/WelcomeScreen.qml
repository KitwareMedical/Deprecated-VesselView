import QtQuick 1.0

Rectangle  {

    id: welcomeRectangle
    width: 500
    height: 300
    SystemPalette { id: activePalette; colorGroup: SystemPalette.Active }
    color: activePalette.base

    property string selectedModule: ""
    property int selectedLayout: -1
    property int generalMargin: 5

    ListModel {
        id: welcomeScreenModel

        ListElement {
            name: "Interactive Segment Tubes"
            module: "InteractiveSegmentTubes"
            imageSource: "InteractiveSegmentTubesIcon.svg"
            description:  "Interactively segment the vessels of a given image."
            layout: -1
        }
        ListElement {
            name: "Workflow"
            module: "Workflow"
            imageSource: "Workflow.svg"
            description:  "Workflow that guides you on all the necessary steps requiredt to show the vessels of an image."
            layout: -1
        }
        ListElement {
            name: "Tortuosity"
            module: "Tortuosity"
            imageSource: "Workflow.svg"
            description:  "Compute tortuosity metrics on vessels."
            layout: 4 // SlicerLayoutOneUp3DView
        }
        ListElement {
            name: "Spatial Objects Display"
            module: "SpatialObjects"
            imageSource: "Workflow.svg"
            description: "Visualize the vessels and all their related metrics."
            layout: 1 // SlicerLayoutDefaultView
        }
    }

    ListView {
        id: welcomeListView
        anchors.left: parent.left
        anchors.leftMargin: generalMargin
        anchors.bottom: parent.bottom
        anchors.bottomMargin: generalMargin
        anchors.top: parent.top
        anchors.topMargin: generalMargin
        anchors.rightMargin: generalMargin
        width: (parent.width - 2*generalMargin) / 3

        focus: true
        highlightFollowsCurrentItem: true
        spacing: 10

        model: welcomeScreenModel
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
            radius: generalMargin

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
                anchors.bottomMargin: generalMargin
                anchors.horizontalCenter: elementItem.horizontalCenter
                text: name
                color: activePalette.text
                font.pixelSize: 22
                z: 1
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    selectedModule = module
                    selectedLayout = layout

                    descriptionRectangleText.text = description
                    descriptionRectangleImage.source = imageSource
                }
                onDoubleClicked: {
                    welcomeScreen.loadModule(selectedModule, selectedLayout)
                }
                z: 2
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

    Rectangle {
        id: descriptionRectangle
        color: activePalette.base
        anchors.rightMargin: generalMargin
        anchors.right: parent.right
        anchors.left: welcomeListView.right
        anchors.leftMargin: generalMargin
        anchors.bottom: parent.bottom
        anchors.bottomMargin: generalMargin
        anchors.top: parent.top
        anchors.topMargin: generalMargin

        Image {
            id: descriptionRectangleImage
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 0
            height: Math.floor( (parent.height - generalMargin) / 4)
            fillMode: Image.PreserveAspectFit
        }

        Text {
            id: descriptionRectangleText
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.top: descriptionRectangleImage.bottom
            anchors.topMargin: generalMargin
            height: Math.floor( (parent.height - generalMargin) / 2)

            wrapMode: Text.WordWrap
            font.pointSize: 20
            verticalAlignment: Text.AlignTop
            horizontalAlignment: Text.AlignHCenter
            z: 1
        }

        Text {
            id: openText
            text: "Open in VesselView"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.top: descriptionRectangleText.bottom
            anchors.topMargin: generalMargin
            height: Math.floor( (parent.height - generalMargin) / 4)

            font.pointSize: 22
            color: activePalette.text
            z: 1
        }
        Rectangle {
            id: openRectangle
            color: activePalette.button
            anchors.fill: openText
            anchors.bottomMargin: Math.floor( (openText.height - openText.paintedHeight) / 2) - generalMargin
            anchors.topMargin: anchors.bottomMargin
            anchors.rightMargin: Math.floor( (openText.width - openText.paintedWidth) / 2) - generalMargin
            anchors.leftMargin: anchors.rightMargin
            border.color: activePalette.dark
            radius: generalMargin
        }
        MouseArea {
            anchors.fill: openRectangle
            hoverEnabled: true
            onEntered: {
                openRectangle.color = activePalette.highlight
                openText.color = activePalette.highlightedText
            }
            onExited: {
                openRectangle.color = activePalette.button
                openText.color = activePalette.text
            }
            onClicked: {
                welcomeScreen.loadModule(selectedModule, selectedLayout)
            }
        }
    }
}
