// SPDX-FileCopyrightText: 2018 - 2023 Jolla Ltd.
// SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

import QtQuick 2.0
import Sailfish.Silica 1.0

DetailItem {
    readonly property bool onLeft: Positioner.index % 2 == 0

    width: parent.width / parent.columns
    leftMargin: onLeft || isPortrait ? Theme.horizontalPageMargin : Theme.paddingMedium
    rightMargin: !onLeft || isPortrait ? Theme.horizontalPageMargin : Theme.paddingMedium
}
