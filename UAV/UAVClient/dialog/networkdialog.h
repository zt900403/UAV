#ifndef NETWORKDIALOG_H
#define NETWORKDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QVector>
#include "object/uav.h"
#include "object/weapon.h"
namespace Ui {
class NetworkDialog;
}

class NetworkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NetworkDialog(QWidget *parent = 0);
    ~NetworkDialog();

private slots:
    void sendRequest();
    void connectionClosedByServer();
    void error();
    void updateUAVWidgets();
    void on_connectToServerBtn_clicked();
private:
    void initWidgets();
    void closeConnection();
private:
    enum MyDataRoles {
       MyImgRole = Qt:: UserRole + 10,
       MyDescRole = Qt::UserRole + 11
    };
    Ui::NetworkDialog *ui;
    QTcpSocket m_tcpSocket;
    quint16 m_nextBlockSize;
    QVector<UAV> m_uavs;
    QVector<Weapon> m_weapons;
};

#endif // NETWORKDIALOG_H
