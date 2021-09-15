#include "completer_menu.h"

// TODO

CompleterMenu::CompleterMenu(QWidget *parent): QListWidget(parent)
{
    hide();
}

void CompleterMenu::fill(QStringList elements)
{
    for (int i=0, l=elements.length(); i<l; i++) {
        addItem(elements[i]);
    }
}
