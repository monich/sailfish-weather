/*
 * SPDX-FileCopyrightText: 2014 - 2023 Jolla Ltd.
 * SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
