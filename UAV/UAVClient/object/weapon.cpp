#include "weapon.h"
Weapon::Weapon(const QString &name, const QString &description, const QString &R_L, const QString &guidedType, float fieldOfFire, float killRadius, float weight)
{
    setName(name);
    setDescription(description);
    setR_L(R_L);
    setGuidedType(guidedType);
    setFieldOfFire(fieldOfFire);
    setKillRadius(killRadius);
    setWeight(weight);
}

QString Weapon::R_L() const
{
    return m_R_L;
}

void Weapon::setR_L(const QString &R_L)
{
    m_R_L = R_L;
}

QString Weapon::description() const
{
    return m_description;
}

void Weapon::setDescription(const QString &description)
{
    m_description = description;
}

float Weapon::fieldOfFire() const
{
    return m_fieldOfFire;
}

void Weapon::setFieldOfFire(float fieldOfFire)
{
    m_fieldOfFire = fieldOfFire;
}

QString Weapon::guidedType() const
{
    return m_guidedType;
}

void Weapon::setGuidedType(const QString &guidedType)
{
    m_guidedType = guidedType;
}

float Weapon::killRadius() const
{
    return m_killRadius;
}

void Weapon::setKillRadius(float killRadius)
{
    m_killRadius = killRadius;
}

QString Weapon::name() const
{
    return m_name;
}

void Weapon::setName(const QString &value)
{
    m_name = value;
}

float Weapon::weight() const
{
    return m_weight;
}

void Weapon::setWeight(float value)
{
    m_weight = value;
}

QDataStream &operator<<(QDataStream &out, const Weapon &weapon)
{
    out << weapon.name()
        << weapon.description()
        << weapon.R_L()
        << weapon.guidedType()
        << weapon.fieldOfFire()
        << weapon.killRadius()
        << weapon.weight();
    return out;
}

QDataStream &operator>>(QDataStream &in, Weapon &weapon)
{
    QString R_L;
    QString d;
    float f;
    QString g;
    float k;
    QString n;
    float w;

    in >> n >> d >> R_L >> g >> f >> k >> w;
    weapon = Weapon(n, d, R_L, g, f, k ,w);
    return in;
}
