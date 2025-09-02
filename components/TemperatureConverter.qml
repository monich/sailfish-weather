// SPDX-FileCopyrightText: 2014 - 2023 Jolla Ltd.
// SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

pragma Singleton
import QtQuick 2.2
import Nemo.Configuration 1.0

ConfigurationValue {
    property bool celsius: {
        switch (value) {
        case "celsius":
            return true
        case "fahrenheit":
            return false
        default:
            console.log("TemperatureConverter: Invalid temperature unit value", value)
            return true
        }
    }
    function formatWithoutUnit(temperature) {
        return celsius ? temperature : Math.round(9/5*parseInt(temperature)+32).toString()
    }
    function format(temperature) {
        return formatWithoutUnit(temperature) + "\u00B0"
    }
    key: "/sailfish/weather/temperature_unit"
    defaultValue: "celsius"
}
