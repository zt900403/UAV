#ifndef WEATHERDIALOG_H
#define WEATHERDIALOG_H

#include <QDialog>

namespace Ui {
class WeatherDialog;
}

class WeatherDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WeatherDialog(const QString &str,
                           bool checked,
                           QWidget *parent = 0);
    ~WeatherDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::WeatherDialog *ui;
    QWidget *m_parent;
};

#endif // WEATHERDIALOG_H
