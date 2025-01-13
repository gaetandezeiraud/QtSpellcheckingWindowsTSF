#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <windows.h>
#include <spellcheck.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

class SpellChecker : public QObject {
    Q_OBJECT
public:
    explicit SpellChecker(QObject *parent = nullptr);
    ~SpellChecker();

    Q_INVOKABLE QVariantList checkSpelling(const QString &text);

private:
    QString getMisspelledWord(const QString &text, ULONG startIndex, ULONG length);

private:
    ComPtr<ISpellCheckerFactory> _factory;
    ComPtr<ISpellChecker> _spellChecker;
    BOOL _isSupported;
};

#endif // SPELLCHECKER_H
