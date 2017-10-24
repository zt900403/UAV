#ifndef UAVTCPSERVER_H
#define UAVTCPSERVER_H
#include <QtCore>
#include <QTcpServer>
#include <QVector>
#include "object/uav.h"
#include "object/weapon.h"
class UAVTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    UAVTcpServer(QObject *parent = 0);

    QVector<UAV> uavs() const;
    void setUavs(const QVector<UAV> &uavs);

    QVector<Weapon> weapons() const;
    void setWeapons(const QVector<Weapon> &weapons);

private:
    void incomingConnection(int socketId);

private:
    QVector<UAV> m_uavs;
    QVector<Weapon> m_weapons;
};

#endif // UAVTCPSERVER_H
