import QtQuick
import QtQuick.Controls
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
                property int start: 0
                property int end: 0

                Instantiator {
                    model: suggestions

                    MenuItem {
                        text: model.newWord
                        onTriggered: () => {
                            if (textArea.selectedText.length > 0)
                            {
                                menu.start = textArea.selectionStart
                                menu.end = textArea.selectionEnd
                            }
                            textArea.remove(menu.start, menu.end)
                            textArea.insert(menu.start, model.newWord)
                            menu.close()
                        }
                    }

                    onObjectAdded: function(index, object) {
                        menu.insertItem(index, object)
                    }
                    onObjectRemoved: function(index, object) {
                        menu.removeItem(object)
                    }
                }

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
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.RightButton

                function isWordBoundary(charBoundary) {
                    return charBoundary === " " || charBoundary === "\t" || charBoundary === "\n" || /[.,!?;:'"(){}[\]]/.test(charBoundary);
                }

                onClicked: (mouse) => {
                    menu.start = 0
                    menu.end = 0

                    // Fetch spellchecker suggestion(s)
                    if (textArea.selectedText.length > 0)
                    {
                        suggestions.clear()
                        var tempSuggestions = highlighthandler.fetchSuggestions(textArea.selectedText);
                        tempSuggestions.forEach(i => suggestions.append({newWord: i}))
                    }
                    // Very naive implementation, don't use it on a very long text
                    // Allow a simple right click on a word, without selection
                    else
                    {
                        var pos = textArea.positionAt(mouse.x, mouse.y)
                        var fullText = textArea.text;

                        // Find start of the word
                        let start = pos;
                        while (start > 0 && !isWordBoundary(fullText[start - 1])) {
                            start--;
                        }

                        // Find end of the word
                        let end = pos;
                        while (end < fullText.length && !isWordBoundary(fullText[end])) {
                            end++;
                        }

                        // Extract the word
                        let word = fullText.substring(start, end);
                        menu.start = start
                        menu.end = end

                        suggestions.clear()
                        var tempSuggestions = highlighthandler.fetchSuggestions(word);
                        if (tempSuggestions.length === 0) return // Cancel if no suggestions are available, nothing to show in the menu
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
