#ifndef UAVTYPEDATADIALOG_H
#define UAVTYPEDATADIALOG_H

#include <QDialog>
#include "utils/json.h"

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

private:
    void updateAllData();
    void initWidgets();

private:
    Ui::UAVTypeDataDialog *ui;
    QtJson::JsonArray m_uavs;
    QtJson::JsonArray m_weapons;
};

#endif // UAVTYPEDATADIALOG_H
