#include "maindialog.h"
#include <QApplication>
#include <QTextCodec>
#include <dialog/networkdialog.h>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);

    NetworkDialog d;
    if (d.exec() != QDialog::Accepted) {
        return 0;
    }
    MainDialog w;
    w.setUav(d.uavs()[d.selected().first]);
    w.setWeapons(d.weapons());
    w.setIp(d.ip());
    w.setPort(d.port());
    w.setId(d.id());
    w.setIndex(d.index());
    w.show();
    return a.exec();
}
