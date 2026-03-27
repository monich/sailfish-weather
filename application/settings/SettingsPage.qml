// SPDX-FileCopyrightText: 2014 - 2023 Jolla Ltd.
// SPDX-FileCopyrightText: 2024 - 2026 Jolla Mobile Ltd
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

    property int selectedProviderIndex: WeatherProvider.indexOfProvider(weatherDataProvider.value)
    property var selectedProvider: WeatherProvider.providerInfo(weatherDataProvider.value)

    function temperatureUnitIndex() {
        return temperatureUnitValue.value === "fahrenheit" ? 1 : 0
    }

    function weatherProviderMenuIndex() {
        return selectedProviderIndex >= 0
                ? selectedProviderIndex + (WeatherProvider.allowUnsetProvider ? 1 : 0)
                : 0
    }

    function syncTemperatureUnitIndex() {
        var index = temperatureUnitIndex()
        if (temperatureUnitComboBox.currentIndex !== index) {
            temperatureUnitComboBox.currentIndex = index
        }
    }

    function syncWeatherProviderIndex() {
        var index = weatherProviderMenuIndex()
        if (weatherProviderComboBox.currentIndex !== index) {
            weatherProviderComboBox.currentIndex = index
        }
    }

    function syncProviderApiKeyText() {
        if (!providerAppIdTextField.activeFocus && providerAppIdTextField.text !== providerApiKey.value) {
            providerAppIdTextField.text = providerApiKey.value
        }
    }

    ConfigurationValue {
        id: temperatureUnitValue

        key: "/sailfish/weather/temperature_unit"
        defaultValue: "celsius"

        onValueChanged: root.syncTemperatureUnitIndex()
    }

    ConfigurationValue {
        id: weatherDataProvider

        key: "/sailfish/weather/data_provider"
        defaultValue: WeatherProvider.defaultProviderId

        onValueChanged: root.syncWeatherProviderIndex()
    }

    ConfigurationValue {
        id: providerApiKey

        key: WeatherProvider.apiKeyConfigurationKey(weatherDataProvider.value)
        defaultValue: ""

        onValueChanged: root.syncProviderApiKeyText()
        onKeyChanged: root.syncProviderApiKeyText()
    }

    ComboBox {
        id: temperatureUnitComboBox

        //% "Temperature units"
        label: qsTrId("weather_settings-la-temperature_units")
        Component.onCompleted: root.syncTemperatureUnitIndex()

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
        id: weatherProviderComboBox

        //% "Weather Provider"
        label: qsTrId("weather_settings-la-weather-provider")
        Component.onCompleted: root.syncWeatherProviderIndex()

        menu: ContextMenu {
            MenuItem {
                //% "Choose"
                text: qsTrId("weather-me-choose")
                visible: WeatherProvider.allowUnsetProvider
                onClicked: weatherDataProvider.value = ""
            }

            Repeater {
                model: WeatherProvider.providers

                delegate: MenuItem {
                    property var provider: modelData

                    text: provider.title
                    onClicked: weatherDataProvider.value = provider.id
                }
            }
        }
    }

    onSelectedProviderIndexChanged: root.syncWeatherProviderIndex()

    TextField {
        id: providerAppIdTextField

        visible: !!root.selectedProvider && root.selectedProvider.requiresApiKey
        text: providerApiKey.value
        //% "API Key"
        label: qsTrId("weather_settings-api-key")
        onFocusChanged: {
            if (!focus) {
                providerApiKey.value = text.trim()
            }
        }
        onVisibleChanged: if (visible) root.syncProviderApiKeyText()
        EnterKey.iconSource: "image://theme/icon-m-enter-close"
        EnterKey.onClicked: parent.focus = true
    }

    Label {
        visible: !!root.selectedProvider
                 && root.selectedProvider.requiresApiKey
                 && root.selectedProvider.apiKeyInstructions.length > 0

        text: "<style>a:link { color: " + Theme.primaryColor + " }</style>"
                + root.selectedProvider.apiKeyInstructions
        wrapMode: Text.Wrap
        color: Theme.highlightColor
        textFormat: Text.RichText
        leftPadding: Theme.paddingLarge
        rightPadding: Theme.paddingLarge
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * x

        onLinkActivated: Qt.openUrlExternally(link)
    }

    Label {
        visible: !!root.selectedProvider
                 && root.selectedProvider.attributionText.length > 0

        text: "<style>a:link { color: " + Theme.primaryColor + " }</style>"
                + root.selectedProvider.attributionText
        wrapMode: Text.Wrap
        color: Theme.secondaryHighlightColor
        textFormat: Text.RichText
        leftPadding: Theme.paddingLarge
        rightPadding: Theme.paddingLarge
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * x

        onLinkActivated: Qt.openUrlExternally(link)
    }
}
