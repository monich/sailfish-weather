// SPDX-FileCopyrightText: 2014 - 2023 Jolla Ltd.
// SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.Weather 1.0

BackgroundItem {
    id: root

    property var weather
    property int topMargin: Theme.paddingLarge
    property int bottomMargin: 2*Theme.paddingLarge
    readonly property bool hasProvider: WeatherProvider.currentProvider().length > 0

    visible: hasProvider
    height: hasProvider ? column.height + topMargin + bottomMargin : 0

    onClicked: {
        if (WeatherProvider.externalUrl(weather).trim().length > 0)
            Qt.openUrlExternally(WeatherProvider.externalUrl(weather))
    }

    Column {
        id: column

        width: parent.width
        spacing: Theme.paddingSmall

        Label {
            //% "Powered by"
            text: qsTrId("weather-la-powered_by")
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: Theme.fontSizeTiny
            color: highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
        }
        Image {
            anchors.horizontalCenter: parent.horizontalCenter
            source: WeatherProvider.providerImage() + (highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor)
        }
        Label {
            visible: text.length > 0
            width: parent.width - 2 * Theme.horizontalPageMargin
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: Theme.fontSizeTiny
            color: highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
            text: WeatherProvider.attributionText()
            textFormat: Text.StyledText
            linkColor: highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter

            onLinkActivated: Qt.openUrlExternally(link)
        }
        anchors {
            bottom: parent.bottom
            bottomMargin: root.bottomMargin
        }
    }
}
