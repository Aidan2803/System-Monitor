#include "cpuusage.h"
#include <windows.h>
#include <strsafe.h>

#include "QDebug"

using namespace std;

cpuUsage::cpuUsage() : nCpuUsage(-1), dwLastRun(0), runCount(0) {

    //nullify system time
    ZeroMemory(&fileTimePrevKernelSyst, sizeof(FILETIME));
    ZeroMemory(&fileTimePrevUserSyst, sizeof(FILETIME));

    //nullify process time
    ZeroMemory(&fileTimePrevKernelProc, sizeof(FILETIME));
    ZeroMemory(&fileTimePrevUserProc, sizeof(FILETIME));

}

short cpuUsage::getUsage(DWORD processID) {
    //cout << "deb" << endl;
    if (processID != 0) {
        short nCpuCopy = nCpuUsage;
        HANDLE h = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);

        if (h == NULL) {
           qDebug() << "i dont know why";
           qDebug() << GetLastError();
           //system("pause");
        }

        //	std::cout << &runCount << std::endl;
        if(h != NULL){
            if (::InterlockedIncrement(&runCount) == 1) {
                //if called to often return copy of prev result, so that programm itself wont affect the result

                if (!isEnoughTimePassed()) {
                    InterlockedDecrement(&runCount);
                    //	std::cout << "it`s not enough" << std::endl;
                    return nCpuCopy;
                }

                FILETIME fileTimeIdleSyst, fileTimeKernelSyst, fileTimeUserSyst;
                FILETIME fileTimeCreationProc, fileTimeExitProc, fileTimeKernelProc, fileTimeUserProc;

                //if couldnt get system or process time return copy of prev result
                if (!GetSystemTimes(&fileTimeIdleSyst, &fileTimeKernelSyst, &fileTimeUserSyst) ||
                    !GetProcessTimes(h, &fileTimeCreationProc, &fileTimeExitProc, &fileTimeKernelProc, &fileTimeUserProc)) {
                    return nCpuCopy;
                }

                if (!isFirstRun()) {
                    ULONGLONG fileTimeKernelDiffSyst = SubTime(fileTimeKernelSyst, fileTimePrevKernelSyst);	//	KERNEL SYSTEM DIF
                    ULONGLONG fileTimeUserDiffSyst = SubTime(fileTimeUserSyst, fileTimePrevUserSyst);	//USER SYST DIF

                    ULONGLONG fileTimeKernelDiffProc = SubTime(fileTimeKernelProc, fileTimePrevKernelProc);	//	KERNEL PROC DIF
                    ULONGLONG fileTimeUserDiffProc = SubTime(fileTimeUserProc, fileTimePrevUserProc);	//USER PROC DIF

                    ULONGLONG totalSyst = fileTimeKernelDiffSyst + fileTimeUserDiffSyst;
                    ULONGLONG totalProc = fileTimeKernelDiffProc + fileTimeUserDiffProc;

                    if (totalProc > 0) {
                        nCpuUsage = (short)((100.0 * totalProc) / totalSyst);		//count cpu usage for process according to formula
                    }
                }

                fileTimePrevKernelSyst = fileTimeKernelSyst;
                fileTimePrevUserSyst = fileTimeUserSyst;

                fileTimePrevKernelProc = fileTimeKernelProc;
                fileTimePrevUserProc = fileTimeUserProc;

                dwLastRun = GetTickCount64();

                nCpuCopy = nCpuUsage;
            }

            ::InterlockedDecrement(&runCount);

        }

        return nCpuCopy;
        CloseHandle(h);
    }
}

ULONGLONG cpuUsage::SubTime(const FILETIME& fileTimeA, const FILETIME& fileTimeB) {
    LARGE_INTEGER a, b;

    a.LowPart = fileTimeA.dwLowDateTime;
    a.HighPart = fileTimeA.dwHighDateTime;

    b.LowPart = fileTimeB.dwLowDateTime;
    b.HighPart = fileTimeB.dwHighDateTime;

    return a.QuadPart - b.QuadPart;
}

bool cpuUsage::isEnoughTimePassed() {
    const int minPassedMS = 0; //ms

    ULONGLONG dwCurrentTickCount = GetTickCount64();

    return (dwCurrentTickCount - dwLastRun) > minPassedMS;
}

void cpuUsage::setLastRunNull() {
    dwLastRun = 0;
    nCpuUsage = -1;
    runCount = 0;
}
