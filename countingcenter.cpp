#include "countingcenter.h"

CountingCenter::CountingCenter()
    : userAcceptableCpuLoad{0}, userAcceptableRamLoad{0}, cpuMonitoringActive{true},
      ramMonitoringActive{true}, outputTypeIsLogMt{false},
      fileCpuMt{nullptr}, fileCpuMtg{nullptr}, fileRamMt{nullptr}, fileRamMtg{nullptr}
{

    ramSaveArr[0] = 0;
    ramSaveArr[1] = 0;
}

CountingCenter::~CountingCenter(){
    //delete files if exist
    if(fileCpuMt != nullptr){
        delete fileCpuMt;
    }
    else if(fileCpuMtg != nullptr){
        delete fileCpuMtg;
    }
    else if(fileRamMt != nullptr){
        delete fileRamMt;
    }
    else if(fileRamMtg != nullptr){
        delete fileRamMtg;
    }
}

void CountingCenter::setUserAcceptableCpuLoad(short load){
    this->userAcceptableCpuLoad = load;
}

void CountingCenter::setUserAcceptableRamLoad(int load){
    this->userAcceptableRamLoad = load;
}

void CountingCenter::setCpuMonitoringActive(bool isActive){
    this->cpuMonitoringActive = isActive;
}

void CountingCenter::setRamMonitoringActive(bool isActive){
    this->ramMonitoringActive = isActive;
}

void CountingCenter::setRamMonitoringType(bool type){
    monitoringRamType = type;
}

void CountingCenter::setOutputTypeMt(int type){
    outputTypeIsLogMt = type;
    qDebug() << outputTypeIsLogMt;
    system("pause");
}

void CountingCenter::setOutputTypeMtg(bool type){
    outputTypeIsLogMtg = type;
}

void CountingCenter::setStopFromUiCpuProcess(bool isStop){
    stopFromUiCpuProcess = isStop;
}

void CountingCenter::setStopFromUiRamProcess(bool isStop){
    stopFromUiRamProcess = isStop;
}

void CountingCenter::setStopFromUiCpuLoadOverall(bool isStop){
    stopFromUiCpuLoadOverall = isStop;
}

void CountingCenter::setStopFromUiRamLoadOverall(bool isStop){
    stopFromUiRamLoadOverall = isStop;
}

void CountingCenter::setUserAcceptableCpuLoadOverall(float load){
    this->userAcceptableCpuLoadOverall = load;
}

void CountingCenter::setUserAcceptableRamLoadOverall(int load){
    this->userAcceptableRamLoadOverall = load;
}

void CountingCenter::setIgnoreProcessesVectorElement(bool fromCpu, QString elem){
    if(fromCpu){
        ignoreProcessesVector_CPU.push_back(elem);
    }
    else{
        ignoreProcessesVector_RAM.push_back(elem);
        qDebug() << ignoreProcessesVector_RAM[ignoreProcessesVector_RAM.size() - 1];
    }
}

