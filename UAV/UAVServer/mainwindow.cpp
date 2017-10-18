#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils/json.h"

#include <QTextStream>
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_appDir(QCoreApplication::applicationDirPath())
{
    ui->setupUi(this);

    qDebug() << m_appDir;
    QString imageDir = m_appDir.relativeFilePath("./Europe_topic_image_Satellite_image.jpg");
    qDebug() << imageDir;
    QImage image(imageDir);
    ui->gisView->setImage(image);
    ui->gisView->setBackgroundBrush(QBrush(QColor(0x7F,0x7F,0x7F)));

//    QString uavtypeDir = m_appDir.relativeFilePath("./config/uavtype.json");
//    qDebug() << uavtypeDir;
//    QString uavtypeJson = readFile(uavtypeDir);
//    if (uavtypeJson.isEmpty()) {
//        qFatal("Coudl not read uavtype.json file!");
//    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::readFile(const QString &filename)
{
    QFile f(filename);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        return QString();
    } else {
        QTextStream in(&f);
        return in.readAll();
    }
}
