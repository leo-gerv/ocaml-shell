#include "autocompleter.h"

autocompleter::autocompleter(QString dir, QObject *parent) : QObject(parent),
    workingDir(dir)
{
}

void autocompleter::listFiles(FileType type)
{
    // Example: QStringList images = directory.entryList(QStringList() << "*.jpg" << "*.JPG",QDir::Files);
    QStringList r;
    switch (type) {
    case OCaml:
        r = workingDir.entryList(QStringList("*.ml"), QDir::Files);
        break;
    case CMA:
        r = workingDir.entryList(QStringList("*.cma"), QDir::Files);
        break;
    default:
        r = workingDir.entryList(QDir::Files);
        break;
    }
    emit foundFiles(r);
}
