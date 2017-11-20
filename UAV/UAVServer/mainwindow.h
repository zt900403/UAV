#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <object/uav.h>
#include <object/weapon.h>
#include <object/uavstatus.h>
#include <object/detectiondevice.h>
#include "utils/json.h"
#include <QVector>
#include <QTableWidget>


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

    void onCreateUAV(int id, int index, QString name);
    void onUpdateUAVStatus(int id, qint64 frameNum, UAVStatus status);
    void onCloseByClient(int id);
    void onDetectionDeviceStatusChanged(int id, QString deviceName, bool checked);
    void on_addPathPushButton_clicked();

    void on_delPathPushButton_clicked();

    void on_addTagPushButton_clicked();

    void on_delTagPushButton_clicked();

    void on_tagTableWidget_cellClicked(int row, int column);

    void on_openMap_triggered();

    void on_uavParameter_triggered();

    void on_weatherBtn_triggered();

    void on_importTagsBtn_clicked();

    void on_exportTagsBtn_clicked();

    void on_addAirspaceBtn_clicked();

    void on_delAirspaceBtn_clicked();

    void on_exportPathAirspaceBtn_clicked();

    void on_importPathAirspaceBtn_clicked();


    void on_openServerBtn_clicked(bool checked);

private:
    bool updateUavMetaDataGroup();
    void labelDisplayImage(QLabel *label, const QPixmap &pixmap); // const QString &filename);
    void instantiateUAVs(const QtJson::JsonArray &uavs);
    void instantiateWeapons(const QtJson::JsonArray &weapons);
    void instantiateDetections(const QtJson::JsonArray &detections);
    void listenServer();
    void addUAVStatusTab(int id, const UAVStatus &status);
    void addTag(const QString &name, const QString &x, const QString &y);
    void addPoint2TableWidget(const QString &x, const QString &y, QTableWidget *ptable);
    void updateStatusBar();
    void closeListenServer();
public:

    void setWeather(const QString &str, bool checked);

private:
    Ui::MainWindow *ui;
    QVector<UAV> m_uavs;
    QVector<Weapon> m_weapons;
    QVector<DetectionDevice> m_detections;

    UAVTcpServer *m_tcpserver;
    QMap<int, UAVStatus> m_idUAVStatusMap;
    QMap<int, QPoint> m_idOriginPositionMap;
    QMap<int, QPoint> m_idPositionMap;
    QMap<int, int> m_idYawMap;
    QMap<int, QWidget*> m_idTabMap;
//    QPointF m_UAVGisPostion;
//    UAVStatus m_currentStatus;
    QVector<QPoint> m_path;
    QVector<QPoint> m_airspace;
    QVector<QPoint> m_realpath;
    QMap<QString, QPoint> m_tags;
    QString m_weatherInfo;
    bool m_isShowWeather;
};

#endif // MAINWINDOW_H
