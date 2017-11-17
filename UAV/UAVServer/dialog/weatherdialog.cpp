#include "weatherdialog.h"
#include "ui_weatherdialog.h"
#include "mainwindow.h"
#include "QDebug"
WeatherDialog::WeatherDialog(const QString &str, bool checked, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WeatherDialog)
{
    ui->setupUi(this);
    m_parent = parent;
    ui->showCheckBox->setChecked(checked);
    ui->infoTextEdit->setText(str);
}

WeatherDialog::~WeatherDialog()
{
    delete ui;
}

void WeatherDialog::on_pushButton_clicked()
{
    QString str = ui->infoTextEdit->toPlainText();
    bool checked = ui->showCheckBox->checkState();
    ((MainWindow*)m_parent)->setWeather(str, checked);
    accept();
}
