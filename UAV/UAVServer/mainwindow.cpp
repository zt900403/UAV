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
#include <QLineEdit>
#include <QCheckBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_tcpserver(NULL)
{
    ui->setupUi(this);
    QString imageDir = FileSystem::directoryOf("images").absoluteFilePath("Europe_topic_image_Satellite_image.jpg");
    QImage image(imageDir);

//    ui->gisView->setLastGisPosition(QPoint(500,500));
//    ui->gisView->setGisPosition(QPoint(500, 500));
//    m_UAVGisPostion.setX(500);
//    m_UAVGisPostion.setY(500);

//    ui->gisView->setYaw(-46);
    ui->gisView->setImage(image);
    ui->gisView->setBackgroundBrush(QBrush(QColor(0x7F,0x7F,0x7F)));
    ui->gisView->update();

    updateUavMetaDataGroup();

    QRegExp portRegex("^([0-9]{1,6})$");
    QRegExpValidator *portValidator = new QRegExpValidator(portRegex, this);
    ui->pathXLineEdit->setValidator(portValidator);
    ui->pathYLineEdit->setValidator(portValidator);
    ui->tagXLineEdit->setValidator(portValidator);
    ui->tagYLineEdit->setValidator(portValidator);
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

    QtJson::JsonArray detections = result["detectionEquipments"].toList();
    instantiateDetections(detections);

    listenServer();
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

        QVector<QString> detection;
        QtJson::JsonArray detections = obj["detectionEquipments"].toList();
        foreach(QVariant det, detections) {

            detection.append(det.toMap()["name"].toString());
        }

        UAV u(name, descStr, p, acc, fh, fe, lw, ms, v, w, weapon, detection);
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

void MainWindow::instantiateDetections(const QtJson::JsonArray &detections)
{
    foreach(QVariant var, detections) {
        QtJson::JsonObject obj = var.toMap();
        QString n = obj["name"].toString();
        QString r = obj["resolution"].toString();
        QString wh = obj["workingAltitude"].toString();
        float w = obj["weight"].toFloat();
        QString fd = obj["focalDistance"].toString();
        QString v = obj["volume"].toString();
        QString p = obj["power"].toString();

        DetectionDevice d(n, r, wh, w, fd, v, p);
        m_detections.append(d);
    }
}

void MainWindow::listenServer()
{
    if (!m_tcpserver) {
        m_tcpserver = new UAVTcpServer(this);
    }
    if (m_tcpserver->isListening())
        m_tcpserver->close();

    m_tcpserver->setUavs(m_uavs);
    m_tcpserver->setWeapons(m_weapons);
    m_tcpserver->setDetections(m_detections);

    if (!m_tcpserver->listen(QHostAddress::Any, 10666)) {
        QMessageBox::critical(this, tr("错误"), tr("端口监听失败, 请确定其他程序没有使用10666端口!"),
                              QMessageBox::Ok);
    }
}

void MainWindow::addUAVStatusTab(int id, const UAVStatus &status)
{
    QWidget *widget = new QWidget();
    QLabel *l1 = new QLabel(tr("俯仰:"));
    QLabel *l2 = new QLabel(tr("横滚:"));
    QLabel *l3 = new QLabel(tr("偏航:"));
    QLabel *l4 = new QLabel(tr("油门:"));
    QLabel *l5 = new QLabel(tr("空速:"));
    QLabel *l6 = new QLabel(tr("高度:"));
    QLineEdit *pinch = new QLineEdit(QString::number(status.pinch()));
    QLineEdit *roll = new QLineEdit(QString::number(status.roll()));
    QLineEdit *yaw = new QLineEdit(QString::number(status.yaw()));
    QLineEdit *acc = new QLineEdit(QString::number(status.accelerator()));
    QLineEdit *airSpeed = new QLineEdit(QString::number(status.airSpeed()));
    QLineEdit *altitude = new QLineEdit(QString::number(status.altitude()));

    pinch->setObjectName("pinch");
    roll->setObjectName("roll");
    yaw->setObjectName("yaw");
    acc->setObjectName("acc");
    airSpeed->setObjectName("airSpeed");
    altitude->setObjectName("altitude");
    pinch->setDisabled(true);
    roll->setDisabled(true);
    yaw->setDisabled(true);
    acc->setDisabled(true);
    airSpeed->setDisabled(true);
    altitude->setDisabled(true);

    QHBoxLayout *hl1 = new QHBoxLayout();
    hl1->addWidget(l1);
    hl1->addWidget(pinch);

    QHBoxLayout *hl2 = new QHBoxLayout();
    hl2->addWidget(l2);
    hl2->addWidget(roll);

    QHBoxLayout *hl3 = new QHBoxLayout();
    hl3->addWidget(l3);
    hl3->addWidget(yaw);

    QHBoxLayout *hl4 = new QHBoxLayout();
    hl4->addWidget(l4);
    hl4->addWidget(acc);

    QHBoxLayout *hl5 = new QHBoxLayout();
    hl5->addWidget(l5);
    hl5->addWidget(airSpeed);

    QHBoxLayout *hl6 = new QHBoxLayout();
    hl6->addWidget(l6);
    hl6->addWidget(altitude);

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addLayout(hl1);
    vLayout->addLayout(hl2);
    vLayout->addLayout(hl3);
    vLayout->addLayout(hl4);
    vLayout->addLayout(hl5);
    vLayout->addLayout(hl6);

    widget->setLayout(vLayout);

    QString title = m_uavs[status.index()].name();
    title += "#";
    title += QString::number(id);
    m_idTabMap[id] = widget;
    ui->uavStatusTabWidget->addTab(widget, title);
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

void MainWindow::onCreateUAV(int id, int index, QString name)
{
    UAVStatus u;
    u.setIndex(index);
//    m_idOriginPositionMap[id] = QPoint(500, 500);
    m_idPositionMap[id] = QPoint(500, 500);
    m_idYawMap[id] = 0;
    m_idUAVStatusMap[id] = u;
    ui->gisView->setIdLastLocationMap(m_idPositionMap);
    if (!m_idTabMap.contains(id))
        addUAVStatusTab(id, u);
}

void MainWindow::onUpdateUAVStatus(int id, qint64 frameNum, UAVStatus status )
{
    m_idUAVStatusMap[id] = status;
    if (m_idTabMap.contains(id)) {
        QWidget *w = m_idTabMap[id];

        w->findChild<QLineEdit*>("pinch")->setText(QString::number(status.pinch()));
        w->findChild<QLineEdit*>("roll")->setText(QString::number(status.roll()));
        w->findChild<QLineEdit*>("yaw")->setText(QString::number(status.yaw()));
        w->findChild<QLineEdit*>("acc")->setText(QString::number(status.accelerator()));
        w->findChild<QLineEdit*>("airSpeed")->setText(QString::number(status.airSpeed()));
        w->findChild<QLineEdit*>("altitude")->setText(QString::number(status.altitude()));
//        m_currentStatus = status;

        // 更新位置
        QPoint velocity(status.airSpeed()*sin(status.yaw() * 3.1415926 / 180),
                         -status.airSpeed()*cos(status.yaw() * 3.1415926 / 180));
        QPoint tmp = m_idPositionMap[id];
        tmp += velocity / 100;
        m_idPositionMap[id] = tmp;
//        m_idPositionMap[id] += velocity / 100;

//        qDebug()<< velocity.x()<< " , " << velocity.y() << " , yaw: " <<  m_currentStatus.yaw() ;
//        ui->gisView->setGisPosition(m_UAVGisPostion.toPoint());
//        ui->gisView->setYaw(m_currentStatus.yaw());
//        ui->gisView->update();
        m_idYawMap[id] = status.yaw();
        ui->gisView->setIdLocationMap(m_idPositionMap);
        ui->gisView->setIdYawMap(m_idYawMap);
        ui->gisView->viewport()->repaint();
    }
}

void MainWindow::onCloseByClient(int id)
{
    int len = ui->uavStatusTabWidget->count();
    for (int i = 0; i < len; ++i) {
        QString title = ui->uavStatusTabWidget->tabText(i);
        if (title.endsWith(QString::number(id))) {
            m_idTabMap.remove(id);
            m_idUAVStatusMap.remove(id);
            ui->uavStatusTabWidget->removeTab(i);
            break;
        }
    }
}



void MainWindow::on_addPathPushButton_clicked()
{
    QLineEdit *l1 = ui->pathXLineEdit;
    QLineEdit *l2 = ui->pathYLineEdit;
    QString x = l1->text();
    QString y = l2->text();
    if (!x.isEmpty() &&
            !y.isEmpty()) {
        QPoint p(x.toInt(), y.toInt());
        m_path.append(p);

        QTableWidget *t = ui->pathTableWidget;
        int row = t->rowCount();
        t->insertRow(row);
        QTableWidgetItem *itemX = new QTableWidgetItem(x);
        QTableWidgetItem *itemY = new QTableWidgetItem(y);
        t->setItem(row, 0, itemX);
        t->setItem(row, 1, itemY);

        ui->gisView->setPath(m_path);
//        ui->gisView->update();
        ui->gisView->viewport()->repaint();
    }
}

void MainWindow::on_delPathPushButton_clicked()
{
    QTableWidget *t = ui->pathTableWidget;
    if (t->currentRow() != -1) {
        m_path.remove(t->currentRow());
        t->removeRow(t->currentRow());
        ui->gisView->setPath(m_path);
//        ui->gisView->update();
        ui->gisView->viewport()->repaint();
        t->setCurrentCell(-1, -1);
    }

}

void MainWindow::on_addTagPushButton_clicked()
{
    QTableWidget *t = ui->tagTableWidget;
    QString name = ui->tagNameLineEdit->text();
    QString x = ui->tagXLineEdit->text();
    QString y = ui->tagYLineEdit->text();
    int rowCnt = t->rowCount();
    for (int i = 0; i < rowCnt; ++i) {
        QString tmp = t->item(i, 0)->data(Qt::DisplayRole).toString();
        if (tmp == name) {
            QMessageBox::critical(this, "失败", "该名称已经存在!", QMessageBox::Ok);
            return;
        }
    }

    if ( !name.isEmpty() && !x.isEmpty() && !y.isEmpty()) {
        int row = t->rowCount();
        t->insertRow(row);
        QTableWidgetItem *checkBoxItem = new QTableWidgetItem(name);
        checkBoxItem->setCheckState(Qt::Unchecked);
        t->setItem(row, 0, checkBoxItem);

        QTableWidgetItem *itemX = new QTableWidgetItem(x);
        QTableWidgetItem *itemY = new QTableWidgetItem(y);
        t->setItem(row, 1, itemX);
        t->setItem(row, 2, itemY);
//        m_tags[name] = QPoint(x.toInt(), y.toInt());
    }
}

void MainWindow::on_delTagPushButton_clicked()
{
    QTableWidget *t = ui->tagTableWidget;
    int row = t->currentRow();
    if (row != -1) {
        QTableWidgetItem *checkBoxItem = t->item(row, 0);
        if (checkBoxItem->checkState()) {
            QString name = t->item(row, 0)->data(Qt::DisplayRole).toString();
            m_tags.remove(name);

            ui->gisView->setTags(m_tags);
//            ui->gisView->update();
            ui->gisView->viewport()->repaint();
        }
        t->removeRow(row);
        t->setCurrentCell(-1, -1);
    }
}

void MainWindow::on_tagTableWidget_cellClicked(int row, int column)
{
    if (column == 0) {
        QTableWidget * t = ui->tagTableWidget;
        QTableWidgetItem *checkBoxItem = t->item(row, 0);
        if (checkBoxItem->checkState()) {
            QString name = checkBoxItem->data(Qt::DisplayRole).toString();
            int x = t->item(row, 1)->data(Qt::DisplayRole).toInt();
            int y = t->item(row, 2)->data(Qt::DisplayRole).toInt();
            m_tags[name] = QPoint(x, y);

        } else {
            QString name = checkBoxItem->data(Qt::DisplayRole).toString();
            if (m_tags.contains(name)) {
                m_tags.remove(name);
            }
        }
        ui->gisView->setTags(m_tags);
//        ui->gisView->update();
        ui->gisView->viewport()->repaint();
    }
}

void MainWindow::on_openMap_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "",
                                                    tr("Images (*.png *.jpg)"));
    if (!fileName.isEmpty())
        ui->gisView->setImage(QImage(fileName));
}
