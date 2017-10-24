#include "uavtcpsocket.h"
#include <QHostAddress>

UAVTcpSocket::UAVTcpSocket(QVector<UAV> uavs,
                           QVector<Weapon> weapons,
                           int &id, QMap<QString, int> &ipIdMap,
                           QObject *parent)
       : QTcpSocket(parent)
    , m_uavs(uavs)
    , m_weapons(weapons)
    , m_id(id)
    , m_ipIdMap(ipIdMap)
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
        if (bytesAvailable() < sizeof(qint64))
            return ;
        in >> m_nextBlockSize;
    }

    if (m_nextBlockSize == 0xFFFFFFFF) {
        close();
        return ;
    }

    if (bytesAvailable() < m_nextBlockSize)
        return ;

    QString requestType;
    in >> requestType;
    if (requestType == "P0") {
        sendUAVsAndWeapons();
    }
    if (requestType == "P1") {
        quint8 index;
        QString name;
        in >> index >> name;
        if (m_uavs[index].name() == name) {
            int id = sendId();
            emit createUAV(id, index, name);
        }
        sendCloseSign();
    }
    if (requestType == "P2") {
        quint8 id;
        qint64 frameNum;
        UAVStatus status;
        in >> id
                >> frameNum
                >> status;
        emit updateUAVStatus(id, frameNum, status);
    }
    m_nextBlockSize = 0;
}

void UAVTcpSocket::sendCloseSign()
{
    QDataStream out(this);
    out << qint64(0xFFFFFFFF);
    close();
}

void UAVTcpSocket::sendUAVsAndWeapons()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << qint64(0)
        << QString("R0")
        << m_uavs
        << m_weapons;
    out.device()->seek(0);
    out << qint64(block.size() - sizeof(qint64));
    write(block);
}

int UAVTcpSocket::sendId()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << qint64(0)
        << QString("R1");

    QString ip = peerAddress().toString();
    int id ;
    if (m_ipIdMap.contains(ip)) {
        id = m_ipIdMap[ip];
    } else {
        id = m_id++;
        m_ipIdMap[ip] = id;
    }
    out << quint8(id);
    out.device()->seek(0);
    out << qint64(block.size() - sizeof(qint64));
    write(block);
    return id;
}
