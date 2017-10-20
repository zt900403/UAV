#ifndef UAVTYPEDATADIALOG_H
#define UAVTYPEDATADIALOG_H

#include <QDialog>
#include "utils/json.h"
class QTableWidget;
class QTableWidgetItem;
namespace Ui {
class UAVTypeDataDialog;
}

class UAVTypeDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UAVTypeDataDialog(QWidget *parent = 0);
    ~UAVTypeDataDialog();
private slots:
    void on_uavTypeComboBox_currentIndexChanged(const QString &arg1);


    void on_addUAVTypeBtn_clicked();

    void on_addWeaponTypeBtn_clicked();

    void on_delUAVTypeBtn_clicked();

    void on_delWeaponTypeBtn_clicked();

    void on_addWeaponBtn_clicked();

    void on_delWeaponBtn_clicked();

    void on_uavTableWidget_cellChanged(int row, int column);

private:
    void createUAVTableHeader();
    void updateAllTableWithFile();
    void updateAllTable();
    void clearAllTable();
    void updateUAVComboBox();
    void delTableWidgetRow(QTableWidget *t, const int row);
    int indexOfObjectValue(const QtJson::JsonArray &array, const QString &key, const QString &value);
    int indexOfItemValue(const QTableWidget *table, const int col, const QString &value);

    QString randomString(int len) const;
    QString randomString2TableItem(QTableWidget *table, int row, int col);
private:
    Ui::UAVTypeDataDialog *ui;
    QtJson::JsonArray m_uavs;
    QtJson::JsonArray m_weapons;
};

#endif // UAVTYPEDATADIALOG_H
