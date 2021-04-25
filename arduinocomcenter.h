#ifndef ARDUINOCOMCENTER_H
#define ARDUINOCOMCENTER_H

#include <globalVariables.h>

class ArduinoComCenter
{
public:
    ArduinoComCenter();

    void getInfoForPackage();

private:
    char packageForTranser[PACKAGE_SIZE];
    int CPU_Load;
    int CPU_Temp;
    int GPU_Temp;
    int RAM_Load;
    int HDD1_Temp;
    int HDD2_Temp;
    int HDD3_Temp;
    int HDD4_Temp;
    //UPTIME

    void openComPort();
    void writeToComPort();

    void createPackege();
};

#endif // ARDUINOCOMCENTER_H
