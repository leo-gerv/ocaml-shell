#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QKeyEvent>

#include "toplevel.h"
#include "textedit.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    TopLevel caml_toplevel;
    int readOnlyRange;

private slots:
    void readInput();
    void disableInput();
    void displayOutput(QString out);
    void checkCurrentPosition();
    void checkCompleteness();

protected:
};
#endif // MAINWINDOW_H
