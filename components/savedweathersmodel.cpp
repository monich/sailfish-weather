// SPDX-FileCopyrightText: 2013 - 2023 Jolla Ltd.
// SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
//
// SPDX-License-Identifier: BSD-3-Clause

#include "savedweathersmodel.h"
#include <QDir>
#include <qqmlinfo.h>
#include <QStandardPaths>
#include <QFileSystemWatcher>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>

static QString weatherStoragePath()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
           + QStringLiteral("/org.sailfishos/weather/");
}

static QString normalizeProvider(const QString &provider)
{
    return provider.isEmpty() ? QStringLiteral("foreca") : provider;
}

static QString providerStorageFilePath(const QString &provider)
{
    return provider.isEmpty()
            ? QString()
            : weatherStoragePath() + provider + QStringLiteral(".json");
}

static QString locationProvider(const QVariantMap &locationMap)
{
    return normalizeProvider(locationMap.value(QStringLiteral("provider")).toString());
}

static QString locationKey(int locationId, const QString &provider)
{
    return normalizeProvider(provider) + QStringLiteral(":") + QString::number(locationId);
}

SavedWeathersModel::SavedWeathersModel(QObject *parent)
    : QAbstractListModel(parent), m_currentWeather(0), m_autoRefresh(false), m_fileWatcher(0)
{
    load();
}

SavedWeathersModel::~SavedWeathersModel()
{
}

void SavedWeathersModel::clearLoadedState()
{
    if (!m_savedWeathers.isEmpty()) {
        beginResetModel();
        qDeleteAll(m_savedWeathers);
        m_savedWeathers.clear();
        endResetModel();
        emit countChanged();
    }

    if (m_currentWeather) {
        m_currentWeather->deleteLater();
        m_currentWeather = nullptr;
        emit currentWeatherChanged();
    }
}

void SavedWeathersModel::load()
{
    const QString filePath = providerStorageFilePath(m_provider);
    if (filePath.isEmpty()) {
        clearLoadedState();
        return;
    }

    QFile file(filePath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        qmlInfo(this) << "Could not open weather data file!";
        if (!file.exists()) {
            clearLoadedState();
        }
        return;
    }

    QSet<QString> locationKeys;
    int oldCount = m_savedWeathers.count();

    QByteArray data = file.readAll();
    QJsonDocument json = QJsonDocument::fromJson(data);

    QJsonObject root = json.object();

    // update saved weather locations
    QJsonArray savedLocations = root.value("savedLocations").toArray();
    foreach (const QJsonValue &value, savedLocations) {
        QJsonObject location = value.toObject();
        int locationId = location["locationId"].toInt();

        locationKeys.insert(locationKey(locationId, m_provider));
        // add new weather locations
        if (getWeatherIndex(locationId, m_provider) < 0) {
            QVariantMap locationMap = location.toVariantMap();
            locationMap.insert(QStringLiteral("provider"), m_provider);
            addLocation(locationMap);
        }
        QVariantMap weatherMap = location.value("weather").toObject().toVariantMap();
        weatherMap.insert(QStringLiteral("provider"), m_provider);
        // update existing weather locations
        if (weatherMap.value("populated").toBool()) {
            update(locationId, weatherMap, Weather::Status(weatherMap["status"].toInt()),
                    true /* internal */);
        }
    }

    // update current weather location after saved entries are loaded so any
    // save triggered by currentWeatherChanged sees a complete provider state.
    bool currentWeatherLoaded = false;
    QJsonObject currentLocation = root.value("currentLocation").toObject();
    if (!currentLocation.empty()) {
        QVariantMap currentLocationMap = currentLocation.toVariantMap();
        currentLocationMap.insert(QStringLiteral("provider"), m_provider);
        setCurrentWeather(currentLocationMap, true /* internal */);
        QVariantMap weatherMap = currentLocation.value("weather").toObject().toVariantMap();
        if (weatherMap.value("populated").toBool()) {
            m_currentWeather->update(weatherMap);
        }
        m_currentWeather->setStatus(Weather::Status(weatherMap["status"].toInt()));
        currentWeatherLoaded = true;
    }

    if (!currentWeatherLoaded && m_currentWeather) {
        m_currentWeather->deleteLater();
        m_currentWeather = nullptr;
        emit currentWeatherChanged();
    }

    // remove old weather locations
    for (int i = 0; i < m_savedWeathers.count(); i++) {
        Weather *weather = m_savedWeathers[i];
        if (!locationKeys.contains(locationKey(weather->locationId(), weather->provider()))) {
            beginRemoveRows(QModelIndex(), i, i);
            m_savedWeathers.removeAt(i);
            i--;
            endRemoveRows();
        }
    }

    if (!m_currentWeather && !m_savedWeathers.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, 0);
        m_currentWeather = m_savedWeathers.takeFirst();
        endRemoveRows();
        emit currentWeatherChanged();
    }

    if (m_savedWeathers.count() != oldCount) {
        emit countChanged();
    }
}

