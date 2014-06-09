#include <windows.h>
#include <time.h>
#include <process.h>
#include <math.h>

#include "GUI/gui.h"
#include "resource.h"
#include "scale.h"

#define WM_UPDATE_READING       0x0400
#define WM_SCALE_CONNECTED      0x0401
#define WM_SCALE_DISCONNECTED   0x0402

//Declare Windows procedure
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

//Declare Custom functions
void CreateWindowMenu(Window);
void HandleMenu(HWND, DWORD);
void GetScaleReading(void*);
void ScaleConnect(void*);
void Meal(void*);
void GetSaveFile(HWND);
bool SaveMeal();
void AddCurrentValue();
int CheckAndSave();

void ToggleScaleMenu(bool);

//Create menus/popup-menus
Menu mTitle(0);
Menu mFile(1);
Menu mScale(1);

//Initialize the controls
Window cWindow;
Button cButton;
ListBox cListBox;
EditBox cEditBox;

//Initialize the labels
Label cLabelDelay;
EditBox cEditDelay;
Label cLabelDate;
ComboBox cComboDate;
Label cLabelRecord;
ComboBox cComboRecord;
Label cLabelSensitivity;
ComboBox cComboSensitivity;

//Data to store the meal
float mMass[1024];
struct tm* mTime[1024];
LPCSTR mUnit[1024];
int mNumMeals = 0;
int mCompare = 1;
float mSensitivity = 0;

//Option variables
bool bMealActive;
UINT iMealWait;
UINT uLastStable;
float fLastWeight;

//For saving
char cSaveName[300] = "\0";
bool bSaved = 0;

//Initialize scale
Scale* scale;
HANDLE hScaleThread;

//Entry point of the program
int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    //Create a window
    cWindow = Window(WindowProcedure, hThisInstance, "MealTrackApp", nCmdShow);
    cWindow.Create("MealTrack - Untitled", 462, 375);

    //Actually create the button with the window as its parent
    RECT rEditBox = {16, 280, 272, 24};
    cEditBox = EditBox(cWindow, rEditBox, "Disconnected");
    cEditBox.SetReadOnly(1);

    //Create the Button
    RECT rButton = {304, 280, 128, 24};
    cButton = Button(cWindow, rButton, "Start Meal", IDR_START_BUTTON);
    cButton.SetEnabled(0);

    //Create the listbox
    RECT rListBox = {16, 16, 272, 272};
    cListBox = ListBox(cWindow, rListBox, "MealListBox");

    //Meal wait box
    RECT rLabelDelay = {304, 16, 128, 16};
    RECT rEditDelay = {304, 32, 128, 24};
    cLabelDelay = Label(cWindow, rLabelDelay, "Meal wait (seconds)");
    cEditDelay = EditBox(cWindow, rEditDelay, "10");

    //Create Date format box
    RECT rLabelDate = {304, 64, 128, 16};
    RECT rComboDate = {304, 80, 128, 24};
    cLabelDate = Label(cWindow, rLabelDate, "Date format");
    cComboDate = ComboBox(cWindow, rComboDate, "ComboBoxDate");
    cComboDate.AddItem("12 Hour AM/PM");
    cComboDate.AddItem("24 Hour");
    cComboDate.SetDefaultItem(1);

    //Record format box
    RECT rLabelRecord = {304, 112, 128, 16};
    RECT rComboRecord = {304, 128, 128, 24};
    cLabelRecord = Label(cWindow, rLabelRecord, "Record change type");
    cComboRecord = ComboBox(cWindow, rComboRecord, "ComboBoxRecord");
    cComboRecord.AddItem("Increases");
    cComboRecord.AddItem("Decreases");
    cComboRecord.AddItem("Both");
    cComboRecord.SetDefaultItem(1);

    //Record format box
    RECT rLabelSensitivity = {304, 160, 128, 16};
    RECT rComboSensitivity = {304, 176, 128, 24};
    cLabelSensitivity = Label(cWindow, rLabelSensitivity, "Sensitivity");
    cComboSensitivity = ComboBox(cWindow, rComboSensitivity, "ComboBoxSensitivity");
    cComboSensitivity.AddItem("0.01 g");
    cComboSensitivity.AddItem("0.02 g");
    cComboSensitivity.AddItem("0.03 g");
    cComboSensitivity.AddItem("0.04 g");
    cComboSensitivity.AddItem("0.05 g");
    cComboSensitivity.AddItem("0.06 g");
    cComboSensitivity.AddItem("0.07 g");
    cComboSensitivity.AddItem("0.08 g");
    cComboSensitivity.AddItem("0.09 g");
    cComboSensitivity.SetDefaultItem(2);

    //Custom function to creeate window
    CreateWindowMenu(cWindow);

    //Message loop
    MSG msg;
    while (cWindow.GetMessage(&msg))
    {
        // Translate virtual-key messages into character messages
        TranslateMessage(&msg);
        // Send message to WindowProcedure
        DispatchMessage(&msg);
    }

    // The program return-value is 0 - The value that PostQuitMessage() gave
    return msg.wParam;
}

