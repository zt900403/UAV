#ifndef UAVTCPSOCKET_H
#define UAVTCPSOCKET_H
#include <QTcpSocket>
#include <QVector>
#include <object/uav.h>
#include <object/weapon.h>
#include "object/uavstatus.h"

class UAVTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    UAVTcpSocket(QVector<UAV> uavs,
                 QVector<Weapon> weapons,
                 int &id,
                 QMap<QString, int> &ipIdMap,
                 QObject *parent = 0);

signals:
    void createUAV(int id, int index, QString name);
    void updateUAVStatus(int id, qint64 frameNum, UAVStatus status);
private slots:
    void readClient();

private:
    void sendUAVTypes();
    void sendCloseSign();
    void sendUAVsAndWeapons();
    int sendId();
private:
    qint64 m_nextBlockSize;
    QVector<UAV> m_uavs;
    QVector<Weapon> m_weapons;
    int &m_id;
    QMap<QString, int> &m_ipIdMap;
};

#endif // UAVTCPSOCKET_H