void SavedWeathersModel::moveToTop(int index)
{
    if (index > 0 && index < count()) {
        beginMoveRows(QModelIndex(), index, index, QModelIndex(), 0);
        m_savedWeathers.move(index, 0);
        endMoveRows();
        save();
    }
}

void SavedWeathersModel::save()
{
    const QString filePath = providerStorageFilePath(m_provider);
    if (filePath.isEmpty()) {
        return;
    }

    QJsonArray savedLocations;
    foreach (Weather *weather, m_savedWeathers) {
        savedLocations.append(convertToJson(weather));
    }

    QJsonObject root;
    if (m_currentWeather) {
        root.insert("currentLocation", convertToJson(m_currentWeather));
    }
    root.insert("savedLocations", savedLocations);

    QJsonDocument json(root);

    QDir dir(weatherStoragePath());
    if (!dir.mkpath(QStringLiteral("."))) {
        qmlInfo(this) << "Could not create data directory!";
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qmlInfo(this) << "Could not open weather data file!";
        return;
    }

    if (file.write(json.toJson()) < 0) {
        qmlInfo(this) << "Could not write weather data: " << file.errorString();
        return;
    }
}

QJsonObject SavedWeathersModel::convertToJson(const Weather *weather)
{
    QJsonObject location;
    location["locationId"] = weather->locationId();
    location["provider"] = weather->provider();
    location["latitude"] = weather->latitude();
    location["longitude"] = weather->longitude();
    location["city"] = weather->city();
    location["state"] = weather->state();
    location["station"] = weather->station();
    location["country"] = weather->country();
    location["adminArea"] = weather->adminArea();
    location["adminArea2"] = weather->adminArea2();

    QJsonObject weatherData;
    weatherData["populated"] = weather->populated();
    weatherData["status"] = weather->status();
    weatherData["temperature"] = weather->temperature();
    weatherData["feelsLikeTemperature"] = weather->feelsLikeTemperature();
    weatherData["weatherType"] = weather->weatherType();
    weatherData["description"] = weather->description();
    weatherData["timestamp"] = weather->timestamp().toUTC().toString(Qt::ISODate);

    location["weather"] = weatherData;
    return location;
}

void SavedWeathersModel::addLocation(const QVariantMap &locationMap)
{
    int locationId = locationMap["locationId"].toInt();
    QString provider = locationProvider(locationMap);
    int i = getWeatherIndex(locationId, provider);
    if (i >= 0 || (m_currentWeather && m_currentWeather->locationId() == locationId
                   && m_currentWeather->provider() == provider)) {
        qmlInfo(this) << "Location already exists " << provider << locationId;
        return;
    }

    addLocation(new Weather(this, locationMap));
}

void SavedWeathersModel::addLocation(Weather *weather)
{
    beginInsertRows(QModelIndex(), m_savedWeathers.count(), m_savedWeathers.count());
    m_savedWeathers.append(weather);
    endInsertRows();
    emit countChanged();
}

