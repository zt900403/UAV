#include "maindialog.h"
#include "ui_maindialog.h"


#include <QImage>
#include <QtGui>
MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainDialog)
{
    ui->setupUi(this);
    QImage image = QImage("C:\\Users\\zhang\\Desktop\\QT\\UAV\\UVAServer\\Europe_topic_image_Satellite_image.jpg");
    ui->gisView->setImage(image);
    ui->gisView->setBackgroundBrush(QBrush(QColor(0x7F,0x7F,0x7F)));
}

MainDialog::~MainDialog()
{
    delete ui;
}
