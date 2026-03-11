// SPDX-FileCopyrightText: 2014 - 2023 Jolla Ltd.
// SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

import QtQuick 2.0
import Sailfish.Weather 1.0
import 'update-utils.js' as UpdateUtils

ListModel {
    id: root

    property bool hourly
    property var weather
    property alias active: model.active
    property date timestamp
    property alias status: model.status
    property int visibleCount: 6
    property int minimumHourlyRange: 4
    readonly property bool loading: forecastModel.status == Weather.Loading
    readonly property int locationId: weather ? weather.locationId : -1

    onLocationIdChanged: {
        model.status = Weather.Null
        clear()
    }

    function attemptReload(userRequested) {
        model.attemptReload(userRequested)
    }

    function reload(userRequested) {
        model.reload(userRequested)
    }

    readonly property WeatherRequest model: WeatherRequest {
        id: model

        source: root.locationId > 0 ? WeatherProvider.forecastUrl(weather, hourly) : ""

        // update allowed every half hour for hourly weather, every 3 hours for daily weather
        property int maxUpdateInterval: hourly ? 30*60*1000 : 180*60*1000

        function updateAllowed() {
            return status !== Weather.Unauthorized && (status === Weather.Error || status === Weather.Null || UpdateUtils.updateAllowed(maxUpdateInterval))
        }

        onRequestFinished: {
            var weatherData = WeatherProvider.handleForecastResult(result, hourly, visibleCount, minimumHourlyRange);

            if (weatherData === undefined) {
                error = true
                return
            }

            while (root.count > weatherData.length) {
                root.remove(weatherData.length)
            }

            for (var i = 0; i < weatherData.length; i++) {
                if (i < root.count) {
                    root.set(i, weatherData[i])
                } else {
                    root.append(weatherData[i])
                }
            }
        }

        onStatusChanged: {
            if (status === Weather.Error) {
                console.log("WeatherForecastModel - could not obtain forecast weather data", weather ? weather.city : "", weather ? weather.locationId : "")
            }
        }
    }
}
