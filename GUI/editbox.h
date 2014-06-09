#ifndef __GUI__EDT__H
#define __GUI__EDT__H

class EditBox: public Control {
    private:

    public:

    EditBox();
    EditBox(Control, RECT, LPCSTR);

    void SetReadOnly(bool);
    void GetText(char*);
};

#endif
