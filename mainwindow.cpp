#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      caml_toplevel(this),
      readOnlyRange(0),
      expHistory(),
      expIndex(0),
      validExp(),
      lastExpNotEvaluated(false)
    #ifdef MULTIMEDIA_ENABLED
      ,beep_sound(":/beep.wav", this)
    #endif
{
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

    caml_toplevel.initCaml();
}

MainWindow::~MainWindow()
{
    delete ui;
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
        ui->textEdit->setText(buffer.mid(0,buffer.length() - currentWord.length()) + find_common_root(matchingFunctions));
        ui->textEdit->moveCursor(QTextCursor::End);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    caml_toplevel.killCaml();
    QMainWindow::closeEvent(event);
    qApp->quit();
}

void MainWindow::readInput()
{
    ui->statusbar->showMessage("Status: Done evaluating. Ready for input !");
    if (!expHistory.isEmpty()) {
        /*QStringList lines = ui->textEdit->toPlainText().split("\n");
        QStringList evalLine = lines[lines.length()-2].split(":");
        QString partWithStatus = evalLine[evalLine.length()-2];*/
        QString partWithStatus = ui->textEdit->toPlainText().split(";;").last();
        /*if (partWithStatus.length()<5)
            return;
        if ((partWithStatus.length()==5 && partWithStatus.contains("Error")) || (partWithStatus.length()==9 && partWithStatus.contains("Exception"))) {
            validExp.append(false);
            ui->statusbar->showMessage("Status: evaluation failed due to an error or exception. Ready for input !");
            return;
        }
        if (partWithStatus.length() < 10)
            return;
        partWithStatus = partWithStatus.mid(partWithStatus.length()-10);
        if (partWithStatus.contains("Error") || partWithStatus.contains("Exception")) {
            validExp.append(false);
            ui->statusbar->showMessage("Status: evaluation failed due to an error or exception. Ready for input !");
            // some syntax highlight ?
        }*/
        if (partWithStatus.contains("Error") || partWithStatus.contains("Exception")) {
            validExp.append(false);
            ui->statusbar->showMessage("Status: evaluation failed due to an error or exception. Ready for input !");
            // some syntax highlight ?
        }
        else {
            validExp.append(true);
            // some syntax highlight ?
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
    for (int i=0, l=validExp.length(); i<l; i++) {
        if (validExp[i])
            data.append(expHistory[i] + "\n\n");
    }
    f.write(data.toUtf8());
    f.close();
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
        if (lastExpNotEvaluated)
            expHistory.removeLast();
        expHistory+= exp.mid(0, exp.length()-1);
        expIndex = expHistory.length();
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
    if (ok)
        ui->textEdit->setFont(newFont);
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
    if (expIndex == expHistory.length()-1)
        expHistory.removeLast();
}
