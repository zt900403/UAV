#ifndef WEAPON_H
#define WEAPON_H
#include <QString>

class Weapon
{
public:
    Weapon();

    QString R_L() const;
    void setR_L(const QString &R_L);

    QString description() const;
    void setDescription(const QString &description);

    float fieldOfFire() const;
    void setFieldOfFire(float fieldOfFire);

    QString guidedType() const;
    void setGuidedType(const QString &guidedType);

    float killRadius() const;
    void setKillRadius(float killRadius);

    QString getName() const;
    void setName(const QString &value);

    float getWeight() const;
    void setWeight(float value);

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
