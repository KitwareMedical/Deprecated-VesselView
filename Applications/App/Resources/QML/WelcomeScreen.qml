import QtQuick 1.0

Rectangle  {

    id: welcomeRectangle
    width: 500
    height: 300
    SystemPalette { id: activePalette; colorGroup: SystemPalette.Active }
    FontLoader { source: ":/Gutenberg.ttf" }
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
        ListElement {
            name: "Convert Vessel Files"
            module: "ConvertTRE"
            imageSource: "VesselViewLogo.svg"
            description: "<html>
                         The ConvertTRE module is a bridge between TubeX and TubeTK.
                         It allows to convert a TubeX compatible file to a TubeTK file and vice versa.
                         </html>"
            layout: 1 // SlicerLayoutDefaultView
        }
        ListElement {
            name: "Editor"
            module: "Editor"
            imageSource: "Editor.svg"
            description: "<html>
                         The Editor module allows you to manually segment an image.
                         It offers many different tools to makes this process quick and easy. More thourough documentation can be found
                         <a href=\"http://www.slicer.org/slicerWiki/index.php/Documentation/4.3/Modules/Editor\">here</a>.
                         <p>
                         In particular, you may want to use the <a href=\"http://public.kitware.com/Wiki/TubeTK/InteractivePDFSegmenter\">PDF segmenter</a>.
                         </html>"
            layout: 6 // SlicerLayoutOneUpRedSliceView
        }
    }

    function elementHeightFunction(height, numberOfElements, spacing)
    {
        var numberOfFullElements = Math.floor(numberOfElements)
        var heightWithoutSpaces = height - (numberOfFullElements-1) * spacing
        return Math.floor(heightWithoutSpaces / numberOfElements)
    }
    property int elementHeight : elementHeightFunction(welcomeRectangle.height - 2*generalMargin, 4.5 + 1 /*+1 for the about rectangle*/, generalSpacing)

    Rectangle {
        id: aboutRectangle
        anchors.left: parent.left
        anchors.leftMargin: generalMargin
        anchors.top: parent.top
        anchors.topMargin: 0
        width: Math.floor((parent.width - 2*generalMargin) / 3)
        height: elementHeight + generalSpacing

        color: activePalette.base
        border.color: activePalette.base
        z: 1 // So the image isn't hidden by the list view after it moved

        Rectangle {
            id: aboutImageRectangle
            anchors.fill: parent
            anchors.topMargin: welcomeListView.spacing
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

    Rectangle {
        id: moveUpConcealer
        anchors.left: aboutRectangle.anchors.left
        anchors.leftMargin: aboutRectangle.anchors.leftMargin
        anchors.top: aboutRectangle.bottom
        anchors.topMargin: 0
        width: aboutRectangle.width + 2
        height: Math.floor(elementHeight / 4) + 3 * generalSpacing

        // We need to put the rounded button in a concealer rectangle
        // otherwise we can see the list beneath it at the corners
        color: activePalette.base
        gradient: Gradient { // Fake blur with a transparency gradient
                   GradientStop { position: 0.75; color: activePalette.base }
                   GradientStop { position: 1.0; color: '#00000000' }
          }

        z: 1 // So the list is hidden underneath

        Rectangle {
            id: moveUpButton

            anchors.fill: parent
            anchors.topMargin: Math.floor(parent.height / 4)
            anchors.bottomMargin: Math.floor(parent.height / 2)

            color: activePalette.button
            border.color: activePalette.dark
            radius: generalMargin

            Text { // This should be an arrow instead of text
                id: moveUpText
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.family: "FontAwesome"
                text: "\uf077" //fa-chevron-up
                color: activePalette.dark
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    moveUpButton.color = activePalette.highlight
                    moveUpText.color = activePalette.highlightedText
                }
                onExited: {
                    moveUpButton.color = activePalette.button
                    moveUpText.color = activePalette.dark
                }
                onClicked: {
                    welcomeListView.decrementCurrentIndex()
                }
            }
        }
    }

    ListView {
        id: welcomeListView
        spacing: generalSpacing
        anchors.left: aboutRectangle.anchors.left
        anchors.leftMargin: aboutRectangle.anchors.leftMargin
        anchors.top: moveUpConcealer.bottom
        anchors.bottom: moveDownConcealer.top
        width: aboutRectangle.width

        focus: true

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
                    welcomeListView.currentIndex = index
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

        onCurrentItemChanged:
            {
            selectedModule = welcomeScreenModel.get(currentIndex).module
            selectedLayout = welcomeScreenModel.get(currentIndex).layout

            descriptionRectangleText.text = welcomeScreenModel.get(currentIndex).description
            descriptionRectangleImage.source = welcomeScreenModel.get(currentIndex).imageSource
            openButton.visible = true
            }
        currentIndex: -1
    }

    Rectangle {
        id: moveDownConcealer
        anchors.left: aboutRectangle.anchors.left
        anchors.leftMargin: aboutRectangle.anchors.leftMargin
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0 // All the way down to hide the list
        width: aboutRectangle.width + 2
        height: Math.floor(elementHeight / 4) + 3 * generalSpacing

        // We need to put the rounded button in a concealer rectangle
        // otherwise we can see the list beneath it at the corners
        color: activePalette.base
        gradient: Gradient { // Fake blur with a transparency gradient
                   GradientStop { position: 0.0; color: '#00000000' }
                   GradientStop { position: 0.25; color: activePalette.base}
          }
        z: 1 // So the list is hidden underneath

        Rectangle {
            id: moveDownButton

            anchors.fill: parent
            anchors.topMargin: Math.floor(parent.height / 2)
            anchors.bottomMargin: Math.floor(parent.height / 4)

            color: activePalette.button
            border.color: activePalette.dark
            radius: generalMargin

            Text { // This should be an arrow instead of text
                id: moveDownText
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                font.family: "FontAwesome"
                text: "\uf078" //fa-chevron-down
                color: activePalette.dark
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    moveDownButton.color = activePalette.highlight
                    moveDownText.color = activePalette.highlightedText
                }
                onExited: {
                    moveDownButton.color = activePalette.button
                    moveDownText.color = activePalette.dark
                }
                onClicked: {
                    welcomeListView.incrementCurrentIndex()
                }
            }

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
            color: activePalette.text
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
