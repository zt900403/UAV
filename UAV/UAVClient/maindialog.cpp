#include "maindialog.h"
#include "ui_maindialog.h"
#include <math.h>
#include <QtGui>

MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainDialog),
    m_timerId(0),
    m_realTime(0.0f)
{
    ui->setupUi(this);
//    QTimer *timer = new QTimer(this);
//    connect(timer, SIGNAL(timeout()), this, SLOT(pfd_update()));
//    timer->start(1000);
    ui->weapenTreeWidget->setCurrentIndex(QModelIndex());

    m_timerId = startTimer(1000 / 30);
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

//    ui->accelerator->setMaximum(100);
//    ui->accelerator->setMinimum(0);
//    ui->accelerator->setSingleStep(20);
//    ui->accelerator->setTickPosition(QSlider::TicksRight);
}

MainDialog::~MainDialog()
{
    delete ui;
    if (m_timerId) killTimer(m_timerId);
}


void MainDialog::timerEvent(QTimerEvent *event)
{
    QDialog::timerEvent(event);
    float timeStep = m_time.restart();
    m_realTime = m_realTime + timeStep / 1000.0f;
    float roll = 0.0f;
    float pitch = 0.0f;
    float heading = 0.0f;
    float airspeed = 0.0f;
    float pressure = 0.0f;
    float machNo = 0.0f;
    float altitude = 0.0f;
    float climbRate = 0.0f;

    roll = 180.0f * sin(m_realTime / 10.0f);
    pitch = 90.0f * sin(m_realTime / 20.0f);
    heading = 360.0f * sin(m_realTime / 40.0f);
    airspeed = 125.0f * sin(m_realTime / 40.0f);
    pressure = 2.0f * sin(m_realTime / 20.0f);
    machNo = airspeed / 650.0f;
    altitude = 9000.0f * sin(m_realTime / 40.0f);
    climbRate = 650.0f * sin(m_realTime / 20.0f);

    ui->widgetPFD->setRoll(roll);
    ui->widgetPFD->setPitch(pitch);
    ui->widgetPFD->setHeading(heading);
    ui->widgetPFD->setAirspeed(airspeed);
    ui->widgetPFD->setPressure(pressure);
    ui->widgetPFD->setMachNo(machNo);
    ui->widgetPFD->setAltitude(altitude);
    ui->widgetPFD->setClimbRate(climbRate);
    ui->widgetPFD->update();
}



void MainDialog::on_fireButton_clicked()
{
    QTreeWidgetItem* i = ui->weapenTreeWidget->currentItem();
    if (i)
        i->setData(1, Qt::DisplayRole, QString::number(i->data(1, Qt::DisplayRole).toInt() - 1));
}
