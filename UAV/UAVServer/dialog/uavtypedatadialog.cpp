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
//    QMap<QString, QString> uavMap;
    QVector<QPair<QString, QString> > uavVect;
    uavVect.append(QPair<QString, QString>("name", "名称"));
    uavVect.append(QPair<QString, QString>("weight", "自重(kg)"));
    uavVect.append(QPair<QString, QString>("voyage", "航程(km)"));
    uavVect.append(QPair<QString, QString>("flightHeight", "航高(m)"));
    uavVect.append(QPair<QString, QString>("flyEndurance", "续航m"));
    uavVect.append(QPair<QString, QString>("maxSpeed", "最高时速(km/h)"));
    uavVect.append(QPair<QString, QString>("acceleration", "提升加速度(km/s^2)"));
    uavVect.append(QPair<QString, QString>("loadWeight", "载荷(kg)"));
    uavVect.append(QPair<QString, QString>("imageFile", "图片"));
    uavVect.append(QPair<QString, QString>("description", "描述"));
    createTableHeader(uavVect, ui->uavTableWidget, uavVect.size());

    QVector<QPair<QString, QString> > weaponVector;
    weaponVector.append(QPair<QString, QString>("name", "名称"));
    weaponVector.append(QPair<QString, QString>("R_L", "尺寸(m)"));
    weaponVector.append(QPair<QString, QString>("weight", "重量(kg)"));
    weaponVector.append(QPair<QString, QString>("guidedType", "制导方式"));
    weaponVector.append(QPair<QString, QString>("fieldOfFire", "有效射程(km)"));
    weaponVector.append(QPair<QString, QString>("killRadius", "杀伤半径(m)"));
    weaponVector.append(QPair<QString, QString>("description", "描述"));
    createTableHeader(weaponVector, ui->weaponTypeDataTableWidget, weaponVector.size());

    QVector<QPair<QString, QString> > curWeaponVect;
    curWeaponVect.append(QPair<QString, QString>("name", "名称"));
    curWeaponVect.append(QPair<QString, QString>("amount", "数量"));
    createTableHeader(curWeaponVect, ui->currentWeaponTableWidget, curWeaponVect.size());
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

void UAVTypeDataDialog::createTableHeader(const QVector<QPair<QString, QString> > &vect, QTableWidget *table, int columnCnt)
{
    table->setColumnCount(columnCnt);
    int j = 0;
    for(QVector<QPair<QString, QString> >::const_iterator i = vect.begin();
        i != vect.end(); ++i) {
        QTableWidgetItem *temp = new QTableWidgetItem(i->second);
        temp->setData(Qt::UserRole, i->first);
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