//  This function is called by the Windows function DispatchMessage()
LRESULT CALLBACK WindowProcedure (HWND hWnd, UINT mMsg, WPARAM wParam, LPARAM lParam)
{
    switch (mMsg)
    {
        case WM_CREATE:
            break;

        case WM_SIZE:
            break;

        case WM_COMMAND:
            //If handle is the window, handle the menu
            if (hWnd == cWindow.GetHandle())
                HandleMenu(hWnd, LOWORD(wParam));

            if (LOWORD(wParam) == IDR_START_BUTTON) {
                //Check if we have a connection to the scale
                if (scale != NULL && scale->IsConnected()) {
                    //Check if we just finished a meal as well, if we didn't, start one
                    if (!bMealActive) { //If the meal is not active, start it or reset it
                        if (mNumMeals == 0) {
                            _beginthread(Meal, 0, (void*)hWnd);
                        } else {
                            HandleMenu(hWnd, ID_FILE_NEW);
                        }
                    } else { //Otherwise, stop it
                        if (strlen(cSaveName)<=1) GetSaveFile(hWnd);
                        bMealActive = 0;
                    }
                }
            }
            break;

        case WM_CLOSE:
            if (strlen(cSaveName) > 1 && !SaveMeal()) {
                if (CheckAndSave() == -1) break;
            } else if (!bSaved && mNumMeals > 0) {
                switch(MessageBox(hWnd, "You haven't saved the current meal, would you like to?","Hold Up!", MB_YESNOCANCEL | MB_ICONINFORMATION)) {
                    case IDYES:
                        if (CheckAndSave() == -1) break;
                    case IDNO:
                        if (scale) delete scale;
                        DestroyWindow(hWnd);
                        hWnd = NULL;
                    break;

                    case IDCANCEL:
                    break;

                    default:
                    break;
                }
            } else {
                if (hWnd != NULL) {
                    if (scale) delete scale;
                    DestroyWindow(hWnd);
                }
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_UPDATE_READING:
            //Destroy any spare messages if scale is null
            if (scale == NULL) break;

            //Retreive the information and print it on the button
            char tBuf[32];
            sprintf(tBuf, "Connected on %s - %.2f %s %s", scale->GetName(), scale->GetWeight(), scale->GetUnit(), (scale->IsStable()?"":"(unstable)"));
            cEditBox.SetText(tBuf);

            hScaleThread = (HANDLE)_beginthread(GetScaleReading, 0, (void*) hWnd);
            break;

        case WM_SCALE_CONNECTED:
            ToggleScaleMenu(1);
            break;

        case WM_SCALE_DISCONNECTED:
            ToggleScaleMenu(0);
            cEditBox.SetText("Disconnected");
            break;

        default:
            return DefWindowProc (hWnd, mMsg, wParam, lParam);
    }

    return 0;
}

void CreateWindowMenu(Window w) {
    //File Menu
    mFile.AddItem(ID_FILE_NEW, "&New");
    mFile.AddItem(ID_FILE_SAVE, "&Save");
    mFile.AddBreak();
    mFile.AddItem(ID_FILE_EXIT, "E&xit");

    //Scale Menu
    mScale.AddItem(ID_SCALE_CONNECT, "&Connect");
    mScale.AddItem(ID_SCALE_ZERO, "&Zero", MF_DISABLED);

    //Add the sub menu's to the title menu
    mTitle.AddSubMenu(mFile, "&File");
    mTitle.AddSubMenu(mScale, "&Scale");

    //Set the window's menu to the title menu
    w.SetWindowMenu(mTitle);
}

void ToggleScaleMenu(bool a) {
    if (a) { //If scale is active
        mScale.EditItem(ID_SCALE_CONNECT, "&Disconnect");
        mScale.EditItem(ID_SCALE_ZERO, "&Zero", 0);
        cButton.SetEnabled(1);
    } else {
        mScale.EditItem(ID_SCALE_CONNECT, "&Connect");
        mScale.EditItem(ID_SCALE_ZERO, "&Zero", MF_DISABLED);
        cButton.SetEnabled(0);
    }
}

void HandleMenu(HWND hWnd, DWORD opt) {
    switch(opt) {
        case ID_SCALE_CONNECT:
            cEditBox.SetText("Trying to communicate with the scale...");
            mScale.EditItem(ID_SCALE_CONNECT, "&Connecting...", MF_DISABLED);
            _beginthread(ScaleConnect, 0, (void*) hWnd);
        break;

        case ID_FILE_NEW:
            if (!bSaved && mNumMeals > 0) {
                switch(MessageBox(hWnd, "You haven't saved the current meal, would you like to?","Hold Up!", MB_YESNOCANCEL | MB_ICONINFORMATION)) {
                    case IDYES:
                        if (CheckAndSave()==-1) break;
                    case IDNO:
                        cListBox.Clear();
                        mNumMeals = 0;
                        strcpy(cSaveName, "");
                        cWindow.SetTitle("MealTrack - Untitled");
                        cButton.SetText("Start Meal");
                    break;

                    case IDCANCEL:
                    default:
                    break;
                }
            } else {
                cListBox.Clear();
                mNumMeals = 0;
                strcpy(cSaveName, "");
                cWindow.SetTitle("MealTrack - Untitled");
                cButton.SetText("Start Meal");
            }
        break;

        case ID_FILE_SAVE:
            GetSaveFile(hWnd);
            if (mNumMeals >= 1) CheckAndSave();

            char tmp[313];
            sprintf(tmp, "MealTrack - %s", strrchr(cSaveName,'\\') + 1);
            cWindow.SetTitle(tmp);
        break;

        case ID_FILE_EXIT:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
        break;

        case ID_SCALE_ZERO:
            if (scale != NULL) scale->Zero();
        break;
    }
}

void GetScaleReading(void* args) {
    //Acquire a reading from the scale
    if (scale != NULL) {
        scale->GetReading();
        SendMessage((HWND)args, WM_UPDATE_READING, 0, 0);
    }
}

void ScaleConnect(void* args) {
    //If we aren't connected, connect!
    if (scale == NULL) {
        //Create a new scale and attempt to connect
        scale = new Scale("COM1");
        if (!scale->IsConnected()) {
            delete scale;
            scale = new Scale();
        }

        if (!scale->IsConnected()) {
            char err[256];
            sprintf(err, "Failed to connect! ERROR: %i", scale->GetError());

            mScale.EditItem(ID_SCALE_CONNECT, "&Connect");
            cEditBox.SetText(err);
            delete scale;
            scale = NULL;
            return;
        }

        //Begin the thread which sends a message to the Window's event and loops it
        hScaleThread = (HANDLE)_beginthread(GetScaleReading, 0, args);

        //Switch the menu to allow for disconnect & other features
        SendMessage((HWND)args, WM_SCALE_CONNECTED, 0, 0);
    } else {
        //Deactivate the meal
        bMealActive = 0;

        //Delete the scale which will disconnect it
        delete scale;
        scale = NULL;

        SendMessage((HWND)args, WM_SCALE_DISCONNECTED, 0, 0);
    }
}

void Meal(void* args) {
    //Disable buttons/settings
    cButton.SetText("End Meal");
    cEditDelay.SetEnabled(0);
    cComboDate.SetEnabled(0);
    cComboRecord.SetEnabled(0);
    cComboSensitivity.SetEnabled(0);
    mScale.EditItem(ID_SCALE_ZERO, "&Zero", MF_DISABLED);

    //Get Variables
    char* tmpWait = new char[255];
    cEditDelay.GetText(tmpWait);
    iMealWait = atoi(tmpWait);
    if (iMealWait==0) iMealWait = 10;

    mCompare = cComboRecord.GetSelectedItem();
    mSensitivity = ((float)cComboSensitivity.GetSelectedItem()+1.0)/100.0;

    //Initialize the first weight
    AddCurrentValue();

    //Activate the meal
    bMealActive = 1;

    while(bMealActive) {
        //So if the scale is not stable, set the current time to when the scale was last stable
        if (!scale->IsStable()) uLastStable = time(0);

        if (time(0) - uLastStable >= iMealWait) {
            if (scale!=NULL && scale->IsConnected()) {
                if (fabs(scale->GetWeight() - fLastWeight) >= mSensitivity) {
                    if ((mCompare == 0 || mCompare == 2) && scale->GetWeight() > fLastWeight) AddCurrentValue();
                    if ((mCompare == 1 || mCompare == 2) && scale->GetWeight() < fLastWeight) AddCurrentValue();
                }
            }
        }

        //Sleep for .1 second
        Sleep(100);
    }

    //Check and see if the person saved the file
    CheckAndSave();

    //Enable them
    cButton.SetText("New Meal");
    cEditDelay.SetEnabled(1);
    cComboDate.SetEnabled(1);
    cComboRecord.SetEnabled(1);
    cComboSensitivity.SetEnabled(1);
    mScale.EditItem(ID_SCALE_ZERO, "&Zero", 0);
}

void AddCurrentValue() {
    //For temporary strings
    char tmp[128], tmp2[128];
    time_t rwtm;

    //Retrieve the current time
    time(&rwtm);

    //Get the current values from the scale and computer
    fLastWeight = scale->GetWeight();
    mMass[mNumMeals] = fLastWeight;
    mTime[mNumMeals] = new tm(*localtime(&rwtm));
    mUnit[mNumMeals] = scale->GetUnit();

    //Convert time to string (12/24 hour format)
    if (cComboDate.GetSelectedItem()==0) strftime(tmp2, 128, "%I:%M:%S %p", mTime[mNumMeals]);
    else strftime(tmp2, 128, "%H:%M:%S", mTime[mNumMeals]);

    //Print to the textbox
    sprintf(tmp, "%s: %.2f %s", tmp2, mMass[mNumMeals], mUnit[mNumMeals]);

    mNumMeals++;
    cListBox.AddItem(tmp);
}

void GetSaveFile(HWND hWnd) {
    OPENFILENAME ofName;

    ZeroMemory(&ofName, sizeof(ofName));
    ofName.lStructSize = sizeof(ofName);
    ofName.hwndOwner = hWnd;
    ofName.lpstrFile = cSaveName;

    ofName.lpstrFile[0] = '\0';
    ofName.nMaxFile = sizeof(cSaveName);
    ofName.lpstrFilter = "Comma Seperated Values\0*.csv\0";
    ofName.nFilterIndex = 1;
    ofName.lpstrFileTitle = NULL;
    ofName.nMaxFileTitle = 0;
    ofName.lpstrInitialDir = NULL;

    GetSaveFileName(&ofName);
}

bool SaveMeal() {
    if (cSaveName==NULL || strlen(cSaveName)<=1) return 0;
    if (mNumMeals<=0) return 0;

    if (strstr(cSaveName,".csv") == NULL) strcat(cSaveName,".csv");

    //Open the file
    FILE* fSave = fopen(cSaveName, "w+");
    if (fSave == NULL) return 0;

    char tmp[313];
    strftime(tmp, 128, "%m/%d/%Y", mTime[0]);

    if (fprintf(fSave, "MEAL START: %s,,\n", tmp) == 0) {
        fclose(fSave);
        return 0;
    }

    fprintf(fSave, "Time,Mass,Unit\n");

    for (int i=0; i<mNumMeals; i++) {
        if (cComboDate.GetSelectedItem()==0) strftime(tmp, 128, "%I:%M:%S %p", mTime[i]);
        else strftime(tmp, 128, "%H:%M:%S", mTime[i]);
        fprintf(fSave, "%s,%.2f,%s\n", tmp, mMass[i], mUnit[i]);
    }

    strftime(tmp, 128, "%m/%d/%Y", mTime[mNumMeals-1]);
    fprintf(fSave, "MEAL END: %s,,\n", tmp);

    fclose(fSave);

    sprintf(tmp, "MealTrack - %s", strrchr(cSaveName,'\\') + 1);
    cWindow.SetTitle(tmp);

    bSaved = 1;
    return(1);
}

int CheckAndSave() {
    //Get the filename if we don't have one
    if (strlen(cSaveName)<=1) GetSaveFile(cWindow.GetHandle());

    //Save the meal & make sure it saved
    bool okay = 0;
    while(!okay) {
        if (!SaveMeal()) {
            switch(MessageBox(cWindow.GetHandle(), "Failed to save the file, do you wish to try again?", "Error", MB_YESNOCANCEL | MB_ICONERROR)) {
                case IDYES:
                GetSaveFile(cWindow.GetHandle());
                break;

                case IDNO:
                return(0);
                break;

                case IDCANCEL:
                return(-1);
                break;

                default:
                break;
            }
        } else {
            okay = 1;
        }
    }

    return(1);
}
