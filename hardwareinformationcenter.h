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
    QString getRAMInfo();


    //get drivers;
    //get hardware names;
private:
    bool isGetUpTimeLoopRunning;

    HRESULT hres;

    IWbemServices *pSvc;
    IWbemLocator *pLoc;

    void initCOM();
    void WMI_getRAMInfo(QString *pArrToWrite);
    void cleanUpCOM();
};

#endif // HARDWAREINFORMATIONCENTER_H
