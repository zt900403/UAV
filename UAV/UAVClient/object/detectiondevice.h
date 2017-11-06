#ifndef DETECTIONDEVICE_H
#define DETECTIONDEVICE_H
#include <QString>

#include <QDataStream>
class DetectionDevice
{
public:
    DetectionDevice(const QString &name = QString(),
                    const QString &resolution = QString(),
                    const QString &workingAltitude = QString(),
                    float weight = 0.0f,
                    const QString &focalDistance = QString(),
                    const QString &volume = QString(),
                    const QString &power = QString());

    QString name() const;
    void setName(const QString &name);

    QString resolution() const;
    void setResolution(const QString &resolution);

    QString workingAltitude() const;
    void setWorkingAltitude(const QString &workingAltitude);

    float weight() const;
    void setWeight(float weight);

    QString focalDistance() const;
    void setFocalDistance(const QString &focalDistance);

    QString volume() const;
    void setVolume(const QString &volume);

    QString power() const;
    void setPower(const QString &power);

private:
    QString m_name;
    QString m_resolution;
    QString m_workingAltitude;
    float m_weight;
    QString m_focalDistance;
    QString m_volume;
    QString m_power;
};

QDataStream &operator<<(QDataStream &out, const DetectionDevice &detection);
QDataStream &operator>>(QDataStream &in, DetectionDevice &detection);
#endif // DETECTIONDEVICE_H
