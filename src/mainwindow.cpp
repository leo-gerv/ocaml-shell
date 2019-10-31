#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "highlighter.h"

MainWindow *MainWindow::instance = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      caml_toplevel(this),
      readOnlyRange(0),
      expHistory(),
      expIndex(0),
      validExp(),
      lastExpNotEvaluated(false),
      doubleTab(false),
      lastExp(),
      settings(QSettings::NativeFormat, QSettings::UserScope, "qt-designed-apps", "ocaml-shell-settings", this),
      historyShift(0)
    #ifdef MULTIMEDIA_ENABLED
      ,beep_sound(":/beep.wav", this)
    #endif
{
    // Singleton
    instance = this;

    ui->setupUi(this);

    ui->statusbar->showMessage("Status: not initialized");

    connect(&caml_toplevel, &TopLevel::evalDone, this, &MainWindow::readInput);
    connect(&caml_toplevel, &TopLevel::requestEval, this, &MainWindow::disableInput);
    connect(&caml_toplevel, &TopLevel::writeOutput, this, &MainWindow::displayOutput);
    connect(&caml_toplevel, SIGNAL(exitRequested()), qApp, SLOT(quit()));
    connect(ui->textEdit, &QTextEdit::cursorPositionChanged, this, &MainWindow::checkCurrentPosition);
    connect(ui->textEdit, &TextEdit::enterPressed, this, &MainWindow::checkCompleteness);
    connect(ui->textEdit, &TextEdit::nextExp, this, &MainWindow::displayNextExp);
    connect(ui->textEdit, &TextEdit::previousExp, this, &MainWindow::displayPreviousExp);
    connect(ui->textEdit, &QTextEdit::selectionChanged, this, &MainWindow::checkSelection);
    connect(ui->textEdit, &TextEdit::tabRequest, this, &MainWindow::requestAutocomplete);
    connect(ui->textEdit, &QTextEdit::textChanged, this, &MainWindow::tabtabdisable);
    connect(ui->textEdit, &TextEdit::leftClickReleased, this, &MainWindow::onLeftClickReleased);

    caml_toplevel.initCaml();

    // Setup higlighter
    highlighter = new Highlighter(ui->textEdit->document());

    loadSettings();

    completer_menu = new CompleterMenu(ui->textEdit);
    completer_menu->setStyleSheet("background-color: #303030;\ncolor: #954575;");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete highlighter;
}

void MainWindow::tabtabdisable()
{
    doubleTab = false;
}

QString MainWindow::find_common_root(const QStringList& list)
{
    QString root = list.front();
    for(const auto& item : list)
    {
        if (root.length() > item.length())
        {
            root.truncate(item.length());
        }

        for(int i = 0; i < root.length(); ++i)
        {
            if (root[i] != item[i])
            {
                root.truncate(i);
                break;
            }
        }
    }

    return root;
}

void MainWindow::displayTabMatches(QStringList &matches)
{
    QString out;
    {
        QString swapper;
        for (int i=0, l=matches.length(); i<l; i++) {
            swapper = matches[i];
            if (i>0) {
                out += "     ";
            }
            out += swapper;
        }
    }
    QString currentExp = ui->textEdit->toPlainText().mid(readOnlyRange);
    QString past = ui->textEdit->toPlainText().mid(0, readOnlyRange);
    out = "Possible expressions: \n" + out + "\n# ";
    readOnlyRange += out.length();
    ui->textEdit->setText(past + out + currentExp);
    ui->textEdit->moveCursor(QTextCursor::End);
}

