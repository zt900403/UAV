#ifndef UAVTCPSOCKET_H
#define UAVTCPSOCKET_H
#include <QTcpSocket>

class UAVTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    UAVTcpSocket(QObject *parent = 0);

private slots:
    void readClient();

private:
    void sendUAVTypes();
private:
    quint16 nextBlockSize;
};

#endif // UAVTCPSOCKET_H
