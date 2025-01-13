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
            id: textArea
            focus: true
            wrapMode: TextEdit.Wrap
            font.pixelSize: 16

            placeholderText: qsTr("Enter text")
            text: highlighthandler.text

            Menu {
                id: menu
                ListModel {
                    id: suggestions
                }

                Repeater{
                    model: suggestions
                    MenuItem {
                        text: newWord
                        onTriggered: () => {
                            const start = textArea.selectionStart
                            const end = textArea.selectionEnd
                            textArea.remove(start, end)
                            textArea.insert(textArea.cursorPosition, newWord)
                            menu.close()
                        }
                    }
                }

                /*
                MenuSeparator {}

                MenuItem {
                    text: "Cut"
                    onTriggered: textArea.cut()
                }
                MenuItem {
                    text: "Copy"
                    onTriggered: textArea.copy()
                }
                MenuItem {
                    text: "Paste"
                    onTriggered: textArea.paste()
                }
                */
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.RightButton
                onClicked: (mouse) => {
                    // Fetch spellchecker suggestion(s)
                    if (textArea.selectedText.length > 0)
                    {
                        suggestions.clear()
                        var tempSuggestions = highlighthandler.fetchSuggestions(textArea.selectedText);
                        tempSuggestions.forEach(i => suggestions.append({newWord: i}))
                    }

                    // Open the menu
                    menu.x = mouse.x
                    menu.y = mouse.y
                    menu.open()
                }
            }

            HighlightComponent {
                id: highlighthandler
                Component.onCompleted: onCompleted()
            }
        }
    }
}
