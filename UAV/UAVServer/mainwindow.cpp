#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils/json.h"
#include "dialog/uavtypedatadialog.h"
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include "utils/filesystem.h"

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
        foreach (QVariant wn, uavs["weapon"].toList()) {
            QtJson::JsonObject w = wn.toMap();
            weapon[w["name"]] = w["amount"];
        }

        QListWidgetItem *item= new QListWidgetItem(name);
        QVariant i;
        QVariant d;
        i.setValue(imageFile);
        d.setValue(descStr);
        item->setData(MyImgRole, i);
        item->setData(MyDescRole, d);
        ui->uavslistWidget->addItem(item);
    }
    return true;
}

void MainWindow::labelDisplayImage(QLabel *label, const QString &filename)
{
    QString f = FileSystem::directoryOf("images/uavs").absoluteFilePath(filename);
    QPixmap p(f);
    if (!p.isNull()) {
        p = p.scaled(150, 150);
    }
    label->setPixmap(p);
}

void MainWindow::on_uavslistWidget_itemClicked(QListWidgetItem *item)
{
    QVariant i = item->data(MyImgRole);
    QVariant d = item->data(MyDescRole);

    ui->uavDescriptionLabel->setText(d.toString());
    labelDisplayImage(ui->uavImageLabel, i.toString());
}

void MainWindow::on_changeUAVTypeBtn_clicked()
{
    UAVTypeDataDialog d(this);
    int result = d.exec();
    if (result == QDialog::Accepted) {
        updateUavMetaDataGroup();
    }
}
