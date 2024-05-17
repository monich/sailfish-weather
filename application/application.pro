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
    oneshot/sailfish-weather-move-data-to-new-location

oneshot.files = oneshot/sailfish-weather-move-data-to-new-location
oneshot.path  = /usr/lib/oneshot.d

INSTALLS += dbus_service oneshot
