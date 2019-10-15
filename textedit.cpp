#include "textedit.h"

TextEdit::TextEdit(QWidget *parent): QTextEdit(parent)
{

}

void TextEdit::keyPressEvent(QKeyEvent *event)
{
    QTextEdit::keyPressEvent(event);
    if (event->key() == Qt::Key_Return)
        emit enterPressed();
}

