#include "uav.h"
#include <QTextStream>
#include <QTextCodec>
UAV::UAV(const QString &name,
         const QString &description,
         const QPixmap &pixmap,
         float acceleration,
         float flightHeight,
         float flyEndurance,
         float loadWeight,
         float maxSpeed,
         float voyage,
         float weight,
         const QMap<QString, int> &weapon, const QVector<QString> &detection)
{
    setName(name);
    setDescription(description);
    setPixmap(pixmap);
    setAcceleration(acceleration);
    setFlightHeight(flightHeight);
    setFlyEndurance(flyEndurance);
    setLoadWeight(loadWeight);
    setMaxSpeed(maxSpeed);
    setVoyage(voyage);
    setWeight(weight);
    setWeapon(weapon);
    setDetection(detection);
}

float UAV::acceleration() const
{
    return m_acceleration;
}

void UAV::setAcceleration(float acceleration)
{
    m_acceleration = acceleration;
}

QString UAV::description() const
{
    return m_description;
}

void UAV::setDescription(const QString &description)
{
    m_description = description;
}

float UAV::flightHeight() const
{
    return m_flightHeight;
}

void UAV::setFlightHeight(float flightHeight)
{
    m_flightHeight = flightHeight;
}

float UAV::flyEndurance() const
{
    return m_flyEndurance;
}

void UAV::setFlyEndurance(float flyEndurance)
{
    m_flyEndurance = flyEndurance;
}

float UAV::loadWeight() const
{
    return m_loadWeight;
}

void UAV::setLoadWeight(float loadWeight)
{
    m_loadWeight = loadWeight;
}

float UAV::maxSpeed() const
{
    return m_maxSpeed;
}

void UAV::setMaxSpeed(float maxSpeed)
{
    m_maxSpeed = maxSpeed;
}

QString UAV::name() const
{
    return m_name;
}

void UAV::setName(const QString &name)
{
    m_name = name;
}

float UAV::voyage() const
{
    return m_voyage;
}

void UAV::setVoyage(float voyage)
{
    m_voyage = voyage;
}

float UAV::weight() const
{
    return m_weight;
}

void UAV::setWeight(float weight)
{
    m_weight = weight;
}

QMap<QString, int> UAV::weapon() const
{
    return m_weapon;
}

void UAV::setWeapon(const QMap<QString, int> &weapon)
{
    m_weapon = weapon;
}

QPixmap UAV::pixmap() const
{
    return m_pixmap;
}

void UAV::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
}

QVector<QString> UAV::detection() const
{
    return m_detection;
}

void UAV::setDetection(const QVector<QString> &detection)
{
    m_detection = detection;
}

QString UAV::getParametersDesc()
{
    QString cache;
    QTextStream out(&cache);
//    QTextCodec *codec=QTextCodec::codecForName("UTF-8");
//    out.setCodec(codec);
    out << QObject::tr("名称: ") << this->name() << "\n"
        << QObject::tr("自重(kg): ") << this->weight() << "\n"
        << QObject::tr("航程(km): ") << this->voyage() << "\n"
        << QObject::tr("航高(m): ") << this->flightHeight() << "\n"
        << QObject::tr("续航(h): ") << this->flyEndurance() << "\n"
        << QObject::tr("载荷(kg): ") << this->loadWeight() << "\n"
        << QObject::tr("最高时速(km/h): ") << this->maxSpeed() << "\n"
        << QObject::tr("提升加速度(km/s^2): ") << this->acceleration() << "\n"
        << QObject::tr("描述: ") << this->description() << "\n";
    out << QObject::tr("侦查设备:") << "\n";
    for (int i = 0; i < m_detection.size(); ++i) {
        out << m_detection[i] << "\n";
    }
    out << QObject::tr("武器系统:") << "\n";
    QMapIterator<QString, int> it(m_weapon);
    while(it.hasNext()) {
        it.next();
        out << it.key() << " : " << it.value() << "\n";
    }
    return cache;
}

QDataStream &operator<<(QDataStream &out, const UAV &uav)
{
    out << uav.name() << uav.description()
        << uav.acceleration()
        << uav.flightHeight()
        << uav.flyEndurance()
        << uav.loadWeight()
        << uav.maxSpeed()
        << uav.voyage()
        << uav.weight()
        << uav.pixmap()
        << uav.weapon()
        << uav.detection();
    return out;
}

QDataStream &operator>>(QDataStream &in, UAV &uav)
{
    float acc;
    QString d;
    float fh;
    float fe;
    QPixmap p;
    float l;
    float m;
    QString n;
    float v;
    float w;
    QMap<QString, int> weapon;
    QVector<QString> detection;
    in >> n >> d >> acc >> fh >> fe >> l >> m >> v >> w >> p >> weapon >> detection;
    uav = UAV(n, d, p, acc, fh, fe, l, m, v, w, weapon, detection);
    return in;
}
