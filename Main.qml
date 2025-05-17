import QtCore
import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import com.q565viewer.components
import com.pr.rust.q565

Window {
    width: 640
    height: 480
    visible: true
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
        visible: inputLabel.visible
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
        id:inputSize
        visible: inputLabel.visible
        anchors{
            horizontalCenter:inputType.horizontalCenter
            top:inputType.bottom
            topMargin:6
        }
        font.pixelSize: 18
        text: "Size [bytes]: "
        horizontalAlignment: Text.AlignHCenter
    }




    Text{
        id:outputLabel
        visible: outputFile.status == Image.Ready
        anchors{
            horizontalCenter:resultsBox.horizontalCenter
            top:parent.top
            topMargin:8
        }
        font.pixelSize: 32
        text: "Output File"
        horizontalAlignment: Text.AlignHCenter
    }

    Rectangle{
        id: imageButton
        border.width: 2
        radius:4
        color: !outputFile.visible ? "lightblue": "grey"
        enabled: !outputFile.visible
        anchors{
            top:outputLabel.bottom
            topMargin:10
            right: outputLabel.horizontalCenter
            rightMargin:8
        }
        width: 150
        height: 32
        Text{
            anchors.fill: parent
            anchors.margins: 4
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 20
            text:"Image"
        }
        MouseArea{
            anchors.fill: parent
            onClicked:{
                resultsView.visible = false;
                outputFile.visible = true;
            }
        }
    }
    Rectangle{
        id: textButton
        border.width: 2
        radius:4
        color: outputFile.visible ? "lightblue": "grey"
        enabled: outputFile.visible
        anchors{
            top:outputLabel.bottom
            topMargin:10
            left: outputLabel.horizontalCenter
            leftMargin:8
        }
        width: 150
        height: 32
        Text{
            anchors.fill: parent
            anchors.margins: 4
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 20
            text:"Text"
        }
        MouseArea{
            anchors.fill: parent
            onClicked:{
                outputFile.visible = false;
                resultsView.visible = true;
            }
        }
    }
    Rectangle{
        id: resultsBox
        border.width: 2
        radius: 4
        anchors{
            right:parent.right
            top:textButton.bottom
            bottom:convertButton.top
            left:parent.horizontalCenter
            margins:20
            topMargin:4

        }
        Image{
            id:outputFile
            fillMode: Image.PreserveAspectFit
            anchors.fill: parent
            anchors.margins: 8
            anchors.bottomMargin:65
            visible:outputFile.status == Image.Ready
        }
        Text{
            id:outputType
            visible: outputFile.visible
            anchors{
                horizontalCenter:outputFile.horizontalCenter
                top:outputFile.bottom
                topMargin:2
            }
            font.pixelSize: 20
            text: "Output format:"
            horizontalAlignment: Text.AlignHCenter
        }

        Text{
            id:outputSize
            visible: outputFile.visible
            anchors{
                horizontalCenter:outputType.horizontalCenter
                top:outputType.bottom
                topMargin:2
            }
            font.pixelSize: 16
            text: "Size [bytes]: "
            horizontalAlignment: Text.AlignHCenter
        }
        ScrollView{
            id:resultsView
            visible:false
            anchors.fill: parent
            anchors.margins: 2
            Text{
                id: outputText
            }
        }
    }
    Rectangle{
        id: convertButton
        border.width: 2
        radius:4
        color:"lightblue"
        anchors{
            bottom:parent.bottom
            bottomMargin:10
            horizontalCenter: parent.horizontalCenter
        }
        width: 220
        height: 42
        Text{
            anchors.fill: parent
            anchors.margins: 4
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: 24
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
                outputText.text = transcoder.textData
                inputType.text = "Input format: Q565"
                outputType.text = "Output format: PNG"
                inputSize.text = "Size [bytes]: " + transcoder.sizeIn
                outputSize.text = "Size [bytes]: " + transcoder.sizeOut
            } else {
                console.log("Loading other file " + selectedFile)
                inputFile.source = selectedFile;
                let img = transcoder.loadFile(selectedFile)
                rustQ565.loadImage(selectedFile, img);
                outputPath = transcoder.encodeQARGB32(selectedFile);
                console.log("Output file: " + outputPath);
                outputFile.source = outputPath;
                inputType.text = "Input format: PNG"
                outputType.text = "Output format: Q565"
                inputSize.text = "Size [bytes]: " + transcoder.sizeIn
                outputSize.text = "Size [bytes]: " + transcoder.sizeOut
            }
        }
    }
    Q565Transcoder {
        id:transcoder
    }
    QRustQ565 {
        id: rustQ565
    }

}
