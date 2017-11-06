#include "maindialog.h"
#include "ui_maindialog.h"
#include <math.h>
#include <QtGui>

MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainDialog),
    m_timerId(0),
    m_realTime(0.0f),
    m_airspeed(125.0f),
    m_altitude(5000.0f),
    m_Status_pitch(0.0f),
    m_Status_roll(0.0f),
    m_nextBlockSize(0),
    m_frameNum(0),
    m_id(-1),
    m_index(-1),
    PI(3.1415926)
{
    ui->setupUi(this);


//    QTimer *timer = new QTimer(this);

//    timer->start(1000);
    ui->weapenTreeWidget->setCurrentIndex(QModelIndex());

    m_timerId = startTimer(1000 / 10);
    m_time.start();

    ui->cameraButton->setButtonStyle(SwitchButton::ButtonStyle_CircleIn);
    ui->cameraButton->setText("Off", "On");
    ui->cameraButton->setTextColor(Qt::red, Qt::green);

    ui->radarButton->setButtonStyle(SwitchButton::ButtonStyle_CircleIn);
    ui->radarButton->setText("Off", "On");
    ui->radarButton->setTextColor(Qt::red, Qt::green);

//    ui->cameraButton->setStyleSheet("background-color: transparent;border-image: url(:/buttons/images/buttons/onbutton);background: none;border: none;background-repeat: none;");
    QPixmap cameraPixmap = QPixmap(":/images/scout/camera").scaled(50, 50, Qt::IgnoreAspectRatio, Qt::FastTransformation);
//    QPainter painter(&cameraPixmap);
//    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
//    painter.fillRect(cameraPixmap.rect(), Qt::red);
//    painter.end();
    ui->cameraLabel->setPixmap(cameraPixmap);

    QPixmap radarPixmap = QPixmap(":/images/scout/radar").scaled(50, 50, Qt::IgnoreAspectRatio, Qt::FastTransformation);
//    QPainter painter2(&radarPixmap);
//    painter2.setCompositionMode(QPainter::CompositionMode_SourceIn);
//    painter2.fillRect(radarPixmap.rect(), Qt::red);
//    painter2.end();
    ui->radarLabel->setPixmap(radarPixmap);

    QObject::connect(ui->UAV_rollAndPitchController, SIGNAL(rollAndPitch(float, float)), this, SLOT(onRollAndPitch(float, float)));

    initPFD();

//    connect(&m_tcpSocket, SIGNAL(connected()), this, SLOT(sendUAVStatus()));
    connect(&m_tcpSocket, SIGNAL(disconnected()), this, SLOT(onConnectionClosed()));
    connect(&m_tcpSocket, SIGNAL(readyRead()), this, SLOT(updateWidgets()));
    connect(&m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));

}

MainDialog::~MainDialog()
{
    delete ui;
    if (m_timerId) killTimer(m_timerId);
//    QObject::disconnect(ui->UAV_rollAndPitchController, SIGNAL(rollAndPitch(float, float)), this, SLOT(onRollAndPitch(float, float)));
    if (m_tcpSocket.isOpen()) m_tcpSocket.close();
}

void MainDialog::onRollAndPitch(float roll, float pitch)
{
    ui->widgetPFD->setRoll(roll);
    ui->widgetPFD->setPitch(pitch);
    ui->widgetPFD->update();
    ui->rollEdit->setText( QString::number(roll));
    ui->pitchEdit->setText(QString::number(pitch));
    m_Status_pitch = pitch;
    m_Status_roll = roll;
    //调整roll的同时，计算yaw

    //if(yaw)
}


