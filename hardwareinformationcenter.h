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

    static void getUptime(long &hours, long &minutes, long &seconds, long &millies);
    //get drivers;
    //get hardware names;
};

#endif // HARDWAREINFORMATIONCENTER_H
