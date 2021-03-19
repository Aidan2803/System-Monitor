#include "hardwareinformationcenter.h"

HardWareInformationCenter::HardWareInformationCenter() : isGetUpTimeLoopRunning{true}
{}

void HardWareInformationCenter::initCOM(){
    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
        if (FAILED(hres))
        {
            cout << "Failed to initialize COM library. Error code = 0x" << hex << hres << endl; // Program has failed.
        }
}

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

        hours = hr;
        minutes = min;
        seconds = sec;
        millies = buff;

        Sleep(1000);
    }
}

QString HardWareInformationCenter::getCPUInfo() const{
    // Get extended ids
        int CPUInfo[4] = { -1 };
        __cpuid(CPUInfo, 0x80000000);
        unsigned int nExIds = CPUInfo[0];

        // Get the information associated with each extended ID
        char CPUBrandString[0x40] = { 0 };
        for (unsigned int i = 0x80000000; i <= nExIds; ++i)
        {
            __cpuid(CPUInfo, i);

            // Interpret CPU brand string and cache information
            if (i == 0x80000002)
            {
                memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
            }
            else if (i == 0x80000003)
            {
                memcpy(CPUBrandString + 16,	CPUInfo, sizeof(CPUInfo));
            }
            else if (i == 0x80000004)
            {
                memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
            }
        }

       // cout << "Cpu String: " << CPUBrandString;

        unsigned short counterOfNotNullChars{0};

        for(int i{0}; i < 64; ++i){
            if(CPUBrandString[i] != 0){
                ++counterOfNotNullChars;
            }
        }

        QString str = QString::fromUtf8(CPUBrandString, counterOfNotNullChars);

        return str;
}

QString HardWareInformationCenter::getGPUInfo() const{
    QString test;

    for (int i = 0; ; i++)
        {
            DISPLAY_DEVICE dd = { sizeof(dd), 0 };
            BOOL f = EnumDisplayDevices(NULL, i, &dd, EDD_GET_DEVICE_INTERFACE_NAME);
            if (!f)
                break;

            if(i == 0){
                 test = QString::fromWCharArray(dd.DeviceString);
            }
         }

    return test;
}

QString HardWareInformationCenter::getRAMInfo() const {


}
