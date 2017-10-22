#include "uavtcpsocket.h"

UAVTcpSocket::UAVTcpSocket(QObject *parent)
    : QTcpSocket(parent)
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

    quint8 requestType;
    in >> requestType;
    switch(requestType) {
        case 'P0' :
            break;
        default:
            QDataStream out(this);
            out << quint16(0xFFFF);
    }

}
