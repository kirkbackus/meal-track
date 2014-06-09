#include "gui.h"

Label::Label() {
}

Label::Label(Control c, RECT r, LPCSTR t) {
    Control();

    //Create font for the Label
    hFont = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, "Arial");

    //Create the static label
    hWnd = CreateWindowEx(WS_EX_CONTROLPARENT, "Static", t, WS_TABSTOP | WS_VISIBLE | WS_CHILD,
                        r.left, r.top, r.right, r.bottom,
                        c.GetHandle(), NULL, c.GetInstance(), NULL);

    //Retrieve the rectangle
    rect = r;

    //Set the button font and make sure it's visible
    SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, 0);
    ShowWindow(hWnd, SW_SHOW);
}
