TEMPLATE = lib
TARGET  = weathersettingsplugin
TARGET = $$qtLibraryTarget($$TARGET)

MODULENAME = org/sailfishos/weather/settings
TARGETPATH = $$[QT_INSTALL_QML]/$$MODULENAME

QT += qml
CONFIG += plugin

import.files = qmldir
import.path = $$TARGETPATH
target.path = $$TARGETPATH

settings_entries.files = sailfish-weather.json
settings_entries.path = /usr/share/jolla-settings/entries

settings_qml.files = *.qml
settings_qml.path = /usr/share/jolla-settings/pages/sailfish-weather

OTHER_FILES += \
    qmldir \
    *.qml

SOURCES += plugin.cpp

INSTALLS += target import settings_entries settings_qml
