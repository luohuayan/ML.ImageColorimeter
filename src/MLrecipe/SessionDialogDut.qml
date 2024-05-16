import QtQuick 2.3
import QtQuick.Dialogs 1.3 as Dl
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15

Dl.Dialog {
    property alias text: dialogMsg.text
    property bool xButton: true
    property bool retrieveContext: false
    signal xButtonClicked

    id: sessionDialogDut
    title: "Session Dut"
    standardButtons: Dl.StandardButton.NoButton
    Rectangle {
        id: rect
        implicitWidth: parent.width
        implicitHeight: 100
        color: "#e1e1e2"

        Column {
            anchors.fill: parent
            anchors.topMargin: 20
            spacing: 10
            Text {
                id: dialogMsg
            }
            TextField {
                id: dialogTextField
                placeholderText: qsTr("Enter dut id")
                width: rect.width
                Keys.onReturnPressed: {
                    sessionDialogDut.checkSerialFormat()
                }
                Keys.onEnterPressed: {
                    sessionDialogDut.checkSerialFormat()
                }
            }
        }
    }

    function checkSerialFormat() {
            xButton = false

            if (sessionDialogDut.retrieveContext) {
                recipewidget.saveDUTID(dialogTextField.text)
            }
            sessionDialogDut.close()
    }

    modality: Qt.ApplicationModal

    onRejected: {
        _recipeDataModules.setProperty(recipeControl.currentIndex,
                                       "status", "pause")
        xButton = false
        recipeControl.state = "pause"
    }

    onXButtonClicked: {
        rejected()
    }

    onVisibilityChanged: {
        if (!visible && xButton) {
            xButtonClicked()
        }
        if (visible)
            xButton = true
    }
    onActionChosen: {
        if (action.key === Qt.Key_Escape || action.key === Qt.Key_Enter
                || action.key === Qt.Key_Return)
            action.accepted = false
    }
}