void CountingCenter::createFile(int whichFile, bool global, QString fileName){
    QString tempString;
    if(fileName == ""){
        auto nameTimer = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t(nameTimer);
        qDebug() << "end time " << ctime(&end_time);
       // system("pause");
        tempString = ctime(&end_time);

        for(int i = 0; i < tempString.length(); i++){
            if(tempString[i] == ':'){
                tempString[i] = '.';
            }
            else if(tempString[i] == '\n'){
                tempString.remove('\n');
            }
        }

        fileName = tempString;
    }    

    if(whichFile == FOR_CPU_FILE){
        fileName.append("_CPU");


        if(!global){
            fileName.append(".txt");
            fileCpuMt = new QFile(fileName);
        }
        else{
            fileName.append("_global");
            fileName.append(".txt");
            fileCpuMtg = new QFile(fileName);
        }

    }
    else if(whichFile == FOR_RAM_FILE){
        fileName.append("_RAM");
        qDebug() << "FOR_RAM_FILE";
        system("pause");

        if(!global){
            fileName.append(".txt");
            fileRamMt = new QFile(fileName);
            qDebug() << "created";
            system("pause");
        }
        else{
            fileName.append("_global");
            fileName.append(".txt");
            fileRamMtg = new QFile(fileName);
        }
    }
    else if(whichFile == BOTH_FILES){
        QString fileNameBuff = fileName;

        fileName.append("_CPU");
        fileNameBuff.append("_RAM");


        if(!global){
            fileName.append(".txt");
            fileNameBuff.append(".txt");
            fileCpuMt = new QFile(fileName);
            fileRamMt = new QFile(fileNameBuff);
        }
        else{
            fileName.append("_global");
            fileNameBuff.append("_global");
            fileName.append(".txt");
            fileNameBuff.append(".txt");
            fileCpuMtg = new QFile(fileName);
            fileRamMtg = new QFile(fileNameBuff);
        }
    }

}

//**GET LIST OF PROCESSES**
int CountingCenter::getProcesses(bool debFromCpu){
    smartHandle processSnap(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
    smartHandle threadSnap(CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0));

    if (!processSnap || !threadSnap) {
        return 1;
    }

    std::vector<THREADENTRY32> threads;

    THREADENTRY32 te{ sizeof(THREADENTRY32) };
    if (Thread32First(threadSnap, &te) == FALSE) {
        return 2;
    }   

    //getting threads
    do {
        threads.push_back(te);
    } while (Thread32Next(threadSnap, &te));

    PROCESSENTRY32 pe{ sizeof(PROCESSENTRY32) };  
    if (Process32First(processSnap, &pe) == FALSE) {
        return 3;
    }
    do {
        std::vector<THREADENTRY32> subThreads;
        for (const auto &thread : threads) {           
            if (thread.th32OwnerProcessID == pe.th32ProcessID) {
                subThreads.push_back(thread);              
            }
        }
        if(debFromCpu){
            processInfosCPU.push_back(processInfo{ pe, subThreads });
        }
        else{
             processInfosRAM.push_back(processInfo{ pe, subThreads });
        }      
    } while (Process32Next(processSnap, &pe));

    return 0;
}

//**CPU MONITORING**
bool CountingCenter::processCPUProcLoadMonitoring(DWORD processID, const WCHAR* exeFile){
    qDebug() << "processCPUProcLoadMonitoring";
    int currentLoad = 0;

    QString infoString;

    auto timer = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(timer);

    if(!stopFromUiCpuProcess){  //if user didnt stop the process
         currentLoad = usage.getUsage(processID);

        if (currentLoad > userAcceptableCpuLoad) {
            /*forming text for message*/
            infoString.append(ctime(&end_time));
            infoString.append( "PROCESS: ");
            QString procNameBuff;
            infoString.append(procNameBuff.fromWCharArray(exeFile));
            infoString.append(" LOAD: ");
            QString currentLoadString = QString::number(currentLoad);
            infoString.append(currentLoadString);

            if(outputTypeIsLogMt == 0){
                emit emitMessage(infoString, true, false);      //emits signa to mainwindow, where message box being formed
                return false;
            }
            else{
                qDebug() << "log";
                if (fileCpuMt->open(QFile::Append |QFile::Text))
                {
                    qDebug() << "i";
                    fileCpuMt->write(infoString.toUtf8() + "\n");
                    fileCpuMt->close();
                }
            }

        }
        return true;
    }
    else{       //if user stoped the process
        QString stoppingMsg = "Monitoring was stopped by a user at ";
        stoppingMsg.append(ctime(&end_time));
        emit emitMessage(stoppingMsg, true, false);
        if(outputTypeIsLogMt == 1){
            if (fileCpuMt->open(QFile::Append |QFile::Text))
            {
                fileCpuMt->write(stoppingMsg.toUtf8() + "\n");
                fileCpuMt->close();
            }
        }
        return false;
    }
}

