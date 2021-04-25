#ifndef ARDUINOCOMCENTER_H
#define ARDUINOCOMCENTER_H

#include <globalVariables.h>
#include <windows.h>

class ArduinoComCenter
{
public:
    ArduinoComCenter();

    void getInfoForPackage();

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


    char packageForTranser[PACKAGE_SIZE];

    LPCTSTR m_comName;

    int CPU_Load;
    int CPU_Temp;
    int GPU_Temp;
    int RAM_Load;
    int HDD1_Temp;
    int HDD2_Temp;
    int HDD3_Temp;
    int HDD4_Temp;
    int UpTimeArray[MAX_AMOUNT];



    void openComPort();
    void writeToComPort();

    void createPackege();
};

#endif // ARDUINOCOMCENTER_H
