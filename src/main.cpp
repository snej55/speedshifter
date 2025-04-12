#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QUrl>
#include <QQuickStyle>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <KIconTheme>
#include <KAboutData>

#include "player.h"

void registerPlayer(Player& player) {
    qmlRegisterSingletonInstance<Player>("org.kde.speedshifter", 1, 0, "Player", &player);
}

// basic initialization code
int main(int argc, char* argv[])
{
    KIconTheme::initTheme();
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("speedshifter");
    QApplication::setApplicationName(QStringLiteral("Speed Shifter"));
    QApplication::setDesktopFileName(QStringLiteral("org.kde.speedshifter.desktop"));

    QApplication::setStyle(QStringLiteral("breeze"));
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE"))
    {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }

    KAboutData aboutData {
        QStringLiteral("speedshifter"),
        i18nc("@title", "Speed Shifter"),
        QStringLiteral("1.0"),
        i18n("A practice tool to adjust the speed of any audio track without altering the pitch"),
        KAboutLicense::GPL,
        i18n("(c) 2025")
    };

    aboutData.addAuthor(
        i18nc("@info:credit", "Jens Kromdijk"),
        i18nc("@info:credit", "Developer"),
        QStringLiteral("snej55.dev@gmail.com"),
        QStringLiteral("https://github.com/snej55")
    );

    // set about data as information
    KAboutData::setApplicationData(aboutData);

    qmlRegisterSingletonType(
        "org.kde.speedshifter",
        1, 0, // major/minor versions
        "About",
        [](QQmlEngine* engine, QJSEngine*) -> QJSValue {
            return engine->toScriptValue(KAboutData::applicationData());
        }
    );

    QQmlApplicationEngine engine;

    Player player;
    registerPlayer(player);

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.loadFromModule("org.kde.speedshifter", "Main");

    if (engine.rootObjects().isEmpty())
    {
        return -1;
    }

    return app.exec();
}
