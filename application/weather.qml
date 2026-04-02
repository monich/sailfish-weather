// SPDX-FileCopyrightText: 2013 - 2023 Jolla Ltd.
// SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

import QtQuick 2.1
import Sailfish.Silica 1.0
import Sailfish.Weather 1.0
import "cover"
import "model"
import "pages"

ApplicationWindow {
    id: weatherApplication

    property var weatherModels
    property bool currentWeatherAvailable: savedWeathersModel.currentWeather
                                        && savedWeathersModel.currentWeather.populated
                                        && WeatherProvider.isLocationCompatible(savedWeathersModel.currentWeather)

    function weatherModelKey(provider, locationId) {
        return provider + ":" + locationId
    }

    initialPage: Component { MainPage {} }
    cover: Component { WeatherCover {} }
    allowedOrientations: Screen.sizeCategory > Screen.Medium
                         ? defaultAllowedOrientations
                         : defaultAllowedOrientations & Orientation.PortraitMask
    _defaultPageOrientations: Orientation.All

    signal reload(int locationId)
    signal reloadAll()
    signal reloadAllIfAllowed()

    Connections {
        target: Qt.application
        onActiveChanged: {
            if (!Qt.application.active) {
                savedWeathersModel.save()
            }
        }
    }
    ApplicationWeatherModel {
        id: currentWeatherModel

        savedWeathers: savedWeathersModel
        weather: savedWeathersModel.currentWeather
    }
    Instantiator {
        asynchronous: true
        onObjectAdded: {
            var models = weatherModels ? weatherModels : {}
            models[weatherModelKey(object.provider, object.locationId)] = object
            weatherModels = models
        }
        onObjectRemoved: {
            if (!weatherModels) {
                return
            }

            var models = weatherModels
            delete models[weatherModelKey(object.provider, object.locationId)]
            weatherModels = models
        }

        model: SavedWeathersModel {
            id: savedWeathersModel
            provider: WeatherProvider.currentProvider()
        }
        ApplicationWeatherModel {
            savedWeathers: savedWeathersModel
            weather: model
        }
    }
}
