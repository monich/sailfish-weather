
#include <QtQml>
#include <QQmlExtensionPlugin>
#include <QQmlEngine>
#include <QTranslator>
#include <QGuiApplication>
#include <QLocale>

#include "weather.h"
#include "savedweathersmodel.h"
#include "weatherlauncher.h"

static QObject *weatherlauncher_api_factory(QQmlEngine *, QJSEngine *)
{
    return new WeatherLauncher;
}


// using custom translator so it gets properly removed from qApp when engine is deleted
class AppTranslator: public QTranslator
{
    Q_OBJECT
public:
    AppTranslator(QObject *parent)
        : QTranslator(parent)
    {
        qApp->installTranslator(this);
    }

    virtual ~AppTranslator()
    {
        qApp->removeTranslator(this);
    }
};

class SailfishWeatherPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "Sailfish.Weather")

public:
    void initializeEngine(QQmlEngine *engine, const char *uri)
    {
        Q_UNUSED(uri)
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Sailfish.Weather"));

        AppTranslator *engineeringEnglish = new AppTranslator(engine);
        AppTranslator *translator = new AppTranslator(engine);
        engineeringEnglish->load("sailfish_components_weather_qt5_eng_en", "/usr/share/translations");
        translator->load(QLocale(), "sailfish_components_weather_qt5", "-", "/usr/share/translations");
    }
    virtual void registerTypes(const char *uri)
    {
        Q_UNUSED(uri)
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Sailfish.Weather"));
        qmlRegisterType<SavedWeathersModel>(uri, 1, 0, "SavedWeathersModel");
        qmlRegisterSingletonType<WeatherLauncher>(uri, 1, 0, "WeatherLauncher", weatherlauncher_api_factory);
        qmlRegisterUncreatableType<Weather>(uri, 1, 0, "Weather", "Weather element cannot be created from QML.");
    }
};

#include "plugin.moc"

