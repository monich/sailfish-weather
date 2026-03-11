// SPDX-FileCopyrightText: 2015 - 2023 Jolla Ltd.
// SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Weather 1.0

Column {
    property bool highlighted

    width: parent.width
    anchors.centerIn: parent

    Label {
        property bool truncate: implicitWidth > parent.width - Theme.paddingSmall

        x: truncate ? Theme.paddingSmall : parent.width/2 - width/2
        // Difficult layout due to limited horizontal space
        // Fade truncation overflows slightly to the adjacent delegate,
        // but should be ok since there is horizontal padding
        width: truncate ? parent.width : implicitWidth
        truncationMode: truncate ? TruncationMode.Fade : TruncationMode.None
        text: model.index === 0
              ? //% "Today"
                qsTrId("weather-la-today")
              : //% "ddd"
                Qt.formatDateTime(timestamp, qsTrId("weather-la-date_pattern_shortweekdays"))
        color: highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
        font.pixelSize: Theme.fontSizeSmall
    }
    Label {
        text: TemperatureConverter.format(model.high)
        anchors.horizontalCenter: parent.horizontalCenter
    }
    Image {
        property string prefix: "image://theme/icon-" + (Screen.sizeCategory >= Screen.Large ? "l" : "m")

        anchors.horizontalCenter: parent.horizontalCenter
        source: model.weatherType.length > 0 ? prefix + "-weather-" + model.weatherType
                                               + (highlighted ? "?" + Theme.highlightColor : "")
                                             : ""
    }
    Label {
        text: TemperatureConverter.format(model.low)
        anchors.horizontalCenter: parent.horizontalCenter
        color: highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
    }
}
