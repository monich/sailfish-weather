// SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

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

    if (!code || code.length < 3) {
        return ""
    }

    return localizations[code.substr(code.length - 3, 3)] || ""
}
