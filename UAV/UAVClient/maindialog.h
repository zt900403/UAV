#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <QTime>
#include <QVector>
#include "object/uav.h"
#include "object/weapon.h"

namespace Ui {
class MainDialog;
}

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MainDialog(QWidget *parent = 0);
    ~MainDialog();

    UAV uav() const;
    void setUav(const UAV &uav);

    QVector<Weapon> weapons() const;
    void setWeapons(const QVector<Weapon> &weapons);

    QString ip() const;
    void setIp(const QString &ip);

    int port() const;
    void setPort(int port);

private slots:
    void onRollAndPitch(float roll, float pitch);
    void on_fireButton_clicked();
    void on_yawSlider_valueChanged(int value);

protected:
    void timerEvent(QTimerEvent *event);

private:
    void initPFD();
    void initWeaponGroup();
    void initUAVInfoGroup();

private:
    Ui::MainDialog *ui;
    int m_timerId;
    QTime m_time;
    float m_realTime;
    QString m_ip;
    int m_port;
    UAV m_uav;
    QVector<Weapon> m_weapons;
    float m_airspeed;// = 125.0f;
    float m_altitude;// = 5000.0f;
    float m_Status_pitch;// = 0.0f;
    float m_Status_roll;// = 0.0f;
};

#endif // MAINDIALOG_H
