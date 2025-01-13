#ifndef HIGHLIGHTCOMPONENT_H
#define HIGHLIGHTCOMPONENT_H

#include <QObject>
#include <QString>
#include <memory.h>

class SpellChecker;
class HighlightComponent : public QObject
{
    Q_OBJECT
    //@formatter:off
    Q_PROPERTY(QString text
                    READ getText
                    WRITE setText
                    NOTIFY textChanged)
    //@formatter:on
    using inherited = QObject;
public:
    explicit HighlightComponent(QObject* parent = nullptr);

    static void registerQmlType();

    const QString& getText()
    {
        return _text;
    }

    void setText(const QString& newText)
    {
        if (newText != _text)
        {
            _text = newText;
            emit textChanged();
        }
    }

    Q_INVOKABLE void onCompleted();
    Q_INVOKABLE QStringList fetchSuggestions(const QString& word);

signals:
    void textChanged();

private:
    std::unique_ptr<SpellChecker> _highlight;
    QString _text = "";
};

#endif // HIGHLIGHTCOMPONENT_H
