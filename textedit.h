#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QTextEdit>
#include <QObject>
#include <QKeyEvent>

class TextEdit : public QTextEdit
{
    Q_OBJECT
public:
    TextEdit(QWidget *parent = nullptr);

signals:
    void enterPressed();

protected:
    void keyPressEvent(QKeyEvent *event);
};

#endif // TEXTEDIT_H
