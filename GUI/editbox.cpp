#include "gui.h"

EditBox::EditBox() {
}

EditBox::EditBox(Control c, RECT r, LPCSTR t) {
    Control();

    //Create font and the button
    hFont = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, "Arial");

    hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, "Edit", t, WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                   r.left, r.top, r.right, r.bottom,
                   c.GetHandle(), NULL, c.GetInstance(), NULL);


    //Retrieve the rectangle
    rect = r;

    //Set the button font and make sure it's visible
    SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, 0);

    ShowWindow(hWnd, SW_SHOW);
}

void EditBox::SetReadOnly(bool r) {
    SendMessage(hWnd, EM_SETREADONLY, (WPARAM)r, 0);
}

void EditBox::GetText(char* t) {
    SendMessage(hWnd, WM_GETTEXT, (WPARAM)256, (LPARAM)t);
}
