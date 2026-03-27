# SPDX-FileCopyrightText: 2013 - 2023 Jolla Ltd.
# SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
#
# SPDX-License-Identifier: BSD-3-Clause

TARGET = sailfish-weather

CONFIG += warn_on

SOURCES += weather.cpp

qml.files = weather.qml cover model pages
desktop.files = sailfish-weather.desktop

dbus_service.files = org.sailfishos.weather.service
dbus_service.path = /usr/share/dbus-1/services

include(sailfishapplication/sailfishapplication.pri)
include(translations/translations.pri)

OTHER_FILES = \
    org.sailfishos.weather.service \
    oneshot/sailfish-weather-remove-obsolete-data

oneshot.files = oneshot/sailfish-weather-remove-obsolete-data
oneshot.path  = /usr/lib/oneshot.d

images.files = images/*
images.path =  /usr/share/themes/sailfish-default/silica/icons-monochrome/

INSTALLS += dbus_service oneshot images
