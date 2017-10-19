#ifndef UAVTYPEDATADIALOG_H
#define UAVTYPEDATADIALOG_H

#include <QDialog>

namespace Ui {
class UAVTypeDataDialog;
}

class UAVTypeDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UAVTypeDataDialog(QWidget *parent = 0);
    ~UAVTypeDataDialog();
private:
    void updateAllData();

private:
    Ui::UAVTypeDataDialog *ui;
};

#endif // UAVTYPEDATADIALOG_H
