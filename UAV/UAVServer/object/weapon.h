#ifndef WEAPON_H
#define WEAPON_H
#include <QString>

class Weapon
{
public:
    Weapon();

private:
    QString m_R_L;
    QString m_description;
    float m_fieldOfFire;
    QString m_guidedType;
    float m_killRadius;
    QString name;
    float weight;
};

#endif // WEAPON_H
