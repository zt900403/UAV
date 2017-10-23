#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <object/uav.h>
#include <object/weapon.h>

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

private:
    bool updateUavMetaDataGroup();
    void labelDisplayImage(QLabel *label, const QPixmap &pixmap); // const QString &filename);
    void instantiateUAVs(const QtJson::JsonArray &uavs);
    void instantiateWeapons(const QtJson::JsonArray &weapons);

private:
    Ui::MainWindow *ui;
    QVector<UAV> m_uavs;
    QVector<Weapon> m_weapons;
    UAVTcpServer *m_tcpserver;
};

#endif // MAINWINDOW_H
