#include "hardwareinformationcenter.h"

HardWareInformationCenter::HardWareInformationCenter() : isGetUpTimeLoopRunning{true}, pSvc{nullptr}, pLoc{nullptr}
{}

void HardWareInformationCenter::initCOM(){
    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
        if (FAILED(hres))
        {
            cout << "Failed to initialize COM library. Error code = 0x" << hex << hres << endl; // Program has failed.
        }

        hres = CoInitializeSecurity(
                NULL,
                -1,                          // COM authentication
                NULL,                        // Authentication services
                NULL,                        // Reserved
                RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
                RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
                NULL,                        // Authentication info
                EOAC_NONE,                   // Additional capabilities
                NULL                         // Reserved
            );


        if (FAILED(hres))
        {
            cout << "Failed to initialize security. Error code = 0x" << hex << hres << endl;
            CoUninitialize();      // Program has failed.
        }



        hres = CoCreateInstance(
                CLSID_WbemLocator,
                0,
                CLSCTX_INPROC_SERVER,
                IID_IWbemLocator, (LPVOID *)&pLoc);

        if (FAILED(hres))
        {
             cout << "Failed to create IWbemLocator object." << " Err code = 0x" << hex << hres << endl;
                    CoUninitialize();   // Program has failed.
        }

                // Step 4: -----------------------------------------------------
                // Connect to WMI through the IWbemLocator::ConnectServer method


                // Connect to the root\cimv2 namespace with
                // the current user and obtain pointer pSvc
                // to make IWbemServices calls.
         hres = pLoc->ConnectServer(
             _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
             NULL,                    // User name. NULL = current user
             NULL,                    // User password. NULL = current
             0,                       // Locale. NULL indicates current
             NULL,                    // Security flags.
             0,                       // Authority (e.g. Kerberos)
             0,                       // Context object
             &pSvc                    // pointer to IWbemServices proxy
             );

         if (FAILED(hres))
          {
               cout << "Could not connect. Error code = 0x" << hex << hres << endl;
               pLoc->Release();
               CoUninitialize(); // Program has failed.
          }

          cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;

                // Step 5: --------------------------------------------------
                // Set security levels on the proxy -------------------------

         hres = CoSetProxyBlanket(
                    pSvc,                        // Indicates the proxy to set
                    RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
                    RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
                    NULL,                        // Server principal name
                    RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
                    RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
                    NULL,                        // client identity
                    EOAC_NONE                    // proxy capabilities
          );

         if (FAILED(hres))
         {
             cout << "Could not set proxy blanket. Error code = 0x" << hex << hres << endl;
             pSvc->Release();
             pLoc->Release();
             CoUninitialize();             // Program has failed.
         }
}



void HardWareInformationCenter::cleanUpCOM(){
    // Cleanup
    // ========

    pSvc->Release();
    pLoc->Release();
    CoUninitialize();
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
    QString cpuInfo;

    for (int i = 0; ; i++)
        {
            DISPLAY_DEVICE dd = { sizeof(dd), 0 };
            BOOL f = EnumDisplayDevices(NULL, i, &dd, EDD_GET_DEVICE_INTERFACE_NAME);
            if (!f)
                break;

            if(i == 0){
                 cpuInfo = QString::fromWCharArray(dd.DeviceString);
            }
         }

    return cpuInfo;
}

int HardWareInformationCenter::WMI_getRAMInfo(QString *pArrToWrite, int *amountOfBars){
    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_PhysicalMemory"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);

    if (FAILED(hres))
    {
        cout << "Query for operating system name failed." << " Error code = 0x" << hex << hres << endl;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();     // Program has failed.
    }

    IWbemClassObject *pclsObj = nullptr;
    ULONG uReturn = 0;

    short arrayIterator{0};
    while (pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
                &pclsObj, &uReturn);

        if (0 == uReturn)
        {
            break;
        }

        VARIANT vtProp;

        //-----------------------------------RAM MANUFACTURER-----------------------------------------
        hr = pclsObj->Get(L"manufacturer", 0, &vtProp, 0, 0);
        pArrToWrite[arrayIterator] = QString::fromWCharArray(vtProp.bstrVal);

        VariantClear(&vtProp);

        //-----------------------------------RAM MEMORY TYPE-----------------------------------------
        hr = pclsObj->Get(L"memorytype", 0, &vtProp, 0, 0);
        pArrToWrite[arrayIterator] += " ";

        switch (vtProp.intVal) {
            case 0:{
                pArrToWrite[arrayIterator] += "DDR4(?) UNKNOWN TYPE";
                break;
            }
            case 20:{
                pArrToWrite[arrayIterator] += "DDR";
                break;
            }
            case 21:{
                pArrToWrite[arrayIterator] += "DDR2";
                break;
            }
            case 24:{
                pArrToWrite[arrayIterator] += "DDR3";
                break;
            }
            default:{
                pArrToWrite[arrayIterator] += "UNKNOWN TYPE";
                break;
            }
        }

        qDebug() << "pArrToWrite[arrayIterator]" << pArrToWrite[arrayIterator] << "index = " << arrayIterator;
        qDebug() << " manufacturer memtype: " << pArrToWrite[arrayIterator];

        VariantClear(&vtProp);

        //-----------------------------------RAM FORMFACTOR-----------------------------------------

        hr = pclsObj->Get(L"formfactor", 0, &vtProp, 0, 0);
        pArrToWrite[arrayIterator] += " ";

        switch (vtProp.intVal) {
            case 8:{
                pArrToWrite[arrayIterator] += "DIMM";
                break;
            }
            case 12:{
                pArrToWrite[arrayIterator] += "SODIMM ";
                break;
            }
            default:{
                pArrToWrite[arrayIterator] += "UNKNOWN FORMFACTOR";
                break;
            }
        }

        VariantClear(&vtProp);

        //-----------------------------------RAM CAPACITY-----------------------------------------

        pArrToWrite[arrayIterator] += " ";

        QString bufferForCapacity;
        long long capacity;
        hr = pclsObj->Get(L"capacity", 0, &vtProp, 0, 0);
        bufferForCapacity = QString::fromWCharArray(vtProp.bstrVal);
        capacity = bufferForCapacity.toLongLong();

        capacity /= 1024;   //kb
        capacity /= 1024;   //mb
        capacity /= 1024;   //gb

        pArrToWrite[arrayIterator] += QString::number(capacity);
        pArrToWrite[arrayIterator] += " GB";

        qDebug() << "pArrToWrite[arrayIterator]" << pArrToWrite[arrayIterator] << "index = " << arrayIterator;

        VariantClear(&vtProp);

        //-----------------------------------RAM SPEED-----------------------------------------

        pArrToWrite[arrayIterator] += " ";

        hr = pclsObj->Get(L"speed", 0, &vtProp, 0, 0);
        pArrToWrite[arrayIterator] += QString::number(vtProp.intVal);

        qDebug() << "pArrToWrite[arrayIterator]" << pArrToWrite[arrayIterator] << "index = " << arrayIterator;

        VariantClear(&vtProp);

        ++arrayIterator;
    }

    *amountOfBars = arrayIterator;

    qDebug() << "amountOfBars = " << *amountOfBars;

    return arrayIterator;

    pEnumerator->Release();
    pclsObj->Release();

}

QString* HardWareInformationCenter::getRAMInfo(int *amountOfBars) {
    initCOM();
    WMI_getRAMInfo(ramInfo, amountOfBars);

    return ramInfo;
}
