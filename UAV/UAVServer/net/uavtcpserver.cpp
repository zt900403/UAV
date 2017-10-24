#include "uavtcpserver.h"
#include "uavtcpsocket.h"

UAVTcpServer::UAVTcpServer(QObject *parent)
    : QTcpServer(parent)
{

}

void UAVTcpServer::incomingConnection(int socketId)
{
    UAVTcpSocket *socket = new UAVTcpSocket(m_uavs, m_weapons, this);
    socket->setSocketDescriptor(socketId);
}

QVector<Weapon> UAVTcpServer::weapons() const
{
    return m_weapons;
}

void UAVTcpServer::setWeapons(const QVector<Weapon> &weapons)
{
    m_weapons = weapons;
}

QVector<UAV> UAVTcpServer::uavs() const
{
    return m_uavs;
}

void UAVTcpServer::setUavs(const QVector<UAV> &uavs)
{
    m_uavs = uavs;
}
