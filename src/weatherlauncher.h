#include <QObject>
#include <contentaction.h>

#ifndef WEATHERLAUNCHER_H
#define WEATHERLAUNCHER_H

class WeatherLauncher : public QObject
{
    Q_OBJECT
public:
    WeatherLauncher() {}
    Q_INVOKABLE void launch()
    {
        ContentAction::Action action = ContentAction::Action::launcherAction(
                    QStringLiteral("sailfish-weather.desktop"), QStringList());
        action.trigger();
    }
};

#endif // WEATHERLAUNCHER_H
