#ifndef __GUI__WIN__H
#define __GUI__WIN__H

class Window: public Control {
    private:

    int iShow;

    WNDCLASSEX wClass;

    int w,h;
    bool bRegistered;

    public:

    Window();
    Window(WNDPROC, HINSTANCE, char*, int);
    bool Create(LPCSTR, int, int);
    bool GetMessage(MSG*);

    void SetWindowMenu(Menu);
};

#endif
