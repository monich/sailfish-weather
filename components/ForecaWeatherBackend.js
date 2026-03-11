// SPDX-FileCopyrightText: 2014 - 2023 Jolla Ltd.
// SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause


function currentWeatherUrl(weather) {
    return 'https://pfa.foreca.com/api/v1/current/' + weather.locationId + authParam()
}

function latestObservationUrl(weather) {
    return "https://pfa.foreca.com/api/v1/observation/latest/" + authParam()
}

function forecastUrl(weather, isHourly) {
    return 'https://pfa.foreca.com/api/v1/forecast/' + (hourly ? "hourly/" : "daily/") + weather.locationId + authParam()
}

function searchLocationUrl(filter, language) {
    return "https://pfa.foreca.com/api/v1/location/search/" + filter.toLowerCase() + "&lang=" + language + authParam()
}

function handleCurrentWeatherResult(result) {

    var current = result["current"]
    if (result.length === 0 || current.temperature === "") {
        return undefined
    }

    var weather = getWeatherData(current)
    weather.timestamp =  new Date(current.time)
    this.timestamp = weather.timestamp

    weather.temperature = current.temperature
    weather.feelsLikeTemperature = current.feelsLikeTemp
    return weather
}


function handleObservationResult(result) {
    var observations = result["observations"]
    if (observations.length > 0) {
        return observations[0].station
    }

    return ""
}

function handleForecastResult(result, hourly, visibleCount, minimumHourlyRange) {
    var forecast = result["forecast"]
    if (result.length === 0 || forecast.length === 0) {
        return undefined
    }

    var weatherData = []
    for (var i = 0; i < forecast.length; i++) {
        var data = forecast[i]
        var weather = getWeatherData(data)
        if (hourly) {
            if (i % 3 !== 0) continue
            weather.timestamp =  new Date(data.time)
            weather.temperature = data.temperature
        } else {
            var dateArray = data.date.split("-")
            weather.timestamp = new Date(parseInt(dateArray[0]),
                                         parseInt(dateArray[1] - 1),
                                         parseInt(dateArray[2]))
            weather.accumulatedPrecipitation = data.precipAccum
            weather.maximumWindSpeed = data.maxWindSpeed
            weather.windDirection = data.windDir
            weather.high = data.maxTemp
            weather.low = data.minTemp
        }
        weatherData[weatherData.length] = weather
    }

    if (hourly) {
        var minimumTemperature = weatherData[0].temperature
        var maximumTemperature = weatherData[0].temperature
        for (i = 1; i < visibleCount + 1; i++) {
            var temperature = weatherData[i].temperature
            minimumTemperature = Math.min(minimumTemperature, temperature)
            maximumTemperature = Math.max(maximumTemperature, temperature)
        }
        var range = maximumTemperature - minimumTemperature
        if (range < minimumHourlyRange) {
            minimumTemperature -= Math.floor((minimumHourlyRange - range ) / 2)
            range = minimumHourlyRange
        }

        for (i = 0; i < visibleCount + 1; i++) {
            weatherData[i].relativeTemperature = (weatherData[i].temperature - minimumTemperature) / range
        }
    }

    return weatherData;
}

function handleSearchLocationResult(result) {
    return result["locations"]
}

function externalUrl(weather) {
    return "https://foreca.mobi/spot.php?l=" + weather.locationId;
}

function providerImage() {
    return "image://theme/graphic-foreca-large?"
}

function smallProviderImage() {
    return "image://theme/graphic-foreca-small?"
}

function getWeatherData(weather) {
    var data = {
        "description": description(weather.symbol),
        "weatherType": weatherType(weather.symbol),
        "cloudiness": (100*parseInt(weather.symbol.charAt(1))/4)
    }
    return data
}

function weatherType(code) {
    // just direct mapping, but ensure we receive valid data
    if (code.length === 4) {
        return code
    } else {
        console.warn("Invalid weather code")
        return ""
    }
}

function description(code) {
    var localizations = {
        //% "Clear"
        "000": qsTrId("weather-la-description_clear"),
        //% "Mostly clear"
        "100": qsTrId("weather-la-description_mostly_clear"),
        //% "Partly cloudy"
        "200": qsTrId("weather-la-description_partly_cloudy"),
        //% "Cloudy"
        "300": qsTrId("weather-la-description_cloudy"),
        //% "Overcast"
        "400": qsTrId("weather-la-description_overcast"),
        //% "Thin high clouds"
        "500": qsTrId("weather-la-description-thin_high_clouds"),
        //% "Fog"
        "600": qsTrId("weather-la-description-fog"),
        //% "Partly cloudy and light rain"
        "210": qsTrId("weather-la-description_partly_cloudy_and_light_rain"),
        //% "Cloudy and light rain"
        "310": qsTrId("weather-la-description_cloudy_and_light_rain"),
        //% "Overcast and light rain"
        "410": qsTrId("weather-la-description_overcast_and_light_rain"),
        //% "Partly cloudy and showers"
        "220": qsTrId("weather-la-description_partly_cloudy_and_showers"),
        //% "Cloudy and showers"
        "320": qsTrId("weather-la-description_cloudy_and_showers"),
        //% "Overcast and showers"
        "420": qsTrId("weather-la-description_overcast_and_showers"),
        //% "Overcast and rain"
        "430": qsTrId("weather-la-description_overcast_and_rain"),
        //% "Partly cloudy, possible thunderstorms with rain"
        "240": qsTrId("weather-la-description_partly_cloudy_possible_thunderstorms_with_rain"),
        //% "Cloudy, thunderstorms with rain"
        "340": qsTrId("weather-la-description_cloudy_thunderstorms_with_rain"),
        //% "Overcast, thunderstorms with rain"
        "440": qsTrId("weather-la-description_overcast_thunderstorms_with_rain"),
        //% "Partly cloudy and light wet snow"
        "211": qsTrId("weather-la-description_partly_cloudy_and_light_wet_snow"),
        //% "Cloudy and light wet snow"
        "311": qsTrId("weather-la-description_cloudy_and_light_wet_snow"),
        //% "Overcast and light wet snow"
        "411": qsTrId("weather-la-description_overcast_and_light_wet_snow"),
        //% "Partly cloudy and wet snow showers"
        "221": qsTrId("weather-la-description_partly_cloudy_and_wet_snow_showers"),
        //% "Cloudy and wet snow showers"
        "321": qsTrId("weather-la-description_cloudy_and_wet_snow_showers"),
        //% "Overcast and wet snow showers"
        "421": qsTrId("weather-la-description_overcast_and_wet_snow_showers"),
        //% "Overcast and wet snow"
        "431": qsTrId("weather-la-description_overcast_and_wet_snow"),
        //% "Partly cloudy and light snow"
        "212": qsTrId("weather-la-description_partly_cloudy_and_light_snow"),
        //% "Cloudy and light snow"
        "312": qsTrId("weather-la-description_cloudy_and_light_snow"),
        //% "Overcast and light snow"
        "412": qsTrId("weather-la-description_overcast_and_light_snow"),
        //% "Partly cloudy and snow showers"
        "222": qsTrId("weather-la-description_partly_cloudy_and_snow_showers"),
        //% "Cloudy and snow showers"
        "322": qsTrId("weather-la-description_cloudy_and_snow_showers"),
        //% "Overcast and snow showers"
        "422": qsTrId("weather-la-description_overcast_and_snow_showers"),
        //% "Overcast and snow"
        "432": qsTrId("weather-la-description_overcast_and_snow")
    }

    return localizations[code.substr(1,3)]
}

function authParam() {
    return '&token='
}
