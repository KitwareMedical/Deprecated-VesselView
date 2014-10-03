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
    property int generalSpacing: 2*generalMargin
    property real gradientStart: 0.8
    property real gradientEnd: 1.0

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
            description:  "Workflow that guides you on all the necessary steps required to show the vessels of an image."
            layout: -1
        }
        ListElement {
            name: "Tortuosity"
            module: "Tortuosity"
            imageSource: "VesselViewLogo.svg"
            description:  "Compute tortuosity metrics on vessels."
            layout: 4 // SlicerLayoutOneUp3DView
        }
        ListElement {
            name: "Spatial Objects Display"
            module: "SpatialObjects"
            imageSource: "VesselViewLogo.svg"
            description: "Visualize the vessels and all their related metrics."
            layout: 1 // SlicerLayoutDefaultView
        }
    }

    function elementHeightFunction(height, numberOfElements, spacing)
    {
        return Math.floor( (height - (numberOfElements-1)*spacing) / numberOfElements )
    }
    property int elementHeight : elementHeightFunction(welcomeRectangle.height - 2*generalMargin, welcomeListView.count + 1, generalSpacing)

    Rectangle {
        id: aboutRectangle
        anchors.left: parent.left
        anchors.leftMargin: generalMargin
        anchors.top: parent.top
        anchors.topMargin: generalMargin
        anchors.rightMargin: generalMargin
        width: Math.floor((parent.width - 2*generalMargin) / 3)
        height: elementHeight + generalSpacing

        color: activePalette.base

        Rectangle {
            id: aboutImageRectangle
            anchors.fill: parent
            anchors.bottomMargin: welcomeListView.spacing
            color: activePalette.base

            Image {
                id: aboutImage
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                source: ":/Icons/Medium/kitware_full_logo.svg"
            }
        }
    }

    ListView {
        id: welcomeListView
        spacing: generalSpacing
        anchors.left: parent.left
        anchors.leftMargin: generalMargin
        anchors.bottom: parent.bottom
        anchors.bottomMargin: generalMargin
        anchors.rightMargin: generalMargin
        anchors.top: aboutRectangle.bottom
        width: Math.floor((parent.width - 2*generalMargin) / 3)

        focus: true
        highlightFollowsCurrentItem: true

        model: welcomeScreenModel
        delegate: Rectangle {

            id: elementItem
            height: elementHeight
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
                    openButton.visible = true
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
            source: ":/Icons/Medium/VesselViewSplashScreen.svg"
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

            text: "<html>
                   VesselView is a open-source custom graphical interface to the vessel segmentation, registration, and analysis
                   methods of Kitware’s <a href=\"http://www.tubetk.org/\">TubeTK</a> software toolkit.
                   <p>
                   Based on <a href=\"http://www.slicer.org/\">3D Slicer</a>, VesselView is free for academic and commercial use.
                   Furthermore, it can be customized by Kitware to provide workflows that streamline the analysis of vessels for
                   tumor malignancy assessment, surgical planning, surgical guidance, stroke assessment, and more.

                   For more information, visit <a href=\"http://public.kitware.com/Wiki/TubeTK/About\">http://public.kitware.com/Wiki/TubeTK/About</a>
                   </html>
                  "
            onLinkActivated: Qt.openUrlExternally(link)
        }

        Item
            {
            id: openButton
            visible: false

            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.top: descriptionRectangleText.bottom
            anchors.topMargin: generalMargin
            height: Math.floor( (parent.height - generalMargin) / 4)

            Text {
                id: openText
                text: "Open in VesselView"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.fill: parent

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

}
