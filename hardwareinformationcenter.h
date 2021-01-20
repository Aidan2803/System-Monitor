#ifndef HARDWAREINFORMATIONCENTER_H
#define HARDWAREINFORMATIONCENTER_H

#include <QDebug>

#include <iostream>
#include <windows.h>
#include <chrono>

using namespace std;

class HardWareInformationCenter
{
public:
    HardWareInformationCenter();

    void setIsGetUpTimeLoopRunning(bool isRunning){this->isGetUpTimeLoopRunning = isRunning;}

    static void getUptime(long &hours, long &minutes, long &seconds, long &millies, HardWareInformationCenter &hc);
    //get drivers;
    //get hardware names;
private:
    bool isGetUpTimeLoopRunning;
};

#endif // HARDWAREINFORMATIONCENTER_H
