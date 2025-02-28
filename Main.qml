import QtCore
import QtQuick
import QtQuick.Dialogs
import com.q565viewer.components
Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")
    Text{
        id:inputLabel
        visible: inputFile.status == Image.Ready
        anchors{
            horizontalCenter:inputFile.horizontalCenter
            bottom:inputFile.top
            bottomMargin:6
        }
        font.pixelSize: 32
        text: "Input File"
        horizontalAlignment: Text.AlignHCenter
    }

    Image{
        id:inputFile
        fillMode: Image.PreserveAspectFit
        anchors{
            centerIn:parent
            horizontalCenterOffset:  -170
        }
        height:300
        width: 300
    }

    Text{
        id:inputType
        visible: inputFile.status == Image.Ready
        anchors{
            horizontalCenter:inputFile.horizontalCenter
            top:inputFile.bottom
            topMargin:6
        }
        font.pixelSize: 32
        text: "Input format:"
        horizontalAlignment: Text.AlignHCenter
    }
    Text{
        id:outputLabel
        visible: outputFile.status == Image.Ready
        anchors{
            horizontalCenter:outputFile.horizontalCenter
            bottom:outputFile.top
            bottomMargin:6
        }
        font.pixelSize: 32
        text: "Output File"
        horizontalAlignment: Text.AlignHCenter
    }
    Image{
        id:outputFile
        fillMode: Image.PreserveAspectFit
        height:300
        width: 300
        anchors{
            centerIn:parent
            horizontalCenterOffset:170
        }
    }

    Text{
        id:outputType
        visible: outputFile.status == Image.Ready
        anchors{
            horizontalCenter:outputFile.horizontalCenter
            top:outputFile.bottom
            topMargin:6
        }
        font.pixelSize: 32
        text: "Output format:"
        horizontalAlignment: Text.AlignHCenter
    }
    Rectangle{
        border.width: 2
        radius:4
        color:"lightblue"
        anchors{
            bottom:parent.bottom
            bottomMargin:16
            horizontalCenter: parent.horizontalCenter
        }
        width: 220
        height: 56
        Text{
            anchors.fill: parent
            anchors.margins: 4
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 28
            text:"Convert File"
        }
        MouseArea{
            anchors.fill: parent
            onClicked:{
                convertFileSelect.open()
            }
        }
    }
    FileDialog{
        id:convertFileSelect
        currentFolder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]
        onAccepted: {
            var outputPath;
            if(selectedFile.toString().endsWith(".q565")){
                console.log("Loading q565 file " + selectedFile)
                inputFile.source = selectedFile;
                outputPath = transcoder.decodeQRGB565(selectedFile);
                console.log("Output file: " + outputPath);
                outputFile.source = outputPath;
                inputType.text = "Input format: Q565"
                outputType.text = "Output format: PNG"
            } else {
                console.log("Loading other file " + selectedFile)
                inputFile.source = selectedFile;
                outputPath = transcoder.encodeQARGB32(selectedFile);
                console.log("Output file: " + outputPath);
                outputFile.source = outputPath;
                inputType.text = "Input format: PNG"
                outputType.text = "Output format: Q565"
            }
        }
    }
    Q565Transcoder {
        id:transcoder
    }
}