void CountingCenter::monitoringCpuStart(){    
    CreateThread(NULL, 0, StaticThreadStart_CPU, (void*) this, 0, NULL);
}

void CountingCenter::monitoringRamStart(){
    qDebug() << "monitoringRamStart";
    CreateThread(NULL, 0, StaticThreadStart_RAM, (void*) this, 0, NULL);
}

//**RAM MONITORING**
bool CountingCenter::processRAMProcLoadMonitoring(DWORD processID, const WCHAR* exeFile){
    QString infoString;

    auto timer = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(timer);

    if(!stopFromUiRamProcess){
        HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
        if (h == NULL) {
            std::cout << "i dont know why" << std::endl;
            std::cout << "error = " << GetLastError() << std::endl;
        }
        else{
            //system("pause");

            if (GetProcessMemoryInfo(h, (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc))) {
               ramSaveArr[0] = pmc.WorkingSetSize/1024; //kb
               ramSaveArr[1] = pmc.PrivateUsage/1024;   //kb
            }
            else{
                qDebug() << GetLastError();
            }
            if(monitoringRamType){  //type = working set
                if(ramSaveArr[0] >= userAcceptableRamLoad){
                    infoString.append(ctime(&end_time));
                    infoString.append( "PROCESS: ");
                    QString procNameBuff;
                    infoString.append(procNameBuff.fromWCharArray(exeFile));
                    infoString.append(" CURRENT WORKING SET: ");
                    QString currentLoadString = QString::number(ramSaveArr[0]);
                    infoString.append(currentLoadString);

                    if(outputTypeIsLogMt == 0){
                         emit emitMessage(infoString, false, false);
                         return false;
                    }
                    else{
                        qDebug() << "log";
                        if (fileRamMt->open(QFile::Append |QFile::Text))
                        {
                            qDebug() << "i";
                            fileRamMt->write(infoString.toUtf8() + "\n");
                            fileRamMt->close();
                        }
                    }
                    CloseHandle(h);                   
                }
            }
            else{   //type = private set
                if(ramSaveArr[1] >= userAcceptableRamLoad){
                    infoString.append(ctime(&end_time));
                    infoString.append( "PROCESS: ");
                    QString procNameBuff;
                    infoString.append(procNameBuff.fromWCharArray(exeFile));
                    infoString.append(" CURRENT PRIVATE SET: ");
                    QString currentLoadString = QString::number(ramSaveArr[1]);
                    infoString.append(currentLoadString);

                    if(outputTypeIsLogMt == 0){
                        emit emitMessage(infoString, false, false);
                        return false;
                    }
                    else{
                        qDebug() << "log";
                        if (fileRamMt->open(QFile::Append |QFile::Text))
                        {
                            qDebug() << "i";
                            fileRamMt->write(infoString.toUtf8() + "\n");
                            fileRamMt->close();
                        }
                    }
                    CloseHandle(h);
                }
            }
        }
        CloseHandle(h);
        return true;
    }
    else{
        QString stoppingMsg = "Monitoring was stopped by a user at ";
        stoppingMsg.append(ctime(&end_time));

        emit emitMessage(stoppingMsg, false, false);
        if(outputTypeIsLogMt == 1){
            if (fileRamMt->open(QFile::Append |QFile::Text))
            {
                fileRamMt->write(stoppingMsg.toUtf8() + "\n");
                fileRamMt->close();
            }
        }
        return false;
    }
}

DWORD WINAPI CountingCenter::StaticThreadStart_CPU(void* Param){
        CountingCenter* This = (CountingCenter*) Param;
        return This->watchCpuProcThread();
    }

