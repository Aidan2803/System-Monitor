#include "arduinocomcenter.h"

ArduinoComCenter::ArduinoComCenter()
{

}

void ArduinoComCenter::getInfoForPackage(int cpuLoad, int cpuTemp, int gpuTemp, int ramLoad, int hdd1Temp, int hdd2Temp, int hdd3Temp, int hdd4Temp){
    this->CPU_Load = cpuLoad;
    this->CPU_Temp = cpuTemp;
    this->GPU_Temp = gpuTemp;
    this->RAM_Load = ramLoad;
    this->HDD1_Temp = hdd1Temp;
    this->HDD2_Temp = hdd2Temp;
    this->HDD3_Temp = hdd3Temp;
    this->HDD4_Temp = hdd4Temp;
}

void ArduinoComCenter::openComPort(){}

void ArduinoComCenter::writeToComPort(){}

void ArduinoComCenter::createPackege(){}
