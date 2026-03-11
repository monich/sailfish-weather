// SPDX-FileCopyrightText: 2014 - 2023 Jolla Ltd.
// SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

import QtQuick 2.0
import Sailfish.Silica 1.0

Image {
    property var weatherType
    property bool highlighted

    source: weatherType.length > 0 ? "image://theme/graphic-weather-" + weatherType
                                     + (highlighted ? "?" + Theme.highlightColor : "")
                                   : ""
}