DWORD WINAPI CountingCenter::StaticThreadStart_RAM(void* Param)
    {
        CountingCenter* This = (CountingCenter*) Param;
        return This->watchRamProcThread();
    }

 DWORD CountingCenter::watchCpuProcThread(void) {   
    bool isThreadRunning = true;
    DWORD crutch;
    while (isThreadRunning) {
        this->getProcesses(true);

        for (const auto &processInfo : this->processInfosCPU) {
            qDebug() << "procInfo loop";
            if(ignoreProcessesVector_CPU.size() > 0){
                for(int i = 0; i < ignoreProcessesVector_CPU.size(); i++){
                    if(QString::fromWCharArray(processInfo.pe.szExeFile) != ignoreProcessesVector_CPU[i]){
                        for (int i = 0; i < 3; i++) {
                            isThreadRunning = this->processCPUProcLoadMonitoring(processInfo.pe.th32ProcessID, processInfo.pe.szExeFile);
                            Sleep(1);
                            if(!isThreadRunning){break;}
                        }
                        if(!isThreadRunning){break;}
                        this->usage.setLastRunNull();
                    }
                }
            }
            else{
                for (int i = 0; i < 3; i++) {
                    isThreadRunning = this->processCPUProcLoadMonitoring(processInfo.pe.th32ProcessID, processInfo.pe.szExeFile);
                    Sleep(1);
                    if(!isThreadRunning){break;}
                }
                if(!isThreadRunning){break;}
                this->usage.setLastRunNull();
            }
        }
    }
    return crutch;
}

DWORD CountingCenter::watchRamProcThread(void) {
    bool isThreadRunning = true;

   DWORD crutch;

   this->getProcesses(false);
   while(isThreadRunning){
       for (const auto &processInfo : this->processInfosRAM) {
           if(ignoreProcessesVector_RAM.size() != 0){
               for(int i = 0; i < ignoreProcessesVector_RAM.size(); i++){
                   if(QString::fromWCharArray(processInfo.pe.szExeFile) != ignoreProcessesVector_RAM[i]){
                       isThreadRunning = processRAMProcLoadMonitoring(processInfo.pe.th32ProcessID, processInfo.pe.szExeFile);
                       Sleep(1);

                      if(!isThreadRunning){break;}
                   }
               }
               if(!isThreadRunning){break;}
           }
           else{
               isThreadRunning = processRAMProcLoadMonitoring(processInfo.pe.th32ProcessID, processInfo.pe.szExeFile);
               Sleep(1);

              if(!isThreadRunning){break;}
           }
       }
   }

   return crutch;
}

void CountingCenter::monitoringCpuOverallStart(){
    CreateThread(NULL, 0, StaticThreadStart_CPU_Overall, (void*) this, 0, NULL);
}

void CountingCenter::monitoringRamOverallStart(){
    CreateThread(NULL, 0, StaticThreadStart_RAM_Overall, (void*) this, 0, NULL);
}

float CountingCenter::calculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks){
    static unsigned long long previousTotalTicks = 0;
    static unsigned long long previousIdleTicks = 0;

    unsigned long long totalTicksSinceLastTime = totalTicks - previousTotalTicks;
    unsigned long long idleTicksSinceLastTime = idleTicks - previousIdleTicks;

    float ret = 1.0f - ((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime) / totalTicksSinceLastTime : 0);

    previousTotalTicks = totalTicks;
    previousIdleTicks = idleTicks;
    return ret;
}

unsigned long long CountingCenter::fileTimeToInt64(const FILETIME & ft){
    return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime);
}

float CountingCenter::getCPULoad()
{
    FILETIME idleTime, kernelTime, userTime;
    return GetSystemTimes(&idleTime, &kernelTime, &userTime) ? calculateCPULoad(fileTimeToInt64(idleTime), fileTimeToInt64(kernelTime) + fileTimeToInt64(userTime)) : -1.0f;
}

