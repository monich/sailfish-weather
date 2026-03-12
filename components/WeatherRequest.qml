// SPDX-FileCopyrightText: 2020 - 2023 Jolla Ltd.
// SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause
//
// @author Anton Turko <turok@duck.com>

import QtQuick 2.0
import Sailfish.Weather 1.0

QtObject {
    id: root

    property string token
    property bool active: true
    property string source
    readonly property bool online: WeatherConnectionHelper.online
    property int status: Weather.Null
    property var request
    property bool _completed

    signal requestFinished(var result)

    onTokenChanged: sendRequest()
    onActiveChanged: if (active && _completed) attemptReload()
    onOnlineChanged: if (online && _completed) attemptReload()
    onSourceChanged: if (source.length > 0 && _completed) attemptReload()

    Component.onCompleted: {
        WeatherProvider.fetchToken(this)
        _completed = true
        attemptReload()
    }

    // Note: this is overridden in WeatherModel and WeatherForecastModel
    function updateAllowed() {
        return active
    }

    function attemptReload(userRequested) {
        if (updateAllowed()) {
            reload(userRequested)
        } else if (userRequested) {
            console.log("Weather update not allowed (not active)")
        }
    }

    // userRequested: true to open a connection dialog in case
    //                there's no currently available connection;
    //                false for the request to fail silently
    function reload(userRequested) {
        if (online && source.length > 0) {
            status = Weather.Loading
            if (WeatherProvider.fetchToken(root)) {
                sendRequest()
            }
        } else if (source.length === 0) {
            status = Weather.Null
        } else if (!userRequested && WeatherConnectionHelper.status == WeatherConnectionHelper.Unknown) {
            status = Weather.Null
        } else {
            status = Weather.Error
            if (userRequested) {
                WeatherConnectionHelper.attemptToConnectNetwork()
            } else {
                WeatherConnectionHelper.requestNetwork()
            }
        }
    }

    function sendRequest() {
        if (source.length > 0 && !request) {
            status = Weather.Loading
            request = new XMLHttpRequest()
            timeout.restart()

            // Send the proper header information along with the request
            request.onreadystatechange = function() { // Call a function when the state changes.
                if (request.readyState == XMLHttpRequest.DONE) {
                    timeout.stop()
                    if (request.status === 200) {
                        var data = JSON.parse(request.responseText)
                        requestFinished(data)
                        status = Weather.Ready
                    } else if (request.status === 401) {
                        console.warn("Unauthorized request")
                        status = Weather.Unauthorized
                    } else {
                        console.warn("Failed to obtain weather data. HTTP error code: " + request.status)
                        status = Weather.Error
                    }
                    request = undefined
                }
            }
            const url = getUrl()
            request.open("GET", url)
            request.send()
        }
    }

    function getUrl() {
        return source + token
    }

    property Timer timeout: Timer {
        id: timeout

        interval: 8000
        onTriggered: {
            if (request) {
                request.abort()
                request = undefined
                console.warn("Failed to obtain weather data. The request timed out after 8 seconds")
                status = Weather.Error
            }
        }
    }
}
