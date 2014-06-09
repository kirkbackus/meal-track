#include "gui.h"

Window::Window() {
    Control();
}

Window::Window(WNDPROC WndProc, HINSTANCE hInstance, char* cClassName, int nCmdShow) {
    Control();

    //Assign local variables
    hInst = hInstance;
    iShow = nCmdShow;
    bRegistered = 0;

    //The window structure
    wClass.hInstance = hInst;
    wClass.lpszClassName = cClassName;
    wClass.lpfnWndProc = WndProc;
    wClass.style = CS_DBLCLKS;
    wClass.cbSize = sizeof (WNDCLASSEX);

    //Use default icon and default mouse pointer
    wClass.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wClass.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wClass.hCursor = LoadCursor (NULL, IDC_ARROW);
    wClass.lpszMenuName = NULL;
    wClass.cbClsExtra = 0;
    wClass.cbWndExtra = 0;

    //Set window background
    wClass.hbrBackground = (HBRUSH) COLOR_WINDOW;

    //Register the window class
    if (!RegisterClassEx (&wClass)) return;

    //Window was registered successfully
    bRegistered = 1;
}

bool Window::Create(LPCSTR sTitle, int iWidth, int iHeight) {
    //Check if class is registered
    if (bRegistered == 0) return 0;

    //Create the window
    hWnd = CreateWindowEx (
           0,
           wClass.lpszClassName,
           sTitle,
           WS_OVERLAPPEDWINDOW | WS_POPUPWINDOW,
           CW_USEDEFAULT,
           CW_USEDEFAULT,
           iWidth,
           iHeight,
           HWND_DESKTOP,
           NULL,
           wClass.hInstance,
           NULL
    );

    //Get window rect
    GetWindowRect(hWnd, &rect);

    //Check and make sure window was created
    if (hWnd == INVALID_HANDLE_VALUE) return 0;

    //Make the window visible on the screen
    ShowWindow (hWnd, iShow);

    //We created and showed the window successfully
    return 1;
}

//Retrieve the message from the window
bool Window::GetMessage(MSG* msg) {
    return(GetMessageW(msg, NULL, 0, 0));
}

//Set the window menu
void Window::SetWindowMenu(Menu m) {
    SetMenu(hWnd, m.GetHMenu());
}
