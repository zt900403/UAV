#include "uavtcpsocket.h"

UAVTcpSocket::UAVTcpSocket(QVector<UAV> uavs, QVector<Weapon> weapons, QObject *parent)
       : QTcpSocket(parent)
    , m_uavs(uavs)
    , m_weapons(weapons)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(this, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    m_nextBlockSize = 0;
}

void UAVTcpSocket::readClient()
{
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_4_8);
    if (m_nextBlockSize == 0) {
        if (bytesAvailable() < sizeof(quint16))
            return;
        in >> m_nextBlockSize;
    }
    if (bytesAvailable() < m_nextBlockSize)
        return;

    QString requestType;
    in >> requestType;
    if (requestType == "P0") {
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_8);
        out << quint16(0)
            << QString("R0")
            << m_uavs
            << m_weapons;
        out.device()->seek(0);
        out << quint16(block.size() - sizeof(quint16));
        write(block);
    }
    QDataStream out(this);
    out << quint16(0xFFFF);
    close();
}
