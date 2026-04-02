// SPDX-FileCopyrightText: 2013 - 2023 Jolla Ltd.
// SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

import QtQuick 2.0
import Sailfish.Weather 1.0
import 'update-utils.js' as UpdateUtils

WeatherRequest {
    property var weather
    property var savedWeathers
    property date timestamp: new Date()
    readonly property int locationId: !!weather ? weather.locationId : -1
    readonly property string provider: weather ? WeatherProvider.locationProvider(weather)
                                               : WeatherProvider.defaultProviderId

    onWeatherChanged: {
        status = Weather.Null
        latestObservation.active = false
        latestObservation.requestedLocationId = -1
        latestObservation.weatherJson = undefined
    }

    readonly property WeatherRequest latestObservation: WeatherRequest {
        property var weatherJson
        // Store our own copy of locationId, since parent.locationId may change mid-fetch
        property int requestedLocationId: -1

        active: false
        source: requestedLocationId > 0 && weatherJson && WeatherProvider.isLocationCompatible(weatherJson)
                ? WeatherProvider.latestObservationUrl(weatherJson)
                : ""

        onRequestFinished: {
            if (!weatherJson)
                return

            active = false
            var stationName = WeatherProvider.handleObservationResult(result)

            if (stationName.length > 0) {
                weatherJson["station"] = stationName
            }
            if (savedWeathers) {
                savedWeathers.update(requestedLocationId, weatherJson)
            }
        }

        onStatusChanged: {
            if (status === Weather.Error || status == Weather.Unauthorized) {
                if (savedWeathers) {
                    savedWeathers.setErrorStatus(requestedLocationId, status,
                                                 WeatherProvider.locationProvider(weatherJson))
                }

                console.log("WeatherModel - could not obtain weather station data",
                            weather ? weather.city : "", weather ? weather.locationId : "")
            }
        }
    }

    source: locationId > 0 && WeatherProvider.isLocationCompatible(weather)
            ? WeatherProvider.currentWeatherUrl(weather)
            : ""

    // overriding WeatherRequest function
    function updateAllowed() {
        return status === Weather.Null || status === Weather.Error || UpdateUtils.updateAllowed()
    }

    onRequestFinished: {
        var weatherData = WeatherProvider.handleCurrentWeatherResult(result)
        if (weatherData === undefined) {
            status = Weather.Error
            console.log("Error: request finished with undefined data")
            return
        }

        this.timestamp = weatherData.timestamp

        var json = {
            "locationId": weather.locationId,
            "provider": WeatherProvider.locationProvider(weather),
            "latitude": weather.latitude,
            "longitude": weather.longitude,
            "temperature": weatherData.temperature,
            "feelsLikeTemperature": weatherData.feelsLikeTemperature,
            "weatherType": weatherData.weatherType,
            "description": weatherData.description,
            "timestamp": weatherData.timestamp
        }
        var observationUrl = WeatherProvider.latestObservationUrl(json)

        // Some backends reuse the same endpoint for both current conditions and
        // latest observation details. Update immediately so the UI doesn't stay
        // stuck in loading while waiting for a redundant second request.
        if (!observationUrl || observationUrl.length === 0 || observationUrl === source) {
            var stationName = WeatherProvider.handleObservationResult(result)
            if (stationName.length > 0) {
                json["station"] = stationName
            }
            if (savedWeathers) {
                savedWeathers.update(locationId, json)
            }
            latestObservation.active = false
            return
        }

        latestObservation.weatherJson = json
        latestObservation.requestedLocationId = locationId
        latestObservation.active = true
    }

    onStatusChanged: {
        if (status === Weather.Error || status == Weather.Unauthorized) {
            if (savedWeathers) {
                savedWeathers.setErrorStatus(locationId, status,
                                             WeatherProvider.locationProvider(weather))
            }

            console.log("WeatherModel - could not obtain weather data",
                        weather ? weather.city : "",
                        weather ? weather.locationId : "",
                        "status:", status)
        }
    }
}
