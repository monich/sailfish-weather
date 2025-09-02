// SPDX-FileCopyrightText: 2014 - 2023 Jolla Ltd.
// SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

import QtQuick 2.1
import Sailfish.Silica 1.0
import Sailfish.Weather 1.0

WeatherModel {
    id: model

    active: Qt.application.active
    property Connections reloadOnUsersRequest: Connections {
        target: weatherApplication
        onReload: {
            if (locationId === model.locationId) {
                model.reload()
            }
        }
        onReloadAll: model.reload(true)
        onReloadAllIfAllowed: if (model.updateAllowed()) model.reload()
    }
}
