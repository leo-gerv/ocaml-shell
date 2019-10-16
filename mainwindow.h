#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QKeyEvent>
#include <QStringList>
#include <QList>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextCursor>

#include <QDebug>

#include <QCloseEvent>
#include <QSound>
#include <QTextFragment>

#include "toplevel.h"
#include "textedit.h"
#include "builtin_exps.h"

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
    QList<int> validExp;
    bool lastExpNotEvaluated;
    QSound beep_sound;

private slots:
    void readInput();
    void disableInput();
    void displayOutput(QString out);
    void checkCurrentPosition();
    void checkCompleteness();
    void checkSelection();
    void requestAutocomplete();

    // Navigation through previously evaluated expressions
    void displayPreviousExp();
    void displayNextExp();

    void on_saveExp_triggered();

protected:
    void closeEvent(QCloseEvent *event);
};
#endif // MAINWINDOW_H
