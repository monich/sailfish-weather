# SPDX-FileCopyrightText: 2013 - 2023 Jolla Ltd.
# SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
#
# SPDX-License-Identifier: BSD-3-Clause

TEMPLATE = lib
TARGET  = sailfishweatherplugin
TARGET = $$qtLibraryTarget($$TARGET)

MODULENAME = Sailfish/Weather
TARGETPATH = $$[QT_INSTALL_QML]/$$MODULENAME

QT += qml
CONFIG += plugin link_pkgconfig

# C++ sources
SOURCES += plugin.cpp \
    backendregistry.cpp \
    savedweathersmodel.cpp

# C++ headers
HEADERS += weather.h \
           backendregistry.h \
           savedweathersmodel.h \

import.files = *.qml *.js qmldir
import.path = $$TARGETPATH
backend.files = $$PWD/../backends/*.qml $$PWD/../backends/*.js
backend.path = /usr/share/sailfish-weather/backends
target.path = $$TARGETPATH

OTHER_FILES += *.qml *.js $$PWD/../backends/*.qml $$PWD/../backends/*.js

TS_FILE = $$OUT_PWD/sailfish_components_weather_qt5.ts
EE_QM = $$OUT_PWD/sailfish_components_weather_qt5_eng_en.qm

translations.commands += lupdate $$PWD $$PWD/../backends -ts $$TS_FILE
translations.depends = $$PWD/*.qml $$PWD/../backends/*.qml $$PWD/../backends/*.js
translations.CONFIG += no_check_exist no_link
translations.output = $$TS_FILE
translations.input = .

translations_install.files = $$TS_FILE
translations_install.path = /usr/share/translations/source
translations_install.CONFIG += no_check_exist

# should add -markuntranslated "-" when proper translations are in place (or for testing)
engineering_english.commands += lrelease -idbased $$TS_FILE -qm $$EE_QM
engineering_english.CONFIG += no_check_exist no_link
engineering_english.depends = translations
engineering_english.input = $$TS_FILE
engineering_english.output = $$EE_QM

engineering_english_install.path = /usr/share/translations
engineering_english_install.files = $$EE_QM
engineering_english_install.CONFIG += no_check_exist

QMAKE_EXTRA_TARGETS += translations engineering_english

PRE_TARGETDEPS += translations engineering_english

INSTALLS += target import backend translations_install engineering_english_install
