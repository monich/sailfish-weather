// SPDX-FileCopyrightText: 2014 - 2023 Jolla Ltd.
// SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

import QtQuick 2.0
import Sailfish.Silica 1.0
import org.sailfishos.weather.settings 1.0
import Nemo.Configuration 1.0
import com.jolla.settings 1.0
import Sailfish.Weather 1.0

ApplicationSettings {
    id: root
    ConfigurationValue {
        id: temperatureUnitValue

        key: "/sailfish/weather/temperature_unit"
        defaultValue: "celsius"
    }
    ConfigurationValue {
        id: weatherDataProvider

        key: "/sailfish/weather/data_provider"
        defaultValue: WeatherProvider.name.FORECA
    }
    ConfigurationValue {
        id: openWeatherAppId

        key: "/sailfish/weather/open_weather_app_id"
    }

    ComboBox {
        //% "Temperature units"
        label: qsTrId("weather_settings-la-temperature_units")
        Component.onCompleted: {
            switch (temperatureUnitValue.value) {
            case "celsius":
                currentIndex = 0
                break
            case "fahrenheit":
                currentIndex = 1
                break
            default:
                console.log("WeatherSettings: Invalid temperature unit value", temperatureUnitValue.value)
                break
            }
        }

        menu: ContextMenu {
            MenuItem {
                //% "Celsius"
                text: qsTrId("weather_settings-me-celsius")
                onClicked: temperatureUnitValue.value = "celsius"
            }
            MenuItem {
                //% "Fahrenheit"
                text: qsTrId("weather_settings-me-fahrenheit")
                onClicked: temperatureUnitValue.value = "fahrenheit"
            }
        }
    }

    ComboBox {
        //% "Weather Provider"
        label: qsTrId("weather_settings-la-weather-provider")
        Component.onCompleted: {
            switch (weatherDataProvider.value) {
            case WeatherProvider.name.FORECA:
                currentIndex = 0
                break
            case WeatherProvider.name.OPEN_WEATHER:
                currentIndex = 1
                break
            default:
                console.log("WeatherSettings: Invalid weather provider value", weatherDataProvider.value)
                break
            }
        }

        menu: ContextMenu {
            MenuItem {
                //% "Foreca"
                text: qsTrId("weather_settings-me-foreca")
                onClicked: weatherDataProvider.value = WeatherProvider.name.FORECA
            }
            MenuItem {
                //% "Open Weather"
                text: qsTrId("weather_settings-me-open-weather")
                onClicked: weatherDataProvider.value = WeatherProvider.name.OPEN_WEATHER
            }
        }
    }

    TextField {
        id: providerAppIdTextField

        visible: weatherDataProvider.value === WeatherProvider.name.OPEN_WEATHER
        text: openWeatherAppId.value
        //% "API Key"
        label: qsTrId("weather_settings-api-key")
        onFocusChanged: {
            if (!focus) {
                openWeatherAppId.value = text.trim()
            }
        }
        EnterKey.iconSource: "image://theme/icon-m-enter-close"
        EnterKey.onClicked: parent.focus = true
    }

    Label {
        visible: weatherDataProvider.value === WeatherProvider.name.OPEN_WEATHER

        //% "To obtain your API key:"
        //% "<ol><li>Register an account."
        //% "<p>Go to <b><a href='%1'>OpenWeatherMap</a></b> and create an account.</p></li>"
        //% "<li>Generate your API key"
        //% "<p>After logging in, navigate to the <b><a href='%2'>API keys</a></b> section and generate a new API key.</p></li>"
        //% "<li>Enter the API key<p>Copy and paste the API key in the <b>API Key</b> field above.</p></li></ol>"
        //: Step by step instruction how to obtain api key for OpenWeather provider. Where %1 gets replaced by sign up url and %2 by api key page
        text: "<style>a:link { color: " + Theme.primaryColor + " }</style>" +
            qsTrId("weather_settings-open-weather-instruction").arg('https://home.openweathermap.org/users/sign_up')
                .arg('https://home.openweathermap.org/api_keys')
        wrapMode: Text.Wrap
        color: Theme.highlightColor
        textFormat: Text.RichText
        leftPadding: Theme.paddingLarge
        rightPadding: Theme.paddingLarge
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * x

        onLinkActivated: Qt.openUrlExternally(link)
    }
}
