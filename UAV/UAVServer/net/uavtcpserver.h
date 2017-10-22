#ifndef UAVTCPSERVER_H
#define UAVTCPSERVER_H

#include <QTcpServer>
class UAVTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    UAVTcpServer(QObject *parent = 0);

public:
    void incomingConnection(int socketId);
};

#endif // UAVTCPSERVER_H
