#include "uavtcpsocket.h"

UAVTcpSocket::UAVTcpSocket(QVector<UAV> uavs, QVector<Weapon> weapons, QObject *parent)
       : QTcpSocket(parent)
    , m_uavs(uavs)
    , m_weapons(weapons)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(this, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    nextBlockSize = 0;
}

void UAVTcpSocket::readClient()
{
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_4_5);
    if (nextBlockSize == 0) {
        if (bytesAvailable() < sizeof(quint16))
            return;
        in >> nextBlockSize;
    }
    if (bytesAvailable() < nextBlockSize)
        return;

    QString requestType;
    in >> requestType;
    if (requestType == "P0") {
        QDataStream out(this);
        out << QString("R0")
            << m_uavs
            << quint16(0xFFFF);
    }
    close();
}
