/*=========================================================================

Library:   VesselView

Copyright 2010 Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=========================================================================*/

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

