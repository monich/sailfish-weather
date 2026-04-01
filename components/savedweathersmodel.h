/*
 * SPDX-FileCopyrightText: 2013 - 2023 Jolla Ltd.
 * SPDX-FileCopyrightText: 2024 - 2025 Jolla Mobile Ltd
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SAVEDWEATHERSMODEL_H
#define SAVEDWEATHERSMODEL_H

#include <QAbstractListModel>

#include "weather.h"

class QFileSystemWatcher;

class SavedWeathersModel: public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(Weather *currentWeather READ currentWeather NOTIFY currentWeatherChanged)
    Q_PROPERTY(bool autoRefresh READ autoRefresh WRITE setAutoRefresh NOTIFY autoRefreshChanged)
    Q_PROPERTY(QString provider READ provider WRITE setProvider NOTIFY providerChanged)

public:
    enum Roles {
        LocationId = Qt::UserRole,
        Provider,
        Latitude,
        Longitude,
        Status,
        Station,
        City,
        AdminArea,
        AdminArea2,
        State,
        Country,
        Temperature,
        FeelsLikeTemperature,
        WeatherType,
        Description,
        Timestamp,
        Populated
    };

    SavedWeathersModel(QObject *parent = 0);
    ~SavedWeathersModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void addLocation(const QVariantMap &locationMap);
    Q_INVOKABLE void moveToTop(int index);
    Q_INVOKABLE void save();
    Q_INVOKABLE void setCurrentWeather(const QVariantMap &locationMap);
    Q_INVOKABLE void setErrorStatus(int locationId, int status, const QString &provider = QString());
    Q_INVOKABLE void update(int locationId, const QVariantMap &weatherMap,
                            Weather::Status status = Weather::Ready);
    Q_INVOKABLE void remove(int locationId, const QString &provider = QString());
    Q_INVOKABLE Weather *get(int locationId, const QString &provider = QString());

    int count() const;

    Weather *currentWeather() const;

    // Automatically reload cached data when it is changed by another model
    // Default false
    bool autoRefresh() const;
    void setAutoRefresh(bool enabled);
    QString provider() const;
    void setProvider(const QString &provider);

    void addLocation(Weather * weather);
    void load();

signals:
    void countChanged();
    void currentWeatherChanged();
    void autoRefreshChanged();
    void providerChanged();

protected:
    QHash<int, QByteArray> roleNames() const;
    QJsonObject convertToJson(const Weather *weather);
private:
    Weather *m_currentWeather;
    QList <Weather *> m_savedWeathers;
    bool m_autoRefresh;
    QString m_provider;
    QFileSystemWatcher *m_fileWatcher;

    int getWeatherIndex(int locationId, const QString &provider = QString()) const;
    void clearLoadedState();
    void setCurrentWeather(const QVariantMap &locationMap, bool internal);
    void update(int locationId, const QVariantMap &weatherMap, Weather::Status status, bool internal);
};

QML_DECLARE_TYPE(SavedWeathersModel)

#endif // SAVEDWEATHERSMODEL_H
