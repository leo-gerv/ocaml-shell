#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QTextEdit>
#include <QObject>
#include <QKeyEvent>

#include <QDebug>

class TextEdit : public QTextEdit
{
    Q_OBJECT
public:
    TextEdit(QWidget *parent = nullptr);

private:

signals:
    void enterPressed();
    void previousExp();
    void nextExp();
    void tabRequest();

protected:
    void keyPressEvent(QKeyEvent *event);
};

#endif // TEXTEDIT_H
