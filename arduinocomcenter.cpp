#include "arduinocomcenter.h"

ArduinoComCenter::ArduinoComCenter() : send{false}, comName{L"COM3"}
{

}

bool ArduinoComCenter::getInfoForPackage(int cpuLoad, int cpuTemp, int gpuTemp, int ramLoad, int hdd1Temp, int hdd2Temp, int hdd3Temp, int hdd4Temp){
    qDebug() << "info for package";
    if(cpuLoad > 100 || cpuTemp <= 0){
        return 0;
    }
    else{
        this->CPU_Load = cpuLoad;
        this->CPU_Temp = cpuTemp;
        this->GPU_Temp = gpuTemp;
        this->RAM_Load = ramLoad;
        this->HDD1_Temp = hdd1Temp;
        this->HDD2_Temp = hdd2Temp;
        this->HDD3_Temp = hdd3Temp;
        this->HDD4_Temp = hdd4Temp;
        qDebug() << CPU_Load << " " << CPU_Temp << " " << GPU_Temp <<  " " << RAM_Load
                 << " " << HDD1_Temp << " " << HDD2_Temp << " " << HDD3_Temp << " " << HDD4_Temp;
        send = true;
    }
}

bool ArduinoComCenter::openComPort(){
    hComPort = CreateFile(comName, GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
       if (hComPort == INVALID_HANDLE_VALUE) {
           comState = false;
           return comState;
       }

    DCB serialParameters = { 0 };
    serialParameters.DCBlength = sizeof(serialParameters);

    if (!GetCommState(hComPort, &serialParameters)) {
        comState = false;
        return comState;
    }

    serialParameters.BaudRate = CBR_9600;
    serialParameters.ByteSize = 8;
    serialParameters.Parity = NOPARITY;
    serialParameters.StopBits = ONESTOPBIT;

    if (!SetCommState(hComPort, &serialParameters)) {
        comState = false;
        return comState;
    }

    comState = true;
    return comState;
}

void ArduinoComCenter::writeToComPort(){
    DWORD bytesWritten;
    bool isWritten = WriteFile(hComPort, packageForTransfer, PACKAGE_SIZE, &bytesWritten, NULL);


    memset(packageForTransfer, 0, PACKAGE_SIZE);
}

void ArduinoComCenter::closeCom(){
    CloseHandle(hComPort);
}

void ArduinoComCenter::createPackege(){
    packageForTransfer[CPU_LOAD] = CPU_Load;
    packageForTransfer[CPU_TEMP] = CPU_Temp;
    packageForTransfer[GPU_TEMP] = GPU_Temp;
    packageForTransfer[RAM_LOAD] = RAM_Load;
    packageForTransfer[HDD1_TEMP] = HDD1_Temp;
    packageForTransfer[HDD2_TEMP] = HDD2_Temp;
    packageForTransfer[HDD3_TEMP] = HDD3_Temp;
    packageForTransfer[HDD4_TEMP] = HDD4_Temp;
}

void ArduinoComCenter::startCommunication(){
    openComPort();
    qDebug() << "open, comstate = " << comState;
    createPackege();
    qDebug() << "created";
    writeToComPort();
    qDebug() << "written";
    closeCom();
    qDebug() << "close";
}
