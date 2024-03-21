#include "savedweathersmodel.h"
#include <QDir>
#include <qqmlinfo.h>
#include <QStandardPaths>
#include <QFileSystemWatcher>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

static QString weatherStoragePath()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
           + QStringLiteral("/org.sailfishos/weather/");
}

SavedWeathersModel::SavedWeathersModel(QObject *parent)
    : QAbstractListModel(parent), m_currentWeather(0), m_autoRefresh(false),
      m_fileWatcher(0)
{
    load();
}

SavedWeathersModel::~SavedWeathersModel()
{
}

void SavedWeathersModel::load()
{
    QFile file(weatherStoragePath() + QStringLiteral("/weather.json"));
    if (!file.exists())
        return;

    if (!file.open(QIODevice::ReadOnly)) {
        qmlInfo(this) << "Could not open weather data file!";
        return;
    }

    QList <int> locationIds;
    int oldCount = m_savedWeathers.count();

    QByteArray data = file.readAll();
    QJsonDocument json = QJsonDocument::fromJson(data);

    QJsonObject root = json.object();

    // update current weather locations
    QJsonObject currentLocation = root.value("currentLocation").toObject();
    if (!currentLocation.empty()) {
        setCurrentWeather(currentLocation.toVariantMap(), true /* internal */);
        QVariantMap weatherMap = currentLocation.value("weather").toObject().toVariantMap();
        if (weatherMap.value("populated").toBool()) {
            m_currentWeather->update(weatherMap);
        }
        m_currentWeather->setStatus(Weather::Status(weatherMap["status"].toInt()));
    }

    // update saved weather locations
    QJsonArray savedLocations = root.value("savedLocations").toArray();
    foreach (const QJsonValue &value, savedLocations) {
        QJsonObject location = value.toObject();
        int locationId = location["locationId"].toInt();

        locationIds.append(locationId);
        // add new weather locations
        if (getWeatherIndex(locationId) < 0) {
            addLocation(location.toVariantMap());
        }
        QVariantMap weatherMap = location.value("weather").toObject().toVariantMap();
        // update existing weather locations
        if (weatherMap.value("populated").toBool()) {
            update(locationId, weatherMap, Weather::Status(weatherMap["status"].toInt()),
                    true /* internal */);
        }
    }

    // remove old weather locations
    for (int i = 0; i < m_savedWeathers.count(); i++) {
        Weather *weather = m_savedWeathers[i];
        if (!locationIds.contains(weather->locationId())) {
            beginRemoveRows(QModelIndex(), i, i);
            m_savedWeathers.removeAt(i);
            i--;
            endRemoveRows();
        }
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

    QFile file(dir.filePath(QStringLiteral("weather.json")));
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
    int i = getWeatherIndex(locationId);
    if (i >= 0 || (m_currentWeather && m_currentWeather->locationId() == locationId)) {
        qmlInfo(this) << "Location already exists " << locationId;
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
    if (!m_currentWeather || m_currentWeather->locationId() != locationId
            // location API can return different place names, but the same weather station location id
            || m_currentWeather->city() != map["city"].toString()) {
        Weather *weather = new Weather(this, map);
        if (map.contains("populated")) {
            weather->update(map);
            weather->setStatus(Weather::Ready);
        }
        if (m_currentWeather) {
            addLocation(m_currentWeather);
        }
        remove(locationId);
        m_currentWeather = weather;
        emit currentWeatherChanged();
        if (!internal) {
            save();
        }
    }
}

void SavedWeathersModel::setErrorStatus(int locationId, int status)
{
    if (m_currentWeather && m_currentWeather->locationId() == locationId) {
        m_currentWeather->setStatus(static_cast<Weather::Status>(status));
    } else {
        int i = getWeatherIndex(locationId);
        if (i < 0) {
            qmlInfo(this) << "No location with id " << locationId << " exists";
            return;
        }
        Weather *weather = m_savedWeathers[i];
        weather->setStatus(static_cast<Weather::Status>(status));
        dataChanged(index(i), index(i));
    }
}

void SavedWeathersModel::update(int locationId, const QVariantMap &weatherMap, Weather::Status status, bool internal)
{
    bool updatedCurrent = false;
    if (m_currentWeather && locationId == m_currentWeather->locationId()) {
        m_currentWeather->update(weatherMap);
        m_currentWeather->setStatus(status);
        updatedCurrent = true;
    }
    int i = getWeatherIndex(locationId);
    if (i < 0) {
        if (!updatedCurrent) {
            qmlInfo(this) << "Location hasn't been saved " << locationId;
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

void SavedWeathersModel::remove(int locationId)
{
    int i = getWeatherIndex(locationId);
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

Weather *SavedWeathersModel::get(int locationId)
{
    int index = getWeatherIndex(locationId);
    if (index >= 0) {
        return m_savedWeathers.at(index);
    } else {
        qmlInfo(this) << "SavedWeathersModel::get(locationId) - no location with id " << locationId << " stored";
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

int SavedWeathersModel::getWeatherIndex(int locationId)
{
    for (int i = 0; i < m_savedWeathers.count(); i++) {
        if (m_savedWeathers[i]->locationId() == locationId) {
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
        QString filePath = weatherStoragePath() + QStringLiteral("weather.json");
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
