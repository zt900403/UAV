#include "maindialog.h"
#include "ui_maindialog.h"
#include <math.h>
#include <QtGui>
float airspeed = 125.0f;
float altitude = 5000.0f;
float Status_pitch = 0.0f;
float Status_roll = 0.0f;

MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainDialog),
    m_timerId(0),
    m_realTime(0.0f)
{
    ui->setupUi(this);

    initPFD();

    QTimer *timer = new QTimer(this);

    timer->start(1000);
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

    QObject::connect(ui->UAV_rollAndPitchController, SIGNAL(rollAndPitch(float, float)), this, SLOT(onRollAndPitch(float, float)));

}

MainDialog::~MainDialog()
{
    delete ui;
    if (m_timerId) killTimer(m_timerId);
//    QObject::disconnect(ui->UAV_rollAndPitchController, SIGNAL(rollAndPitch(float, float)), this, SLOT(onRollAndPitch(float, float)));
}

void MainDialog::onRollAndPitch(float roll, float pitch)
{
    ui->widgetPFD->setRoll(roll);
    ui->widgetPFD->setPitch(pitch);
    ui->widgetPFD->update();
    ui->rollEdit->setText( QString::number(roll));
    ui->pitchEdit->setText(QString::number(pitch));
    Status_pitch = pitch;
    Status_roll = roll;
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

    Status_roll = 180.0f * sin(m_realTime / 10.0f);
//    heading = 360.0f * sin(m_realTime / 40.0f);
    float horizontalSpeed;
    float seedtmp = m_realTime / 100.0f* (ui->acceleratorSlider->value()-50);
    airspeed = (airspeed + seedtmp) > 125.0f ? (airspeed +seedtmp) : 125.0f;
    altitude = altitude + airspeed * sin(Status_pitch/360*2*3.1415926);
    horizontalSpeed = airspeed * cos(Status_pitch/360*2*3.1415926);

    machNo = airspeed / 650.0f;

    climbRate = 650.0f * sin(m_realTime / 20.0f);

    ui->speedEdit->setText(QString::number(airspeed));
    ui->widgetPFD->setAirspeed(airspeed);
//    ui->widgetPFD->setPressure(pressure);
    ui->widgetPFD->setMachNo(machNo);
    ui->widgetPFD->setAltitude(altitude);
    ui->widgetPFD->setClimbRate(climbRate);
    ui->widgetPFD->update();
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

