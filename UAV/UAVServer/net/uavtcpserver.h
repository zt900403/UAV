#ifndef UAVTCPSERVER_H
#define UAVTCPSERVER_H
#include <QtCore>
#include <QTcpServer>
#include <QVector>
#include "object/uav.h"
#include "object/weapon.h"
#include "object/detectiondevice.h"
class UAVTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    UAVTcpServer(QObject *parent = 0);

    QVector<UAV> uavs() const;
    void setUavs(const QVector<UAV> &uavs);

    QVector<Weapon> weapons() const;
    void setWeapons(const QVector<Weapon> &weapons);

    QVector<DetectionDevice> detections() const;
    void setDetections(const QVector<DetectionDevice> &detections);

private:
    void incomingConnection(int socketId);
private:
    QVector<UAV> m_uavs;
    QVector<Weapon> m_weapons;
    QVector<DetectionDevice> m_detections;
    int m_id;
//    QMap<QString, int> m_ipIdMap;
    QObject *m_parent;
};

#endif // UAVTCPSERVER_H
