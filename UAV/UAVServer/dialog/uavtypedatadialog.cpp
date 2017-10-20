#include "uavtypedatadialog.h"
#include "ui_uavtypedatadialog.h"
#include <QString>
#include <QDir>

#include "utils/filesystem.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDebug>
#include <QMessageBox>

UAVTypeDataDialog::UAVTypeDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UAVTypeDataDialog)
{
    ui->setupUi(this);
    createUAVTableHeader();
    updateAllTableWithFile();
}

UAVTypeDataDialog::~UAVTypeDataDialog()
{
    delete ui;
}

void UAVTypeDataDialog::updateAllTableWithFile()
{

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
    m_weapons = result["weapons"].toList();

    updateAllTable();
}

void UAVTypeDataDialog::clearAllTable()
{
    ui->uavTableWidget->clearContents();
    ui->uavTableWidget->setRowCount(0);

    ui->uavTypeComboBox->clear();

    ui->currentWeaponTableWidget->clearContents();
    ui->currentWeaponTableWidget->setRowCount(0);

    ui->weaponTypeDataTableWidget->clearContents();
    ui->weaponTypeDataTableWidget->setRowCount(0);
}

void UAVTypeDataDialog::updateUAVComboBox()
{
    ui->uavTypeComboBox->clear();
    qDebug() << m_uavs.size();
    foreach(QVariant var, m_uavs) {
        QtJson::JsonObject obj = var.toMap();
        QString name = obj["name"].toString();
        ui->uavTypeComboBox->addItem(name);
    }
}

void UAVTypeDataDialog::delTableWidgetRow(QTableWidget *t, const int row)
{
    if (row != -1) {
        t->removeRow(row);
        t->setCurrentCell(-1, -1);
    }
}

int UAVTypeDataDialog::indexOfObjectValue(const QtJson::JsonArray &array, const QString &key, const QString &value)
{
    int i = 0;
    foreach(QVariant var, array) {
        if (var.toMap()[key].toString() == value) {
            return i;
        }
        ++i;
    }
    return -1;
}

int UAVTypeDataDialog::indexOfItemValue(const QTableWidget *table, const int col, const QString &value)
{
    int index = 0;
    int len = table->rowCount();
    for (int i = 0; i < len; ++i) {
        QString target = table->item(i, col)->data(Qt::DisplayRole).toString();
        if (target == value) {
            return index;
        }
        ++index;
    }
    return -1;
}

