import QtQuick 2.2
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.0

Rectangle {
    property alias loadbtn: loadbtn
    color: "#FFFFFF"
    Button {
        id: loadbtn
        visible: !recipewidget.operatorMode
        anchors {
            top: parent.top
            topMargin: 10
        }
        anchors.horizontalCenter: parent.horizontalCenter
        text: "load Recipe"
        font.family: "Microsoft YaHei"
        font.pixelSize: 14
        height: 35
        width: parent.width * 0.4
        background: Rectangle {
            radius: 10
            color: loadbtn.pressed ?  "#adceda":"#d5e6ed"
            border.color: "#2f3640"
            border.width: 2
        }
        onClicked: {
            fileDialog.visible = true
        }
    }

    Rectangle {
        anchors {
            top: loadbtn.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            topMargin: 5
            leftMargin: 5
            rightMargin: 5
            bottomMargin:0
        }
        color: "#FFFFFF"
        id: optmode
        RowLayout {
            anchors {
                left: parent.left
                right: parent.right
                top: loadbtn.bottom
                leftMargin: 15
            }
            id: ly1
            Text {
                text: "Execution Mode:"
                font.family: "Microsoft YaHei"
                font.pixelSize: 12
            }
            RadioButton {
                id: keyboardControl
                scale: 0.9
                checked: true
                text: "Normal"
                font.family: "Microsoft YaHei"
                font.pixelSize: 12
                onClicked: recipewidget.mode = true
            }

            RadioButton {
                id: xboxControl
                scale: 0.9
                text: "Unattended"
                font.family: "Microsoft YaHei"
                font.pixelSize: 12
                onClicked: recipewidget.mode = false
            }
        }
        Rectangle {
            anchors {
                top: ly1.bottom
                bottom: parent.bottom
                left: parent.left
                right: parent.right
                topMargin: 5
                leftMargin: 15
            }
            color: "#FFFFFF"
            RowLayout {
                id: grid
                anchors {
                    left: parent.left
                    right: parent.right
                    top: ly1.bottom
                }
                Label {
                    text: "Recipe:"
                    font.family: "Microsoft YaHei"
                    font.pixelSize: 12
                    font.bold: true
                }
                
                Text {
                    id: recipe
                    text: ""
                    font.family: "Microsoft YaHei"
                    font.pixelSize: 12
                    width: 60
                    wrapMode: Text.Wrap
                }
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        folder: shortcuts.home
        property string path
        onAccepted: {
            recipewidget.file = recipewidget.urlToNativeFilePath(
                        fileDialog.fileUrls[0])
            rootdisp.startLoadingModules()
            path = (fileDialog.fileUrls[0])
            recipe.text = (fileDialog.fileUrls[0])
        }
        onRejected: {

        }
    }
}
