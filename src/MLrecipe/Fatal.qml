import QtQuick 2.3
import QtQuick.Dialogs 1.3

Dialog {
    property alias text: dialogMsg.text
    property bool xButton: true
    signal xButtonClicked

    id: errorDialog
    title: "Fatal Error Status"
    standardButtons: StandardButton.Ok
    Text {
        id: dialogMsg
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: 20
        color: "red"
    }
    onRejected: {
        _recipeDataModules.setProperty(recipeControl.currentIndex,
                                       "result", "fail")
        _recipeDataModules.setProperty(recipeControl.currentIndex, "error",
                                       dialogMsg.text)
        recipeControl.currentIndex = modules.recipeListView.count - 1
        recipeControl.state = "pause"
        xButton = false
        this.close()
        gotoNextModule()
    }

    modality: Qt.ApplicationModal
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
