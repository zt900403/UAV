#include "uavstatus.h"

UAVStatus::UAVStatus(float roll,
                     float yaw,
                     float pitch,
                     float accelerator,
                     float airSpeed,
                     float altitude,
                     float flyEnduranceHour,
                     float flyEnduranceKM,
                     int index,
                     const QTime &time,
                     const QMap<QString, int> &weapon)
    : m_roll(roll)
    , m_yaw(yaw)
    , m_pitch(pitch)
    , m_accelerator(accelerator)
    , m_airSpeed(airSpeed)
    , m_altitude(altitude)
    , m_flyEnduranceHour(flyEnduranceHour)
    , m_flyEnduranceKM(flyEnduranceKM)
    , m_index(index)
    , m_time(time)
    , m_weapon(weapon)
{

}

float UAVStatus::roll() const
{
    return m_roll;
}

void UAVStatus::setRoll(float roll)
{
    m_roll = roll;
}

float UAVStatus::yaw() const
{
    return m_yaw;
}

void UAVStatus::setYaw(float yaw)
{
    m_yaw = yaw;
}

float UAVStatus::pitch() const
{
    return m_pitch;
}

void UAVStatus::setpitch(float pitch)
{
    m_pitch = pitch;
}

float UAVStatus::accelerator() const
{
    return m_accelerator;
}

void UAVStatus::setAccelerator(float accelerator)
{
    m_accelerator = accelerator;
}

float UAVStatus::airSpeed() const
{
    return m_airSpeed;
}

void UAVStatus::setAirSpeed(float airSpeed)
{
    m_airSpeed = airSpeed;
}

float UAVStatus::altitude() const
{
    return m_altitude;
}

void UAVStatus::setAltitude(float altitude)
{
    m_altitude = altitude;
}

float UAVStatus::flyEnduranceHour() const
{
    return m_flyEnduranceHour;
}

void UAVStatus::setFlyEnduranceHour(float flyEnduranceHour)
{
    m_flyEnduranceHour = flyEnduranceHour;
}

float UAVStatus::flyEnduranceKM() const
{
    return m_flyEnduranceKM;
}

void UAVStatus::setFlyEnduranceKM(float flyEnduranceKM)
{
    m_flyEnduranceKM = flyEnduranceKM;
}

QTime UAVStatus::time() const
{
    return m_time;
}

void UAVStatus::setTime(const QTime &time)
{
    m_time = time;
}

QMap<QString, int> UAVStatus::weapon() const
{
    return m_weapon;
}

void UAVStatus::setWeapon(const QMap<QString, int> &weapon)
{
    m_weapon = weapon;
}

int UAVStatus::index() const
{
    return m_index;
}

void UAVStatus::setIndex(int index)
{
    m_index = index;
}

QDataStream &operator<<(QDataStream &out, const UAVStatus &status)
{
    out << status.roll()
        << status.pitch()
        << status.yaw()
        << status.accelerator()
        << status.airSpeed()
        << status.altitude()
        << status.flyEnduranceHour()
        << status.flyEnduranceKM()
        << status.index()
        << status.time()
        << status.weapon();
    return out;
}

QDataStream &operator>>(QDataStream &in, UAVStatus &status)
{
    float roll;
    float pitch;
    float yaw;
    float accelerator;
    float airspeed;
    float altitude;
    float feh;
    float fek;
    int index;
    QTime time;
    QMap<QString, int> weapon;

    in >> roll
            >> pitch
            >> yaw
            >> accelerator
            >> airspeed
            >> altitude
            >> feh
            >> fek
            >> index
            >> time
            >> weapon;
    status = UAVStatus(roll,
                       yaw,
                       pitch,
                       accelerator,
                       airspeed,
                       altitude,
                       feh,
                       fek,
                       index,
                       time,
                       weapon);
    return in;
}
