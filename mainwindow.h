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

#include "countingcenter.h"
#include "hardwareinformationcenter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    void initConnections();
    void startUpTimeThread();

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    bool typeHintButtonActivated = false;
    bool cpuHintButtonActivated = false;
    bool ramHintButtonActivated = false;

    long upTime_hours;
    long upTime_minutes;
    long upTime_seconds;
    long upTime_milliseconds;

private slots:

    void on_onScreenCpuLoadCheckBox_clicked();

    void on_onScreenCpuTempCheckBox_clicked();

    void on_onScreenRamLoadCheckBox_clicked();

    void on_onScreenGpuTempCheckBox_clicked();

    void on_onScreenGpuLoadCheckBox_clicked();

    void on_onScreenMotherboardTempCheckBox_clicked();

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


public slots:
    void getMessage(QString infoString, bool fromCpu, bool mtGloval);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
