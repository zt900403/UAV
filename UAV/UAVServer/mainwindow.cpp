#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils/json.h"
#include "dialog/uavtypedatadialog.h"
#include <QTextStream>
#include <QDir>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString imageDir = directoryOf("images").absoluteFilePath("Europe_topic_image_Satellite_image.jpg");
    QImage image(imageDir);
    ui->gisView->setImage(image);
    ui->gisView->setBackgroundBrush(QBrush(QColor(0x7F,0x7F,0x7F)));

    updateUavMetaDataGroup();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::readFile(const QString &filename)
{
    QFile f(filename);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        return QString();
    } else {
        QTextStream in(&f);
        return in.readAll();
    }
}

QDir MainWindow::directoryOf(const QString &subdir)
{
    QDir dir(QApplication::applicationDirPath());

#if defined(Q_OS_WIN)
    if (dir.dirName().toLower() == "debug"
            || dir.dirName().toLower() == "release"
            || dir.dirName().toLower() == "bin")
        dir.cdUp();
#elif defined(Q_OS_MAC)
    if (dir.dirName() == "MacOS") {
        dir.cdUp();
        dir.cdUp();
        dir.cdUp();
    }
#endif
    dir.cd(subdir);
    return dir;
}

bool MainWindow::updateUavMetaDataGroup()
{
    QString uavtypeDir = directoryOf("config").absoluteFilePath("uvatype.json");
    QString uavtypeJson = readFile(uavtypeDir);
    if (uavtypeJson.isEmpty()) {
        qFatal("Could not read uavtype.json file!");
        return false;
    }
    bool ok;
    QtJson::JsonObject result = QtJson::parse(uavtypeJson, ok).toMap();
    if (!ok) {
        qFatal("Could not parse uavtype.json file! json syntax is wrong!");
        return false;
    }

    QtJson::JsonArray uavs = result["uavs"].toList();
    foreach (QVariant var, uavs) {
        QtJson::JsonObject uavsObj = var.toMap();
        QString name = uavsObj["name"].toString();
        QString imageFile = uavsObj["imageFile"].toString();
        QString descStr = uavsObj["description"].toString();
        QListWidgetItem *item= new QListWidgetItem(name);
        QVariant i;
        QVariant d;
        i.setValue(imageFile);
        d.setValue(descStr);
        item->setData(MyImgRole, i);
        item->setData(MyDescRole, d);
        ui->uavslistWidget->addItem(item);
    }
    return true;
}

void MainWindow::labelDisplayImage(QLabel *label, const QString &filename)
{
    QString f = directoryOf("images/uavs").absoluteFilePath(filename);
    QPixmap p(f);
    p = p.scaled(150, 150);
    label->setPixmap(p);
}

void MainWindow::on_uavslistWidget_itemClicked(QListWidgetItem *item)
{
    QVariant i = item->data(MyImgRole);
    QVariant d = item->data(MyDescRole);

    ui->uavDescriptionLabel->setText(d.toString());
    labelDisplayImage(ui->uavImageLabel, i.toString());
}

void MainWindow::on_pushButton_clicked()
{
    UAVTypeDataDialog d(this);
    d.exec();
}
