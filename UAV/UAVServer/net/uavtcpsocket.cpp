#include "uavtcpsocket.h"
#include <QHostAddress>

UAVTcpSocket::UAVTcpSocket(const QVector<UAV> &uavs,
                           const QVector<Weapon> &weapons,
                           const QVector<DetectionDevice> &detections,
                           int &id,
                           QObject *parent)
       : QTcpSocket(parent)
    , m_uavs(uavs)
    , m_weapons(weapons)
    , m_detections(detections)
    , m_id(id)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(this, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    m_nextBlockSize = 0;
}

void UAVTcpSocket::readClient()
{
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_4_8);


    forever {
        if (m_nextBlockSize == 0) {
            if (bytesAvailable() < sizeof(qint64))
                break ;
            in >> m_nextBlockSize;
        }
        if (m_nextBlockSize == 0xFFFFFFFF) {
//            QString ip = getClientIp();
//            int id = m_ipIdMap[ip];
//            m_ipIdMap.remove(ip);
//            emit closeByClient(id);
            close();
            break ;
        }

        if (bytesAvailable() < m_nextBlockSize)
            break ;

        QString requestType;
        in >> requestType;
        if (requestType == "P0") {
            sendUAVWeaponDetectionTypes();
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
        if (requestType == "P3") {
            quint8 id;
            in >> id;
            emit closeByClient(id);
            close();
        }
        m_nextBlockSize = 0;
    }

}

void UAVTcpSocket::sendCloseSign()
{
    QDataStream out(this);
    out.setVersion(QDataStream::Qt_4_8);
    out << qint64(0xFFFFFFFF);
    flush();
    close();
}

void UAVTcpSocket::sendUAVWeaponDetectionTypes()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << qint64(0)
        << QString("R0")
        << m_uavs
        << m_weapons
        << m_detections;
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

    /*
    QString ip = getClientIp();
    int id ;
    if (m_ipIdMap.contains(ip)) {
        id = m_ipIdMap[ip];
    } else {
        id = m_id++;
        m_ipIdMap[ip] = id;
    }
    */
    int id = m_id++;
    out << quint8(id);
    out.device()->seek(0);
    out << qint64(block.size() - sizeof(qint64));
    write(block);
    return id;
}

QString UAVTcpSocket::getClientIp()
{
    return peerAddress().toString();
}
