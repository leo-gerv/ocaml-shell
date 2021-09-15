#ifndef COMPLETER_MENU_H
#define COMPLETER_MENU_H

#include <QListWidget>
#include <QActionEvent>
#include <QAction>
#include <QWidget>
#include <QStringList>

// TODO

class CompleterMenu : public QListWidget
{
    Q_OBJECT
public:
    CompleterMenu(QWidget *parent);

public slots:
    void fill(QStringList elements);

signals:
    void textSelected(QString text);
};

#endif // COMPLETER_MENU_H
