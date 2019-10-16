#include "textedit.h"

TextEdit::TextEdit(QWidget *parent): QTextEdit(parent)
{

}

void TextEdit::keyPressEvent(QKeyEvent *event)
{
    bool moveCursorToEnd = false;
    if (!((event->key() == Qt::Key_Return) && (textCursor().position() < toPlainText().length())))
        QTextEdit::keyPressEvent(event);
    switch (event->key()) {
    case Qt::Key_Up:
        emit previousExp();
        moveCursorToEnd = true;
        break;
    case Qt::Key_Down:
        emit nextExp();
        moveCursorToEnd = true;
        break;
    case Qt::Key_Return:
        emit enterPressed();
        break;
    default:
        break;
    }
    if (moveCursorToEnd)
        moveCursor(QTextCursor::End);
}
