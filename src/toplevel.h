#ifndef TOPLEVEL_H
#define TOPLEVEL_H

#include <QObject>

#include <QProcess>

class TopLevel : public QObject
{
    Q_OBJECT
public:
    explicit TopLevel(QObject *parent = nullptr);
    void initCaml();

private:
    QProcess ocaml_process;

    QString currentExp;

signals:
    void exitRequested();
    void evalDone();
    void requestEval();
    void writeOutput(QString out);

public:
    void eval(QString text);
    void killCaml(); // I like that name :)

public slots:


private slots:
    void readOutput();
    void evalInput();
    void sendExitRequest(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // TOPLEVEL_H
