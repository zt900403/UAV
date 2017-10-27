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
    m_uavMetaData = result["uavMetaData"].toList();
    m_weaponMetaData = result["weaponMetaData"].toList();

    createTableHeader(m_uavMetaData, ui->uavTableWidget, m_uavMetaData.size());
    createTableHeader(m_weaponMetaData, ui->weaponTypeDataTableWidget, m_weaponMetaData.size());
    QtJson::JsonArray weapon;
    QtJson::JsonObject col1;
    col1["name"] = "name";
    col1["description"] = "名称";
    QtJson::JsonObject col2;
    col2["name"] = "amount";
    col2["description"] = "数量";
    weapon.append(col1);
    weapon.append(col2);
    createTableHeader(weapon, ui->currentWeaponTableWidget, weapon.size());
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
    foreach(QVariant var, m_uavs) {
        QtJson::JsonObject obj = var.toMap();
        QString name = obj["name"].toString();
        ui->uavTypeComboBox->addItem(name);
    }
}

void UAVTypeDataDialog::deleteRow(QTableWidget *t, QtJson::JsonArray &array, const int row)
{
    if (row != -1) {
        QString name = t->item(row, 0)->data(Qt::DisplayRole).toString();
        t->removeRow(row);
        t->setCurrentCell(-1, -1);
        int index = indexOfObjectValue(array, "name", name);
        if (index != -1) {
            array.removeAt(index);
        }
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

void UAVTypeDataDialog::addRow(QTableWidget *table, QtJson::JsonArray &array)
{
    int row = table->rowCount();
    table->insertRow(row);
    QString value = randomString2TableItem(table, row, 0);
    QtJson::JsonObject obj;
    obj["name"] = value;
    array.append(obj);
    updateUAVComboBox();
}

void UAVTypeDataDialog::saveFile(const QString &filename)
{
    QtJson::JsonObject obj;
    obj["uavMetaData"] = m_uavMetaData;
    obj["weaponMetaData"] = m_weaponMetaData;
    obj["uavs"] = m_uavs;
    obj["weapons"] = m_weapons;
    bool ok;
    QString str = QtJson::serializeStr(obj, ok);
    if (!ok) {
        qFatal("json object serializeStr error!");
        return ;
    }
    if (QFile::exists(filename))
    {
        QFile::remove(filename);
    }
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << str << endl;
        file.flush();
        file.close();
        QMessageBox::information(this, "成功", "保存完成!", QMessageBox::Ok);
    }

}

void UAVTypeDataDialog::on_addUAVTypeBtn_clicked()
{
    QTableWidget *t = ui->uavTableWidget;
    addRow(t, m_uavs);
}

void UAVTypeDataDialog::on_addWeaponTypeBtn_clicked()
{
    QTableWidget *t = ui->weaponTypeDataTableWidget;
    addRow(t, m_weapons);
}

void UAVTypeDataDialog::on_delUAVTypeBtn_clicked()
{
    QTableWidget *t = ui->uavTableWidget;
    int cur = t->currentRow();
    deleteRow(t, m_uavs, cur);
    m_uavs.removeAt(cur);
    updateUAVComboBox();
}

void UAVTypeDataDialog::on_delWeaponTypeBtn_clicked()
{
    QTableWidget *t = ui->weaponTypeDataTableWidget;
    int cur = t->currentRow();
    deleteRow(t, m_weapons, cur);
}

void UAVTypeDataDialog::on_addWeaponBtn_clicked()
{
    QTableWidget *ct = ui->currentWeaponTableWidget;
    QTableWidget *t = ui->weaponTypeDataTableWidget;
    int tRow = t->currentRow();
    if (tRow != -1) {
        QString name = t->item(tRow, 0)->data(Qt::DisplayRole).toString();
        //检查添加武器是否已经存在
        if (indexOfItemValue(ct, 0, name) != -1) {
            QMessageBox::critical(this, tr("错误"),
                                  tr("选择的武器已经存在!"));
            return ;
        }
        int cur = ct->rowCount();
        ct->insertRow(cur);
        ct->setItem(cur, 0, new QTableWidgetItem(name));

        int index = ui->uavTypeComboBox->currentIndex();
        QtJson::JsonObject uav = m_uavs[index].toMap();
        QtJson::JsonObject weapon;
        weapon["name"] = name;
        QtJson::JsonArray array = uav["weapon"].toList();
        array.append(weapon);
        uav["weapon"] = array;
        m_uavs.replace(index, uav);
    }
}


void UAVTypeDataDialog::on_delWeaponBtn_clicked()
{
    QTableWidget *t = ui->currentWeaponTableWidget;
    int cur = t->currentRow();
    QString name = t->item(cur, 0)->data(Qt::DisplayRole).toString();
    t->removeRow(cur);
    t->setCurrentCell(-1, -1);
    int index = ui->uavTypeComboBox->currentIndex();
    QtJson::JsonObject uav = m_uavs[index].toMap();
    QtJson::JsonArray array = uav["weapon"].toList();
    int i = indexOfObjectValue(array, "name", name);
    if (i != -1) {
        array.removeAt(i);
    }
    uav["weapon"] = array;
    m_uavs.replace(index, uav);
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
        foreach(QVariant attr, m_uavMetaData) {
            QtJson::JsonObject a = attr.toMap();
            table->setItem(i, j++, new QTableWidgetItem(obj[a["name"].toString()].toString()));
        }
    }


    table = ui->weaponTypeDataTableWidget;

    foreach(QVariant var, m_weapons) {
        QtJson::JsonObject obj = var.toMap();
        int i = table->rowCount();
        table->insertRow(i);
        int j = 0;
        foreach(QVariant attr, m_weaponMetaData) {
            QtJson::JsonObject a = attr.toMap();
            table->setItem(i, j++, new QTableWidgetItem(obj[a["name"].toString()].toString()));
        }

    }

    updateUAVComboBox();
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

void UAVTypeDataDialog::createTableHeader(const QtJson::JsonArray &array, QTableWidget *table, int columnCnt)
{
    table->setColumnCount(columnCnt);
    int j = 0;
    foreach(QVariant var, array) {
        QtJson::JsonObject obj = var.toMap();
        QString key = obj["name"].toString();
        QString value = obj["description"].toString();
        QTableWidgetItem *temp = new QTableWidgetItem(value);
        temp->setData(Qt::UserRole, key);
        table->setHorizontalHeaderItem(j++, temp);
    }
}


void UAVTypeDataDialog::on_weaponTypeDataTableWidget_cellChanged(int row, int column)
{
    QTableWidget *t = ui->weaponTypeDataTableWidget;
    QTableWidgetItem *i = t->item(row, column);
    QTableWidgetItem *h = t->horizontalHeaderItem(column);
    QString value = i->data(Qt::DisplayRole).toString();
    QString key = h->data(Qt::UserRole).toString();
    if (row < m_weapons.size()) {
        QtJson::JsonObject obj = m_weapons[row].toMap();
        obj[key] = value;
        m_weapons.replace(row, obj);
    }
}

void UAVTypeDataDialog::on_currentWeaponTableWidget_cellChanged(int row, int column)
{
    QTableWidget *t = ui->currentWeaponTableWidget;
    QTableWidgetItem *i = t->item(row, column);
    QTableWidgetItem *h = t->horizontalHeaderItem(column);
    QString value = i->data(Qt::DisplayRole).toString();
    QString key = h->data(Qt::UserRole).toString();
    QString name = t->item(row, 0)->data(Qt::DisplayRole).toString();

    int index = ui->uavTypeComboBox->currentIndex();
    if (index != -1) {
        QtJson::JsonObject uav = m_uavs[index].toMap();
        QtJson::JsonArray weapon = uav["weapon"].toList();
        int j = indexOfObjectValue(weapon, "name", name);
        if (j != -1) {
            QtJson::JsonObject one = weapon[j].toMap();
            one[key] = value;
            weapon.replace(j, one);
            uav["weapon"] = weapon;
            m_uavs.replace(index, uav);
        }
    }

}

void UAVTypeDataDialog::on_saveBtn_clicked()
{
    QString path = FileSystem::directoryOf("config").absoluteFilePath("uvatype.json");
    saveFile(path);
}
