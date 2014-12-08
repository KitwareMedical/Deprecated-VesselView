import QtQuick 1.0

Rectangle  {

    id: welcomeRectangle
    width: 500
    height: 300
    SystemPalette { id: activePalette; colorGroup: SystemPalette.Active }
    FontLoader { source: ":/Gutenberg.ttf" }
    color: activePalette.base

    property string selectedModule: ""
    property variant selectedFiles: []
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
            fileTypes: "SpatialObjectFile"
        }
        ListElement {
            name: "Workflow"
            module: "Workflow"
            imageSource: "Workflow.svg"
            description:  "Workflow that guides you on all the necessary steps required to show the vessels of an image."
            layout: -1
            fileTypes: "Image"
        }
        ListElement {
            name: "Tortuosity"
            module: "Tortuosity"
            imageSource: "VesselViewLogo.svg"
            description:  "Compute tortuosity metrics on vessels."
            layout: 4 // SlicerLayoutOneUp3DView
            fileTypes: "SpatialObjectFile"
        }
        ListElement {
            name: "Spatial Objects Display"
            module: "SpatialObjects"
            imageSource: "VesselViewLogo.svg"
            description: "Visualize the vessels and all their related metrics."
            layout: 1 // SlicerLayoutDefaultView
            fileTypes: "SpatialObjectFile"
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
            fileTypes: "SpatialObjectFile"
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
            fileTypes: "Image"
        }
    }

    function elementHeightFunction(height, numberOfElements, spacing)
    {
        var numberOfFullElements = Math.floor(numberOfElements)
        var heightWithoutSpaces = height - (numberOfFullElements-1) * spacing
        return Math.floor(heightWithoutSpaces / numberOfElements)
    }
    property int elementHeight : elementHeightFunction(
        welcomeRectangle.height - 2*generalMargin,
        4.5 + 1, //+1 for the about rectangle
        generalSpacing
    )

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

    VerticalListViewWithButtons {
        id: welcomeListView

        buttonRadius: generalMargin
        buttonHeight: Math.floor(elementHeight / 4) + 3 * generalSpacing

        spacing: generalSpacing

        anchors.left: aboutRectangle.anchors.left
        anchors.leftMargin: aboutRectangle.anchors.leftMargin
        anchors.top: aboutRectangle.bottom
        anchors.bottom: parent.bottom
        width: aboutRectangle.width

        model: welcomeScreenModel
        delegate: Rectangle {

            id: elementItem
            height: elementHeight
            anchors.left: parent.left
            anchors.leftMargin: generalMargin
            anchors.right: parent.right
            anchors.rightMargin: generalMargin
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

            recentlyLoadedFilesModel.fileTypes = welcomeScreenModel.get(currentIndex).fileTypes
            selectedFiles = []
            }
    }

    Rectangle {
        id: descriptionRectangle
        color: activePalette.base
        anchors.rightMargin: generalMargin
        anchors.right: parent.right
        anchors.left: welcomeListView.right
        anchors.leftMargin: generalMargin
        anchors.top: parent.top
        anchors.topMargin: 0
        height: descriptionRectangleText.height + descriptionRectangleImage.height + 3* generalMargin

        z: 1 // so it's above the recentFilesView

        Image {
            id: descriptionRectangleImage
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 0
            height: Math.floor( welcomeListView.height / 5)
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

            wrapMode: Text.WordWrap
            font.pointSize: 20
            color: activePalette.text
            verticalAlignment: Text.AlignTop
            horizontalAlignment: Text.AlignHCenter

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
    }

    Text {
        id: recentFilesTextBox
        anchors.top: descriptionRectangle.bottom
        anchors.topMargin: 4*generalMargin
        height: recentFilesTextBox.paintedHeight
        anchors.rightMargin: generalMargin
        anchors.right: welcomeRectangle.right
        anchors.left: welcomeListView.right
        anchors.leftMargin: generalMargin

        text: "Recent files:"
        color: activePalette.text
        font.pixelSize: 22
    }

    Binding {
         target: recentFilesTextBox
         property: "visible"
         value: recentlyLoadedFilesModel.hasAtLeastOneEntry()
     }

    ListView {
        id: recentFilesView
        spacing: generalSpacing
        anchors.top: recentFilesTextBox.bottom
        anchors.topMargin: generalMargin
        anchors.bottom: openButton.top
        anchors.bottomMargin: generalMargin
        anchors.rightMargin: 4*generalMargin
        anchors.right: welcomeRectangle.right
        anchors.left: welcomeListView.right
        anchors.leftMargin: 4*generalMargin

        model: recentlyLoadedFilesModel

        delegate: Rectangle{
            id: recentFilesDelegateItem
            anchors.rightMargin: generalMargin
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.leftMargin: generalMargin
            height: 50

            color: activePalette.base
            border.color: activePalette.dark
            radius: generalMargin

            Text {
                id: recentFilesDelegateItemTextIcon
                anchors.left: parent.left
                anchors.leftMargin: generalMargin
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                text: "\uf15b" // fa-file
                color: activePalette.text
                font.pixelSize: 22
                font.family: "FontAwesome"
                verticalAlignment: Text.AlignVCenter
            }
            Text {
                id: recentFilesDelegateItemText
                anchors.left: recentFilesDelegateItemTextIcon.right
                anchors.leftMargin: generalMargin
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                text: filename
                color: activePalette.text
                font.pixelSize: 22
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideLeft
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    recentFilesView.currentIndex = index

                    var selected = (recentFilesDelegateItem.color == activePalette.base)
                    var fileToLoad = recentlyLoadedFilesModel.filename(recentFilesView.currentIndex)

                    if (selected)
                    {
                        recentFilesDelegateItem.color = activePalette.highlight
                        recentFilesDelegateItemTextIcon.color = activePalette.highlightedText
                        recentFilesDelegateItemText.color = activePalette.highlightedText

                        welcomeScreen.addUniqueFileToOpenOnLoad(fileToLoad)
                    }
                    else
                    {
                        recentFilesDelegateItem.color = activePalette.base
                        recentFilesDelegateItemTextIcon.color = activePalette.text
                        recentFilesDelegateItemText.color = activePalette.text

                        welcomeScreen.removeOneFileToOpenOnLoad(fileToLoad)
                    }
                }
                onDoubleClicked: {
                    recentFilesView.currentIndex = index
                    var fileToLoad = recentlyLoadedFilesModel.filename(recentFilesView.currentIndex)
                    welcomeScreen.addUniqueFileToOpenOnLoad(fileToLoad)
                    welcomeScreen.loadModule(selectedModule, selectedLayout)
                }
            }
        }

        currentIndex: -1
    }

    Button {
        id: openButton
        visible: false

        anchors.right: welcomeRectangle.right
        anchors.rightMargin: 0
        anchors.left: welcomeListView.right
        anchors.leftMargin: 0
        height: Math.floor(parent.height / 10)
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0

        bottomMarginRatio: 0.2
        leftMarginRatio: 0.3
        rightMarginRatio: 0.3
        topMarginRatio: 0.2

        text: "Open in VesselView"
        font.pointSize: 20
        radius: generalMargin
        hoverEnabled: true

        onButtonClicked: {
            welcomeScreen.loadModule(selectedModule, selectedLayout)
        }

    }

}
