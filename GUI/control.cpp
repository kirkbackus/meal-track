#include "gui.h"

Control::Control() {
    colBackground = COLOR_BACKGROUND;
}

void Control::SetPosition(POINT p) {
    rect.left = p.x;
    rect.top = p.y;
    SetWindowPos(hWnd, NULL, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, SWP_NOZORDER);
}

void Control::SetSize(POINT s) {
    rect.right = rect.left + s.x;
    rect.bottom = rect.top + s.y;
    SetWindowPos(hWnd, NULL, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, SWP_NOZORDER);
}

void Control::SetWidth(UINT w) {
    rect.right = rect.left + w;
    SetWindowPos(hWnd, NULL, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, SWP_NOZORDER);
}

void Control::SetHeight(UINT h) {
    rect.bottom = rect.right + h;
    SetWindowPos(hWnd, NULL, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, SWP_NOZORDER);
}

void Control::SetText(LPCTSTR t) {
    SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)t);
}

void Control::SetTitle(LPCTSTR t) {
    SetWindowText(hWnd, t);
}

void Control::SetEnabled(bool e) {
    EnableWindow(hWnd, e);
}

void Control::GetMessage(MSG* msg) {
    GetMessageW(msg, hWnd, 0, 0);
}

POINT Control::GetPosition() {
    POINT p; p.x = rect.left; p.y = rect.top;
    return(p);
}

POINT Control::GetSize() {
    POINT s; s.x = rect.right-rect.left; s.y = rect.bottom-rect.top;
    return(s);
}

LPCTSTR Control::GetTitle() {
    LPCTSTR t = "";
    GetWindowText(hWnd, (char*)t, 65536);
    return(t);
}

HWND Control::GetHandle() {
    return(hWnd);
}

HINSTANCE Control::GetInstance() {
    return(hInst);
}

void Control::Invalidate() {
    RECT tmp = {0,0, rect.right-rect.left, rect.bottom-rect.top};
    InvalidateRect(hWnd, &tmp, 1);
}

void Control::OnEraseBackground(HDC bHDC) {
    // Set brush to desired background color
    HBRUSH backBrush = CreateSolidBrush(colBackground);

    // Save old brush
    HBRUSH pOldBrush = (HBRUSH)SelectObject(bHDC, backBrush);

    RECT rect;
    GetClipBox(bHDC, &rect);     // Erase the area needed

    PatBlt(bHDC, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, PATCOPY);
    SelectObject(bHDC, pOldBrush);
}