void MainDialog::timerEvent(QTimerEvent *event)
{
    QDialog::timerEvent(event);


    float timeStep = m_time.restart();
    m_realTime = m_realTime + timeStep / 1000.0f;

//    float heading = 0.0f;
//    float pressure = 0.0f;
    float machNo = 0.0f;
    float altitude = 0.0f;
    float climbRate = 0.0f;
//    pressure = 2.0f * sin(m_realTime / 20.0f);

    m_Status_roll = 180.0f * sin(m_realTime / 10.0f);
//    heading = 360.0f * sin(m_realTime / 40.0f);
    //float horizontalSpeed;
    float speeddelta = m_realTime / 100.0f* (ui->acceleratorSlider->value()-50);
    m_airspeed = (m_airspeed + speeddelta) > 125.0f ? (m_airspeed +speeddelta) : 125.0f;
    m_altitude = m_altitude + m_airspeed * tan(m_Status_pitch/360*2*PI);

    /*
    float accelerationN = 100 * sin(m_Status_roll);
    QPointF At(speeddelta*sin(m_currentUAVstatus.yaw()*2*PI/360), speeddelta*cos(m_currentUAVstatus.yaw()*2*PI/360));

    QPointF An(accelerationN*sin((m_currentUAVstatus.yaw()+90)), accelerationN*cos((m_currentUAVstatus.yaw()+90)));

    QPointF Velocity(m_airspeed * sin(m_currentUAVstatus.yaw()), m_airspeed * cos(m_currentUAVstatus.yaw()));
    Velocity+= (An + At);
    float tmpyaw = qAtan(Velocity.x()/Velocity.y())/(2*PI)*360;
    if(Velocity.y()>0)
    {
        ui->yawSlider->setValue(tmpyaw);
    }else
    {
        if (tmpyaw<0)
        {
            ui->yawSlider->setValue(-180-tmpyaw);
        }else
        {
            ui->yawSlider->setValue(180-tmpyaw);
        }
    }
    //yaw = qAtan(Velocity.x()/Velocity.y());


*/

    machNo = m_airspeed / 650.0f;

    climbRate = 650.0f * sin(m_realTime / 20.0f);

    ui->speedEdit->setText(QString::number(m_airspeed));
    ui->widgetPFD->setAirspeed(m_airspeed);
//    ui->widgetPFD->setPressure(pressure);
    ui->widgetPFD->setMachNo(machNo);
    ui->widgetPFD->setAltitude(m_altitude);
    ui->widgetPFD->setClimbRate(climbRate);
    ui->widgetPFD->update();

    if (m_tcpSocket.isOpen()) {
        UAVStatus status(m_Status_roll,
                         ui->yawSlider->value(),
                         m_Status_pitch,
                         ui->acceleratorSlider->value(),
                         m_airspeed,
                         m_altitude,
                         0.0f,
                         0.0f,
                         index(),
                         QTime(),
                         m_uav.weapon());
        sendUAVStatus(status);
        m_currentUAVstatus = status;
    }
}

void MainDialog::initPFD()
{
    WidgetPFD *pfd = ui->widgetPFD;
    pfd->setAirspeed(0.0f);
    pfd->setRoll(0.0f);
    pfd->setHeading(0.0f);
    pfd->setPitch(0.0f);
    pfd->setMachNo(0.0f);
    pfd->setClimbRate(0.0f);
    pfd->setAltitude(0.0f);
    pfd->setPressure(0.0f);
    pfd->update();
}

void MainDialog::initWeaponGroup()
{
    QTreeWidget *t = ui->weapenTreeWidget;
    t->clear();
    int index = 0;
    QMapIterator<QString, int> i(m_uav.weapon());
    while(i.hasNext()) {
        i.next();
        QTreeWidgetItem *item= new QTreeWidgetItem();
        item->setText(0, i.key());
        item->setText(1, QString::number(i.value()));
        t->insertTopLevelItem(index++, item);
    }
}

