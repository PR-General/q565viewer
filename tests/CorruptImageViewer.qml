import QtCore
import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
Window {
    id:testWindow
    width: 640
    height: 480
    visible: true
    property int currentIndex:0
    Text{
        id: label
        anchors{
            top:parent.top
            horizontalCenter: parent.horizontalCenter
        }
        font.pixelSize:32
        text:"Corrupt Images found [" + CorruptImages.length + "]"
    }
    Text{
        id: decodedImagePath
        anchors{
            top:label.bottom
            topMargin:4
            horizontalCenter: parent.horizontalCenter
        }
        width:parent.width*.8
        wrapMode: Text.WrapAnywhere
        font.pixelSize:20
    }
    Rectangle{
        radius:8
        border.width: 2
        anchors.fill: originalImage
        anchors.margins: -8
    }
    Image{
        id:originalImage
        height:parent.width/3
        fillMode: Image.PreserveAspectFit
        anchors{
            left:parent.left
            leftMargin: 30
            verticalCenter: parent.verticalCenter
            verticalCenterOffset: 20
        }
    }
    Rectangle{
        radius:8
        border.width: 2
        anchors.fill: corruptImage
        anchors.margins: -8
    }
    Image{
        id:corruptImage
        height:parent.width/3
        fillMode: Image.PreserveAspectFit
        anchors{
            right:parent.right
            rightMargin:30
            verticalCenter: parent.verticalCenter
            verticalCenterOffset: 20
        }
    }
    Rectangle{
        id:previousButton
        width:128
        height: 48
        radius:8
        color:"lightblue"
        anchors{
            bottom:parent.bottom
            bottomMargin:20
            left:parent.left
            leftMargin: parent.width/4 - 64
        }
        Text{
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 22
            text:"Previous"
        }
        MouseArea{
            anchors.fill: parent
            onClicked:{
                if(testWindow.currentIndex > 0) {
                    testWindow.currentIndex--;
                    testWindow.showNextImage();
                }
                previousButton.visible = (testWindow.currentIndex > 0);
                nextButton.visible = (CorruptImages.length-1 !== testWindow.currentIndex);

            }
        }
    }
    Rectangle{
        id: nextButton
        width:128
        height: 48
        radius:8
        color:"lightblue"
        anchors{
            bottom:parent.bottom
            bottomMargin:20
            right:parent.right
            rightMargin: parent.width/4 - 64
        }
        Text{
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 22
            text:"Next"
        }
        MouseArea{
            anchors.fill: parent
            onClicked:{
                if(CorruptImages.length > testWindow.currentIndex) {
                    testWindow.currentIndex++;
                    testWindow.showNextImage();
                }
                previousButton.visible = (testWindow.currentIndex > 0);
                nextButton.visible = (CorruptImages.length-1 != testWindow.currentIndex);
            }
        }
    }

    function showNextImage() {
        if(CorruptImages.length > testWindow.currentIndex) {
            originalImage.source = OriginalImages[testWindow.currentIndex];
            decodedImagePath.text = CorruptImages[testWindow.currentIndex];
            corruptImage.source = decodedImagePath.text;
        }
    }

    Component.onCompleted: {
        previousButton.visible = (testWindow.currentIndex > 0);
        nextButton.visible = (CorruptImages.length-1 !== testWindow.currentIndex);
        showNextImage();
    }
}
