#ifndef UAVTCPSERVER_H
#define UAVTCPSERVER_H
#include <QTcpServer>
#include <QVector>
#include "object/uav.h"
#include "object/weapon.h"
class UAVTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    UAVTcpServer(QVector<UAV> uavs, QVector<Weapon> weapons, QObject *parent = 0);

public:
    void incomingConnection(int socketId);

private:
    QVector<UAV> m_uavs;
    QVector<Weapon> m_weapons;
};

#endif // UAVTCPSERVER_H
