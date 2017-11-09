#ifndef UAVTCPSOCKET_H
#define UAVTCPSOCKET_H
#include <QTcpSocket>
#include <QVector>
#include <object/uav.h>
#include <object/weapon.h>
#include "object/uavstatus.h"
#include "object/detectiondevice.h"

class UAVTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    UAVTcpSocket(const QVector<UAV> &uavs,
                 const QVector<Weapon> &weapons,
                 const QVector<DetectionDevice> &detections,
                 int &id,
//                 QMap<QString, int> &ipIdMap,
                 QObject *parent = 0);

signals:
    void createUAV(int id, int index, QString name);
    void updateUAVStatus(int id, qint64 frameNum, UAVStatus status);
    void closeByClient(int id);
    void detectionDeviceStatusChanged(int id, QString deviceName, bool checked);
private slots:
    void readClient();

private:
    void sendUAVTypes();
    void sendCloseSign();
    void sendUAVWeaponDetectionTypes();
    int sendId();
    QString getClientIp();
private:
    qint64 m_nextBlockSize;
    QVector<UAV> m_uavs;
    QVector<Weapon> m_weapons;
    QVector<DetectionDevice> m_detections;
    int &m_id;
//    QMap<QString, int> &m_ipIdMap;
};

#endif // UAVTCPSOCKET_H
