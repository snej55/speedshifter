#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QtQuick>

int main(int argc, char* argv[])
{
    // qputenv("QT_QUICK_CONTROLS_STYLE", "Fusion");
    QGuiApplication app(argc, argv);
    QGuiApplication::setWindowIcon(QIcon(":/icon.png"));

    QQmlApplicationEngine engine;

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("speedshifter", "Main");

    return app.exec();
}
