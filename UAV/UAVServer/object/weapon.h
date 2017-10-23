#ifndef WEAPON_H
#define WEAPON_H
#include <QObject>
#include <QString>
#include <QDataStream>
class Weapon
{
public:
    Weapon(const QString &name = QString(), const QString &description = QString(),
           const QString &R_L = QString(), const QString &guidedType = QString(),
           float fieldOfFire = 0.0f, float killRadius = 0.0f, float weight = 0.0f);

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

    QString name() const;
    void setName(const QString &value);

    float weight() const;
    void setWeight(float value);

private:
    QString m_R_L;
    QString m_description;
    float m_fieldOfFire;
    QString m_guidedType;
    float m_killRadius;
    QString m_name;
    float m_weight;
};
QDataStream &operator<<(QDataStream &out, const Weapon &weapon);
QDataStream &operator>>(QDataStream &in, Weapon &weapon);
#endif // WEAPON_H
