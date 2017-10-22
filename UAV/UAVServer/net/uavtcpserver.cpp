#include "uavtcpserver.h"
#include "uavtcpsocket.h"
UAVTcpServer::UAVTcpServer(QObject *parent)
    : QTcpServer(parent)
{

}

void UAVTcpServer::incomingConnection(int socketId)
{
    UAVTcpSocket *socket = new UAVTcpSocket(this);
    socket->setSocketDescriptor(socketId);
}
