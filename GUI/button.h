#ifndef __GUI__BTN__H
#define __GUI__BTN__H

class Button: public Control {
    private:

    HFONT hFont;

    public:

    Button();
    Button(Control, RECT, LPCSTR, int);
};

#endif
