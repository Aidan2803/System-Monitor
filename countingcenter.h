#ifndef COUNTINGCENTER_H
#define COUNTINGCENTER_H

#include "mainwindow.h"

#include <QObject>
#include <QDebug>
#include <QWidget>
#include <QMessageBox>

#include<iostream>
#include <windows.h>
#include <psapi.h>
#include <string>
#include <chrono>
#include <ctime>
#include <vector>

#include "smarthandle.h"	//for windows.h and TlHelp32.h
#include "cpuusage.h"


class CountingCenter : public QObject
{
    Q_OBJECT;
public:    
    cpuUsage usage;		//to use getUsage for getting current usage of CPU by certain process6   

    struct processInfo {	//to store
        PROCESSENTRY32 pe;
        std::vector<THREADENTRY32> threads;
    };
    //PROCESS_MEMORY_COUNTERS pmc;	//for RAM monitoring
    PROCESS_MEMORY_COUNTERS_EX pmc;

    std::vector<processInfo> processInfosCPU; //list of processes to operate with (CPU)
    std::vector<processInfo> processInfosRAM; //list of processes to operate with (RAM)

    void setUserAcceptableCpuLoad(short load);
    void setUserAcceptableRamLoad(int load);
    void setCpuMonitoringActive(bool isActive);
    void setRamMonitoringActive(bool isActive);
    void setRamMonitoringType(bool type);
    void setOutputTypeMt(int type);
    void setOutputTypeMtg(bool type);
    void setStopFromUiCpuProcess(bool isStop);
    void setStopFromUiRamProcess(bool isStop);
    void setStopFromUiCpuLoadOverall(bool isStop);
    void setStopFromUiRamLoadOverall(bool isStop);
    void setUserAcceptableCpuLoadOverall(float load);
    void setUserAcceptableRamLoadOverall(int load);
    void setIgnoreProcessesVectorElement(bool fromCpu, QString elem);

    void createFile(int whichFile, bool global, QString fileName = "");

    int getProcesses(bool debFromCpu); //func that gets procesess we are working with, they are stored in procesInfos

    bool processCPUProcLoadMonitoring(DWORD processID, const WCHAR* exeFile); // func that counts CPU load from certain process;
    bool processRAMProcLoadMonitoring(DWORD processID, const WCHAR* exeFile); // func that counts RAM load from certain process;

    bool overallCpuLoadMonitoring();        //monitoring for unnaccaptable values CPU
    bool overallRamLoadMonitoring();        //monitoring for unnaccaptable values RAM

    float calculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks);     //calculates cpu load (starts in getCPULoad)
    unsigned long long fileTimeToInt64(const FILETIME & ft);     //convert FileTime to int64

    float getCPULoad();    //gets current system info and starts calculateCpuLoad function
    float getRAMLoad();     //gets current system info about memory returns (USED_MEMORY/ALL_MEMORY) * 100

    void monitoringCpuStart();  // starts thread that gets pointer to working thread (CPU)
    void monitoringRamStart();  // starts thread that gets pointer to working thread (RAM)

    void monitoringCpuOverallStart();
    void monitoringRamOverallStart();

    DWORD watchCpuProcThread(void); //thread where getting values from processCPUProcLoadMonitoring
    DWORD watchRamProcThread(void); //thread where getting values from processRAMProcLoadMonitoring
    static DWORD WINAPI StaticThreadStart_CPU(void* Param); //start watchCpuProcThread
    static DWORD WINAPI StaticThreadStart_RAM(void* Param); //start watchRamProcThread

    DWORD watchProcLoadThread(void);
    DWORD watchRamOverallLoadThread(void);
    static DWORD WINAPI StaticThreadStart_CPU_Overall(void* Param);    
    static DWORD WINAPI StaticThreadStart_RAM_Overall(void* Param);

    CountingCenter();
    ~CountingCenter();

private:
    enum whichFile{FOR_CPU_FILE = 0, FOR_RAM_FILE, BOTH_FILES};

    std::vector<QString> ignoreProcessesVector_CPU;
    std::vector<QString> ignoreProcessesVector_RAM;

    int newVal = 0; //debug

    short userAcceptableCpuLoad;
    int userAcceptableRamLoad;

    float userAcceptableCpuLoadOverall;
    int userAcceptableRamLoadOverall;

    bool cpuMonitoringActive;     //from ui->checkbox
    bool ramMonitoringActive;     //from ui->checkbox

    bool outputTypeIsLogMt;    //from ui->outputCombobox 0 - message, 1 - log
    bool outputTypeIsLogMtg;    //from ui->outputCombobox 0 - message, 1 - log

    bool monitoringRamType; //true - workingset, false - private set

    bool stopFromUiCpuProcess;
    bool stopFromUiRamProcess;

    bool stopFromUiCpuLoadOverall;
    bool stopFromUiRamLoadOverall;

    SIZE_T ramSaveArr[2];

    QFile *fileCpuMt;
    QFile *fileRamMt;
    QFile *fileCpuMtg;
    QFile *fileRamMtg;   

signals:
    void emitMessage(QString infoStringForMsg, bool fromCpu, bool mtGloval);
};

#endif // COUNTINGCENTER_H
