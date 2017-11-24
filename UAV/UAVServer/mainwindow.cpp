#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog/uavtypedatadialog.h"
#include "dialog/weatherdialog.h"
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
    m_tcpserver(NULL),
    m_isShowWeather(false)
{
    ui->setupUi(this);
    QString imageDir = FileSystem::directoryOf("images").absoluteFilePath("Europe_topic_image_Satellite_image.jpg");
    QImage image(imageDir);

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

    m_normalPixmap = QPixmap("://images/icos/normal.ico").scaled(30, 30);
    m_alertPixmap = QPixmap("://images/icos/alert.ico").scaled(30, 30);
    ui->workspaceWarningLabel->setPixmap(m_normalPixmap);
    ui->altitudeWarningLabel->setPixmap(m_normalPixmap);
    ui->statusbar->showMessage("无人机连接数量0|");

    m_pathSamplingTimer = startTimer(5000);
}


MainWindow::~MainWindow()
{
    delete ui;
    if (m_pathSamplingTimer) killTimer(m_pathSamplingTimer);
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
    QtJson::JsonArray weapons = result["weapons"].toList();
    instantiateWeapons(weapons);

    QtJson::JsonArray detections = result["detectionEquipments"].toList();
    instantiateDetections(detections);

    return true;
}

void MainWindow::labelDisplayImage(QLabel *label, const QPixmap &pixmap)
{
    QPixmap p;
    if (!pixmap.isNull()) {
        p = pixmap.scaled(100, 100);
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

void MainWindow::addUAVStatusComboxItem(int id, const UAVStatus &status)
{
    QString title = m_uavs[status.index()].name();
    title += "#";
    title += QString::number(id);
    ui->uavStatusComboBox->addItem(title);
}

/*
void MainWindow::addUAVStatusTab(int id, const UAVStatus &status)
{
    QWidget *widget = new QWidget();
    QLabel *l1 = new QLabel(tr("俯仰:"));
    QLabel *l2 = new QLabel(tr("横滚:"));
    QLabel *l3 = new QLabel(tr("偏航:"));
    QLabel *l4 = new QLabel(tr("油门:"));
    QLabel *l5 = new QLabel(tr("空速:"));
    QLabel *l6 = new QLabel(tr("高度:"));
    QLineEdit *pitch = new QLineEdit(QString::number(status.pitch()));
    QLineEdit *roll = new QLineEdit(QString::number(status.roll()));
    QLineEdit *yaw = new QLineEdit(QString::number(status.yaw()));
    QLineEdit *acc = new QLineEdit(QString::number(status.accelerator()));
    QLineEdit *airSpeed = new QLineEdit(QString::number(status.airSpeed()));
    QLineEdit *altitude = new QLineEdit(QString::number(status.altitude()));

    pitch->setObjectName("pitch");
    roll->setObjectName("roll");
    yaw->setObjectName("yaw");
    acc->setObjectName("acc");
    airSpeed->setObjectName("airSpeed");
    altitude->setObjectName("altitude");
    pitch->setDisabled(true);
    roll->setDisabled(true);
    yaw->setDisabled(true);
    acc->setDisabled(true);
    airSpeed->setDisabled(true);
    altitude->setDisabled(true);

    QHBoxLayout *hl1 = new QHBoxLayout();
    hl1->addWidget(l1);
    hl1->addWidget(pitch);

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

    QVBoxLayout *vLayout1 = new QVBoxLayout();
    vLayout1->addLayout(hl1);
    vLayout1->addLayout(hl2);
    vLayout1->addLayout(hl3);
    vLayout1->addLayout(hl4);
//    vLayout1->addLayout(hl5);
//    vLayout1->addLayout(hl6);

    QHBoxLayout *hl7 = new QHBoxLayout();
    QLabel *l7 = new QLabel(tr("高度报警:"));
    QLabel *l71 = new QLabel();
    l71->setPixmap(m_normalPixmap);
    l71->setObjectName("altitudeWarning");
    hl7->addWidget(l7);
    hl7->addWidget(l71);

    QHBoxLayout *hl8 = new QHBoxLayout();
    QLabel *l8 = new QLabel(tr("作业范围报警:"));
    QLabel *l81 = new QLabel();
    l81->setObjectName("workingRangeWarning");
    l81->setPixmap(m_normalPixmap);
    hl8->addWidget(l8);
    hl8->addWidget(l81);

    QVBoxLayout *vLayout2 = new QVBoxLayout();
    vLayout2->addItem(new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
    vLayout2->addLayout(hl5);
    vLayout2->addLayout(hl6);
    vLayout2->addLayout(hl7);
    vLayout2->addLayout(hl8);
    vLayout2->addItem(new QSpacerItem(20,10, QSizePolicy::Minimum, QSizePolicy::Expanding));

    QHBoxLayout *topHBoxLayout = new QHBoxLayout();
    topHBoxLayout->addLayout(vLayout1);
    topHBoxLayout->addLayout(vLayout2);
    widget->setLayout(topHBoxLayout);

    QString title = m_uavs[status.index()].name();
    title += "#";
    title += QString::number(id);
    m_idTabMap[id] = widget;
    ui->uavStatusTabWidget->addTab(widget, title);
}
*/
void MainWindow::addTag(const QString &name, const QString &x, const QString &y)
{
    QTableWidget *t = ui->tagTableWidget;
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
    }
}

void MainWindow::addPoint2TableWidget(const QString &x, const QString &y, QTableWidget *ptable)
{
    int row = ptable->rowCount();
    ptable->insertRow(row);
    QTableWidgetItem *itemX = new QTableWidgetItem(x);
    QTableWidgetItem *itemY = new QTableWidgetItem(y);
    ptable->setItem(row, 0, itemX);
    ptable->setItem(row, 1, itemY);
}

void MainWindow::updateStatusBar()
{
    QString str = QString("无人机连接数量%1|").arg(m_idUAVStatusMap.size());
    QMapIterator<int, QPoint> it(m_idPositionMap);
    while(it.hasNext()) {
        it.next();
        str.append(QString("无人机%1坐标为(%2,%3)|")
                   .arg(it.key())
                   .arg(it.value().x())
                   .arg(it.value().y()));
    }
    ui->statusbar->showMessage(str);
}

void MainWindow::closeListenServer()
{
    if (m_tcpserver->isListening())
        m_tcpserver->close();
}

void MainWindow::pathSampling()
{
    QMapIterator<int, QPoint> it(m_idPositionMap);
    while(it.hasNext()) {
        it.next();
        int id = it.key();
        QString *cache = m_pathCache[id];
        cache->append(QString::number(m_idPositionMap[id].x()));
        cache->append(" ");
        cache->append(QString::number(m_idPositionMap[id].y()));
        cache->append(" ");
        cache->append("-65535");
        cache->append("\n");
    }
}

void MainWindow::savePathSampling()
{
    QMapIterator<int, QString*> it(m_pathCache);
    while (it.hasNext()) {
        it.next();
        int id = it.key();
        QString filename = m_uavs[m_idUAVStatusMap[id].index()].name();
        filename = filename + "#" + QString::number(id) + ".txt";

        filename = FileSystem::directoryOf("data").absoluteFilePath(filename);
        FileSystem::saveFile(filename, *it.value());
    }
    qDeleteAll(m_pathCache);
    m_pathCache.clear();
}

void MainWindow::setWeather(const QString &str, bool checked)
{
    ui->gisView->setWeahter(str, checked);
    m_weatherInfo = str;
    m_isShowWeather = checked;
}


void MainWindow::on_uavslistWidget_itemClicked(QListWidgetItem *item)
{
    QVariant i = item->data(MyImgRole);
    QVariant d = item->data(MyDescRole);

//    ui->uavDescriptionLabel->setText(d.toString());
    ui->uavParameterPlainTextEdit->setPlainText(d.toString());
    labelDisplayImage(ui->uavImageLabel, i.value<QPixmap>());
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

    QString title = name + "#" + QString::number(id);

    if(ui->uavStatusComboBox->findText(title) == -1)
        addUAVStatusComboxItem(id, u);

    UAV uav = m_uavs[index];
    QPixmap pixmap = uav.pixmap();
    QString desc = uav.getParametersDesc();

    QListWidgetItem *item= new QListWidgetItem(title);
    QVariant i;
    QVariant d;
    i.setValue(pixmap);
    d.setValue(desc);
    item->setData(MyImgRole, i);
    item->setData(MyDescRole, d);
    ui->uavslistWidget->addItem(item);

    m_pathCache[id] = new QString();

    updateStatusBar();
}

void MainWindow::onUpdateUAVStatus(int id, qint64 frameNum, UAVStatus status )
{

    // 更新位置
    QPoint velocity(status.airSpeed()*sin(status.yaw() * 3.1415926 / 180),
                     -status.airSpeed()*cos(status.yaw() * 3.1415926 / 180));
    QPoint tmp = m_idPositionMap[id];
    tmp += velocity / 100;
    m_idPositionMap[id] = tmp;
    m_idYawMap[id] = status.yaw();
    ui->gisView->setIdLocationMap(m_idPositionMap);
    ui->gisView->setIdYawMap(m_idYawMap);
    ui->gisView->viewport()->repaint();

    m_idUAVStatusMap[id] = status;
    QString title = m_uavs[status.index()].name();
    title += "#";
    title += QString::number(id);
    if (ui->uavStatusComboBox->currentText() == title) {
        /*
        QWidget *w = m_idTabMap[id];

        w->findChild<QLineEdit*>("pitch")->setText(QString::number(status.pitch()));
        w->findChild<QLineEdit*>("roll")->setText(QString::number(status.roll()));
        w->findChild<QLineEdit*>("yaw")->setText(QString::number(status.yaw()));
        w->findChild<QLineEdit*>("acc")->setText(QString::number(status.accelerator()));
        w->findChild<QLineEdit*>("airSpeed")->setText(QString::number(status.airSpeed()));
        w->findChild<QLineEdit*>("altitude")->setText(QString::number(status.altitude()));
        */

        ui->pitchLineEdit->setText(QString::number(status.pitch()));
        ui->rollLineEdit->setText(QString::number(status.roll()));
        ui->yawLineEdit->setText(QString::number(status.yaw()));
        ui->accLineEdit->setText(QString::number(status.accelerator()));
        ui->airspeedLineEdit->setText(QString::number(status.airSpeed()));
        ui->altitudeLineEdit->setText(QString::number(status.altitude()));

        QLabel* workingRangeWarning = ui->workspaceWarningLabel;
        QRect workingRange = ui->gisView->getWorkingRange();
        if (workingRange.contains(tmp)) {
            workingRangeWarning->setPixmap(m_normalPixmap);
        } else {
            workingRangeWarning->setPixmap(m_alertPixmap);
        }

        QLabel* altitudeWarning = ui->altitudeWarningLabel;
        if (status.altitude() < m_uavs[status.index()].flightHeight()) {
            altitudeWarning->setPixmap(m_normalPixmap);
        } else {
            altitudeWarning->setPixmap(m_alertPixmap);
        }


    }

    updateStatusBar();
}

void MainWindow::onCloseByClient(int id)
{
    /*
    int len = ui->uavStatusTabWidget->count();
    for (int i = 0; i < len; ++i) {
        QString title = ui->uavStatusTabWidget->tabText(i);
        if (title.endsWith(QString::number(id))) {
//            m_idTabMap.remove(id);
            m_idUAVStatusMap.remove(id);
            m_idPositionMap.remove(id);
            m_idOriginPositionMap.remove(id);
            m_idYawMap.remove(id);

            ui->uavStatusTabWidget->removeTab(i);

            ui->gisView->setIdLocationMap(m_idPositionMap);
            ui->gisView->setIdYawMap(m_idYawMap);
            ui->gisView->viewport()->repaint();
            break;
        }
    }
    */
    UAVStatus status = m_idUAVStatusMap[id];
    QString title = m_uavs[status.index()].name();
    title += "#";
    title += QString::number(id);
    int index = ui->uavStatusComboBox->findText(title);
    if (index != -1) {
        ui->uavStatusComboBox->removeItem(index);
    }

    QListWidget *lp = ui->uavslistWidget;
    int rows = lp->count();
    for (int i = 0 ; i < rows; ++i) {
        QListWidgetItem *item = lp->item(i);
        QString name = item->data(Qt::DisplayRole).toString();
        if (name.endsWith(QString::number(id))) {
            delete lp->takeItem(lp->row(item));
        }
    }
    updateStatusBar();
}

void MainWindow::onDetectionDeviceStatusChanged(int id, QString deviceName, bool checked)
{
    qDebug() << id << deviceName << checked;
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
        addPoint2TableWidget(x, y, t);

        ui->gisView->setPath(m_path);
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
        ui->gisView->viewport()->repaint();
        t->setCurrentCell(-1, -1);
    }

}

