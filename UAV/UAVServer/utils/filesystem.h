#ifndef FILESYSTEM_H
#define FILESYSTEM_H

class QString;
class QDir;

class FileSystem
{
public:
    FileSystem();
    static QString readFile(const QString &filename);
    static QDir directoryOf(const QString &subdir);
};

#endif // FILESYSTEM_H
