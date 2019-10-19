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
#include <QProcess>
#include <QTime>

#include <QDebug>

#include <QCloseEvent>

#ifdef MULTIMEDIA_ENABLED
#include <QSound>
#endif

#include <QTextFragment>
#include <QTextDocumentFragment>
#include <QFontDialog>
#include <QFont>

#include <QSettings>

#include "toplevel.h"
#include "textedit.h"
#include "builtin_exps.h"

class Highlighter;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Singleton
    static MainWindow *instance;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    Ui::MainWindow *ui;

private:
    TopLevel caml_toplevel;
    int readOnlyRange;
    QStringList expHistory;
    int expIndex;
    QList<int> validExp;
    bool lastExpNotEvaluated;
    bool doubleTab;
    QString lastExp;
    Highlighter *highlighter = nullptr;
    QSettings settings;
    int historyShift;
#ifdef MULTIMEDIA_ENABLED
    QSound beep_sound;
#endif

    QString find_common_root(const QStringList& list);
    void displayTabMatches(QStringList &matches);
    void loadSettings();

private slots:
    void readInput();
    void disableInput();
    void displayOutput(QString out);
    void checkCurrentPosition();
    void checkCompleteness();
    void checkSelection();
    void requestAutocomplete();
    void onLeftClickReleased();

    // Navigation through previously evaluated expressions
    void displayPreviousExp();
    void displayNextExp();

    // Tab related
    void tabtabdisable();

    void on_saveExp_triggered();
    void on_fontAction_triggered();
    void on_clearHist_triggered();


protected:
    void closeEvent(QCloseEvent *event);
};

// Settings keys
#define FONT_KEY "selectedFont"
#define HISTORY_KEY "expressionsHistory"


#endif // MAINWINDOW_H