void MainDialog::initUAVInfoGroup()
{
    QPixmap p = m_uav.pixmap().scaled(50,50);
    ui->uavImageLabel->setPixmap(p);

    QTreeWidget *t = ui->uavTreeWidget;
    t->clear();
    int index = 0;
    QTreeWidgetItem *i1= new QTreeWidgetItem();
    i1->setText(0, "自重kg");
    i1->setText(1, QString::number(m_uav.weight()));
    t->insertTopLevelItem(index++, i1);
    QTreeWidgetItem *i2= new QTreeWidgetItem();
    i2->setText(0, "航程km");
    i2->setText(1, QString::number(m_uav.voyage()));
    t->insertTopLevelItem(index++, i2);
    QTreeWidgetItem *i3= new QTreeWidgetItem();
    i3->setText(0, "航高m");
    i3->setText(1, QString::number(m_uav.flightHeight()));
    t->insertTopLevelItem(index++, i3);
    QTreeWidgetItem *i4= new QTreeWidgetItem();
    i4->setText(0, "续航h");
    i4->setText(1, QString::number(m_uav.flyEndurance()));
    t->insertTopLevelItem(index++, i4);
    QTreeWidgetItem *i5= new QTreeWidgetItem();
    i5->setText(0, "最高时速km/h");
    i5->setText(1, QString::number(m_uav.maxSpeed()));
    t->insertTopLevelItem(index++, i5);
    QTreeWidgetItem *i6= new QTreeWidgetItem();
    i6->setText(0, "提速加速度km/s^2");
    i6->setText(1, QString::number(m_uav.acceleration()));
    t->insertTopLevelItem(index++, i6);
    QTreeWidgetItem *i7= new QTreeWidgetItem();
    i7->setText(0, "载荷kg");
    i7->setText(1, QString::number(m_uav.loadWeight()));
    t->insertTopLevelItem(index++, i7);
}

void MainDialog::closeConnection()
{
    m_tcpSocket.close();
}

void MainDialog::sendUAVStatus(const UAVStatus &status)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << qint64(0)
        << QString("P2")
        << quint8(m_id)
        << m_frameNum++
        << status;
    out.device()->seek(0);
    out << qint64(block.size() - sizeof(qint64));
    m_tcpSocket.write(block);
}

void MainDialog::sendCloseSign()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out << qint64(0)
        << QString("P3")
        << quint8(m_id);
    out.device()->seek(0);
    out << qint64(block.size() - sizeof(qint64));
    m_tcpSocket.write(block);
    m_tcpSocket.flush();
}

void MainDialog::closeEvent(QCloseEvent *event)
{
    switch( QMessageBox::information( this, tr("退出"),
                                      tr("确定要退出吗?"),
                                      tr("是"), tr("否"),
                                      0, 1 ) )
    {
    case 0:
        sendCloseSign();
        event->accept();
        break;
    case 1:
    default:
        event->ignore();
        break;
    }
}

void MainDialog::connectToHost()
{
    if (!m_tcpSocket.isOpen()) {
        m_tcpSocket.connectToHost(m_ip, m_port);
    }
}

void MainDialog::onConnectionClosed()
{
    if (m_nextBlockSize != 0xFFFFFFFF) {

    }
    closeConnection();
}

void MainDialog::updateWidgets()
{

}

void MainDialog::error()
{
    closeConnection();
}

int MainDialog::index() const
{
    return m_index;
}

void MainDialog::setIndex(int index)
{
    m_index = index;
}

int MainDialog::id() const
{
    return m_id;
}

void MainDialog::setId(int id)
{
    m_id = id;
}

int MainDialog::port() const
{
    return m_port;
}

void MainDialog::setPort(int port)
{
    m_port = port;
}

QString MainDialog::ip() const
{
    return m_ip;
}

void MainDialog::setIp(const QString &ip)
{
    m_ip = ip;
}

QVector<Weapon> MainDialog::weapons() const
{
    return m_weapons;
}

void MainDialog::setWeapons(const QVector<Weapon> &weapons)
{
    m_weapons = weapons;
    initWeaponGroup();
}

UAV MainDialog::uav() const
{
    return m_uav;
}

void MainDialog::setUav(const UAV &uav)
{
    m_uav = uav;
    initUAVInfoGroup();
}



void MainDialog::on_fireButton_clicked()
{
    QTreeWidgetItem* i = ui->weapenTreeWidget->currentItem();
    if (i)
        i->setData(1, Qt::DisplayRole, QString::number(i->data(1, Qt::DisplayRole).toInt() - 1));
}

void MainDialog::on_yawSlider_valueChanged(int value)
{
    ui->widgetPFD->setHeading(value);
    ui->widgetPFD->update();
    ui->yawEdit->setText(QString::number(value));
}

