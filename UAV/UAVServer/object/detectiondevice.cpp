#include "detectiondevice.h"
DetectionDevice::DetectionDevice(const QString &name, const QString &resolution, const QString &workingAltitude, float weight, const QString &focalDistance, const QString &volume, const QString &power)
    :m_name(name)
    ,m_resolution(resolution)
    ,m_workingAltitude(workingAltitude)
    ,m_weight(weight)
    ,m_focalDistance(focalDistance)
    ,m_volume(volume)
    ,m_power(power)
{

}

QString DetectionDevice::name() const
{
    return m_name;
}

void DetectionDevice::setName(const QString &name)
{
    m_name = name;
}

QString DetectionDevice::resolution() const
{
    return m_resolution;
}

void DetectionDevice::setResolution(const QString &resolution)
{
    m_resolution = resolution;
}

QString DetectionDevice::workingAltitude() const
{
    return m_workingAltitude;
}

void DetectionDevice::setWorkingAltitude(const QString &workingAltitude)
{
    m_workingAltitude = workingAltitude;
}

float DetectionDevice::weight() const
{
    return m_weight;
}

void DetectionDevice::setWeight(float weight)
{
    m_weight = weight;
}

QString DetectionDevice::focalDistance() const
{
    return m_focalDistance;
}

void DetectionDevice::setFocalDistance(const QString &focalDistance)
{
    m_focalDistance = focalDistance;
}

QString DetectionDevice::volume() const
{
    return m_volume;
}

void DetectionDevice::setVolume(const QString &volume)
{
    m_volume = volume;
}

QString DetectionDevice::power() const
{
    return m_power;
}

void DetectionDevice::setPower(const QString &power)
{
    m_power = power;
}

QDataStream &operator<<(QDataStream &out, const DetectionDevice &detection)
{    out << detection.name()
        << detection.resolution()
        << detection.workingAltitude()
        << detection.weight()
        << detection.focalDistance()
        << detection.volume()
        << detection.power();
    return out;
}

QDataStream &operator>>(QDataStream &in, DetectionDevice &detection)
{
    QString name;
    QString res;
    QString wh;
    float w;
    QString fd;
    QString v;
    QString p;
    in >> name >> res >> wh >> w >> fd >> v >> p;
    detection = DetectionDevice(name, res, wh, w, fd, v, p);
    return in;
}
