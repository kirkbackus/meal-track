#ifndef __GUI__CMB__H
#define __GUI__CMB__H

class ComboBox: public Control {
    private:

    public:

    ComboBox();
    ComboBox(Control, RECT, LPCSTR);

    void AddItem(LPCSTR);
    void SetItem(UINT, LPCSTR);
    void SetDefaultItem(UINT i);
    int GetSelectedItem();
    void Clear();
};

#endif
