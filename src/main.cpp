#include <KAboutData>
#include <KIconTheme>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QUrl>
#include <QtQml>
#include <QIcon>
#include <QString>

#include "player.h"

#include <iostream>

void registerPlayer(Player& player)
{
    qmlRegisterSingletonInstance<Player>("org.speedshifter", 0, 1, "Player", &player);
}

// basic initialization code
int main(int argc, char* argv[])
{
    std::cout << "Hello world!\n";
    KIconTheme::initTheme();
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("speedshifter");
    QApplication::setApplicationName(QStringLiteral("Speed Shifter"));
    QApplication::setDesktopFileName(QStringLiteral("org.speedshifter.desktop"));
    QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/icon.png")));

    QApplication::setStyle(QStringLiteral("breeze"));
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }

    KAboutData aboutData {
        QStringLiteral("speedshifter"),
        i18nc("@title", "Speed Shifter"),
        QStringLiteral("0.1"),
        i18n("A practice tool to adjust the speed of any audio track without altering the pitch"),
        KAboutLicense::GPL,
        i18n("(c) 2025")
    };

    aboutData.addAuthor(
        i18nc("@info:credit", "Jens Kromdijk"),
        i18nc("@info:credit", "Developer"),
        QStringLiteral("snej55.dev@gmail.com"),
        QStringLiteral("https://github.com/snej55"));

    // set about data as information
    KAboutData::setApplicationData(aboutData);

    qmlRegisterSingletonType(
        "org.speedshifter",
        0, 1, // major/minor versions
        "About",
        [](QQmlEngine* engine, QJSEngine*) -> QJSValue {
            return engine->toScriptValue(KAboutData::applicationData());
        });

    QQmlApplicationEngine engine;

    Player player;
    registerPlayer(player);

    player.init(argc, argv);

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.loadFromModule("org.speedshifter", "Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
