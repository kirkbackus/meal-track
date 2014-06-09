#ifndef __GUI__LST__H
#define __GUI__LST__H

class ListBox: public Control {
    private:

    public:

    ListBox();
    ListBox(Control, RECT, LPCSTR);

    void AddItem(LPCSTR);
    void SetItem(UINT, LPCSTR);
    void Clear();
};

#endif
