#include "toplevel.h"

TopLevel::TopLevel(QObject *parent) : QObject(parent),
    ocaml_process(this),
    currentExp()
{
    connect(&ocaml_process, &QProcess::readyReadStandardOutput, this, &TopLevel::readOutput); // Print out output whenever its sent instead of all at once at the end
    connect(this, &TopLevel::requestEval, this, &TopLevel::evalInput);
    connect(&ocaml_process,  SIGNAL(finished(int, QProcess::ExitStatus)),this, SLOT(sendExitRequest(int, QProcess::ExitStatus)));
}


void TopLevel::initCaml() // Warning: no failsafe mechanism.
{
    ocaml_process.start("ocaml");
}

void TopLevel::readOutput()
{
    QString output = ocaml_process.readAllStandardOutput();
    emit writeOutput(output);
    if (output.mid(output.length()-2) == "# ")
        emit evalDone();
}

void TopLevel::eval(QString text) // Public one: Checks expression's completeness, and simply stock it if it's not
{
    currentExp = text;
    emit requestEval(); // Actually kinda useless
}

void TopLevel::evalInput() // Private one: Directly evaluates expression. We could do that directly, but this method allows for more control and to edit later on previously typed lines
{
    ocaml_process.write(currentExp.toStdString().c_str());
}

void TopLevel::sendExitRequest(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)
    emit exitRequested(exitCode);
}
