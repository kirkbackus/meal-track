#include "scale.h"

Scale::Scale() {
    isConnected = Connect(CBR_9600, 8);

    stable = 0;
    ouflow = 0;
    sprintf(unit,"g");
}

Scale::Scale(LPCSTR p) {
    isConnected = Connect(p, CBR_9600, 8);

    stable = 0;
    ouflow = 0;
    sprintf(unit,"g");
}

Scale::~Scale() {
    if (hScale != INVALID_HANDLE_VALUE) CloseHandle(hScale);
}

HANDLE Scale::Initialize(char* n, DWORD br, DWORD bits) {
    hScale = CreateFile(n, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
    sprintf(serialName, n);

    //Check if the values are correct
    if (hScale == INVALID_HANDLE_VALUE) {
        lastError = GetLastError();
        printf("INVALID HANDLE VALUE - ERROR: %i\n", lastError);

        return(hScale);
    }

    //Initialize Serial Communication Settings
    DCB dParams;
    dParams.DCBlength=sizeof(dParams);

    if (!GetCommState(hScale, &dParams)) {
        lastError = GetLastError();
        printf("FAILED TO RETREIVE COMM STATE\n");
    }

    dParams.BaudRate = br;
    dParams.ByteSize = bits;
    dParams.Parity = NOPARITY;

    dParams.StopBits = ONESTOPBIT;
    dParams.fDtrControl = DTR_CONTROL_HANDSHAKE;
    dParams.fRtsControl = RTS_CONTROL_HANDSHAKE;


    if(!SetCommState(hScale, &dParams)){
        lastError = GetLastError();
        printf("FAILED TO SET COMM STATE\n");
    }

    //Set the timeouts for reading data
    COMMTIMEOUTS cTimeOuts={0};

    cTimeOuts.ReadIntervalTimeout = 50;
    cTimeOuts.ReadTotalTimeoutConstant = 50;
    cTimeOuts.ReadTotalTimeoutMultiplier = 10;

    cTimeOuts.WriteTotalTimeoutConstant = 50;
    cTimeOuts.WriteTotalTimeoutMultiplier = 10;

    if(!SetCommTimeouts(hScale, &cTimeOuts)) {
        lastError = GetLastError();
        printf("FAILED TO SET COMM TIMEOUTS\n");
    }

    if (!SetCommMask(hScale, 0)) {
        lastError = GetLastError();
        printf("FAILED TO SET COMM MASK\n");
    }

    return(hScale);
}

bool Scale::Connect(DWORD br, DWORD bits) {
    //So basically find which comm ports exist
    int numports;
    LPCSTR ports[128];
    numports = EnumCommPorts((char**)ports, 128);

    printf("%i Ports detected.\n\n", numports);
    printf("Port Names\n");
    for (int i=0; i<numports; i++) {
        printf("%i: %s\n", i+1, ports[i]);
    }

    printf("\n");

    //Then connect to all of them and check if it is the scale
    for (int i=0; i<numports; i++) {
        printf("Trying to connect on port: %s\n", ports[i]);
        if (Connect(ports[i], br, bits)) return 1;
        /*//Check if we are connecte
        isConnected = (Initialize((char*)ports[i], br, bits) != INVALID_HANDLE_VALUE)  ? 1:0;

        //If we are connected, check and see if it is the scale
        if (isConnected) {
            //If we got a proper reading from the scale we found it and are connected
            int t = 4;

            printf("Trying:*");
            while(t-- > 0) {
                if (GetReading()) break;
                printf("*");
            }

            printf("\n");

            if (t > 0) {
                printf("CONNECTED!\n");
                return(1);
            } else { //Otherwise, disconnect and try again
                printf("FAILED TO CONNECT!\n");
                CloseHandle(hScale);
                isConnected = 0;
            }
        }
        */
    }

    //We couldn't find it.  Oh well...
    return(0);
}

bool Scale::Connect(LPCSTR p, DWORD br, DWORD bits) {
    isConnected = (Initialize((char*)p, br, bits) != INVALID_HANDLE_VALUE)  ? 1:0;

    //If we are connected, check and see if it is the scale
    if (isConnected) {
        //If we got a proper reading from the scale we found it and are connected
        int t = 4;

        printf("Trying:*");
        while(t-- > 0) {
            if (GetReading()) break;
            printf("*");
        }

        printf("\n");

        if (t > 0) {
            printf("CONNECTED!\n");
            return(1);
        } else { //Otherwise, disconnect and try again
            printf("FAILED TO CONNECT!\n");
            CloseHandle(hScale);
            isConnected = 0;
        }
    }

    return(0);
}

int Scale::EnumCommPorts(char** p, DWORD l) {
    HKEY hKey;

    char cName[1024];
    DWORD dNameLen = 1024;
    unsigned char cValue[1024];
    DWORD cValueLen = 1024;
    DWORD cType = REG_SZ;

    int i = 0;
    int err;

    if ((err = RegOpenKey(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", &hKey)) == ERROR_SUCCESS) {
        while((err = RegEnumValue(hKey, i, cName, &dNameLen, 0, &cType, cValue, &cValueLen)) == ERROR_SUCCESS) {
            //Allocate array to match the correct length
            p[i] = new char[cValueLen+1];
            sprintf(p[i],"%s",cValue);
            if (i >= (int)l) break;
            else i++;
        }

        printf("ERR: %i\n", err);

        RegCloseKey(hKey);
    } else {
        printf("ERROR: %i\n", err);
    }

    return(i);
}

bool Scale::IsConnected() {
    return(isConnected);
}

bool Scale::SendData(char* data) {
    DWORD bw = 0;
    if (data == NULL || strlen(data) == 0) return 0;
    return(WriteFile(hScale, data, strlen(data), &bw, 0));
}

bool Scale::ReadData(char* data, DWORD length) {
    DWORD br;
    return(ReadFile(hScale, data, length, &br, 0));
}


void Scale::Zero() {
    SendData("Z\r\n");
}

bool Scale::GetReading() {
    char read[256];
    char* temp;

    SendData("SI\r\n");
    ReadData(read, 256);

    //Make sure it returned something
    if (strlen(read) > 0) {
        //Split up string and read first string
        temp = strtok(read, " \r\n");

        //Should always return 'S' character
        if (strcmp(temp,"S")!=0) return 0;

        //Read second string which gives us information about what we are weighing
        temp = strtok(NULL, " \r\n");

        //Switch that value because it should be a character
        switch(temp[0]) {
            case 'S': //The scale is stable
                stable = 1;
                ouflow = 0;
            break;

            case 'D': //The scale is unstable
                stable = 0;
                ouflow = 0;
            break;

            case '+': //Overflow
                stable = 0;
                ouflow = 1;
                return 1;
            break;

            case '-': //Underflow
                stable = 0;
                ouflow = -1;
                return 1;
            break;

            case 'I': //Command not executable
                stable = 1;
                ouflow = 0;
                lastError = -1;
                return 1;
            break;

            default:
                stable = 1;
                ouflow = 0;
            break;
        }

        //Get the weight from the scale;
        temp = strtok(NULL, " \r\n");
        weight = atof(temp);

        //Finally, get the measured unit
        sprintf(unit, strtok(NULL, " \r\n"));
    } else {
        return 0;
    }

    return 1;
}

float Scale::GetWeight() {
    return(weight);
}

char* Scale::GetUnit() {
    return(unit);
}

int Scale::GetOUFlow() {
    return(ouflow);
}

bool Scale::IsStable() {
    return(stable);
}

char* Scale::GetName() {
    return(serialName);
}

int Scale::GetError() {
    return(lastError);
}
