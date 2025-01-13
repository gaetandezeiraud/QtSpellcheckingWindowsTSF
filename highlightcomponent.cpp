#include "highlightcomponent.h"

#include <QGuiApplication>
#include <QQuickTextDocument>
#include <QSyntaxHighlighter>
#include <QTextLayout>

#include "spellchecker.h"

HighlightComponent::HighlightComponent(QObject* parent)
    : inherited(parent)
    , _highlight(nullptr)
{

}

void HighlightComponent::onCompleted()
{
    auto property = parent()->property("textDocument");
    auto textDocument = property.value<QQuickTextDocument*>();
    auto document = textDocument->textDocument();

    // Init SpellChecker
    if (document)
        _highlight = std::make_unique<SpellChecker>(document);
}

QStringList HighlightComponent::fetchSuggestions(const QString& word)
{
    if (_highlight)
        return _highlight->spellingSuggestions(word);
    return QStringList();
}

void HighlightComponent::registerQmlType()
{
    qmlRegisterType<HighlightComponent>("HighlightComponent", 1, 0, "HighlightComponent");
}