float CountingCenter::getRAMLoad()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);

    DWORDLONG totalPhysMemory = memInfo.ullTotalPhys;
    DWORDLONG usedPhysMemory = totalPhysMemory - memInfo.ullAvailPhys;

    for(int i = 1; i < 10000; i++)
    {
        i/(float)100;
    }

    return ((float)usedPhysMemory/(float)totalPhysMemory) * 100;
}

bool CountingCenter::overallCpuLoadMonitoring(){
    float CPULoad = 0, cpuLoadFromUser = 0;

    cpuLoadFromUser = userAcceptableCpuLoadOverall / 100;

    auto timer = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(timer);

    if(!stopFromUiCpuLoadOverall){
        CPULoad = getCPULoad();

        if(CPULoad >= cpuLoadFromUser){
            qDebug() << "CPULoad = " << CPULoad;
            QString infoString;

            infoString.append(ctime(&end_time));

            infoString.append(" USAGE: ");
            QString currentLoadString = QString::number((int)(CPULoad * 100));
            infoString.append(currentLoadString);

            qDebug() << "11111";
            emit emitMessage(infoString, true, true);
            qDebug() << "22222";
            return false;
        }
        else{
            qDebug() << "true";
            return true;
        }
    }
    else{
        QString stoppingMsg = "Monitoring was stopped by a user at ";
        stoppingMsg.append(ctime(&end_time));
        emit emitMessage(stoppingMsg, true, true);
        return false;
    }
}

bool CountingCenter::overallRamLoadMonitoring(){
    float RAMLoad = 0, ramLoadFromUser = 0;

    ramLoadFromUser = userAcceptableRamLoadOverall;

    auto timer = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(timer);

    if(!stopFromUiRamLoadOverall){
        RAMLoad = getRAMLoad();
        if(RAMLoad >= ramLoadFromUser){
            QString infoString;

            infoString.append(ctime(&end_time));

            infoString.append(" USAGE: ");
            QString currentLoadString = QString::number((int)(RAMLoad));
            infoString.append(currentLoadString);

            if(outputTypeIsLogMtg == 0){
                emit emitMessage(infoString, false, true);
                return false;
            }
            else{
                qDebug() << "log";
                if (fileRamMtg->open(QFile::Append |QFile::Text))
                {
                    qDebug() << "i";
                    fileRamMtg->write(infoString.toUtf8() + "\n");
                    fileRamMtg->close();
                    return true;
                }
            }
        }
        else{
            qDebug() << "true";
            return true;
        }
    }
    else{
        QString stoppingMsg = "Monitoring was stopped by a user at ";
        stoppingMsg.append(ctime(&end_time));
        emit emitMessage(stoppingMsg, false, true);

        if (fileRamMtg->open(QFile::Append |QFile::Text))
        {
            fileRamMtg->write(stoppingMsg.toUtf8() + "\n");
            fileRamMtg->close();
        }
        return false;
     }
}

DWORD WINAPI CountingCenter::StaticThreadStart_CPU_Overall(void* Param){
        CountingCenter* This = (CountingCenter*) Param;
        return This->watchProcLoadThread();
}

DWORD WINAPI CountingCenter::StaticThreadStart_RAM_Overall(void* Param){
        CountingCenter* This = (CountingCenter*) Param;
        return This->watchRamOverallLoadThread();
}

DWORD CountingCenter::watchProcLoadThread(void) {
    bool isThreadRunning = true;
    DWORD crutch;

    while (isThreadRunning) {
        isThreadRunning = overallCpuLoadMonitoring();
        qDebug() << "isThreadRunning" << isThreadRunning;
        Sleep(500);
    }
    return crutch;    

}

DWORD CountingCenter::watchRamOverallLoadThread(void) {
    bool isThreadRunning = true;
    DWORD crutch;

    while (isThreadRunning) {
         isThreadRunning = overallRamLoadMonitoring();
         qDebug() << "ram isThreadRunning" << isThreadRunning;
         Sleep(500);
     }
     return crutch;
}
