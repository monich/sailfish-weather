Name:       sailfish-components-weather-qt5
Summary:    Sailfish weather UI components
Version:    1.0.7
Release:    1
License:    Proprietary
URL:        https://bitbucket.org/jolla/ui-sailfish-weather
Source0:    %{name}-%{version}.tar.bz2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  qt5-qttools
BuildRequires:  qt5-qttools-linguist
BuildRequires:  pkgconfig(contentaction5)

Requires: sailfishsilica-qt5 >= 1.1.110
Requires: sailfish-content-graphics >= 1.0.42
Requires: ambient-icons-closed >= 0.5.8
Requires: qt5-qtpositioning
Requires: nemo-qml-plugin-systemsettings >= 0.2.26
Requires: sailfish-weather
Requires: qt5-qtdeclarative-import-xmllistmodel
Requires: qt5-qtdeclarative-import-positioning
Requires: libkeepalive >= 1.7.0
Requires: nemo-qml-plugin-connectivity >= 0.1.0
Requires: jolla-settings-accounts

BuildRequires: %{name}-all-translations
%define _all_translations_version %(rpm -q --queryformat "%%{version}-%%{release}" %{name}-all-translations)
Requires: %{name}-all-translations >= %{_all_translations_version}

%description
Sailfish weather UI components

%package ts-devel
Summary:   Translation source for sailfish-weather

%description ts-devel
Translation source for sailfish-weather

%prep
%setup -q -n %{name}-%{version}

%build

%qmake5

make %{_smp_mflags}

%install
rm -rf %{buildroot}

%qmake5_install

%files
%defattr(-,root,root,-)
%dir %{_libdir}/qt5/qml/Sailfish/Weather
%{_libdir}/qt5/qml/Sailfish/Weather/*
%{_datadir}/translations/sailfish_components_weather_qt5_eng_en.qm

%files ts-devel
%defattr(-,root,root,-)
%{_datadir}/translations/source/sailfish_components_weather_qt5.ts
