#include "textedit.h"

TextEdit::TextEdit(QWidget *parent): QTextEdit(parent)
{

}

void TextEdit::keyPressEvent(QKeyEvent *event)
{
    bool moveCursorToEnd = false;
    if (!((event->key() == Qt::Key_Return) && (textCursor().position() < toPlainText().length())) && (event->key() != Qt::Key_Tab))
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
    case Qt::Key_Tab:
        emit tabRequest();
        break;
    case Qt::Key_End:
        setText(toPlainText()+";;");
        emit enterPressed();
        break;
    default:
        break;
    }
    if (moveCursorToEnd)
        moveCursor(QTextCursor::End);
}

void TextEdit::mouseReleaseEvent(QMouseEvent *e)
{
    switch (e->button()) {
    case Qt::LeftButton:
        emit leftClickReleased();
        break;
    default:
        break;
    }

    QTextEdit::mouseReleaseEvent(e);
}
