#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QKeyEvent>
#include <QStringList>

#include <QDebug>

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
    QStringList expHistory;
    int expIndex;
    bool lastExpNotEvaluated;

private slots:
    void readInput();
    void disableInput();
    void displayOutput(QString out);
    void checkCurrentPosition();
    void checkCompleteness();

    // Navigation through previously evaluated expressions
    void displayPreviousExp();
    void displayNextExp();

protected:
};
#endif // MAINWINDOW_H
