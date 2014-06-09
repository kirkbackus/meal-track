#include <windows.h>
#include <iostream>

class Scale {
    private:

    HANDLE hScale;

    char serialName[32];
    float weight;
    char unit[5];
    bool stable;
    int ouflow;

    bool isConnected;
    int lastError;

    bool Connect(DWORD, DWORD);
    bool Connect(LPCSTR , DWORD, DWORD);
    int EnumCommPorts(char**, DWORD);
    HANDLE Initialize(char*, DWORD, DWORD);

    bool SendData(char* data);
    bool ReadData(char* data, DWORD length);

    public:

    Scale();
    Scale(LPCSTR);
    ~Scale();

    bool IsConnected();
    char* GetName();

    void Zero();
    bool GetReading();
    bool IsStable();

    float GetWeight();
    char* GetUnit();
    int GetOUFlow();
    int GetError();
};
