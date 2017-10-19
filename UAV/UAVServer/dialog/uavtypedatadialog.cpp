#include "uavtypedatadialog.h"
#include "ui_uavtypedatadialog.h"
#include <QString>
#include <QDir>

#include "utils/filesystem.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDebug>

UAVTypeDataDialog::UAVTypeDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UAVTypeDataDialog)
{
    ui->setupUi(this);
    updateAllData();
}

UAVTypeDataDialog::~UAVTypeDataDialog()
{
    delete ui;
}

void UAVTypeDataDialog::updateAllData()
{
    initWidgets();

    QString file = FileSystem::directoryOf("config").absoluteFilePath("uvatype.json");
    QString json = FileSystem::readFile(file);
    if (json.isEmpty()) {
        qFatal("Could not read uavtype.json file!");
        return ;
    }
    bool ok;
    QtJson::JsonObject result = QtJson::parse(json, ok).toMap();
    if (!ok) {
        qFatal("Could not parse uavtype.json file! json syntax is wrong!");
        return ;
    }

    m_uavs = result["uavs"].toList();
    QTableWidget *table = ui->uavTableWidget;


    foreach(QVariant var, m_uavs) {
        QtJson::JsonObject obj = var.toMap();
        int i = table->rowCount();
        table->insertRow(i);
        int j = 0;
        QString name = obj["name"].toString();
        table->setItem(i, j++, new QTableWidgetItem(name));
        table->setItem(i, j++, new QTableWidgetItem(obj["weight"].toString()));
        table->setItem(i, j++, new QTableWidgetItem(obj["voyage"].toString()));
        table->setItem(i, j++, new QTableWidgetItem(obj["flightHeight"].toString()));
        table->setItem(i, j++, new QTableWidgetItem(obj["flyEndurance"].toString()));
        table->setItem(i, j++, new QTableWidgetItem(obj["maxSpeed"].toString()));
        table->setItem(i, j++, new QTableWidgetItem(obj["acceleration"].toString()));
        table->setItem(i, j++, new QTableWidgetItem(obj["loadWeight"].toString()));
        table->setItem(i, j++, new QTableWidgetItem(obj["imageFile"].toString()));
        table->setItem(i, j++, new QTableWidgetItem(obj["description"].toString()));

        ui->uavTypeComboBox->addItem(name);
    }

    m_weapons = result["weapons"].toList();

    table = ui->weaponTypeDataTableWidget;

    foreach(QVariant var, m_weapons) {
        QtJson::JsonObject obj = var.toMap();
        int i = table->rowCount();
        table->insertRow(i);
        int j = 0;
        table->setItem(i, j++, new QTableWidgetItem(obj["name"].toString()));
        table->setItem(i, j++, new QTableWidgetItem(obj["R_L"].toString()));
        table->setItem(i, j++, new QTableWidgetItem(obj["weight"].toString()));
        table->setItem(i, j++, new QTableWidgetItem(obj["guidedType"].toString()));
        table->setItem(i, j++, new QTableWidgetItem(obj["fieldOfFire"].toString()));
        table->setItem(i, j++, new QTableWidgetItem(obj["killRadius"].toString()));
        table->setItem(i, j++, new QTableWidgetItem(obj["description"].toString()));
    }
}

void UAVTypeDataDialog::initWidgets()
{
    ui->uavTableWidget->clearContents();
    ui->uavTableWidget->setRowCount(0);
    QStringList list;
    list << "名称"
         << "自重kg"
         << "航程km"
         << "航高m"
         << "续航h"
         << "最高时速km/h"
         << "提速加速度km/s^2"
         << "载荷kg"
         << "文件"
         << "描述";
    ui->uavTableWidget->setHorizontalHeaderLabels(list);

    ui->uavTypeComboBox->clear();

    ui->currentWeaponTableWidget->clearContents();
    ui->currentWeaponTableWidget->setRowCount(0);
    QStringList curWeapons;
    curWeapons << "名称" << "数量";
    ui->currentWeaponTableWidget->setHorizontalHeaderLabels(curWeapons);

    ui->weaponTypeDataTableWidget->clearContents();
    ui->weaponTypeDataTableWidget->setRowCount(0);
    QStringList weapons;
    weapons << "名称"
            << "尺寸 m"
            << "重量 kg"
            << "制导方式"
            << "有效射程Km"
            << "杀伤半径m"
            << "描述";
    ui->weaponTypeDataTableWidget->setHorizontalHeaderLabels(weapons);
}



void UAVTypeDataDialog::on_uavTypeComboBox_currentIndexChanged(const QString &arg1)
{
    ui->currentWeaponTableWidget->clearContents();
    ui->currentWeaponTableWidget->setRowCount(0);

    QStringList curWeapons;
    curWeapons << "名称" << "数量";
    ui->currentWeaponTableWidget->setHorizontalHeaderLabels(curWeapons);

    QtJson::JsonArray weapons;
    foreach(QVariant var, m_uavs) {
        QtJson::JsonObject obj = var.toMap();
        if (obj["name"].toString() == arg1) {
            weapons = obj["weapon"].toList();
            break;
        }
    }
    QTableWidget *table = ui->currentWeaponTableWidget;

    foreach(QVariant var, weapons) {
        QtJson::JsonObject obj = var.toMap();
        int i = table->rowCount();
        table->insertRow(i);
        int j = 0;
        table->setItem(i, j++, new QTableWidgetItem(obj["name"].toString()));
        table->setItem(i, j++, new QTableWidgetItem(obj["amount"].toString()));
    }
}
