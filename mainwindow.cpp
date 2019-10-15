#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      caml_toplevel(this),
      readOnlyRange(0),
      expHistory(),
      expIndex(0),
      lastExpNotEvaluated(false)
{
    ui->setupUi(this);

    ui->statusbar->showMessage("Status: not initialized");

    connect(&caml_toplevel, &TopLevel::evalDone, this, &MainWindow::readInput);
    connect(&caml_toplevel, &TopLevel::requestEval, this, &MainWindow::disableInput);
    connect(&caml_toplevel, &TopLevel::writeOutput, this, &MainWindow::displayOutput);
    connect(&caml_toplevel, &TopLevel::exitRequested, qApp, &QApplication::quit);
    connect(ui->textEdit, &QTextEdit::cursorPositionChanged, this, &MainWindow::checkCurrentPosition);
    connect(ui->textEdit, &TextEdit::enterPressed, this, &MainWindow::checkCompleteness);
    connect(ui->textEdit, &TextEdit::nextExp, this, &MainWindow::displayNextExp);
    connect(ui->textEdit, &TextEdit::previousExp, this, &MainWindow::displayPreviousExp);

    caml_toplevel.initCaml();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readInput()
{
    ui->statusbar->showMessage("Status: Done evaluating. Ready for input !");
}

void MainWindow::disableInput()
{}

void MainWindow::displayOutput(QString out)
{
    ui->textEdit->setText(ui->textEdit->toPlainText() + out);
    ui->textEdit->moveCursor(QTextCursor::End);
    readOnlyRange = ui->textEdit->toPlainText().length(); // fk optimization
}

void MainWindow::checkCurrentPosition()
{
    if (ui->textEdit->textCursor().position() < readOnlyRange) {
        QString buffer = ui->textEdit->toPlainText();
        if (buffer.length() < readOnlyRange) {
            ui->textEdit->setText(buffer + " ");
            ui->textEdit->moveCursor(QTextCursor::End);
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

void MainWindow::displayNextExp()
{
    if (expIndex > expHistory.length()-2) {
        // Annoying error sound too !!
        return;
    }
    QString buffer = ui->textEdit->toPlainText();
    expIndex++;
    ui->textEdit->setText(buffer.mid(0, readOnlyRange) + expHistory[expIndex]);
}
