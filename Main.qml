import QtQuick 2.15
import QtQuick.Controls 2.15
import HighlightComponent 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("SpellChecker Window")

    ScrollView {
        anchors.fill: parent

        TextArea {
            focus: true
            wrapMode: TextEdit.Wrap
            textFormat: TextEdit.RichText
            font.pixelSize: 16

            placeholderText: qsTr("Enter text")
            text: highlighthandler.text

            HighlightComponent {
                id: highlighthandler
                Component.onCompleted: onCompleted()
            }
        }
    }
}
