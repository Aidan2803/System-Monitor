#ifndef CPUUSAGE_H
#define CPUUSAGE_H

#include <windows.h>
#include <iostream>

class cpuUsage {
public:
    cpuUsage();
    ~cpuUsage() {};

    short getUsage(DWORD processID);
    void setLastRunNull();
private:

    //methods prototypes
    ULONGLONG SubTime(const FILETIME& fileTimeA, const FILETIME& fileTimeB);
    bool isEnoughTimePassed();
    bool isFirstRun() const { return (dwLastRun == 0); }

    //syst total time
    FILETIME fileTimePrevKernelSyst;
    FILETIME fileTimePrevUserSyst;

    //procc total time
    FILETIME fileTimePrevKernelProc;
    FILETIME fileTimePrevUserProc;

    short nCpuUsage;
    ULONGLONG dwLastRun;

    volatile LONG runCount;
};
#endif // CPUUSAGE_H