void SavedWeathersModel::setCurrentWeather(const QVariantMap &map, bool internal)
{
    int locationId = map["locationId"].toInt();
    QString provider = locationProvider(map);
    if (!m_currentWeather || m_currentWeather->locationId() != locationId
            || m_currentWeather->provider() != provider
            // location API can return different place names, but the same weather station location id
            || m_currentWeather->city() != map["city"].toString()) {
        Weather *oldCurrentWeather = m_currentWeather;
        Weather *weather = new Weather(this, map);
        if (map.contains("populated")) {
            weather->update(map);
            weather->setStatus(Weather::Ready);
        }
        if (oldCurrentWeather) {
            bool sameStableLocation = oldCurrentWeather->locationId() == locationId
                    && oldCurrentWeather->provider() == provider;
            bool alreadySaved = getWeatherIndex(oldCurrentWeather->locationId(),
                                                oldCurrentWeather->provider()) >= 0;
            if (!internal && !sameStableLocation && !alreadySaved) {
                addLocation(oldCurrentWeather);
            } else {
                oldCurrentWeather->deleteLater();
            }
        }
        remove(locationId, provider);
        m_currentWeather = weather;
        emit currentWeatherChanged();
        if (!internal) {
            save();
        }
    }
}

void SavedWeathersModel::setErrorStatus(int locationId, int status, const QString &provider)
{
    QString normalizedProvider = provider.isEmpty() ? m_provider : normalizeProvider(provider);

    if (m_currentWeather && m_currentWeather->locationId() == locationId
            && m_currentWeather->provider() == normalizedProvider) {
        m_currentWeather->setStatus(static_cast<Weather::Status>(status));
    } else {
        int i = getWeatherIndex(locationId, normalizedProvider);
        if (i < 0) {
            qmlInfo(this) << "No location with id/provider exists" << locationId << normalizedProvider;
            return;
        }
        Weather *weather = m_savedWeathers[i];
        weather->setStatus(static_cast<Weather::Status>(status));
        dataChanged(index(i), index(i));
    }
}

void SavedWeathersModel::update(int locationId, const QVariantMap &weatherMap, Weather::Status status, bool internal)
{
    QString provider = locationProvider(weatherMap);
    bool updatedCurrent = false;
    if (m_currentWeather && locationId == m_currentWeather->locationId()
            && provider == m_currentWeather->provider()) {
        m_currentWeather->update(weatherMap);
        m_currentWeather->setStatus(status);
        updatedCurrent = true;
    }
    int i = getWeatherIndex(locationId, provider);
    if (i < 0) {
        if (!updatedCurrent) {
            qmlInfo(this) << "Location hasn't been saved" << provider << locationId;
        }

        if (!internal) {
            save();
        }
        return;
    }
    Weather *weather = m_savedWeathers[i];
    weather->update(weatherMap);
    weather->setStatus(status);
    dataChanged(index(i), index(i));
    if (!internal) {
        save();
    }
}

void SavedWeathersModel::remove(int locationId, const QString &provider)
{
    QString normalizedProvider = provider.isEmpty() ? m_provider : normalizeProvider(provider);
    int i = getWeatherIndex(locationId, normalizedProvider);
    if (i >= 0) {
        beginRemoveRows(QModelIndex(), i, i);
        m_savedWeathers.removeAt(i);
        endRemoveRows();
        emit countChanged();
    }
}

int SavedWeathersModel::count() const
{
    return rowCount();
}

Weather *SavedWeathersModel::currentWeather() const
{
    return m_currentWeather;
}

Weather *SavedWeathersModel::get(int locationId, const QString &provider)
{
    QString normalizedProvider = provider.isEmpty() ? m_provider : normalizeProvider(provider);
    int index = getWeatherIndex(locationId, normalizedProvider);
    if (index >= 0) {
        return m_savedWeathers.at(index);
    } else {
        qmlInfo(this) << "SavedWeathersModel::get(locationId, provider) - no location with id/provider"
                      << locationId << normalizedProvider << "stored";
        return 0;
    }
}


int SavedWeathersModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_savedWeathers.count();
}

