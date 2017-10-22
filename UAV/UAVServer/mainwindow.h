#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QDir;
class QListWidgetItem;
class QLabel;

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
//    QString readFile(const QString &filename);
//    QDir directoryOf(const QString &subdir);
    bool updateUavMetaDataGroup();
    void labelDisplayImage(QLabel *label, const QString &filename);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
