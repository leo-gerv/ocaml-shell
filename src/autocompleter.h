#ifndef AUTOCOMPLETER_H
#define AUTOCOMPLETER_H

#include <QObject>

#include <QDir>
#include <QString>
#include <QStringList>

class autocompleter : public QObject
{
    Q_OBJECT
public:
    explicit autocompleter(QString dir, QObject *parent = nullptr);

    enum FileType {
        OCaml,
        CMA,
        Any
    };

private:
    QDir workingDir;

signals:
    void foundFiles(QStringList fileNames);

public slots:
    void listFiles(FileType type = Any);

};

#endif // AUTOCOMPLETER_H
