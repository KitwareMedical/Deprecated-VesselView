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

// This item describes a button. The button has the following structure:
//  ________________________________________________________________
// |         ______________________________________                 |
// |        |                                      |     ratio      |
// |        |        Button With some text         | <------------> |
// |        |______________________________________|                |
// |                                                                |
// |  Concealer                                                     |
// |________________________________________________________________|
//
// The concealer has by default the same size as the button. The concealer
// is used when things have be hidden under the button. An example would be
// when a list has to go under a rounded button. We do not want to see the
// underlying list a the corner of the button.
// The ratio is an example of how the properties button*MarginToConcealerRatio
// can be used to move the button inside the concealer.
// Finally, the button signals when it is clicked with the buttonClicked()
// signal. It can also be highlighted on hover.


Rectangle {
    id: concealer
    SystemPalette { id: activePalette; colorGroup: SystemPalette.Active }

    // Concealer aliases
    property alias concealerAnchors: concealer.anchors
    property alias concealerGradient: concealer.gradient
    property alias concealerHeight: concealer.height
    property alias concealerWidth: concealer.width
    property alias concealerZ: concealer.z
    property int concealerRadius: radius

    // Button properties/aliases
    property double bottomMarginRatio: 0.0
    property double leftMarginRatio: 0.0
    property double rightMarginRatio: 0.0
    property double topMarginRatio: 0.0

    property alias font: buttonText.font
    property alias radius: button.radius
    property alias text: buttonText.text
    property color textColor: activePalette.text

    // MouseArea signals/aliases
    signal buttonClicked()
    property alias hoverEnabled: buttonMouseArea.hoverEnabled

    Rectangle {
        id: button

        color: activePalette.button
        anchors.fill: parent
        anchors.bottomMargin: Math.floor( bottomMarginRatio * concealer.height )
        anchors.leftMargin: Math.floor( leftMarginRatio * concealer.width )
        anchors.rightMargin: Math.floor( rightMarginRatio * concealer.width )
        anchors.topMargin: Math.floor( topMarginRatio * concealer.height )
        border.color: activePalette.dark

        Text {
            id: buttonText
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: 'Welcome button'
            color: textColor
        }
        MouseArea {
            id: buttonMouseArea
            anchors.fill: parent
            onEntered: {
                button.color = activePalette.highlight
                buttonText.color = activePalette.highlightedText
            }
            onExited: {
                button.color = concealer.color
                buttonText.color = concealer.textColor
            }
            onClicked: concealer.buttonClicked()
        }
    }

    color: activePalette.base
    radius: concealerRadius

    // Check methods
    function checkRatios(r1, r2)
    {
        if (r1 < 0.0 || r1 >= 1.0)
        {
            console.error("Ration must be in range [0.0; 1.0[")
        }
        if (r1 + r2 >= 1.0)
        {
            console.error("Ratios cannot add to 1.0, the inside button would not exist!")
        }
    }

    onBottomMarginRatioChanged: checkRatios(bottomMarginRatio, topMarginRatio)
    onLeftMarginRatioChanged: checkRatios(leftMarginRatio, rightMarginRatio)
    onRightMarginRatioChanged: checkRatios(rightMarginRatio, leftMarginRatio)
    onTopMarginRatioChanged: checkRatios(topMarginRatio, bottomMarginRatio)

}
