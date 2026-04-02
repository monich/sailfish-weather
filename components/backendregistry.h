// SPDX-FileCopyrightText: 2026 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

#ifndef BACKENDREGISTRY_H
#define BACKENDREGISTRY_H

#include <QObject>
#include <QStringList>

class BackendRegistry : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList backendFiles READ backendFiles CONSTANT)

public:
    explicit BackendRegistry(QObject *parent = nullptr);

    QStringList backendFiles() const;

private:
    QStringList m_backendFiles;
};

#endif // BACKENDREGISTRY_H