QString UAVTypeDataDialog::randomString(int len) const
{
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    const int randomStringLength = len; // assuming you want random strings of 12 characters

    QString randomString;
    for(int i=0; i<randomStringLength; ++i)
    {
        int index = qrand() % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}
QString UAVTypeDataDialog::randomString2TableItem(QTableWidget *table, int row, int col)
{
    QString value;
    if (row == 0) {
        do {
            value = randomString(5);
        } while (indexOfItemValue(table, row, value));
    } else {
        value = randomString(5);
    }
    QTableWidgetItem *i = table->item(row, col);
    if (!i) {
        i = new QTableWidgetItem();
        table->setItem(row, col, i);
    }
    i->setData(Qt::DisplayRole, value);
    return value;
}


void UAVTypeDataDialog::on_uavTypeComboBox_currentIndexChanged(const QString &arg1)
{
    ui->currentWeaponTableWidget->clearContents();
    ui->currentWeaponTableWidget->setRowCount(0);

//    QStringList curWeapons;
//    curWeapons << "名称" << "数量";
//    ui->currentWeaponTableWidget->setHorizontalHeaderLabels(curWeapons);

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

void UAVTypeDataDialog::on_addUAVTypeBtn_clicked()
{
    QTableWidget *t = ui->uavTableWidget;
    int row = t->rowCount();
    t->insertRow(row);
    QString value = randomString2TableItem(t, row, 0);
    QtJson::JsonObject obj;
    obj["name"] = value;
    m_uavs.append(obj);
    updateUAVComboBox();
}

void UAVTypeDataDialog::on_addWeaponTypeBtn_clicked()
{
    QTableWidget *t = ui->weaponTypeDataTableWidget;
    t->insertRow(t->rowCount());
}

void UAVTypeDataDialog::on_delUAVTypeBtn_clicked()
{
    QTableWidget *t = ui->uavTableWidget;
    int cur = t->currentRow();
    delTableWidgetRow(t, cur);
    m_uavs.removeAt(cur);
    updateUAVComboBox();
}

void UAVTypeDataDialog::on_delWeaponTypeBtn_clicked()
{
    QTableWidget *t = ui->weaponTypeDataTableWidget;
    int cur = t->currentRow();
    delTableWidgetRow(t, cur);
}

void UAVTypeDataDialog::on_addWeaponBtn_clicked()
{
    QTableWidget *ct = ui->currentWeaponTableWidget;
    QTableWidget *t = ui->weaponTypeDataTableWidget;
    int tRow = t->currentRow();
    if (tRow != -1) {
        int cur = ct->rowCount();
        QString name = t->item(tRow, 0)->data(Qt::DisplayRole).toString();
        //检查添加武器是否已经存在
        if (indexOfItemValue(ct, 0, name) != -1) {
            QMessageBox::critical(this, tr("错误"),
                                  tr("选择的武器已经存在!"));
            return ;
        }
        ct->insertRow(cur);
        ct->setItem(cur, 0, new QTableWidgetItem(name));
    }
}

void UAVTypeDataDialog::on_delWeaponBtn_clicked()
{
    QTableWidget *t = ui->currentWeaponTableWidget;
    int cur = t->currentRow();
    delTableWidgetRow(t, cur);
}


void UAVTypeDataDialog::updateAllTable()
{
    clearAllTable();

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


void UAVTypeDataDialog::on_uavTableWidget_cellChanged(int row, int column)
{
    QTableWidget *table = ui->uavTableWidget;
    QTableWidgetItem *i = table->item(row, column);
    QTableWidgetItem *h = table->horizontalHeaderItem(column);
    QString value = i->data(Qt::DisplayRole).toString();
    QString key = h->data(Qt::UserRole).toString();
    if (row < m_uavs.size()) {
        QtJson::JsonObject obj = m_uavs[row].toMap();
        obj[key] = value;
        m_uavs.replace(row, obj);
        //如果修改无人机名称则更新combobox
        if (column == 0) {
            updateUAVComboBox();
        }
    }

}

void UAVTypeDataDialog::createUAVTableHeader()
{
    QTableWidget *table = ui->uavTableWidget;
    table->setColumnCount(10);
    QTableWidgetItem *name = new QTableWidgetItem("名称");
    name->setData(Qt::UserRole, "name");
    QTableWidgetItem *weight = new QTableWidgetItem("自重(kg)");
    weight->setData(Qt::UserRole, "weight");
    QTableWidgetItem *voyage = new QTableWidgetItem("航程(km)");
    voyage->setData(Qt::UserRole, "voyage");
    QTableWidgetItem *flightHeight = new QTableWidgetItem("航高(m)");
    flightHeight->setData(Qt::UserRole, "flightHeight");
    QTableWidgetItem *flyEndurance = new QTableWidgetItem("续航m");
    flyEndurance->setData(Qt::UserRole, "flyEndurance");
    QTableWidgetItem *maxSpeed = new QTableWidgetItem("最高时速(km/h)");
    maxSpeed->setData(Qt::UserRole, "maxSpeed");
    QTableWidgetItem *acceleration = new QTableWidgetItem("提升加速度(km/s^2)");
    acceleration->setData(Qt::UserRole, "acceleration");
    QTableWidgetItem *loadWeight = new QTableWidgetItem("载荷(kg)");
    loadWeight->setData(Qt::UserRole, "loadWeight");
    QTableWidgetItem *imageFile = new QTableWidgetItem("图片");
    imageFile->setData(Qt::UserRole, "imageFile");
    QTableWidgetItem *description = new QTableWidgetItem("描述");
    description->setData(Qt::UserRole, "description");
    int i = 0;
    table->setHorizontalHeaderItem(i++, name);
    table->setHorizontalHeaderItem(i++, weight);
    table->setHorizontalHeaderItem(i++, voyage);
    table->setHorizontalHeaderItem(i++, flightHeight);
    table->setHorizontalHeaderItem(i++, flyEndurance);
    table->setHorizontalHeaderItem(i++, maxSpeed);
    table->setHorizontalHeaderItem(i++, acceleration);
    table->setHorizontalHeaderItem(i++, loadWeight);
    table->setHorizontalHeaderItem(i++, imageFile);
    table->setHorizontalHeaderItem(i++, description);
}
