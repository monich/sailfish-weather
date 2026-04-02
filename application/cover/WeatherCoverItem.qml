// SPDX-FileCopyrightText: 2014 - 2023 Jolla Ltd.
// SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

import QtQuick 2.6
import Sailfish.Silica 1.0

Column {
    property alias text: primaryLabel.text
    property alias description: secondaryLabel.text

    Label {
        id: primaryLabel

        width: parent.width
        truncationMode: TruncationMode.Fade
    }
    Label {
        id: secondaryLabel

        width: parent.width
        truncationMode: TruncationMode.Fade
        font.pixelSize: Theme.fontSizeExtraSmall
        color: Theme.secondaryColor
    }
}
