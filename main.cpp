#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "spellchecker.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    qmlRegisterType<SpellChecker>("SpellChecker", 1, 0, "SpellChecker");
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("QtSpellcheckingWindowsTSF", "Main");

    return app.exec();
}
