#include "spellchecker.h"

#include <QRegularExpression>

SpellChecker::SpellChecker(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr))
        qFatal("Failed to initialize COM library");

    hr = CoCreateInstance(__uuidof(SpellCheckerFactory), nullptr, CLSCTX_INPROC_SERVER,
                          IID_PPV_ARGS(&_factory));
    if (FAILED(hr))
        qFatal("Failed to create SpellCheckerFactory");

    hr = _factory->IsSupported(L"en-US", &_isSupported);
    if (FAILED(hr) || !_isSupported)
        qFatal("Spellchecking is not supported for en-US");

    hr = _factory->CreateSpellChecker(L"en-US", &_spellChecker);
    if (FAILED(hr))
        qFatal("Failed to create SpellChecker");

    // Set the error format for highlighting
    _errorFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline); // SpellCheckUnderline doesn't work? Related to https://bugreports.qt.io/browse/QTBUG-46540?jql=project%20%3D%20QTBUG%20AND%20status%20%3D%20Open%20AND%20text%20~%20Underline ?
    _errorFormat.setUnderlineColor(Qt::red); // Color also doesn't work
}

SpellChecker::~SpellChecker()
{
    CoUninitialize();
}

void SpellChecker::highlightBlock(const QString &text)
{
    highlightImmediateWords(text);

    if (text.endsWith(' ')
        || text.endsWith('.')
        || text.endsWith('!')
        || text.endsWith('?'))
    {
        checkSpelling();
    }
    _currentText = text;
}


void SpellChecker::checkSpelling()
{
    QTextDocument *doc = this->document();  // Access the QTextDocument from QML

    ComPtr<IEnumSpellingError> spellingErrors;
    HRESULT hr = _spellChecker->Check(_currentText.toStdWString().c_str(), &spellingErrors);
    if (FAILED(hr)) return;

    ComPtr<ISpellingError> error;
    while (spellingErrors->Next(&error) == S_OK)
    {
        ULONG startIndex, length;
        HRESULT hr = error->get_StartIndex(&startIndex);
        if (FAILED(hr)) continue;

        hr = error->get_Length(&length);
        if (FAILED(hr)) continue;

        // Apply the highlight
        if (startIndex >= 0 && startIndex + length <= _currentText.length())
            setFormat(startIndex, length, _errorFormat);
    }

    setCurrentBlockState(0);  // Clear previous block state if any
}

void SpellChecker::highlightImmediateWords(const QString &text)
{
    // Split the text into words
    QStringList words = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    // Process each word (excluding the last incomplete one if no space is present)
    for (int i = 0; i < words.size() - (text.endsWith(' ') ? 0 : 1); ++i)
    {
        QString word = words[i];

        // Spell-check and highlight the word
        ComPtr<IEnumSpellingError> spellingErrors;
        HRESULT hr = _spellChecker->Check(word.toStdWString().c_str(), &spellingErrors);
        if (FAILED(hr)) continue;

        ComPtr<ISpellingError> error;
        while (spellingErrors->Next(&error) == S_OK)
        {
            ULONG startIndex, length;
            HRESULT hr = error->get_StartIndex(&startIndex);
            if (FAILED(hr)) continue;

            hr = error->get_Length(&length);
            if (FAILED(hr)) continue;

            // Apply highlight for the current word
            int globalStartIndex = text.indexOf(word) + startIndex;
            if (globalStartIndex >= 0 && globalStartIndex + length <= text.length())
                setFormat(globalStartIndex, length, _errorFormat);
        }
    }
}


QString SpellChecker::getMisspelledWord(const QString &text, ULONG startIndex, ULONG length)
{
    if (startIndex >= text.length() || startIndex + length > text.length())
        return QString(); // Out-of-bounds, return empty
    return text.mid(startIndex, length);
}
