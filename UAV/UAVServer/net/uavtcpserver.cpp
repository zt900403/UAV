#include "uavtcpserver.h"
#include "uavtcpsocket.h"

UAVTcpServer::UAVTcpServer(QObject *parent)
    : QTcpServer(parent)
    , m_id(0)
    , m_parent(parent)
{

}

void UAVTcpServer::incomingConnection(int socketId)
{
    UAVTcpSocket *socket = new UAVTcpSocket(m_uavs,
                                            m_weapons,
                                            m_detections,
                                            m_id,
//                                            m_ipIdMap,
                                            this);
    socket->setSocketDescriptor(socketId);
    connect(socket, SIGNAL(createUAV(int,int,QString)), m_parent, SLOT(onCreateUAV(int, int, QString)));
    connect(socket, SIGNAL(updateUAVStatus(int,qint64,UAVStatus)), m_parent, SLOT(onUpdateUAVStatus(int, qint64, UAVStatus)));
    connect(socket, SIGNAL(closeByClient(int)), m_parent, SLOT(onCloseByClient(int)));
}

QVector<DetectionDevice> UAVTcpServer::detections() const
{
    return m_detections;
}

void UAVTcpServer::setDetections(const QVector<DetectionDevice> &detections)
{
    m_detections = detections;
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