QVariant SavedWeathersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_savedWeathers.count())
        return QVariant();

    const Weather *weather = m_savedWeathers.at(index.row());
    switch (role) {
    case LocationId:
        return weather->locationId();
    case Provider:
        return weather->provider();
    case Latitude:
        return weather->latitude();
    case Longitude:
        return weather->longitude();
    case Status:
        return weather->status();
    case Station:
        return weather->station();
    case City:
        return weather->city();
    case State:
        return weather->state();
    case AdminArea:
        return weather->adminArea();
    case AdminArea2:
        return weather->adminArea2();
    case Country:
        return weather->country();
    case Temperature:
        return weather->temperature();
    case FeelsLikeTemperature:
        return weather->feelsLikeTemperature();
    case WeatherType:
        return weather->weatherType();
    case Description:
        return weather->description();
    case Timestamp:
        return weather->timestamp();
    case Populated:
        return weather->populated();
    }

    return QVariant();
}

QHash<int, QByteArray> SavedWeathersModel::roleNames() const
{
    QHash<int,QByteArray> roles;
    roles.insert(LocationId, "locationId");
    roles.insert(Provider, "provider");
    roles.insert(Latitude, "latitude");
    roles.insert(Longitude, "longitude");
    roles.insert(Status, "status");
    roles.insert(Station, "station");
    roles.insert(City, "city");
    roles.insert(State, "state");
    // There roles are directly from Foreca API spec
    roles.insert(AdminArea, "adminArea");
    roles.insert(AdminArea2, "adminArea2");
    roles.insert(Country, "country");
    roles.insert(Temperature, "temperature");
    roles.insert(FeelsLikeTemperature, "feelsLikeTemperature");
    roles.insert(WeatherType, "weatherType");
    roles.insert(Description, "description");
    roles.insert(Timestamp, "timestamp");
    roles.insert(Populated, "populated");

    return roles;
}

int SavedWeathersModel::getWeatherIndex(int locationId, const QString &provider) const
{
    QString normalizedProvider = provider.isEmpty() ? m_provider : normalizeProvider(provider);
    for (int i = 0; i < m_savedWeathers.count(); i++) {
        if (m_savedWeathers[i]->locationId() == locationId
                && m_savedWeathers[i]->provider() == normalizedProvider) {
            return i;
        }
    }
    return -1;
}

bool SavedWeathersModel::autoRefresh() const
{
    return m_autoRefresh;
}

void SavedWeathersModel::setAutoRefresh(bool enabled)
{
    if (m_autoRefresh == enabled)
        return;

    m_autoRefresh = enabled;
    emit autoRefreshChanged();

    if (m_autoRefresh) {
        QString filePath = providerStorageFilePath(m_provider);
        if (filePath.isEmpty()) {
            return;
        }
        if (!QFile::exists(filePath)) {
            // QFileSystemWatcher needs the file to exist, so write out an
            // empty file
            save();
        }

        m_fileWatcher = new QFileSystemWatcher(this);
        connect(m_fileWatcher, &QFileSystemWatcher::fileChanged,
                this, &SavedWeathersModel::load);
        m_fileWatcher->addPath(filePath);
    } else {
        delete m_fileWatcher;
        m_fileWatcher = 0;
    }
}

QString SavedWeathersModel::provider() const
{
    return m_provider;
}

void SavedWeathersModel::setProvider(const QString &provider)
{
    if (m_provider == provider) {
        return;
    }

    if (m_fileWatcher) {
        delete m_fileWatcher;
        m_fileWatcher = 0;
    }

    clearLoadedState();

    m_provider = provider;
    emit providerChanged();
    load();

    if (m_autoRefresh) {
        QString filePath = providerStorageFilePath(m_provider);
        if (filePath.isEmpty()) {
            return;
        }
        if (!QFile::exists(filePath)) {
            save();
        }

        m_fileWatcher = new QFileSystemWatcher(this);
        connect(m_fileWatcher, &QFileSystemWatcher::fileChanged,
                this, &SavedWeathersModel::load);
        m_fileWatcher->addPath(filePath);
    }
}
