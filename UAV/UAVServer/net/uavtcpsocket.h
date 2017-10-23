#ifndef UAVTCPSOCKET_H
#define UAVTCPSOCKET_H
#include <QTcpSocket>
#include <QVector>
#include <object/uav.h>
#include <object/weapon.h>

class UAVTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    UAVTcpSocket(QVector<UAV> uavs, QVector<Weapon> weapons, QObject *parent = 0);

private slots:
    void readClient();

private:
    void sendUAVTypes();
private:
    qint64 m_nextBlockSize;
    QVector<UAV> m_uavs;
    QVector<Weapon> m_weapons;
};

#endif // UAVTCPSOCKET_H