void MainWindow::on_addTagPushButton_clicked()
{

    QString name = ui->tagNameLineEdit->text();
    QString x = ui->tagXLineEdit->text();
    QString y = ui->tagYLineEdit->text();

    addTag(name, x, y);
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

void MainWindow::on_uavParameter_triggered()
{
    UAVTypeDataDialog d(this);
    int result = d.exec();
    if (result == QDialog::Accepted) {
        updateUavMetaDataGroup();
    }
}

void MainWindow::on_weatherBtn_triggered()
{
    WeatherDialog d(m_weatherInfo, m_isShowWeather, this);
    int result = d.exec();
    if (result == QDialog::Accepted) {
    }
}

void MainWindow::on_importTagsBtn_clicked()
{
    QString filename = FileSystem::openOpenDialog();
    QString data = FileSystem::readFile(filename);
    QTextStream in(&data);
    QString line;
    do {
        line = in.readLine();
        if (!line.isEmpty()) {
            //跳过注释行
            if (line[0] == '%')
                continue;
            QStringList list = line.split(" ");
            if (list.size() < 3)
                continue;
            QString name = list[0];
            QString x = list[1];
            QString y = list[2];
            addTag(name, x, y);
        }
    } while (!line.isNull());
}

void MainWindow::on_exportTagsBtn_clicked()
{
    QString cache;
    QTextStream stream(&cache);
    QTextCodec *codec=QTextCodec::codecForName("UTF-8");
    stream.setCodec(codec);
    QString comment = codec->fromUnicode("%名称 x坐标 y坐标%" );
    stream << comment << "\n";
    QTableWidget *t = ui->tagTableWidget;
    int row = t->rowCount();
    for (int i = 0; i < row; ++i) {
        QString name = t->item(i, 0)->data(Qt::DisplayRole).toString();
        name = codec->fromUnicode(name);
        QString x = t->item(i, 1)->data(Qt::DisplayRole).toString();
        QString y = t->item(i, 2)->data(Qt::DisplayRole).toString();
        stream << name << " " << x << " " << y << "\n";
    }
    QString filename = FileSystem::openSaveDialog(this);
    FileSystem::saveFile(filename, cache);
}

void MainWindow::on_addAirspaceBtn_clicked()
{
    QLineEdit *l1 = ui->airspaceXLineEdit;
    QLineEdit *l2 = ui->airspaceYLineEdit;
    QString x = l1->text();
    QString y = l2->text();
    if (!x.isEmpty() &&
            !y.isEmpty()) {
        QPoint p(x.toInt(), y.toInt());
        m_airspace.append(p);

        QTableWidget *t = ui->airspaceTableWidget;
        addPoint2TableWidget(x, y, t);

        ui->gisView->setAirspace(m_airspace);
        ui->gisView->viewport()->repaint();
    }
}

void MainWindow::on_delAirspaceBtn_clicked()
{
    QTableWidget *t = ui->airspaceTableWidget;
    if (t->currentRow() != -1) {
        m_airspace.remove(t->currentRow());
        t->removeRow(t->currentRow());
        ui->gisView->setAirspace(m_airspace);
        ui->gisView->viewport()->repaint();
        t->setCurrentCell(-1, -1);
    }
}

void MainWindow::on_exportPathAirspaceBtn_clicked()
{
    QString cache;
    QTextStream stream(&cache);
    QTextCodec *codec=QTextCodec::codecForName("UTF-8");
    stream.setCodec(codec);
    QString pathPlan = codec->fromUnicode("%规划路径%" );
    QString comment = codec->fromUnicode("%x坐标 y坐标 z坐标%" );
    stream << comment << "\n";
    stream << pathPlan << "\n";
    QTableWidget *t = ui->pathTableWidget;
    int row = t->rowCount();
    for (int i = 0; i < row; ++i) {
        QString x = t->item(i, 0)->data(Qt::DisplayRole).toString();
        QString y = t->item(i, 1)->data(Qt::DisplayRole).toString();
        stream << x << " " << y << " " << "-65535" << "\n";
    }

    QString airspace = codec->fromUnicode("%可行空域%" );
    stream << airspace << "\n";
    t = ui->airspaceTableWidget;
    row = t->rowCount();
    for (int i = 0; i < row; ++i) {
        QString x = t->item(i, 0)->data(Qt::DisplayRole).toString();
        QString y = t->item(i, 1)->data(Qt::DisplayRole).toString();
        stream << x << " " << y << " " << "-65535" << "\n";
    }

    QString realPath = codec->fromUnicode("%实际路径%" );
    stream << realPath << "\n";
    t = ui->realPathTableWidget;
    row = t->rowCount();
    for (int i = 0; i < row; ++i) {
        QString x = t->item(i, 0)->data(Qt::DisplayRole).toString();
        QString y = t->item(i, 1)->data(Qt::DisplayRole).toString();
        stream << x << " " << y << " " << "-65535" << "\n";
    }
    QString filename = FileSystem::openSaveDialog(this);
    FileSystem::saveFile(filename, cache);
}

void MainWindow::on_importPathAirspaceBtn_clicked()
{
    QString filename = FileSystem::openOpenDialog();
    QString data = FileSystem::readFile(filename);
    QTextStream in(&data);

    QVector<QPoint> *pStore = NULL;
    QTableWidget *t = NULL;
    QString line;
    do {
        line = in.readLine();
        if (!line.isEmpty()) {
            if (line == "%规划路径%") {
                pStore = &m_path;
                t = ui->pathTableWidget;
            } else if (line == "%可行空域%") {
                pStore = &m_airspace;
                t = ui->airspaceTableWidget;
            } else if (line == "%实际路径%") {
                pStore = &m_realpath;
                t = ui->realPathTableWidget;
            }
            //跳过注释行
            if (line[0] == '%')
                continue;

            QStringList list = line.split(" ");
            if (list.size() < 3)
                continue;
            QString x = list[0];
            QString y = list[1];
            QString z = list[2];
            pStore->append(QPoint(x.toInt(), y.toInt()));
            addPoint2TableWidget(x, y, t);
        }
    } while (!line.isNull());
    ui->gisView->setPath(m_path);
    ui->gisView->setAirspace(m_airspace);
    ui->gisView->viewport()->repaint();
}

void MainWindow::on_openServerBtn_clicked(bool checked)
{
    if (!checked) {
        listenServer();
        ui->openServerBtn->setIcon(QIcon("://images/icos/connected.png"));
        ui->openServerBtn->setText(tr("等待连接..."));
    } else {
        closeListenServer();
        ui->openServerBtn->setIcon(QIcon("://images/icos/disconnected.png"));
        ui->openServerBtn->setText(tr("开启连接"));
    }
    ui->openServerBtn->setCheckable(!checked);
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    QMainWindow::timerEvent(event);

    if (event->timerId() == m_pathSamplingTimer) {
        pathSampling();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    savePathSampling();
    event->accept();
}
