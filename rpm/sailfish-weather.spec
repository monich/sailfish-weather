Name:       sailfish-weather
Summary:    Weather application
Version:    1.0.3
Release:    1
License:    Proprietary
URL:        https://bitbucket.org/jolla/ui-sailfish-weather
Source0:    %{name}-%{version}.tar.bz2
Source1:    %{name}.privileges
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  desktop-file-utils
BuildRequires:  pkgconfig(qdeclarative5-boostable)
BuildRequires:  qt5-qttools
BuildRequires:  qt5-qttools-linguist
BuildRequires:  oneshot

BuildRequires: %{name}-all-translations
%define _all_translations_version %(rpm -q --queryformat "%%{version}-%%{release}" %{name}-all-translations)
Requires: %{name}-all-translations >= %{_all_translations_version}

Requires:  sailfishsilica-qt5 >= 0.27.0
Requires:  sailfish-components-weather-qt5 >= 1.0.9
Requires:  mapplauncherd-booster-silica-qt5
Requires:  connman-qt5-declarative
%{_oneshot_requires_post}

%description
Sailfish-style Weather application

%package ts-devel
Summary: Translation source for %{name}

%description ts-devel
Translation source for %{name}

%prep
%setup -q -n %{name}-%{version}

%build
%qmake5 sailfish-weather.pro
make %{_smp_mflags}

%install
rm -rf %{buildroot}

%qmake5_install
chmod +x %{buildroot}/%{_oneshotdir}/*

desktop-file-install --delete-original       \
  --dir %{buildroot}%{_datadir}/applications             \
   %{buildroot}%{_datadir}/applications/*.desktop

mkdir -p %{buildroot}%{_datadir}/mapplauncherd/privileges.d
install -m 644 -p %{SOURCE1} %{buildroot}%{_datadir}/mapplauncherd/privileges.d/

%files
%defattr(-,root,root,-)
%{_datadir}/applications/*.desktop
%{_datadir}/sailfish-weather/*
%{_bindir}/sailfish-weather
%{_datadir}/translations/weather_eng_en.qm
%{_datadir}/jolla-settings/entries/sailfish-weather.json
%{_datadir}/jolla-settings/pages/sailfish-weather
%{_datadir}/dbus-1/services/org.sailfishos.weather.service
%{_datadir}/mapplauncherd/privileges.d/*
%{_libdir}/qt5/qml/org/sailfishos/weather/settings
%{_oneshotdir}/sailfish-weather-move-data-to-new-location

%files ts-devel
%defattr(-,root,root,-)
%{_datadir}/translations/source/weather.ts

%post
if [ $1 -eq 2 ]; then
    add-oneshot --all-users sailfish-weather-move-data-to-new-location || :
fi
