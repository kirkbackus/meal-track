#ifndef __GUI__MNU__H
#define __GUI__MNU__H

class Menu {
    private:

    HMENU hMenu;

    public:

    Menu(bool);

    //Creates menu items
    void AddItem(unsigned int, LPCSTR);
    void AddItem(unsigned int, LPCSTR, UINT);
    void AddBreak();

    //Add a sub menu
    void AddSubMenu(Menu, LPCSTR);

    //Edit menu items
    void EditItem(unsigned int, LPCSTR);
    void EditItem(unsigned int, LPCSTR, UINT);

    HMENU GetHMenu();
};

#endif
