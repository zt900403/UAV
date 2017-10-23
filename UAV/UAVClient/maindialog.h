#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <QTime>
namespace Ui {
class MainDialog;
}

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MainDialog(QWidget *parent = 0);
    ~MainDialog();

private slots:
    void onRollAndPitch(float roll, float pitch);
    void on_fireButton_clicked();
    void on_yawSlider_valueChanged(int value);

protected:
    void timerEvent(QTimerEvent *event);

private:
    void initPFD();

private:
    Ui::MainDialog *ui;
    int m_timerId;
    QTime m_time;
    float m_realTime;
};

#endif // MAINDIALOG_H
