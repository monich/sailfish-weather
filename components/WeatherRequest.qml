// SPDX-FileCopyrightText: 2020 - 2023 Jolla Ltd.
// SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause
//
// @author Anton Turko <turok@duck.com>

import QtQuick 2.0
import Sailfish.Weather 1.0
import "WeatherResponseCache.js" as WeatherResponseCache

QtObject {
    id: root

    property string token
    property bool active: true
    property string source
    property string responseType: "json"
    property bool fallbackSessionCache
    readonly property bool online: WeatherConnectionHelper.online
    property int status: Weather.Null
    property var request
    property bool _completed
    property string _inflightUrl: ""

    signal requestFinished(var result)

    onTokenChanged: {
        if (_inflightUrl.length > 0 && getUrl() !== _inflightUrl) {
            WeatherResponseCache.releaseInflight(_inflightUrl, root)
            _inflightUrl = ""
        }
        sendRequest()
    }
    onActiveChanged: {
        if (!active && _inflightUrl.length > 0) {
            WeatherResponseCache.releaseInflight(_inflightUrl, root)
            _inflightUrl = ""
        }
        if (active && _completed) attemptReload()
    }
    onOnlineChanged: if (online && _completed) attemptReload()
    onSourceChanged: {
        if (_inflightUrl.length > 0 && getUrl() !== _inflightUrl) {
            WeatherResponseCache.releaseInflight(_inflightUrl, root)
            _inflightUrl = ""
        }
        if (source.length > 0 && _completed) attemptReload()
    }

    Component.onDestruction: {
        if (_inflightUrl.length > 0) {
            WeatherResponseCache.releaseInflight(_inflightUrl, root)
            _inflightUrl = ""
        }
    }

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
            const url = getUrl()
            var cachedData = WeatherResponseCache.freshResponse(url)
            if (cachedData !== undefined) {
                requestFinished(cachedData)
                if (status === Weather.Loading) {
                    status = Weather.Ready
                }
                return
            }

            if (!WeatherResponseCache.beginInflight(url, root)) {
                _inflightUrl = url
                status = Weather.Loading
                return
            }

            _inflightUrl = url

            status = Weather.Loading
            request = new XMLHttpRequest()
            timeout.restart()

            // Send the proper header information along with the request
            request.onreadystatechange = function() { // Call a function when the state changes.
                if (request.readyState == XMLHttpRequest.DONE) {
                    timeout.stop()
                    if (request.status === 200) {
                        var data = responseType === "text" ? request.responseText : JSON.parse(request.responseText)
                        WeatherResponseCache.store(url, data,
                                                   WeatherResponseCache.responseHeaders(request),
                                                   fallbackSessionCache)
                        notifyInflightSuccess(url, data)
                    } else if (request.status === 304) {
                        WeatherResponseCache.updateMetadata(url, WeatherResponseCache.responseHeaders(request))
                        var cachedResponse = WeatherResponseCache.cachedResponse(url)
                        if (cachedResponse !== undefined) {
                            notifyInflightSuccess(url, cachedResponse)
                        } else {
                            console.warn("Received HTTP 304 without cached weather data")
                            notifyInflightFailure(url, Weather.Error)
                        }
                    } else if (request.status === 401) {
                        console.warn("Unauthorized request")
                        notifyInflightFailure(url, Weather.Unauthorized)
                    } else {
                        console.warn("Failed to obtain weather data. HTTP error code: " + request.status)
                        notifyInflightFailure(url, Weather.Error)
                    }
                    request = undefined
                    _inflightUrl = ""
                }
            }
            request.open("GET", url)
            var headers = WeatherProvider.requestHeaders()
            var cacheHeaders = WeatherResponseCache.conditionalHeaders(url)
            for (var cacheHeaderName in cacheHeaders) {
                headers[cacheHeaderName] = cacheHeaders[cacheHeaderName]
            }
            console.log("WeatherRequest: sending request for", url)
            for (var requestHeaderName in headers) {
                request.setRequestHeader(requestHeaderName, headers[requestHeaderName])
            }
            request.send()
        }
    }

    function notifyInflightSuccess(url, data) {
        var waiters = WeatherResponseCache.takeInflight(url)
        for (var i = 0; i < waiters.length; ++i) {
            if (waiters[i] && waiters[i].getUrl && waiters[i].getUrl() === url) {
                waiters[i]._inflightUrl = ""
                waiters[i].requestFinished(data)
                if (waiters[i].status === Weather.Loading) {
                    waiters[i].status = Weather.Ready
                }
            }
        }
    }

    function notifyInflightFailure(url, failureStatus) {
        var waiters = WeatherResponseCache.takeInflight(url)
        for (var i = 0; i < waiters.length; ++i) {
            if (waiters[i] && waiters[i].getUrl && waiters[i].getUrl() === url) {
                waiters[i]._inflightUrl = ""
                waiters[i].status = failureStatus
            }
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
                var url = getUrl()
                request.abort()
                request = undefined
                notifyInflightFailure(url, Weather.Error)
                _inflightUrl = ""
                console.warn("Failed to obtain weather data. The request timed out after 8 seconds")
            }
        }
    }
}
