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

public:
    enum Roles {
        LocationId = Qt::UserRole,
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

    Q_INVOKABLE void setErrorStatus(int locationId, int status);
    Q_INVOKABLE void addLocation(const QVariantMap &locationMap);
    Q_INVOKABLE void update(int locationId, const QVariantMap &weatherMap, Weather::Status status = Weather::Ready, bool internal = false);
    Q_INVOKABLE void remove(int locationId);
    Q_INVOKABLE Weather *get(int locationId);
    Q_INVOKABLE void moveToTop(int index);
    Q_INVOKABLE void save();

    int count() const;

    Weather *currentWeather() const;
    Q_INVOKABLE void setCurrentWeather(const QVariantMap &locationMap, bool internal = false);

    // Automatically reload cached data when it is changed by another model
    // Default false
    bool autoRefresh() const;
    void setAutoRefresh(bool enabled);

    void addLocation(Weather * weather);
    void load();

signals:
    void countChanged();
    void currentWeatherChanged();
    void autoRefreshChanged();

protected:
    QHash<int, QByteArray> roleNames() const;
    QJsonObject convertToJson(const Weather *weather);
private:
    Weather *m_currentWeather;
    QList <Weather *> m_savedWeathers;
    bool m_autoRefresh;
    QFileSystemWatcher *m_fileWatcher;

    int getWeatherIndex(int locationId);
};

QML_DECLARE_TYPE(SavedWeathersModel)

#endif // SAVEDWEATHERSMODEL_H
