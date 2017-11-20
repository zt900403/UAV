#include "filesystem.h"
#include <QString>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>

FileSystem::FileSystem()
{

}

QString FileSystem::readFile(const QString &filename)
{
    QFile f(filename);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        return QString();
    } else {
        QTextStream in(&f);
        return in.readAll();
    }
}

QDir FileSystem::directoryOf(const QString &subdir)
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

QString FileSystem::openSaveDialog(QWidget *parent)
{
    QString filename = QFileDialog::getSaveFileName(parent, QObject::tr("保存文件"),
                               "",
                               QObject::tr("文本文件 (*.txt)"));

    if (QFile::exists(filename))
    {
        QFile::remove(filename);
    }

    return filename;
}

void FileSystem::saveFile(const QString &filename, const QString &str)
{
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << str;
        file.flush();
        file.close();
        QMessageBox::information(NULL, "成功", "保存完成!", QMessageBox::Ok);
    }
}

QString FileSystem::openOpenDialog(QWidget *parent)
{
    QString filename = QFileDialog::getOpenFileName(parent, QObject::tr("打开文件"),
                               "",
                               QObject::tr("文本文件 (*.txt)"));
    return filename;
}

QString FileSystem::openFile(const QString &filename)
{
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString all = in.readAll();
        file.close();
        return all;
    }
}
