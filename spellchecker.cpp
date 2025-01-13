#include "spellchecker.h"

SpellChecker::SpellChecker(QTextDocument *parent) : QSyntaxHighlighter(parent)
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
    QTextDocument *doc = this->document();  // Access the QTextDocument from QML

    ComPtr<IEnumSpellingError> spellingErrors;
    HRESULT hr = _spellChecker->Check(text.toStdWString().c_str(), &spellingErrors);
    if (FAILED(hr)) return;

    ComPtr<ISpellingError> error;
    while (spellingErrors->Next(&error) == S_OK)
    {
        ULONG startIndex, length;
        HRESULT hr = error->get_StartIndex(&startIndex);
        if (FAILED(hr))
        {
            qWarning("Failed to get start index");
            return; // Or handle appropriately
        }

        hr = error->get_Length(&length);
        if (FAILED(hr))
        {
            qWarning("Failed to get length");
            return; // Or handle appropriately
        }

        // Apply the highlight
        if (startIndex >= 0 && startIndex + length <= text.length())
            setFormat(startIndex, length, _errorFormat);
       setCurrentBlockState(0);  // Clear previous block state if any
    }
}

/*
QVariantList SpellChecker::checkSpelling(const QString &text)
{
    QVariantList errors;
    if (!_spellChecker) return errors;

    ComPtr<IEnumSpellingError> spellingErrors;
    HRESULT hr = _spellChecker->Check(text.toStdWString().c_str(), &spellingErrors);
    if (FAILED(hr)) return errors;

    ComPtr<ISpellingError> error;
    while (spellingErrors->Next(&error) == S_OK)
    {
        ULONG startIndex, length;
        CORRECTIVE_ACTION action;
        HRESULT hr = error->get_StartIndex(&startIndex);
        if (FAILED(hr))
        {
            qWarning("Failed to get start index");
            return errors; // Or handle appropriately
        }

        hr = error->get_Length(&length);
        if (FAILED(hr))
        {
            qWarning("Failed to get length");
            return errors; // Or handle appropriately
        }

        hr = error->get_CorrectiveAction(&action);
        if (FAILED(hr))
        {
            qWarning("Failed to get corrective action");
            return errors; // Or handle appropriately
        }

        QVariantMap errorInfo;
        errorInfo["start"] = static_cast<int>(startIndex);
        errorInfo["length"] = static_cast<int>(length);

        if (action == CORRECTIVE_ACTION_GET_SUGGESTIONS)
        {
            ComPtr<IEnumString> suggestions;
            HRESULT hr = _spellChecker->Suggest(getMisspelledWord(text, startIndex, length).toStdWString().c_str(), &suggestions);
            if (SUCCEEDED(hr) && suggestions)
            {
                LPOLESTR suggestion = nullptr;
                QStringList suggestionList;
                while (suggestions->Next(1, &suggestion, nullptr) == S_OK)
                {
                    suggestionList.append(QString::fromWCharArray(suggestion));
                    CoTaskMemFree(suggestion); // Free the memory allocated for the suggestion
                }
                errorInfo["suggestions"] = suggestionList;
            }
        }
        errors.append(errorInfo);
    }
    return errors;
}
*/

QString SpellChecker::getMisspelledWord(const QString &text, ULONG startIndex, ULONG length)
{
    if (startIndex >= text.length() || startIndex + length > text.length())
        return QString(); // Out-of-bounds, return empty
    return text.mid(startIndex, length);
}
