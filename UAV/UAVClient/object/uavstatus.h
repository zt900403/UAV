#ifndef UAVSTATUS_H
#define UAVSTATUS_H
#include <QTime>
#include <QMap>
#include "weapon.h"

class UAVStatus
{
public:
    UAVStatus(float roll = 0.0f,
              float yaw = 0.0f,
              float pinch = 0.0f,
              float accelerator = 0.0f,
              float airSpeed = 0.0f,
              float altitude = 0.0f,
              float flyEnduranceHour = 0.0f,
              float flyEnduranceKM = 0.0f,
              const QTime &time = QTime(),
              const QMap<QString, int> &weapon = QMap<QString, int>());
    float roll() const;
    void setRoll(float roll);

    float yaw() const;
    void setYaw(float yaw);

    float pinch() const;
    void setPinch(float pinch);

    float accelerator() const;
    void setAccelerator(float accelerator);

    float airSpeed() const;
    void setAirSpeed(float airSpeed);

    float altitude() const;
    void setAltitude(float altitude);

    float flyEnduranceHour() const;
    void setFlyEnduranceHour(float flyEnduranceHour);

    float flyEnduranceKM() const;
    void setFlyEnduranceKM(float flyEnduranceKM);

    QTime time() const;
    void setTime(const QTime &time);

    QMap<QString, int> weapon() const;
    void setWeapon(const QMap<QString, int> &weapon);

private:
    float m_roll;
    float m_yaw;
    float m_pinch;
    float m_accelerator;
    float m_airSpeed;
    float m_altitude;
    float m_flyEnduranceHour;
    float m_flyEnduranceKM;
    QTime m_time;
    QMap<QString, int> m_weapon;
};
QDataStream &operator<<(QDataStream &out, const UAVStatus &status);
QDataStream &operator>>(QDataStream &in, UAVStatus &status);
#endif // UAVSTATUS_H
