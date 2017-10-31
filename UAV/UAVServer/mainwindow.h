#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <object/uav.h>
#include <object/weapon.h>
#include <object/uavstatus.h>
#include "utils/json.h"
#include <QVector>


class QDir;
class QListWidgetItem;
class QLabel;

class UAVTcpServer;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum MyDataRoles {
       MyImgRole = Qt:: UserRole + 10,
       MyDescRole = Qt::UserRole + 11
    };

private slots:

    void on_uavslistWidget_itemClicked(QListWidgetItem *item);

    void on_changeUAVTypeBtn_clicked();
    void onCreateUAV(int id, int index, QString name);
    void onUpdateUAVStatus(int id, qint64 frameNum, UAVStatus status);
    void onCloseByClient(int id);
private:
    bool updateUavMetaDataGroup();
    void labelDisplayImage(QLabel *label, const QPixmap &pixmap); // const QString &filename);
    void instantiateUAVs(const QtJson::JsonArray &uavs);
    void instantiateWeapons(const QtJson::JsonArray &weapons);
    void listenServer();
    void addUAVStatusTab(int id, const UAVStatus &status);

private:
    Ui::MainWindow *ui;
    QVector<UAV> m_uavs;
    QVector<Weapon> m_weapons;
    UAVTcpServer *m_tcpserver;
    QMap<int, UAVStatus> m_idUAVStatusMap;
    QMap<int, QWidget*> m_idTabMap;
    QPointF m_UAVGisPostion;
    UAVStatus m_currentStatus;
};

#endif // MAINWINDOW_H
