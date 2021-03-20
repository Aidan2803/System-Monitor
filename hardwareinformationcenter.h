#ifndef HARDWAREINFORMATIONCENTER_H
#define HARDWAREINFORMATIONCENTER_H

#include <QDebug>

#include <iostream>
#include <windows.h>
#include <chrono>
#include <string>

#include <comdef.h>
#include <Wbemidl.h>
#include <comutil.h>

#include "globalVariables.h"

#define _WIN32_DCOM

#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib,"user32.lib")

using namespace std;

class HardWareInformationCenter
{
public:
    HardWareInformationCenter();

    void setIsGetUpTimeLoopRunning(bool isRunning){this->isGetUpTimeLoopRunning = isRunning;}

    static void getUptime(long &hours, long &minutes, long &seconds, long &millies, HardWareInformationCenter &hc);


    QString getCPUInfo() const;
    QString getGPUInfo() const;
    QString* getRAMInfo(int *amountOfBars);
    QString getBaseboardInfo();
    QString* getStorageInfo(int *amountOfDisks);
    QString* getAudioDevicesInfo(int *amountOfDevices);

    //get drivers;
    //get hardware names;
private:
    bool isGetUpTimeLoopRunning;
    QString ramInfo[AMOUNT_OF_PHYSICAL_MEMORY_BARS];
    QString storageInfo[AMOUNT_OF_PHYSICAL_DISKS];
    QString audioInfo[AMOUNT_OF_PHYSICAL_DISKS];

    HRESULT hres;

    IWbemServices *pSvc;
    IWbemLocator *pLoc;

    void initCOM();

    void WMI_getRAMInfo(QString *pArrToWrite, int *amountOfBars);
    QString WMI_getBaseboardInfo();
    void WMI_getStorageInfo(QString *pArrToWrite, int *amountOfDisks);
    void WMI_getAudioDevicesInfo(QString *pArrToWrite, int *amountOfDevices);

    void cleanUpCOM();
};

#endif // HARDWAREINFORMATIONCENTER_H
