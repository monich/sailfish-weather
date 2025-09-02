/*
 * SPDX-FileCopyrightText: 2013 - 2023 Jolla Ltd.
 * SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SAILFISHAPPLICATION_H
#define SAILFISHAPPLICATION_H

class QString;
class QGuiApplication;
class QQuickView;

namespace Sailfish {

QGuiApplication *createApplication(int &argc, char **argv);
QQuickView *createView(const QString &);
void showView(QQuickView* view);

}

#endif // SAILFISHAPPLICATION_H

