#include "gui.h"

ListBox::ListBox() {
}

ListBox::ListBox(Control c, RECT r, LPCSTR t) {
    Control();

    //Create font and the button
    hFont = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, "Arial");
    hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, "ListBox", t, WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_VSCROLL,
                        r.left, r.top, r.right, r.bottom,
                        c.GetHandle(), 0, c.GetInstance(), NULL);

    //Retrieve the rectangle
    rect = r;

    //Set the button font and make sure it's visible
    SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, 0);
    ShowWindow(hWnd, SW_SHOW);
}

void ListBox::AddItem(LPCSTR s) {
    SendMessage(hWnd, (UINT)LB_ADDSTRING, (WPARAM)0, (LPARAM)s);
}

void ListBox::SetItem(UINT i, LPCSTR s) {
    SendMessage(hWnd, (UINT)LB_SETITEMDATA, (WPARAM)i, (LPARAM)s);
}

void ListBox::Clear() {
    SendMessage(hWnd, (UINT)LB_RESETCONTENT, 0, 0);
}
