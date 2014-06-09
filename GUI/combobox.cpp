#include "gui.h"

ComboBox::ComboBox() {
}

ComboBox::ComboBox(Control c, RECT r, LPCSTR t) {
    Control();

    //Create font and the button
    hFont = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, "Arial");
    hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, "COMBOBOX", t, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
                        r.left, r.top, r.right, r.bottom,
                        c.GetHandle(), 0, c.GetInstance(), NULL);

    //Retrieve the rectangle
    rect = r;

    //Set the button font and make sure it's visible
    SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, 0);
}

void ComboBox::AddItem(LPCSTR s) {
    SendMessage(hWnd, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)s);
}

void ComboBox::SetItem(UINT i, LPCSTR s) {
    SendMessage(hWnd, (UINT)CB_SETITEMDATA, (WPARAM)i, (LPARAM)s);
}

void ComboBox::SetDefaultItem(UINT i) {
    SendMessage(hWnd, CB_SETCURSEL, i, 0);
}

int ComboBox::GetSelectedItem() {
    return(SendMessage(hWnd, CB_GETCURSEL, 0, 0));
}

void ComboBox::Clear() {
    SendMessage(hWnd, (UINT)CB_RESETCONTENT, 0, 0);
}
