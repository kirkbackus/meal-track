#include "gui.h"

Button::Button() {
    //Call the initial Control
    Control();
}

Button::Button(Control c, RECT r, LPCSTR t, int a) {
    //Call the initial Control
    Control();

    //Create font and the button
    hFont = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, "Arial");
    hWnd = CreateWindowEx(0, "BUTTON", t, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                        r.left, r.top, r.right, r.bottom,
                        c.GetHandle(), (HMENU)a, c.GetInstance(), NULL);

    //Retrieve the rectangle
    rect = r;

    //Set the button font and make sure it's visible
    SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, 0);
}
