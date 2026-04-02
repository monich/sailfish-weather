# SPDX-FileCopyrightText: 2013 - 2023 Jolla Ltd.
# SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
#
# SPDX-License-Identifier: BSD-3-Clause

Name:       sailfish-weather
Summary:    Weather application
Version:    1.3.2
Release:    1
License:    BSD-3-Clause
URL:        https://github.com/sailfishos/sailfish-weather
Source0:    %{name}-%{version}.tar.bz2
Source1:    %{name}.privileges
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(qdeclarative5-boostable)
BuildRequires:  qt5-qttools
BuildRequires:  qt5-qttools-linguist
BuildRequires:  oneshot

BuildRequires: %{name}-all-translations
%define _all_translations_version %(rpm -q --queryformat "%%{version}-%%{release}" %{name}-all-translations)
Requires: %{name}-all-translations >= %{_all_translations_version}

Requires: sailfishsilica-qt5
Requires: mapplauncherd-booster-silica-qt5
Requires: sailfish-content-graphics
Requires: sailfish-content-graphics-closed
Requires: qt5-qtpositioning
Requires: qt5-qtdeclarative-import-xmllistmodel
Requires: qt5-qtdeclarative-import-positioning
Requires: libkeepalive
Requires: nemo-qml-plugin-systemsettings
Requires: nemo-qml-plugin-connectivity >= 0.2.24
Requires: jolla-settings-accounts
Requires: sailfish-components-weather-qt5 == %{version}
Requires: lipstick-jolla-home-qt5-weather-widget-settings
%{_oneshot_requires_post}

%description
Sailfish-style Weather application

%package ts-devel
Summary: Translation source for %{name}

%description ts-devel
Translation source for %{name}

%package -n sailfish-components-weather-qt5
Summary: Sailfish weather UI components

%description -n sailfish-components-weather-qt5
Sailfish weather UI components

%package -n sailfish-weather-backend-foreca
Summary: Foreca backend for Sailfish Weather
URL: https://www.foreca.com/
Requires: sailfish-components-weather-qt5 == %{version}

%description -n sailfish-weather-backend-foreca
Foreca backend for Sailfish Weather.

%package -n sailfish-weather-backend-openweather
Summary: OpenWeather backend for Sailfish Weather
URL: https://openweathermap.org/
Requires: sailfish-components-weather-qt5 == %{version}

%description -n sailfish-weather-backend-openweather
OpenWeather backend for Sailfish Weather.

%package -n sailfish-weather-backend-metnorway
Summary: MET Norway backend for Sailfish Weather
URL: https://www.yr.no/
Requires: sailfish-components-weather-qt5 == %{version}

%description -n sailfish-weather-backend-metnorway
MET Norway backend for Sailfish Weather.

%package -n sailfish-weather-backend-openmeteo
Summary: Open-Meteo backend for Sailfish Weather
URL: https://open-meteo.com/
Requires: sailfish-components-weather-qt5 == %{version}

%description -n sailfish-weather-backend-openmeteo
Open-Meteo backend for Sailfish Weather.

%prep
%setup -q -n %{name}-%{version}

%build
%qmake5
%make_build

%install

%qmake5_install
chmod +x %{buildroot}/%{_oneshotdir}/*

mkdir -p %{buildroot}%{_datadir}/mapplauncherd/privileges.d
install -m 644 -p %{SOURCE1} %{buildroot}%{_datadir}/mapplauncherd/privileges.d/

%post
if [ $1 -eq 2 ]; then
    add-oneshot --all-users sailfish-weather-remove-obsolete-data || :
fi

%files
%license LICENSES/BSD-3-Clause.txt
%{_datadir}/applications/*.desktop
%{_datadir}/sailfish-weather/weather.qml
%{_datadir}/sailfish-weather/cover
%{_datadir}/sailfish-weather/model
%{_datadir}/sailfish-weather/pages
%{_bindir}/sailfish-weather
%{_datadir}/translations/weather_eng_en.qm
%{_datadir}/jolla-settings/entries/sailfish-weather.json
%{_datadir}/jolla-settings/pages/sailfish-weather
%{_datadir}/dbus-1/services/org.sailfishos.weather.service
%{_datadir}/mapplauncherd/privileges.d/*
%{_libdir}/qt5/qml/org/sailfishos/weather/settings
%{_oneshotdir}/sailfish-weather-remove-obsolete-data

%files -n sailfish-components-weather-qt5
%dir %{_libdir}/qt5/qml/Sailfish/Weather
%{_libdir}/qt5/qml/Sailfish/Weather/*
%dir %{_datadir}/sailfish-weather/backends
%{_datadir}/sailfish-weather/backends/BackendUtils.js
%{_datadir}/sailfish-weather/backends/WeatherTypeDescriptions.js
%{_datadir}/translations/sailfish_components_weather_qt5_eng_en.qm

%files -n sailfish-weather-backend-foreca
%{_datadir}/sailfish-weather/backends/ForecaWeatherBackend.qml
%{_datadir}/sailfish-weather/backends/ForecaToken.js

%files -n sailfish-weather-backend-openweather
%{_datadir}/sailfish-weather/backends/OpenWeatherBackend.qml
%{_datadir}/themes/sailfish-default/silica/icons-monochrome/open-weather.png
%{_datadir}/themes/sailfish-default/silica/icons-monochrome/open-weather-small.png

%files -n sailfish-weather-backend-metnorway
%{_datadir}/sailfish-weather/backends/MetNorwayBackend.qml
%{_datadir}/themes/sailfish-default/silica/icons-monochrome/met-norway.png
%{_datadir}/themes/sailfish-default/silica/icons-monochrome/met-norway-small.png

%files -n sailfish-weather-backend-openmeteo
%{_datadir}/sailfish-weather/backends/OpenMeteoBackend.qml
%{_datadir}/themes/sailfish-default/silica/icons-monochrome/open-meteo.png
%{_datadir}/themes/sailfish-default/silica/icons-monochrome/open-meteo-small.png

%files ts-devel
%{_datadir}/translations/source/weather.ts
%{_datadir}/translations/source/sailfish_components_weather_qt5.ts
