#include "uavtcpserver.h"
#include "uavtcpsocket.h"

UAVTcpServer::UAVTcpServer(QVector<UAV> uavs, QVector<Weapon> weapons, QObject *parent)
    : QTcpServer(parent)
    , m_uavs(uavs)
    , m_weapons(weapons)
{

}

void UAVTcpServer::incomingConnection(int socketId)
{
    UAVTcpSocket *socket = new UAVTcpSocket(m_uavs, m_weapons, this);
    socket->setSocketDescriptor(socketId);
}