void MainWindow::requestAutocomplete()
{
    QString currentWord = ui->textEdit->toPlainText().mid(readOnlyRange).split(" ").last().split('(').last(),
            buffer = ui->textEdit->toPlainText();


    QStringList matchingFunctions;
    for (int i=0; i<BuiltIn_Expressions::functions_count; i++) {
        if (BuiltIn_Expressions::functions[i].startsWith(currentWord))
            matchingFunctions.append(BuiltIn_Expressions::functions[i]);
    }

    if (matchingFunctions.length() == 1) {
        ui->textEdit->setText(buffer.mid(0, buffer.length() - currentWord.length()) + matchingFunctions.first() + " ");
        ui->textEdit->moveCursor(QTextCursor::End);
    }
    else if (!matchingFunctions.empty()) {
        if (doubleTab) {
            displayTabMatches(matchingFunctions);
            return;
        }
        ui->textEdit->setText(buffer.mid(0,buffer.length() - currentWord.length()) + find_common_root(matchingFunctions));
        ui->textEdit->moveCursor(QTextCursor::End);
        completer_menu->fill(matchingFunctions);
        completer_menu->move(ui->textEdit->cursorRect().bottomLeft());
        completer_menu->show();
        doubleTab = true;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    caml_toplevel.killCaml();
    settings.setValue(HISTORY_KEY, expHistory);
    QMainWindow::closeEvent(event);
    qApp->quit();
}

void MainWindow::readInput()
{
    ui->statusbar->showMessage("Status: Done evaluating. Ready for input !");
    if (!expHistory.isEmpty()) {
        QString partWithStatus = ui->textEdit->toPlainText().split(";;").last();

        if (partWithStatus.contains("Error") || partWithStatus.contains("Exception")) {
            validExp.append(false);
            ui->statusbar->showMessage("Status: evaluation failed due to an error or exception. Ready for input !");
        }
        else {
            validExp.append(true);
            QStringList split_exp = lastExp.split(" ");
            if ((split_exp.length()>2) && (split_exp[0] == QString("let"))) {
                if ((split_exp.length()>3) && (split_exp[1] == QString("rec")))
                    BuiltIn_Expressions::functions.append(split_exp[2]);
                else
                    BuiltIn_Expressions::functions.append(split_exp[1]);
                BuiltIn_Expressions::functions_count++;
            }
        }
    }
}

void MainWindow::disableInput()
{}

void MainWindow::checkSelection()
{
    QTextCursor c = ui->textEdit->textCursor();
    if (c.selectionStart() < readOnlyRange)
        ui->textEdit->setReadOnly(true);
    if (c.selectionStart() == c.selectionEnd())
        checkCurrentPosition();
}

void MainWindow::displayOutput(QString out)
{
    ui->textEdit->setText(ui->textEdit->toPlainText() + out);
    ui->textEdit->moveCursor(QTextCursor::End);
    readOnlyRange = ui->textEdit->toPlainText().length(); // fk optimization
}

void MainWindow::on_saveExp_triggered()
{
    QString url = QFileDialog::getSaveFileName(this, "Save expressions", "~/");
    if (url.isEmpty())
        return;
    QFile f(url);
    if (!f.open(QIODevice::ReadWrite)) {
        QMessageBox::critical(this, "Error", "Access to file denied");
        return;
    }
    QString data;
    for (int i=0, l=validExp.length()-1; i<l; i++) {
        if (validExp[i])
            data.append(expHistory[i + historyShift] + "\n\n"); // Do not save expressions from previous sessions
    }
    f.write(data.toUtf8());
    f.close();
}

void MainWindow::onLeftClickReleased()
{
    QTextCursor c = ui->textEdit->textCursor();
    if ((c.position() < readOnlyRange) && (c.selection().isEmpty())) {
        ui->textEdit->moveCursor(QTextCursor::End);
    }
}

void MainWindow::checkCurrentPosition()
{
    if (ui->textEdit->textCursor().position() < readOnlyRange) {
        QString buffer = ui->textEdit->toPlainText();
        if (buffer.length() < readOnlyRange) {
            ui->textEdit->setText(buffer + " ");
            ui->textEdit->moveCursor(QTextCursor::End);
        }
        else if (ui->textEdit->textCursor().position() == readOnlyRange-1) {
            ui->textEdit->moveCursor(QTextCursor::NextCharacter);
            #ifdef MULTIMEDIA_ENABLED
            beep_sound.play();
#endif
        }
        else
            ui->textEdit->setReadOnly(true);
    }
    else
        ui->textEdit->setReadOnly(false);
}

void MainWindow::checkCompleteness()
{
    QString exp = ui->textEdit->toPlainText().mid(readOnlyRange);
    bool needNewline = false;
    if (!exp.endsWith("\n")) {
        exp += "\n";
        needNewline = true;
    }
    int l = exp.length();
    ui->statusbar->showMessage("Enter press registered, but expression isn't complete yet. Reading further input !");
    if (((l>2) && (exp[l-2] == ';')) && ((exp[l-3] == ';'))) {
        if (needNewline)
            ui->textEdit->setText(ui->textEdit->toPlainText() + "\n");
        caml_toplevel.eval(exp);
        if (lastExpNotEvaluated) {
            //expHistory.removeLast();
        }
        lastExp = exp.mid(0, exp.length()-1);
        expHistory+= lastExp;
        expIndex = expHistory.length();
    }
}

void MainWindow::on_clearHist_triggered()
{
    if (QMessageBox::question(this, "Confirmation", "Are you really willing to clear your command history ? The program will restart") == QMessageBox::Yes) {
        // Creates a backup, just incase. Might later add a function to retrieve backups' data
        settings.setValue("history-backup-" + QDate::currentDate().toString(Qt::ISODate) + "/" + QTime::currentTime().toString(), expHistory);
        settings.remove(HISTORY_KEY);
        if (!QProcess::startDetached(qApp->applicationFilePath()))
            QMessageBox::warning(this, "Error", "Could not restart the application. Exiting anyway...");
        expHistory.clear();
        close();
    }
}

void MainWindow::displayPreviousExp()
{
    if (expIndex == 0) {
        // Annoying error sound ?
#ifdef MULTIMEDIA_ENABLED
        beep_sound.play();
#endif
        return; // cheating
    }
    QString buffer = ui->textEdit->toPlainText();
    if (expIndex >= expHistory.length()) {
        expHistory.append(buffer.mid(readOnlyRange));
        lastExpNotEvaluated = true;
    }
    expIndex--;
    ui->textEdit->setText(buffer.mid(0, readOnlyRange) + expHistory[expIndex]);
}

void MainWindow::on_fontAction_triggered()
{
    bool ok = false;
    QFont newFont = QFontDialog::getFont(&ok, ui->textEdit->font(), this, "Select a new font");

    if (ok) {
        ui->textEdit->setFont(newFont);
        settings.setValue(FONT_KEY, newFont);
    }
}

void MainWindow::loadSettings()
{
    // Load font if it exists
    if (settings.contains(FONT_KEY)) {
        ui->textEdit->setFont(qvariant_cast<QFont>(settings.value(FONT_KEY)));
    }

    // Load expression history
    if (settings.contains(HISTORY_KEY)) {
        expHistory = qvariant_cast<QStringList>(settings.value(HISTORY_KEY));
        historyShift = expHistory.length();
        expIndex = historyShift;
    }
}

void MainWindow::displayNextExp()
{
    if (expIndex > expHistory.length()-2) {
        // Annoying error sound too !!
#ifdef MULTIMEDIA_ENABLED
        beep_sound.play();
#endif
        return;
    }
    QString buffer = ui->textEdit->toPlainText();
    expIndex++;
    ui->textEdit->setText(buffer.mid(0, readOnlyRange) + expHistory[expIndex]);
    if (expIndex == expHistory.length()-1) {
        expHistory.removeLast();
    }
}
