import QtQuick 2.0

Item {
    // providing dummy translations for app descriptions shown on Store
    function qsTrIdString() {
        //% "Weather app shows current weather and forecasts for multiple locations. "
        //% "Peek Events View to quickly check the daily and hourly weather forecast of your current location."
        QT_TRID_NOOP("weather-la-store_app_summary")

        //% "Use the main page pulley to add more weather locations. Tap saved location "
        //% "items to view detailed five day forecast. Weather forecasts include information "
        //% "about daily temperature highs and lows, expected wind speed and direction, precipitation and cloudiness.\n"
        //% "\n"
        //% "By default the temperature is shown in Celsius. If you prefer Fahrenheit go to "
        //% Settings -> App -> Weather to change the used temperature unit.\n"
        //% "\n"
        //% "Weather is powered by Foreca weather service."
        QT_TRID_NOOP("weather-la-store_app_description")
    }
}
