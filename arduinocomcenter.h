#ifndef ARDUINOCOMCENTER_H
#define ARDUINOCOMCENTER_H

#include <globalVariables.h>
#include <windows.h>
#include <QDebug>

class ArduinoComCenter
{
public:
    ArduinoComCenter();


    bool getInfoForPackage(int cpuLoad, int cpuTemp, int gpuTemp, int ramLoad, int hdd1Temp, int hdd2Temp, int hdd3Temp, int hdd4Temp);

    void startCommunication();
private:

    enum PackageInfo{
        CPU_LOAD,
        CPU_TEMP,
        GPU_TEMP,
        RAM_LOAD,
        HDD1_TEMP,
        HDD2_TEMP,
        HDD3_TEMP,
        HDD4_TEMP
    };

    enum UpTimeInfo{
        HOURS,
        MINUTES,
        SECONDS,
        MAX_AMOUNT
    };


    char packageForTransfer[PACKAGE_SIZE];

    LPCTSTR comName;
    HANDLE hComPort;
    bool comState;

    int CPU_Load;
    int CPU_Temp;
    int GPU_Temp;
    int RAM_Load;
    int HDD1_Temp;
    int HDD2_Temp;
    int HDD3_Temp;
    int HDD4_Temp;
    int UpTimeArray[MAX_AMOUNT];

    bool send;

    bool openComPort();
    void writeToComPort();
    void closeCom();

    void createPackege();
};

#endif // ARDUINOCOMCENTER_H
