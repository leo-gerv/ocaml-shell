#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      caml_toplevel(this),
      readOnlyRange(0)
{
    ui->setupUi(this);

    ui->statusbar->showMessage("Status: not initialized");

    connect(&caml_toplevel, &TopLevel::evalDone, this, &MainWindow::readInput);
    connect(&caml_toplevel, &TopLevel::requestEval, this, &MainWindow::disableInput);
    connect(&caml_toplevel, &TopLevel::writeOutput, this, &MainWindow::displayOutput);
    connect(ui->textEdit, &QTextEdit::cursorPositionChanged, this, &MainWindow::checkCurrentPosition);
    connect(ui->textEdit, &TextEdit::enterPressed, this, &MainWindow::checkCompleteness);

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
    int l = exp.length();
    ui->statusbar->showMessage("Enter press registered, but expression isn't complete yet. Reading further input !");
    if ((l>2) && (exp[l-2] == ';') && (exp[l-3] == ';')) {
        caml_toplevel.eval(exp);
    }
}

