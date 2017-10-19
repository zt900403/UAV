#include "uavtypedatadialog.h"
#include "ui_uavtypedatadialog.h"

UAVTypeDataDialog::UAVTypeDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UAVTypeDataDialog)
{
    ui->setupUi(this);
}

UAVTypeDataDialog::~UAVTypeDataDialog()
{
    delete ui;
}


