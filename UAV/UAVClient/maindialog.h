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
    void on_rollAndPitch(float roll, float pitch);
    void on_fireButton_clicked();


protected:
    void timerEvent(QTimerEvent *event);

private:
    Ui::MainDialog *ui;
    int m_timerId;
    QTime m_time;
    float m_realTime;
};

#endif // MAINDIALOG_H
