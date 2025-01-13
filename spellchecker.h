#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QObject>
#include <QSyntaxHighlighter>
#include <QString>
#include <QTextCharFormat>
#include <windows.h>
#include <spellcheck.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

class SpellChecker : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit SpellChecker(QTextDocument *parent = nullptr);
    ~SpellChecker();

protected:
    void highlightBlock(const QString &text) override;

private:
    void checkSpelling();
    void highlightImmediateWords(const QString &text);
    QString getMisspelledWord(const QString &text, ULONG startIndex, ULONG length);

private:
    ComPtr<ISpellCheckerFactory> _factory;
    ComPtr<ISpellChecker> _spellChecker;
    BOOL _isSupported;

    QString _currentText;
    QTextCharFormat _errorFormat;
};
#endif // SPELLCHECKER_H
