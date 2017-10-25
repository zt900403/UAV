#ifndef NETWORKDIALOG_H
#define NETWORKDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QVector>
#include "object/uav.h"
#include "object/weapon.h"
class QListWidgetItem;
namespace Ui {
class NetworkDialog;
}

class NetworkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NetworkDialog(QWidget *parent = 0);
    ~NetworkDialog();

    QVector<UAV> uavs() const;
    void setUavs(const QVector<UAV> &uavs);

    QVector<Weapon> weapons() const;
    void setWeapons(const QVector<Weapon> &weapons);

    QString ip() const;
    void setIp(const QString &ip);

    int port() const;
    void setPort(int port);

    QPair<int, QString> selected() const;
    void setSelected(const QPair<int, QString> &selected);

    int id() const;
    void setId(int id);

    int index() const;
    void setIndex(int index);

private slots:
    void sendRequest();
    void connectionClosedByServer();
    void error();
    void updateUAVWidgets();
    void on_connectToServerBtn_clicked();

    void on_uavslistWidget_itemClicked(QListWidgetItem *item);

    void on_okBtn_clicked();

private:
    void initWidgets();
    void closeConnection();
    void sendCloseSign();
private:
    enum MyDataRoles {
       MyImgRole = Qt:: UserRole + 10,
       MyDescRole = Qt::UserRole + 11
    };
    Ui::NetworkDialog *ui;
    QTcpSocket m_tcpSocket;
    qint64 m_nextBlockSize;
    QVector<UAV> m_uavs;
    QVector<Weapon> m_weapons;
    QString m_ip;
    int m_port;
    QPair<int, QString> m_selected;
    int m_id;
    int m_index;
};

#endif // NETWORKDIALOG_H
