// SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

#include "backendregistry.h"

#include <QDir>
#include <QFileInfo>
#include <QSet>
#include <QStandardPaths>
#include <QUrl>

BackendRegistry::BackendRegistry(QObject *parent)
    : QObject(parent)
{
    const QString backendSubdirectory = QStringLiteral("sailfish-weather/backends");
    const QStringList dataLocations = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    QSet<QString> seenBackendFiles;

    for (const QString &dataLocation : dataLocations) {
        const QDir backendDir(QDir(dataLocation).filePath(backendSubdirectory));
        if (!backendDir.exists()) {
            continue;
        }

        const QFileInfoList backendEntries = backendDir.entryInfoList(QStringList() << "*Backend.qml",
                                                                      QDir::Files,
                                                                      QDir::Name | QDir::IgnoreCase);
        for (const QFileInfo &backendEntry : backendEntries) {
            if (seenBackendFiles.contains(backendEntry.fileName())) {
                continue;
            }

            seenBackendFiles.insert(backendEntry.fileName());
            m_backendFiles.append(QUrl::fromLocalFile(backendEntry.absoluteFilePath()).toString());
        }
    }
}

QStringList BackendRegistry::backendFiles() const
{
    return m_backendFiles;
}
