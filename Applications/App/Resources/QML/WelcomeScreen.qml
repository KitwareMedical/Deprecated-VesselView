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

    property string aboutSource: ":/Icons/Medium/KitwareLogo.svg"
    property string aboutText: "
 <html>
 <div text-align=\"left\">
 VesselView is a demonstration application
 from <a href=\"http://www.kitware.com/\">Kitware, Inc.</a> for the 
 segmentation, registration, and analysis of tubes (e.g., blood vessels) in
 3D images (e.g., MRI, CT, and Ultrasound).
 </div>
 <br><br>
 <center>
 <table>
   <tr> 
     <td>
       <img src=\":WelcomeAbout.png\" width=250>
     </td>
     <td>
       <br><br>
       <center>
       For more information, visit <br>
       <a href=\"http://public.kitware.com/Wiki/TubeTK/About\">http://public.kitware.com/Wiki/TubeTK/About</a>
       </center>
     </td>
   </tr>
 </table>
 </center>
 <br><br>
 <div text-align=\"left\">
 VesselView is free for academic and commercial use.  It is based on
 <a href=\"http://www.tubetk.org/\">TubeTK</a> and
 <a href=\"http://www.slicer.org/\">3D Slicer</a>.  It is provided as-is and
 without expressed or implied warranty of suitability for any task.
 </div>
 </html> "

    ListModel {
        id: welcomeScreenModel

        ListElement {
            name: "Vessel Display"
            module: "SpatialObjects"
            imageSource: ":/Icons/Medium/VesselDisplay.svg"
            description: "
 <html>
 <center>
 <em>Visualize images, vessels, and organs in 3D.</em>
 </center>
 <br><br><br>
 <div text-align=\"left\">
 Loads multiple data files and selectively displays them using a variety of
 basic and advanced visualization formats.
 </div>
 </html>"
            layout: -1
            fileTypes: "SpatialObjectFile"
        }
        ListElement {
            name: "Interactive Vessel Segmentation"
            module: "InteractiveSegmentTubes"
            imageSource: ":/Icons/Medium/InteractiveVesselSegmentation.svg"
            description:  "
 <html>
 <center>
 <em>Point-and-click within an image to segment tubes.</em>
 </center>
 <br><br><br>
 <div text-align=\"left\">
 Provides an interactive interface to
 <a href=\"http://www.tubetk.org\">TubeTK</a>'s vessel segmentation algorithms.
 <br><br>
 Pre-defined algorithm parameterizations are given for common tasks, such as:
 <ul>
   <li> Liver vessels in contrast-enhanced CT </li>
   <li> Brain vessels in MRA </li>
   <li> Lung vessels in CT </li>
 </ul>
 </div>
 </html>"
            layout: 3
            fileTypes: "SpatialObjectFile"
        }
        ListElement {
            name: "Automatic Vessel Segmentation"
            module: "Workflow"
            imageSource: ":/Icons/Medium/AutomaticVesselSegmentation.svg"
            description:  "
 <html>
 <center>
 <em>Apply a trained statistical model to initiate vessel segmentations.</em> 
 </center>
 <br><br><br>
 <div text-align=\"left\">
 Provides a workflow-style interface to
 <a href=\"http://www.tubetk.org\">TubeTK</a>'s statistical methods for
 identifying points in an image for seeding vessel extractions.
 <br><br>
 Statistical models for seed selection are given for common conditions, such as:
 <ul>
   <li> Liver vessels in contrast-enhanced CT </li>
   <li> Brain vessels in MRA </li>
   <li> Lung vessels in CT </li>
 </ul>
 </div>
 </html>"
            layout: 3
            fileTypes: "VolumeFile"
        }
        ListElement {
            name: "Compute Vessel Tortuosity"
            module: "Tortuosity"
            imageSource: ":/Icons/Medium/ComputeVesselTortuosity.svg"
            description:  "
 <html>
 <center>
 <em>Compute tortuosity metrics on vessels.</em> 
 </center>
 <br><br><br>
 <div text-align=\"left\">
 Work initiated by Dr. Bullitt has shown that morphological features of vessels
 within and around tumors can be used to assess malignancy and treatment
 efficacy.   Others have associated vessel morphological features with diabetic
 retinopathy, retinopathy of prematurity, and a variety of arterial diseases.
 <br><br>
 VesselView contains a variety of novel tortuosity metrics including:
 <ul>
   <li> Fourier-based methods </li>
   <li> Normalized inflection count </li>
   <li> Mean curvature </li>
 </ul>
 </div>
 </html>"
            layout: 24 // ConventionalQuantitative
            fileTypes: "SpatialObjectFile"
        }
        ListElement {
            name: "Convert Vessel Files"
            module: "ConvertTRE"
            imageSource: ":/Icons/Medium/ConvertVessels.svg"
            description: "
 <html>
 <center>
 <em>Convert between various tube file formats.</em> 
 </center>
 <br><br><br>
 <div text-align=\"left\">
 Convert between ITK's TubeSpatialObject format (provided by MetaIO) and legacy
 formats such as
 <ul>
   <li> Dr. Bullitt's TubeX files </li>
   <li> UNC CADDLab's .tre files (pre-cursor to MetaIO) </li>
 </ul>
 </div>
 </html>"
            layout: 4 // SlicerLayoutDefaultView
            fileTypes: "SpatialObjectFile"
        }
        ListElement {
            name: "Interactive Organ segmentation"
            module: "Editor"
            imageSource: ":/Icons/Medium/InteractiveOrganSegmentation.svg"
            description: "
 <html>
 <center>
 <em>Semi-automated methods for segmenting organs and regions of interest in images.</em> 
 </center>
 <br><br><br>
 <div text-align=\"left\">
 Provides methods from TubeTK and 3D Slicer for creating and editing masks that
 represent organs and/or regions of interest that can be used to constrain 
 interactive and automated vessel segmentation algorithms in VesselView.
 </div>
 </html>"
            layout: 3 
            fileTypes: "VolumeFile"
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
        anchors.leftMargin: 2 * generalMargin
        anchors.top: parent.top
        anchors.topMargin: generalMargin
        anchors.rightMargin: generalMargin
        width: Math.floor((parent.width - 2*generalMargin) / 4)
        height: elementHeight + generalSpacing

        color: activePalette.base
        border.color: activePalette.base
        radius: generalMargin
        z: 1 // So the image isn't hidden by the list view after it moved

        Rectangle {
            id: aboutImageRectangle
            anchors.fill: parent
            anchors.topMargin: welcomeListView.spacing
            anchors.bottomMargin: welcomeListView.spacing
            anchors.leftMargin: generalMargin
            anchors.rightMargin: generalMargin
            color: activePalette.base

            Image {
                id: aboutImage
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                source: ":/Icons/Medium/VesselViewSplashScreen.svg"
            }
            MouseArea {
                anchors.fill: aboutImage
                onClicked: {
                    selectedModule = ""
                    selectedLayout = -1
                    welcomeListView.currentIndex = -1
                    descriptionRectangleText.text = aboutText
                    descriptionRectangleImage.source = aboutSource
                    openButton.visible = false
                    recentlyLoadedFilesModel.fileTypes = ""
                    selectedFiles = []
                }
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
                    when: (module == selectedModule)
                    PropertyChanges {target: elementItem; color: activePalette.dark;}
                }
            ]
        }

        onCurrentItemChanged: {
            if (currentIndex != -1) {
                selectedModule = welcomeScreenModel.get(currentIndex).module
                selectedLayout = welcomeScreenModel.get(currentIndex).layout
    
                descriptionRectangleText.text =
                  welcomeScreenModel.get(currentIndex).description
                descriptionRectangleImage.source =
                  welcomeScreenModel.get(currentIndex).imageSource
                openButton.visible = true
                recentlyLoadedFilesModel.fileTypes =
                  welcomeScreenModel.get(currentIndex).fileTypes
                selectedFiles = []
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
        anchors.top: parent.top
        height: descriptionRectangleText.height +
          descriptionRectangleImage.height

        Image {
            id: descriptionRectangleImage
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.top: parent.top
            anchors.topMargin: Math.floor( welcomeListView.height / 8 )
            height: Math.floor( welcomeListView.height / 6)
            fillMode: Image.PreserveAspectFit
            source: aboutSource
        }

        Text{
            id: descriptionRectangleText
            anchors.right: parent.right
            anchors.rightMargin: Math.floor( parent.width * 0.125 )
            anchors.left: parent.left
            anchors.leftMargin: Math.floor( parent.width * 0.125 )
            anchors.top: descriptionRectangleImage.bottom
            anchors.topMargin: Math.floor( descriptionRectangleImage.height /
              2 )
            wrapMode: Text.WordWrap
            font.pointSize: 16
            color: activePalette.text
            verticalAlignment: Text.AlignTop
            horizontalAlignment: Text.AlignLeft
            text: aboutText
            onLinkActivated: Qt.openUrlExternally(link)
        } 
    }

    Text {
        id: recentFilesTextBox
        anchors.top: descriptionRectangle.bottom
        anchors.topMargin: descriptionRectangle.height + 
          Math.floor( descriptionRectangleImage.height / 2 )
        anchors.right: welcomeRectangle.right
        anchors.rightMargin: Math.floor( parent.width * 0.125 )
        anchors.left: welcomeListView.right
        anchors.leftMargin: Math.floor( parent.width * 0.125 )
        height: recentFilesTextBox.paintedHeight

        text: "Recent files:"
        color: activePalette.text
        font.pixelSize: 14
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
        anchors.right: welcomeRectangle.right
        anchors.rightMargin: Math.floor( parent.width * 0.125 )
        anchors.left: welcomeListView.right
        anchors.leftMargin: Math.floor( parent.width * 0.125 )
        clip:true
        snapMode: ListView.SnapToItem
        boundsBehavior: Flickable.StopAtBounds

        model: recentlyLoadedFilesModel

        delegate: Rectangle{
            id: recentFilesDelegateItem
            anchors.right: parent.right
            anchors.rightMargin: 5 * generalMargin
            anchors.left: parent.left
            anchors.leftMargin: 5 * generalMargin
            height: 20

            color: activePalette.button
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
                font.pixelSize: 14
                font.family: "FontAwesome"
                verticalAlignment: Text.AlignVCenter
            }
            Text {
                id: recentFilesDelegateItemText
                anchors.left: recentFilesDelegateItemTextIcon.right
                anchors.leftMargin: generalMargin
                anchors.right: parent.right
                anchors.rightMargin: generalMargin
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                text: filename
                color: activePalette.text
                font.pixelSize: 14
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideLeft
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    recentFilesView.currentIndex = index

                    var selected =
                      (recentFilesDelegateItem.color == activePalette.button)
                    var fileToLoad =
                      recentlyLoadedFilesModel.filename(
                      recentFilesView.currentIndex)

                    if (selected) {
                        recentFilesDelegateItem.color = activePalette.dark

                        welcomeScreen.addUniqueFileToOpenOnLoad(fileToLoad)
                    }
                    else {
                        recentFilesDelegateItem.color = activePalette.button

                        welcomeScreen.removeOneFileToOpenOnLoad(fileToLoad)
                    }
                }
                onDoubleClicked: {
                    recentFilesView.currentIndex = index
                    recentFilesDelegateItem.color = activePalette.dark
                    var fileToLoad = recentlyLoadedFilesModel.filename(
                      recentFilesView.currentIndex)
                    welcomeScreen.addUniqueFileToOpenOnLoad(fileToLoad)
                    welcomeScreen.loadModule(selectedModule, selectedLayout)
                }
            }
        }

        currentIndex: -1
    }

    Rectangle {
        id: openButton
        visible: false

        anchors.right: welcomeRectangle.right
        anchors.left: welcomeListView.right

        anchors.bottom: parent.bottom
        anchors.bottomMargin: Math.floor(parent.height / 20)
        height: Math.floor(parent.height / 20)
        color: activePalette.base


        Rectangle {
            id: loadButtonButton
            anchors.fill: openButton
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.leftMargin: Math.floor( parent.width / 4 )-generalMargin
            anchors.rightMargin: Math.floor( parent.width / 2 )+generalMargin
            //anchors.left:loadButtonButton.right
            //anchors.leftMargin: generalMargin
            radius: generalMargin
            color: "olivedrab"            
            Text {
                id: loadButtonText
                anchors.fill: loadButtonButton
                text: "Load New Data"
                font.pointSize: 20
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
            MouseArea {
                anchors.fill: loadButtonButton
                onPressed: {
                  parent.color = activePalette.dark
                }
                onExited: {
                  parent.color = "olivedrab"
                }
                onReleased: {
                  parent.color = "olivedrab"
                }
                onClicked: {
                  parent.color = "olivedrab"
                  welcomeScreen.loadNewData()
                  welcomeScreen.loadModule(selectedModule, selectedLayout)
                }
            }
        }

        Rectangle {
            id: openButtonButton
            anchors.fill: openButton
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.leftMargin: Math.floor( parent.width / 2 )
            anchors.rightMargin: Math.floor( parent.width / 4 )
            radius: generalMargin
            color: "olivedrab"
            Text {
                id: openButtonText
                anchors.fill: openButtonButton
                text: "Open"
                font.pointSize: 20
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
            MouseArea {
                anchors.fill: openButtonButton
                onPressed: {
                  parent.color = activePalette.dark
                }
                onExited: {
                  parent.color = "olivedrab"
                }
                onReleased: {
                  parent.color = "olivedrab"
                }
                onClicked: {
                  parent.color = "olivedrab"
                  welcomeScreen.loadModule(selectedModule, selectedLayout)
                }
            }
        }
    }

}
