// SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

.pragma library

function normalizeHourlyTemperatures(weatherData, visibleCount, minimumHourlyRange, floorTemperature)
{
    if (!weatherData || weatherData.length < visibleCount + 1) {
        return undefined
    }

    var minimumTemperature = weatherData[0].temperature
    var maximumTemperature = weatherData[0].temperature
    for (var i = 1; i < visibleCount + 1; i++) {
        var temperature = weatherData[i].temperature
        minimumTemperature = Math.min(minimumTemperature, temperature)
        maximumTemperature = Math.max(maximumTemperature, temperature)
    }

    var range = maximumTemperature - minimumTemperature
    if (range < minimumHourlyRange) {
        minimumTemperature -= Math.floor((minimumHourlyRange - range) / 2)
        range = minimumHourlyRange
    }

    for (i = 0; i < visibleCount + 1; i++) {
        weatherData[i].relativeTemperature = (weatherData[i].temperature - minimumTemperature) / range
        if (floorTemperature) {
            weatherData[i].temperature = Math.floor(weatherData[i].temperature)
        }
    }

    return weatherData
}
