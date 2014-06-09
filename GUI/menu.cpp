#include "gui.h"

Menu::Menu(bool isPopup) {
    if (isPopup) hMenu = CreatePopupMenu();
    else hMenu = CreateMenu();
}

void Menu::AddItem(unsigned int id, LPCSTR n) {
    AppendMenu(hMenu, MF_STRING, (UINT_PTR)id, n);
}

void Menu::AddItem(unsigned int id, LPCSTR n, UINT add) {
    AppendMenu(hMenu, MF_STRING | add, (UINT_PTR)id, n);
}

void Menu::AddBreak() {
    AppendMenu(hMenu, MF_SEPARATOR, 0, "");
}

void Menu::EditItem(unsigned int id, LPCSTR n) {
    ModifyMenu(hMenu, (UINT_PTR)id, MF_BYCOMMAND | MF_STRING, (UINT_PTR)id, n);
}

void Menu::EditItem(unsigned int id, LPCSTR n, UINT add) {
    ModifyMenu(hMenu, (UINT_PTR)id, MF_BYCOMMAND | MF_STRING | add, (UINT_PTR)id, n);
}

void Menu::AddSubMenu(Menu sub, LPCSTR n) {
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)sub.GetHMenu(), n);
}

HMENU Menu::GetHMenu() {
    return(hMenu);
}
