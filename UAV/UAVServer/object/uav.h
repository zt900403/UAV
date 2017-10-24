#ifndef UAV_H
#define UAV_H
#include <QString>
#include <QMap>
#include <QPixmap>
class UAV
{
public:
    UAV(const QString &name = QString(),
        const QString &description = QString(),
        const QPixmap &pixmap = QPixmap(),
        float acceleration = 0.0f,
        float flightHeight = 0.0f,
        float flyEndurance = 0.0f,
        float loadWeight = 0.0f,
        float maxSpeed = 0.0f,
        float voyage = 0.0f,
        float weight = 0.0f,
        const QMap<QString, int> &weapon = QMap<QString, int>());

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

    QPixmap pixmap() const;
    void setPixmap(const QPixmap &pixmap);
private:
    float m_acceleration;
    QString m_description;
    float m_flightHeight;
    float m_flyEndurance;
    QPixmap m_pixmap;
    float m_loadWeight;
    float m_maxSpeed;
    QString m_name;
    float m_voyage;
    float m_weight;
    QMap<QString, int> m_weapon;
};

QDataStream &operator<<(QDataStream &out, const UAV &uav);
QDataStream &operator>>(QDataStream &in, UAV &uav);
#endif // UAV_H
