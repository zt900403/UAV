#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog/uavtypedatadialog.h"
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include "utils/filesystem.h"
#include "object/uav.h"
#include "object/weapon.h"
#include "net/uavtcpserver.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString imageDir = FileSystem::directoryOf("images").absoluteFilePath("Europe_topic_image_Satellite_image.jpg");
    QImage image(imageDir);
    ui->gisView->setImage(image);
    ui->gisView->setBackgroundBrush(QBrush(QColor(0x7F,0x7F,0x7F)));

    updateUavMetaDataGroup();
    m_tcpserver = new UAVTcpServer(m_uavs, m_weapons, this);
    if (!m_tcpserver->listen(QHostAddress::Any, 10666)) {
        QMessageBox::critical(this, tr("错误"), tr("端口监听失败, 请确定其他程序没有使用10666端口!"),
                              QMessageBox::Ok);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::updateUavMetaDataGroup()
{
    ui->uavslistWidget->clear();

    QString uavtypeDir = FileSystem::directoryOf("config").absoluteFilePath("uvatype.json");
    QString uavtypeJson = FileSystem::readFile(uavtypeDir);
    if (uavtypeJson.isEmpty()) {
        qFatal("Could not read uavtype.json file!");
        return false;
    }
    bool ok;
    QtJson::JsonObject result = QtJson::parse(uavtypeJson, ok).toMap();
    if (!ok) {
        qFatal("Could not parse uavtype.json file! json syntax is wrong!");
        return false;
    }

    QtJson::JsonArray uavs = result["uavs"].toList();
    instantiateUAVs(uavs);

    QVectorIterator<UAV> i(m_uavs);
    while(i.hasNext()) {
        UAV u = i.next();
        QString name = u.name();
        QPixmap pixmap = u.pixmap();
        QString desc = u.description();

        QListWidgetItem *item= new QListWidgetItem(name);
        QVariant i;
        QVariant d;
        i.setValue(pixmap);
        d.setValue(desc);
        item->setData(MyImgRole, i);
        item->setData(MyDescRole, d);
        ui->uavslistWidget->addItem(item);
    }

    QtJson::JsonArray weapons = result["weapons"].toList();
    instantiateWeapons(weapons);
    return true;
}

void MainWindow::labelDisplayImage(QLabel *label, const QPixmap &pixmap)//const QString &filename)
{
//    QString f = FileSystem::directoryOf("images/uavs").absoluteFilePath(filename);
//    QPixmap p(f);
    QPixmap p;
    if (!pixmap.isNull()) {
        p = pixmap.scaled(150, 150);
    }
    label->setPixmap(p);
}

void MainWindow::instantiateUAVs(const QtJson::JsonArray &uavs)
{
    m_uavs.clear();
    foreach (QVariant var, uavs) {
        QtJson::JsonObject obj = var.toMap();
        QString name = obj["name"].toString();
        QString imageFile = obj["imageFile"].toString();
        QString descStr = obj["description"].toString();
        float acc = obj["acceleration"].toFloat();
        float fh = obj["flightHeight"].toFloat();
        float fe = obj["flyEndurance"].toFloat();
        float lw = obj["loadWeight"].toFloat();
        float ms = obj["maxSpeed"].toFloat();
        float v = obj["voyage"].toFloat();
        float w = obj["weight"].toFloat();
        QString f = FileSystem::directoryOf("images/uavs").absoluteFilePath(imageFile);
        QPixmap p(f);
        QMap<QString, int> weapon;
        QtJson::JsonArray weapons = obj["weapon"].toList();
        foreach (QVariant wn, weapons) {
            QtJson::JsonObject w = wn.toMap();
            weapon[w["name"].toString()] = w["amount"].toInt();
        }
        UAV u(name, descStr, p, acc, fh, fe, lw, ms, v, w, weapon);
        m_uavs.append(u);
    }
}

void MainWindow::instantiateWeapons(const QtJson::JsonArray &weapons)
{
    m_weapons.clear();
    foreach(QVariant var, weapons) {
        QtJson::JsonObject obj = var.toMap();
        QString name = obj["name"].toString();
        QString desc = obj["description"].toString();
        QString R_L = obj["R_L"].toString();
        QString g = obj["guidedType"].toString();
        float f = obj["fieldOfFire"].toFloat();
        float k = obj["killRadius"].toFloat();
        float weight = obj["weight"].toFloat();

        Weapon w(name, desc, R_L, g, f, k, weight);
        m_weapons.append(w);
    }
}

void MainWindow::on_uavslistWidget_itemClicked(QListWidgetItem *item)
{
    QVariant i = item->data(MyImgRole);
    QVariant d = item->data(MyDescRole);

    ui->uavDescriptionLabel->setText(d.toString());
    labelDisplayImage(ui->uavImageLabel, i.value<QPixmap>());
}

void MainWindow::on_changeUAVTypeBtn_clicked()
{
    UAVTypeDataDialog d(this);
    int result = d.exec();
    if (result == QDialog::Accepted) {
        updateUavMetaDataGroup();
    }
}
