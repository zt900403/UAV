#include "weapon.h"

Weapon::Weapon()
{

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

QString Weapon::getName() const
{
    return name;
}

void Weapon::setName(const QString &value)
{
    name = value;
}

float Weapon::getWeight() const
{
    return weight;
}

void Weapon::setWeight(float value)
{
    weight = value;
}
