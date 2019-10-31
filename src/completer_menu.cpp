#include "completer_menu.h"

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
