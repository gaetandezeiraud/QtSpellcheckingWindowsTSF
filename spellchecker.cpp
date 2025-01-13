#include "spellchecker.h"

SpellChecker::SpellChecker(QObject *parent) : QObject(parent)
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
}

SpellChecker::~SpellChecker()
{
    CoUninitialize();
}

Q_INVOKABLE QVariantList SpellChecker::checkSpelling(const QString &text)
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

QString SpellChecker::getMisspelledWord(const QString &text, ULONG startIndex, ULONG length)
{
    if (startIndex >= text.length() || startIndex + length > text.length())
        return QString(); // Out-of-bounds, return empty
    return text.mid(startIndex, length);
}
