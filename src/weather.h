#ifndef WEATHER_H
#define WEATHER_H

#include <QObject>
#include <QDebug>
#include <QDateTime>
#include <qqml.h>

class Weather : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(int locationId READ locationId CONSTANT)
    Q_PROPERTY(QString city READ city CONSTANT)
    Q_PROPERTY(QString state READ state CONSTANT)
    Q_PROPERTY(QString country READ country CONSTANT)
    Q_PROPERTY(QString adminArea READ adminArea CONSTANT)
    Q_PROPERTY(QString adminArea2 READ adminArea2 CONSTANT)
    Q_PROPERTY(int temperature READ temperature NOTIFY temperatureChanged)
    Q_PROPERTY(int feelsLikeTemperature READ feelsLikeTemperature NOTIFY feelsLikeTemperatureChanged)
    Q_PROPERTY(QString weatherType READ weatherType NOTIFY weatherTypeChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(QDateTime timestamp READ timestamp NOTIFY timestampChanged)
    Q_PROPERTY(QString station READ station NOTIFY stationChanged)
    Q_PROPERTY(bool populated READ populated NOTIFY populatedChanged)

public:
    Weather(QObject *parent, const QVariantMap &locationMap)
        : QObject(parent),
          m_status(Loading),
          m_locationId(locationMap["locationId"].toInt()),
          m_city(locationMap["city"].toString()),
          m_state(locationMap["state"].toString()),
          m_country(locationMap["country"].toString()),
          m_adminArea(locationMap["adminArea"].toString()),
          m_adminArea2(locationMap["adminArea2"].toString()),
          m_station(locationMap["station"].toString()),
          m_temperature(0),
          m_feelsLikeTemperature(0),
          m_populated(false)
    {
    }

    ~Weather() {}
    enum Status { Null, Ready, Loading, Error, Unauthorized };

    int locationId() const { return m_locationId; }
    Status status() const { return m_status; }
    QString city() const { return m_city; }
    QString state() const { return m_state; }
    QString country() const { return m_country; }
    QString adminArea() const { return m_adminArea; }
    QString adminArea2() const { return m_adminArea2; }
    int temperature() const { return m_temperature; }
    int feelsLikeTemperature() const { return m_feelsLikeTemperature; }
    QString weatherType() const { return m_weatherType; }
    QString description() const { return m_description; }
    QDateTime timestamp() const { return m_timestamp; }
    QString station() const { return m_station; }
    bool populated() const { return m_populated; }

    void setStatus(Status status) {
        if (m_status != status) {
            m_status = status;
            emit statusChanged();
        }
    }
    void setTemperature(int temperature) {
        if (m_temperature != temperature) {
            m_temperature = temperature;
            emit temperatureChanged();
        }
    }
    void setfeelsLikeTemperature(int feelsLikeTemperature) {
        if (m_feelsLikeTemperature != feelsLikeTemperature) {
            m_feelsLikeTemperature = feelsLikeTemperature;
            emit feelsLikeTemperatureChanged();
        }
    }
    void setWeatherType(QString weatherType) {
        if (m_weatherType != weatherType) {
            m_weatherType = weatherType;
            emit weatherTypeChanged();
        }
    }
    void setDescription(QString description) {
        if (m_description != description) {
            m_description = description;
            emit descriptionChanged();
        }
    }
    void setTimestamp(QDateTime timestamp) {
        if (m_timestamp != timestamp) {
            m_timestamp = timestamp;
            emit timestampChanged();
        }
    }
    void setStation(QString station) {
        if (m_station != station) {
            m_station = station;
            emit stationChanged();
        }
    }

    Q_INVOKABLE void update(const QVariantMap &weatherMap) {
        setTemperature(weatherMap["temperature"].toInt());
        setfeelsLikeTemperature(weatherMap["feelsLikeTemperature"].toInt());
        setWeatherType(weatherMap["weatherType"].toString());
        setDescription(weatherMap["description"].toString());
        setTimestamp(weatherMap["timestamp"].toDateTime());
        setStation(weatherMap["station"].toString());
        if (!m_populated) {
            m_populated = true;
            emit populatedChanged();
        }
    }

signals:
    void statusChanged();
    void temperatureChanged();
    void feelsLikeTemperatureChanged();
    void weatherTypeChanged();
    void descriptionChanged();
    void timestampChanged();
    void stationChanged();
    void populatedChanged();

private:
    Status m_status;
    int m_locationId;
    QString m_city;
    QString m_state;
    QString m_country;
    QString m_adminArea;
    QString m_adminArea2;
    QString m_station;
    int m_temperature;
    int m_feelsLikeTemperature;
    QString m_weatherType;
    QString m_description;
    QDateTime m_timestamp;
    bool m_populated;
};

QML_DECLARE_TYPE(Weather)

#endif // WEATHER_H
