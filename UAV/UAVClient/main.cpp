#include "maindialog.h"
#include <QApplication>
#include <dialog/networkdialog.h>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainDialog w;
//    w.show();
    NetworkDialog d;
    d.show();

    return a.exec();
}
