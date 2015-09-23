import QtQuick 1.0

// This widget condenses a vertical list view with two buttons on
// top and bottom for the navigation.
// It looks like this:
// ________________________________________
//| --------------------------------------||
//||                  Up                  ||
//||______________________________________||
//|                                        |
//|                   .                    |
//|                   .                    |
//|                   .                    |
//|              List elements ...         |
//|                   .                    |
//|                   .                    |
//|                   .                    |
//|                                        |
//| --------------------------------------||
//||                  Down                ||
//||______________________________________||
//|________________________________________

Item
{
    id: verticalListWithButtons

    // Item properties
    property int buttonHeight: 20
    property int buttonRadius: 0
    property bool buttonVisible: true

    // List view aliases
    property alias delegate: verticalList.delegate
    property alias model: verticalList.model
    property alias spacing: verticalList.spacing
    property alias currentIndex: verticalList.currentIndex
    signal currentItemChanged()

    // Button aliases
    property alias buttonDown: verticalListMoveDown
    property alias buttonUp: verticalListMoveUp

    Button {
        id: verticalListMoveUp

        concealerAnchors.left: verticalListWithButtons.left
        concealerAnchors.leftMargin: verticalListWithButtons.anchors.leftMargin
        concealerAnchors.right: verticalListWithButtons.right
        concealerAnchors.rightMargin: verticalListWithButtons.anchors.rightMargin

        concealerAnchors.top: verticalListWithButtons.top
        concealerAnchors.topMargin: 0
        concealerHeight: buttonHeight
        concealerRadius: 0

        bottomMarginRatio: 0.5
        topMarginRatio: 0.25

        concealerGradient: Gradient { // Fake blur with a transparency gradient
                   GradientStop { position: 0.75; color: activePalette.base }
                   GradientStop { position: 1.0; color: '#00000000' }
          }
        concealerZ: 1 // So the list is hidden underneath

        radius: buttonRadius
        font.family: "FontAwesome"
        text: "\uf077" //fa-chevron-up
        textColor: activePalette.dark

        hoverEnabled: true
        onButtonClicked: {
            if (verticalList.currentIndex > 0)
            {
                verticalList.decrementCurrentIndex()
            }
            else
            {
                verticalList.currentIndex = verticalList.count - 1
            }
        }
    }

    ListView {
        id: verticalList
        anchors.bottom: verticalListMoveDown.top
        anchors.bottomMargin: 0
        anchors.left: verticalListWithButtons.left
        anchors.leftMargin: verticalListWithButtons.anchors.leftMargin
        anchors.right: verticalListWithButtons.right
        anchors.rightMargin: verticalListWithButtons.anchors.rightMargin
        anchors.top: verticalListMoveUp.bottom
        anchors.topMargin: 0
        clip:true
        snapMode: ListView.SnapToItem
        boundsBehavior: Flickable.StopAtBounds
        onCurrentItemChanged: verticalListWithButtons.currentItemChanged()
        currentIndex: 0
    }

    Button {
        id: verticalListMoveDown
        visible: buttonVisible
        concealerAnchors.left: verticalListWithButtons.left
        concealerAnchors.leftMargin: verticalListWithButtons.anchors.leftMargin
        concealerAnchors.right: verticalListWithButtons.right
        concealerAnchors.rightMargin: verticalListWithButtons.anchors.rightMargin

        concealerAnchors.bottom: verticalListWithButtons.bottom
        concealerAnchors.bottomMargin: 0
        concealerHeight: buttonHeight
        concealerRadius: 0

        bottomMarginRatio: 0.25
        topMarginRatio: 0.5

        concealerGradient: Gradient { // Fake blur with a transparency gradient
                   GradientStop { position: 0.0; color: '#00000000' }
                   GradientStop { position: 0.25; color: activePalette.base }
          }
        concealerZ: 1 // So the list is hidden underneath

        radius: buttonRadius
        font.family: "FontAwesome"
        text: "\uf078" //fa-chevron-down
        textColor: activePalette.dark

        hoverEnabled: true
        onButtonClicked: {
            if (verticalList.currentIndex < verticalList.count - 1)
            {
                verticalList.incrementCurrentIndex()
            }
            else
            {
                verticalList.currentIndex = 0
            }
        }
    }
}
