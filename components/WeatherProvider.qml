// SPDX-FileCopyrightText: 2025 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause
//
// @author Anton Turko <turok@duck.com>

pragma Singleton
import QtQuick 2.6
import Nemo.Configuration 1.0

import "ForecaToken.js" as ForecaToken
import "OpenWeatherBackend.js" as OpenWeatherBackend
import "ForecaWeatherBackend.js" as ForecaWeatherBackend

QtObject {

    readonly property var name: ({
        FORECA: 'foreca',
        OPEN_WEATHER: 'open_weather',
    })
    readonly property var backend: getBackend()

    readonly property ConfigurationValue weatherProvider: ConfigurationValue {
        key: "/sailfish/weather/data_provider"
        defaultValue: name.FORECA
    }

    readonly property ConfigurationValue openWeatherProviderAppKey: ConfigurationValue {
        key: "/sailfish/weather/open_weather_app_id"
        defaultValue: ""
    }

    readonly property var isApiKeyProvided: {
        switch (weatherProvider.value) {
            case name.FORECA:
             return true
            case name.OPEN_WEATHER:
             return openWeatherProviderAppKey.value.length > 0
            default:
             console.log("Weather provider doesn't provide API key.")
             return false
        }
    }

    function getBackend() {
        switch (weatherProvider.value) {
        case name.FORECA:
            return ForecaWeatherBackend
        case name.OPEN_WEATHER:
            return OpenWeatherBackend
        default:
            console.log("Selected weather provider backend is not supported.")
            return null
        }
    }

    function fetchToken(weatherRequest) {
        switch (weatherProvider.value) {
        case name.FORECA:
            return ForecaToken.fetchToken(weatherRequest)
        case name.OPEN_WEATHER:
            weatherRequest.token = openWeatherProviderAppKey.value
            return true
        default:
            console.log("Weather provider doesn't support fetching token.")
            return false
        }
    }

    function providerImage() {
        return this.backend.providerImage()
    }

    function smallProviderImage() {
        return this.backend.smallProviderImage()
    }

    function forecastUrl(weather, hourly) {
        return this.backend.forecastUrl(weather, hourly)
    }

    function latestObservationUrl(weatherJson) {
        return this.backend.latestObservationUrl(weatherJson)
    }

    function searchLocationUrl(filter, language) {
        return this.backend.searchLocationUrl(filter, language)
    }

    function currentWeatherUrl(weather) {
        return this.backend.currentWeatherUrl(weather)
    }

    function handleSearchLocationResult(result) {
        return this.backend.handleSearchLocationResult(result)
    }

    function externalUrl(weather) {
        return this.backend.externalUrl(weather)
    }

    function handleCurrentWeatherResult(result) {
        return this.backend.handleCurrentWeatherResult(result)
    }

    function handleObservationResult(result) {
        return this.backend.handleObservationResult(result)
    }

    function handleForecastResult(result, hourly, visibleCount, minimumHourlyRange) {
        var res = this.backend.handleForecastResult(result, hourly, visibleCount, minimumHourlyRange)
        for (var index in res) {
            var weather = res[index];
            var precipitationRateCode = weather.weatherType.charAt(2)
            var precipitationRate = ""
            switch (precipitationRateCode) {
            case '0':
                //% "No precipitation"
                precipitationRate = qsTrId("weather-la-precipitation_none")
                break
            case '1':
                //% "Slight precipitation"
                precipitationRate = qsTrId("weather-la-precipitation_slight")
                break
            case '2':
                //% "Showers"
                precipitationRate = qsTrId("weather-la-precipitation_showers")
                break
            case '3':
                //% "Precipitation"
                precipitationRate = qsTrId("weather-la-precipitation_normal")
                break
            case '4':
                //% "Thunder"
                precipitationRate = qsTrId("weather-la-precipitation_thunder")
                break
            default:
                console.log("WeatherModel warning: invalid precipitation rate code", precipitationRateCode)
                break
            }

            var precipitationType = ""
            if (precipitationRateCode === '0') { // no rain
                //% "None"
                precipitationType = qsTrId("weather-la-precipitationtype_none")
            } else {
                var precipitationTypeCode = weather.weatherType.charAt(3)
                switch (precipitationTypeCode) {
                case '0':
                    //% "Rain"
                    precipitationType = qsTrId("weather-la-precipitationtype_rain")
                    break
                case '1':
                    //% "Sleet"
                    precipitationType = qsTrId("weather-la-precipitationtype_sleet")
                    break
                case '2':
                    //% "Snow"
                    precipitationType = qsTrId("weather-la-precipitationtype_snow")
                    break
                default:
                    console.log("WeatherModel warning: invalid precipitation type code", precipitationTypeCode)
                    break
                }
            }
            weather['precipitationType'] = precipitationType
            weather['precipitationRate'] = precipitationRate
        }
        return res;
    }
}
