#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QImage>
#include <QtQml>
#include "q565transcoder.h"
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("q565 Image Viewer");
    qmlRegisterType<Q565Transcoder>("com.q565viewer.components", 1, 0, "Q565Transcoder");
    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("q565viewer", "Main");

    return app.exec();
}
