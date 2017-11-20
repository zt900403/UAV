#ifndef FILESYSTEM_H
#define FILESYSTEM_H


#include <QObject>
#include <QTextStream>
class QString;
class QDir;

class FileSystem
{
public:
    FileSystem();
    static QString readFile(const QString &filename);
    static QDir directoryOf(const QString &subdir);
    static QString openSaveDialog(QWidget *parent = NULL);
    static void saveFile(const QString &filename, const QString &str);
    static QString openOpenDialog(QWidget *parent = NULL);
    static QString openFile(const QString &filename);
};

#endif // FILESYSTEM_H
