#ifndef __GUI__CTR__H
#define __GUI__CTR__H

class Control {
    protected:

    //Handles
    HWND hWnd;
    HDC hDC;
    HFONT hFont;
    HINSTANCE hInst;

    //POINT pos, size;
    RECT rect;
    int x,y,w,h;

    //Color
    COLORREF colText;
    COLORREF colBackground;

    public:

    Control();

    //Seting dimensional options
    void SetPosition(POINT);
    void SetSize(POINT);
    void SetWidth(UINT);
    void SetHeight(UINT);

    //Setting other things
    void SetText(LPCTSTR);
    void SetTitle(LPCTSTR);
    void SetEnabled(bool);

    //Get information from the handle
    void GetMessage(MSG*);
    HWND GetHandle();
    HINSTANCE GetInstance();

    //Retreiving dimensions
    POINT GetPosition();
    POINT GetSize();

    //Retreiving other things
    LPCTSTR  GetTitle();

    //Drawing Functions
    void Invalidate();
    void OnEraseBackground(HDC);
};


#endif
