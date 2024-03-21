#include <QGuiApplication>
#include <QQuickView>
#include <QLocale>
#include <QTranslator>
#include <qqml.h>

#include "sailfishapplication.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QTranslator> engineeringEnglish(new QTranslator);
    engineeringEnglish->load("weather_eng_en", TRANSLATIONS_PATH);
    QScopedPointer<QTranslator> translator(new QTranslator);
    translator->load(QLocale(), "weather", "-", TRANSLATIONS_PATH);

    QScopedPointer<QGuiApplication> app(Sailfish::createApplication(argc, argv));

    app->setApplicationName(QStringLiteral("weather"));
    app->setOrganizationName(QStringLiteral("org.sailfishos"));

    app->installTranslator(engineeringEnglish.data());
    app->installTranslator(translator.data());

    QScopedPointer<QQuickView> view(Sailfish::createView("weather.qml"));
    Sailfish::showView(view.data());

    //% "Weather"
    view->setTitle(qtTrId("weather-ap-name"));

    int result = app->exec();
    app->removeTranslator(translator.data());
    app->removeTranslator(engineeringEnglish.data());
    return result;
}
