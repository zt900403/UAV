#ifndef UAV_H
#define UAV_H
#include <QString>
#include <QMap>
#include <QBitmap>
class UAV
{
public:
    UAV(const QString &name,
        const QString &description,
        const QBitmap &bitmap,
        float acceleration,
        float flightHeight,
        float flyEndurance,
        float loadWeight,
        float maxSpeed,
        float voyage,
        float weight,
        const QMap<QString, int> weapon);

    float acceleration() const;
    void setAcceleration(float acceleration);

    QString description() const;
    void setDescription(const QString &description);

    float flightHeight() const;
    void setFlightHeight(float flightHeight);

    float flyEndurance() const;
    void setFlyEndurance(float flyEndurance);

    float loadWeight() const;
    void setLoadWeight(float loadWeight);

    float maxSpeed() const;
    void setMaxSpeed(float maxSpeed);

    QString name() const;
    void setName(const QString &name);

    float voyage() const;
    void setVoyage(float voyage);

    float weight() const;
    void setWeight(float weight);

    QMap<QString, int> weapon() const;
    void setWeapon(const QMap<QString, int> &weapon);

    QBitmap bitmap() const;
    void setBitmap(const QBitmap &bitmap);

private:
    float m_acceleration;
    QString m_description;
    float m_flightHeight;
    float m_flyEndurance;
    QBitmap m_bitmap;
    float m_loadWeight;
    float m_maxSpeed;
    QString m_name;
    float m_voyage;
    float m_weight;
    QMap<QString, int> m_weapon;
};

#endif // UAV_H
