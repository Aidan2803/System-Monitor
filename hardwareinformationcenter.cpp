#include "hardwareinformationcenter.h"

HardWareInformationCenter::HardWareInformationCenter() : isGetUpTimeLoopRunning{true}
{}

void HardWareInformationCenter::getUptime(long &hours, long &minutes, long &seconds, long &millies, HardWareInformationCenter &hc){
    qDebug() << " hc.isGetUpTimeLoopRunning " << hc.isGetUpTimeLoopRunning;
    while(hc.isGetUpTimeLoopRunning){
        auto uptime = std::chrono::milliseconds(GetTickCount64());

        long buff = uptime.count();

        //3600000 milliseconds in an hour
        long hr = buff / 3600000;
        buff = buff - 3600000 * hr;
        //60000 milliseconds in a minute
        long min = buff / 60000;
        buff = buff - 60000 * min;

        //1000 milliseconds in a second
        long sec = buff / 1000;
        buff = buff - 1000 * sec;

        qDebug() << hr << " hours and " << min << " minutes and " << sec << " seconds and " << buff << " milliseconds." << endl;

        hours = hr;
        minutes = min;
        seconds = sec;
        millies = buff;

        Sleep(5000);
    }
}
