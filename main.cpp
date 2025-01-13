#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "highlightcomponent.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    HighlightComponent::registerQmlType();
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("QtSpellcheckingWindowsTSF", "Main");

    return app.exec();
}
