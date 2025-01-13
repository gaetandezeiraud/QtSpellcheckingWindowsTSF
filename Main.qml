import QtQuick 2.15
import QtQuick.Controls 2.15
import SpellChecker 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("SpellChecker Window")

    SpellChecker {
        id: spellChecker
    }

    TextArea {
        id: textArea
        anchors.fill: parent
        onTextChanged: {
            let errors = spellChecker.checkSpelling(text);
            for (let error of errors) {
                console.log("Error at", error.start, "length", error.length, "Suggestions:", error.suggestions);
            }
        }
    }
}
