#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

#include <QObject>
#include <QLabel>
#include <QIcon>
#include <QFileDialog>

#include <iostream>
#include <windows.h>
#include <thread>
#include <deque>

#include "countingcenter.h"
#include "hardwareinformationcenter.h"
#include "arduinocomcenter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    void initConnections();
    void initMWConneciotns();

    void startUpTimeThread();
    void startPrintUpTimeThread();

    void printUpTime();

    void startGetCPULoadThread();
    void startGetRAMLoadThread();
    void startGetTemperaturesThread();

    void setHddTemps(QString *hddTemps);

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QLineSeries *series;
    QChart *chart;
    QChartView *chartView;

    QTimer *timer;
    QTimer *timerForArduino;

    std::deque<int> cpuTemperaturesDeque;
    std::deque<int> gpuTemperaturesDeque;
    std::vector<deque<int>> hddTemperaturesDequesVect;

    void setIsPrintUpTimeRunning(bool isRunning){ this->isPrintUpTimeRunning = isRunning;}
    void setIsRunningGetCPULoad(bool isRunning){this->isRunningGetCPULoad = isRunning;}
    void setIsRunningGetRAMLoad(bool isRunning){this->isRunningGetRAMLoad = isRunning;}
    void setIsRunningGetTemperature(bool isRunning){this->isRunningGetTemperature = isRunning;}

    bool getTemperaturesThreadFirstRun = true;

    bool typeHintButtonActivated = false;
    bool cpuHintButtonActivated = false;
    bool ramHintButtonActivated = false;

    bool isPrintUpTimeRunning = true;
    bool isRunningGetCPULoad = true;
    bool isRunningGetRAMLoad = true;
    bool isRunningGetTemperature = true;

    int temperatureProcessPID;

    long upTime_hours;
    long upTime_minutes;
    long upTime_seconds;
    long upTime_milliseconds;

    enum HARDWAREINDEXES{
      CPU,
      GPU,
      HDD1,
      HDD2,
      HDD3,
      HDD4
     };

    short activeIndexHdd;

    short indexOfHardware;
    short amountOfHDDs;

    string cpuTempReserv;
    string gpuTempReserv;
    string hddTempReserv[MAX_AMOUNT_OF_TEMPERATURE_PARAMETERS - INDEX_OF_FIRST_HDD];

    int cpuLoad;
    int ramLoad;

    QString *ramInfo;
    QString *storageInfo;
    QString *audioInfo;
    QString *networkControllerInfo;

    QString hddTempsFromFile[MAX_AMOUNT_OF_TEMPERATURE_PARAMETERS - INDEX_OF_FIRST_HDD];

    void initWindow();

    void accgiveData_update();

    void chartDesigner();

    Ui::MainWindow *ui;

private slots:

    void on_onScreenCpuLoadCheckBox_clicked();

    void on_onScreenCpuTempCheckBox_clicked();

    void on_onScreenRamLoadCheckBox_clicked();

    void on_onScreenGpuTempCheckBox_clicked();

    void on_onScreenStorageTempCheckBox_clicked();

    void on_onScreenUpTimeCheckBox_clicked();

    void on_monitorCpuCheckBox_clicked();

    void on_monitorRamCheckBox_clicked();

    void on_applyButtonMt_clicked();

    void on_hintButton_clicked();

    void on_resetCPU_clicked();

    void on_resetRAM_clicked();

    void on_outputTypeComboBox_currentIndexChanged(int index);

    void on_stopCPU_clicked();

    void on_stopRAM_clicked();

    void on_applyButtonMtg_clicked();

    void on_monitorCpuCheckBox_mtg_clicked();

    void on_monitorRamCheckBox_mtg_clicked();

    void on_resetCPU_Mtg_clicked();

    void on_stopCPU_Mtg_clicked();

    void on_stopRAM_Mtg_clicked();

    void on_resetRAM_Mtg_clicked();

    void on_cpuBrowseButton_clicked();    

    void on_acceptableCpuProcessesList_itemClicked(QListWidgetItem *item);

    void on_cpuDeleteButton_clicked();

    void on_ramBrowseButton_clicked();

    void on_acceptableRamProcessesList_itemClicked(QListWidgetItem *item);

    void on_ramDeleteButton_clicked();

    void on_hintButton_accProcCpu_clicked();

    void on_hintButton_accProcRam_clicked();

    void on_getProcessesButton_clicked();

    void on_getDriversButton_clicked();

    void on_storageInfoComboBox_currentIndexChanged(int index);

    void on_chartButton_cpu_clicked();

    void on_chartButton_gpu_clicked();

    void on_chartButton_hdd_clicked();

public slots:
    void getMessage(QString infoString, bool fromCpu, bool mtGloval);
    void getCPULoadValue(int cpuLoadValue);
    void getRAMLoadValue(int ramLoadValue);
    void changeHDDTemperatureLabel(int index);

signals:
    void emitCPULoadValue(int cpuLoadValue);
    void emitRAMLoadValue(int ramLoadValue);
    void emitChangeHDDTemperature(int index);

};
#endif // MAINWINDOW_H
