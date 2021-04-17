#include "mainwindow.h"
#include "ui_mainwindow.h"

static CountingCenter cc;
static HardWareInformationCenter hc;

static std::thread *upTimeThread;
static std::thread *printUpTimeThread;

static std::thread *getCPULoadThread;
static std::thread *getRAMLoadThread;

static std::thread *getTemperaturesThread;

/*GLOBAL VARS*/
static int memoryCpuAccaptableLoad = 50;
static int memoryRamAccaptableLoad = 10000;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){

    ui->setupUi(this);

    initWindow();
}

MainWindow::~MainWindow()
{
    hc.setIsGetUpTimeLoopRunning(false);
    upTimeThread->join();
    delete upTimeThread;

    this->setIsPrintUpTimeRunning(false);
    printUpTimeThread->join();
    delete printUpTimeThread;

    this->setIsRunningGetCPULoad(false);
    getCPULoadThread->join();
    delete getCPULoadThread;

    this->setIsRunningGetRAMLoad(false);
    getRAMLoadThread->join();
    delete getRAMLoadThread;

    delete ui;
}

void MainWindow::initWindow(){
    //*********************BEGING:OVERVIEW**********************//

    //--------BEGING:CHART SETTINGS-----------
    QLineSeries *series = new QLineSeries();

    *series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2) << QPointF(29, 32) << QPointF(29, 36);//functionality of the chart

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);

    // Customize series
    QPen pen(QColor(39, 99, 13));
    pen.setWidth(5);
    series->setPen(pen);

       // Customize chart title
    QFont font;
    font.setPixelSize(18);
    chart->setTitleFont(font);
    chart->setTitleBrush(QBrush(Qt::white));
    chart->setTitle("label load");

    chart->setBackgroundBrush(QColor(49,49,49));
    chart->setPlotAreaBackgroundBrush(QColor(49,49,49));
    chart->setPlotAreaBackgroundVisible(true);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setParent(ui->overview);
    chartView->setGeometry(50, 285, 650, 300);

    chartView->setStyleSheet("background-color: rgb(49, 49, 49);");

    //--------END:CHART SETTINGS-----------

    //--------BEGIN:LIST WIDGET SETTINGS-----------
    ui->processList->addItem("Use button below to get processes list");

    ui->driversList->addItem("Use button below to get drivers list");

    //--------END:LIST WIDGET SETTINGS-----------

    //----GETTING UPTIME----

    startUpTimeThread();
    startPrintUpTimeThread();
    qDebug() << "after thread";


    //----GETTING BASEBOARD INFO----

    ui->motherBoardNameTempLabel->setText(hc.getBaseboardInfo());

    //----GETTING CPU INFO----

    ui->cpuNameTempLabel->setText(hc.getCPUInfo());

    //----GETTING GPU INFO----

    ui->GPUNameTempLabel->setText("Graphics card: " + hc.getGPUInfo());

    //----GETTING RAM INFO----

    int amountOfBars{0};
    ramInfo =  hc.getRAMInfo(&amountOfBars);

    for(int i{0}; i < amountOfBars; ++i){
         ui->ramInfoComboBox->addItem(ramInfo[i]);
    }

    //----GETTING STORAGE INFO----

    int amountOfDisks{0};

    storageInfo = hc.getStorageInfo(&amountOfDisks);

    for(int i{0}; i < amountOfDisks; ++i){
        ui->storageInfoComboBox->addItem(storageInfo[i]);
    }

    //----GETTING AUDIO INFO----

    int amountOfAudioDevices{0};

    audioInfo = hc.getAudioDevicesInfo(&amountOfAudioDevices);

    for(int i{0}; i < amountOfAudioDevices; ++i){
        ui->audioDevicesComboBox->addItem(audioInfo[i]);
    }

    //----GETTING NETWORK ADAPTER INFO----

    int amountOfetworkControllers{0};

    networkControllerInfo = hc.getNetworkControllers(&amountOfetworkControllers);

    for(int i{0}; i < amountOfetworkControllers; ++i){
        ui->networkControllerComboBox->addItem(networkControllerInfo[i]);
    }

    initMWConneciotns();
    startGetCPULoadThread();
    startGetRAMLoadThread();

    qDebug() << "after threads";

    activeIndexHdd = 0;

    startGetTemperaturesThread();

    //*********************END:OVERVIEW**********************//



    //*********************BEGING:MONITORING TOOLS**********************//
    ui->acceptableCpuProcessesList->setStyleSheet(
      "QListWidget{"
      "font: 12pt \"ГОСТ тип А\";"
      "color: rgb(214, 214, 214);"
      "background-color: rgb(63, 63, 63);"
      "border: none;"
      "}"
      "QListWidget::item:selected {"
         "background-color: green;"
      "}");

    ui->acceptableRamProcessesList->setStyleSheet(
      "QListWidget{"
      "font: 12pt \"ГОСТ тип А\";"
      "color: rgb(214, 214, 214);"
      "background-color: rgb(63, 63, 63);"
      "border: none;"
      "}"
      "QListWidget::item:selected {"
         "background-color: green;"
      "}");

    ui->outputTypeComboBox->addItem("Message");
    ui->outputTypeComboBox->addItem("Log");

    ui->monitoringTypeRam->addItem("Working Set");
    ui->monitoringTypeRam->addItem("Private Set");

    ui->resetCPU->setDisabled(true);
    ui->resetRAM->setDisabled(true);
    ui->stopCPU->setDisabled(true);
    ui->stopRAM->setDisabled(true);

    if(!ui->monitorCpuCheckBox->isChecked()){
        ui->acceptaleCpuLoadTxtBox->setDisabled(true);
        ui->cpuBrowseButton->setDisabled(true);
        ui->hintButton_accProcCpu->setDisabled(true);
    }

    if(!ui->monitorRamCheckBox->isChecked()){
        ui->acceptaleRamLoadTxtBox->setDisabled(true);
        ui->ramBrowseButton->setDisabled(true);
        ui->monitoringTypeRam->setDisabled(true);
        ui->hintButton->setDisabled(true);
        ui->hintButton_accProcRam->setDisabled(true);
    }

    if(ui->acceptableCpuProcessesList->count() == 0){
        ui->cpuDeleteButton->setDisabled(true);
    }

    if(ui->acceptableRamProcessesList->count() == 0){
        ui->ramDeleteButton->setDisabled(true);
    }

     ui->acceptaleCpuLoadTxtBox->setText("50");
     ui->acceptaleRamLoadTxtBox->setText("100000");

     ui->cpuIsMonitoringLabel->setText("");
     ui->ramIsMonitoringLabel->setText("");

    if(ui->outputTypeComboBox->currentIndex() == 0){
        ui->logNameBox->setDisabled(true);
    }
    else{
        ui->logNameBox->setDisabled(false);
    }

    //*********************END:MONITORING TOOLS**********************//

    //*********************BEGING:MONITORING TOOLS GLOBAL**********************//
     ui->cpuIsMonitoringLabel_Mtg->setText("");
     ui->ramIsMonitoringLabel_Mtg->setText("");

     ui->outputTypeComboBox_Mtg->addItem("Message");
     ui->outputTypeComboBox_Mtg->addItem("Log");

     ui->resetCPU_Mtg->setDisabled(true);
     ui->resetRAM_Mtg->setDisabled(true);
     ui->stopCPU_Mtg->setDisabled(true);
     ui->stopRAM_Mtg->setDisabled(true);

     if(!ui->monitorCpuCheckBox_mtg->isChecked()){
         ui->acceptaleCpuLoadTxtBox_mtg->setDisabled(true);
     }
     if(!ui->monitorRamCheckBox_mtg->isChecked()){
         ui->acceptaleRamLoadTxtBox_mtg->setDisabled(true);
     }


    //*********************END:MONITORING TOOLS GLOBAL**********************//


   //*********************BEGING:EXTERNAL DISPLAY TOOLS**********************//

   //*********************END:EXTERNAL DISPLAY TOOLS**********************//

}

void MainWindow::startUpTimeThread(){
    upTimeThread = new std::thread([&](){
        hc.getUptime(upTime_hours, upTime_minutes, upTime_seconds, upTime_milliseconds, hc);
    });
}

void MainWindow::startPrintUpTimeThread(){
    printUpTimeThread = new std::thread([&](){
        this->printUpTime();
    });
}

void MainWindow::printUpTime(){
    while(isPrintUpTimeRunning){
        QString upTimeLabelString;
        upTimeLabelString += QString::number(upTime_hours);
        upTimeLabelString += ":";
        upTimeLabelString += QString::number(upTime_minutes);
        upTimeLabelString += ":";
        upTimeLabelString += QString::number(upTime_seconds);
        upTimeLabelString += ":";
        upTimeLabelString += QString::number(upTime_milliseconds);

        ui->upTimeLabel->setText(upTimeLabelString);

        Sleep(1000);
    }
}

void MainWindow::startGetCPULoadThread(){
    getCPULoadThread = new std::thread([&](){
        int cpuLoadToShow{0};
        while(this->isRunningGetCPULoad){
            cpuLoadToShow = cc.getCPULoad() * 100;
            emit emitCPULoadValue(cpuLoadToShow);
            Sleep(1000);
        }
    });
}

void MainWindow::startGetRAMLoadThread(){
    getRAMLoadThread = new std::thread([&](){
        int ramLoadToShow{0};
        while(this->isRunningGetRAMLoad){
            ramLoadToShow = cc.getRAMLoad();
            emit emitRAMLoadValue(ramLoadToShow);
            Sleep(1000);
        }
    });
}

void MainWindow::startGetTemperaturesThread(){
    getTemperaturesThread = new std::thread([&](){
        vector<string> temperaturesForUiVect;
        hc.startProcessOfTemperatures();
        while(this->isRunningGetRAMLoad){
            //Sleep(150);
            temperaturesForUiVect = hc.readTemperaturesFromFile();

            QString qTemperatureForUiCpu = QString::fromStdString(temperaturesForUiVect.at(0));
            qTemperatureForUiCpu += "°C";
            ui->cpuTemp_Label->setText(qTemperatureForUiCpu);

            QString qTemperatureForUiGpu = QString::fromStdString(temperaturesForUiVect.at(1));
            qTemperatureForUiGpu += "°C";
            ui->gpuTemp_Label->setText(qTemperatureForUiGpu);


            QString giveHddTemps[MAX_AMOUNT_OF_TEMPERATURE_PARAMETERS - INDEX_OF_FIRST_HDD];

            for(int i = INDEX_OF_FIRST_HDD, k = 0; i < temperaturesForUiVect.size(); ++i, ++k){
                giveHddTemps[k] = QString::fromStdString(temperaturesForUiVect.at(i));
            }

            setHddTemps(giveHddTemps);

            ui->hddTemp_Label->setText(hddTempsFromFile[activeIndexHdd] + "°C");

            Sleep(2000);
        }
    });
}

void MainWindow::setHddTemps(QString *hddTemps){
    for(int i = 0; i < MAX_AMOUNT_OF_TEMPERATURE_PARAMETERS - INDEX_OF_FIRST_HDD; ++i){
        this->hddTempsFromFile[i] = hddTemps[i];
    }
    for(int i = 0; i < MAX_AMOUNT_OF_TEMPERATURE_PARAMETERS - INDEX_OF_FIRST_HDD; ++i){
        qDebug() << this->hddTempsFromFile[i];
    }
}

void MainWindow::initConnections(){
    connect(&cc, SIGNAL(emitMessage(QString, bool, bool)), this, SLOT(getMessage(QString, bool, bool)));
}

void MainWindow::initMWConneciotns(){
    connect(this, &MainWindow::emitCPULoadValue, this, &MainWindow::getCPULoadValue);
    connect(this, &MainWindow::emitRAMLoadValue, this, &MainWindow::getRAMLoadValue);
    connect(this, &MainWindow::emitChangeHDDTemperature, this, &MainWindow::changeHDDTemperatureLabel);
}

//-------------------------------------BEGIN: SLOTS------------------------------
void MainWindow::getMessage(QString infoString, bool fromCpu, bool mtGlobal){
    if(fromCpu){    /*CPU PART*/
        if(!mtGlobal){  /*PROCESSES MONITORING*/
            ui->cpuIsMonitoringLabel->setText("Stoped");
            ui->cpuIsMonitoringLabel->setStyleSheet("color: red");
            ui->resetCPU->setDisabled(false);
            ui->stopCPU->setDisabled(true);
       }
        else{       /*GLOBAL MONITORING*/
            ui->cpuIsMonitoringLabel_Mtg->setText("Stoped");
            ui->cpuIsMonitoringLabel_Mtg->setStyleSheet("color: red");
            ui->resetCPU_Mtg->setDisabled(false);
            ui->stopCPU_Mtg->setDisabled(true);
        }
    }
    else{       /*RAM PART*/
        if(!mtGlobal){
            ui->ramIsMonitoringLabel->setText("Stoped");
            ui->ramIsMonitoringLabel->setStyleSheet("color: red");
            ui->resetRAM->setDisabled(false);
            ui->stopRAM->setDisabled(true);
        }
        else{       /*GLOBAL MONITORING*/
            ui->ramIsMonitoringLabel_Mtg->setText("Stoped");
            ui->ramIsMonitoringLabel_Mtg->setStyleSheet("color: red");
            ui->resetRAM_Mtg->setDisabled(false);
            ui->stopRAM_Mtg->setDisabled(true);
        }
    }

    QMessageBox box;
    box.setStyleSheet("background-color: rgb(54, 54, 54); color : white");
    box.setText(infoString);
    QPixmap ic(":/icon/icon32.png");
    box.setIconPixmap(ic);

    box.exec();

};

void MainWindow::getCPULoadValue(int cpuLoadValue){
    ui->cpuProgressBar->setValue(cpuLoadValue);
}

void MainWindow::getRAMLoadValue(int ramLoadValue){
    ui->ramProgressBar->setValue(ramLoadValue);
}

void MainWindow::changeHDDTemperatureLabel(int index){
    //qDebug() << "in slot " << index;
    ui->hddTemp_Label->setText(this->hddTempsFromFile[index] + "°C");
}

//-------------------------------------END: SLOTS------------------------------

//*********************BEGING:OVERVIEW**********************//

void MainWindow::on_getProcessesButton_clicked()
{
    ui->processList->clear();
    std::vector<QString> *processesList = new std::vector<QString>;

    cc.getProcesses(false, processesList);

    for(int i{0}; i < processesList->size(); ++i){
        ui->processList->addItem(processesList->at(i));
    }

    QString str;
    str = "Amount of processes: ";
    str += QString::number(processesList->size());
    ui->amountOfProcesses->setText(str);
}

void MainWindow::on_getDriversButton_clicked()
{
    ui->driversList->clear();
    std::vector<QString> *driversVect = new std::vector<QString>;
    cc.getDriversList(driversVect);

    for(int i{0}; i < driversVect->size(); ++i){
       ui->driversList->addItem(driversVect->at(i));
    }

    QString str;
    str = "Amount of drivers: ";
    str += QString::number(driversVect->size());
    ui->amountOfDrivers->setText(str);
}

void MainWindow::on_storageInfoComboBox_currentIndexChanged(int index)
{
    qDebug() << "CHAAAAAAAAAAAAAAAAAAAANGEEEEEEEEEEEEEEED";
    activeIndexHdd = index;
    emitChangeHDDTemperature(index);
}


//*********************END:OVERVIEW**********************//

//*********************BEGING:MONITORING TOOLS**********************//


//APPLY BUTTON
void MainWindow::on_applyButtonMt_clicked(){
    bool isSelectedWorkingSet = true;;
    initConnections();

    cc.setStopFromUiCpuProcess(false);
    cc.setStopFromUiRamProcess(false);

    cc.setCpuMonitoringActive(ui->monitorCpuCheckBox->isChecked());
    cc.setRamMonitoringActive(ui->monitorRamCheckBox->isChecked());

    if(ui->monitoringTypeRam->currentText() == "Working Set"){
        isSelectedWorkingSet = true;
    }
    else{
       isSelectedWorkingSet = false;
    }

    if(ui->acceptaleCpuLoadTxtBox->text() != "50"){
        memoryCpuAccaptableLoad = ui->acceptaleCpuLoadTxtBox->text().toInt();
    }
    if(ui->acceptaleRamLoadTxtBox->text() != "50"){
        memoryRamAccaptableLoad = ui->acceptaleRamLoadTxtBox->text().toInt();
    }

    cc.setOutputTypeMt(ui->outputTypeComboBox->currentIndex());
    if(ui->outputTypeComboBox->currentIndex() == 1){
        QString logName;

        logName = ui->logNameBox->text();
        if(ui->monitorCpuCheckBox && !ui->monitorRamCheckBox){
            qDebug("to create file cpu");
            system("pause");
             cc.createFile(0, false, logName);
        }
        else if(!ui->monitorCpuCheckBox && ui->monitorRamCheckBox){

             cc.createFile(1, false, logName);
        }
        else if(ui->monitorCpuCheckBox && ui->monitorRamCheckBox){
             cc.createFile(2, false, logName);
        }
    }


    if(ui->monitorCpuCheckBox->isChecked()){
        ui->cpuIsMonitoringLabel->setText("Monitoring...");
        ui->cpuIsMonitoringLabel->setStyleSheet("color: rgb(43, 117, 34);");
        ui->resetCPU->setDisabled(true);
        ui->stopCPU->setDisabled(false);
        cc.setUserAcceptableCpuLoad(memoryCpuAccaptableLoad);

        //give processes to ignore
        if(ui->acceptableCpuProcessesList->count() != 0){
            for(int i = 0; i < ui->acceptableCpuProcessesList->count(); i++){
                cc.setIgnoreProcessesVectorElement(true, ui->acceptableCpuProcessesList->item(i)->text());
            }
        }

        cc.monitoringCpuStart();

    }
    if(ui->monitorRamCheckBox->isChecked()){
        ui->ramIsMonitoringLabel->setText("Monitoring...");
        ui->ramIsMonitoringLabel->setStyleSheet("color: rgb(43, 117, 34);");
        ui->resetRAM->setDisabled(true);
        ui->stopRAM->setDisabled(false);
        cc.setUserAcceptableRamLoad(memoryRamAccaptableLoad);
        cc.setRamMonitoringType(isSelectedWorkingSet);

        if(ui->acceptableRamProcessesList->count() != 0){
            for(int i = 0; i < ui->acceptableRamProcessesList->count(); i++){
                cc.setIgnoreProcessesVectorElement(false, ui->acceptableRamProcessesList->item(i)->text());
            }
        }

        cc.monitoringRamStart();
    }
}

//CPU MONITORING CHECKBOX
void MainWindow::on_monitorCpuCheckBox_clicked()
{
    if(ui->monitorCpuCheckBox->isChecked()){
        ui->acceptaleCpuLoadTxtBox->setDisabled(false);
        ui->cpuBrowseButton->setDisabled(false);
        ui->hintButton_accProcCpu->setDisabled(false);
    }
    else{
        ui->acceptaleCpuLoadTxtBox->setDisabled(true);
        ui->cpuBrowseButton->setDisabled(true);
        ui->hintButton_accProcCpu->setDisabled(true);
    }
}

//RAM MONITORING CHECKBOX
void MainWindow::on_monitorRamCheckBox_clicked()
{
    if(ui->monitorRamCheckBox->isChecked()){
        ui->acceptaleRamLoadTxtBox->setDisabled(false);
        ui->ramBrowseButton->setDisabled(false);
        ui->monitoringTypeRam->setDisabled(false);
        ui->hintButton->setDisabled(false);
        ui->hintButton_accProcRam->setDisabled(false);
    }
    else{
        ui->acceptaleRamLoadTxtBox->setDisabled(true);
        ui->ramBrowseButton->setDisabled(true);
        ui->monitoringTypeRam->setDisabled(true);
        ui->hintButton->setDisabled(true);
        ui->hintButton_accProcRam->setDisabled(true);
    }
}

void MainWindow::on_hintButton_clicked()
{
    if(!typeHintButtonActivated){
        ui->hinthiddenLabel->setText("fffffffffffffffffff");
        ui->hintButton->setText("X");
        typeHintButtonActivated = true;
    }
    else{
        ui->hinthiddenLabel->setText("");
        ui->hintButton->setText("?");
        typeHintButtonActivated = false;
    }
}

void MainWindow::on_hintButton_accProcCpu_clicked()
{
    if(!cpuHintButtonActivated){
        ui->hinthiddenLabel->setText("Find .exe file of process to ignore");
        ui->hintButton_accProcCpu->setText("X");
        cpuHintButtonActivated = true;
    }
    else{
        ui->hinthiddenLabel->setText("");
        ui->hintButton_accProcCpu->setText("?");
        cpuHintButtonActivated = false;
    }
}

void MainWindow::on_hintButton_accProcRam_clicked()
{
    if(!ramHintButtonActivated){
        ui->hinthiddenLabel->setText("Find .exe file of process to ignore");
        ui->hintButton_accProcRam->setText("X");
        ramHintButtonActivated = true;
    }
    else{
        ui->hinthiddenLabel->setText("");
        ui->hintButton_accProcRam->setText("?");
        ramHintButtonActivated = false;
    }
}

//slot

void MainWindow::on_resetCPU_clicked()
{
    ui->hintResLabel->setText("Press \"Apply\" to start again.");
    ui->cpuIsMonitoringLabel->setText("");
    ui->resetCPU->setDisabled(true);
}

void MainWindow::on_resetRAM_clicked()
{
    ui->hintResLabel->setText("Press \"Apply\" to start again.");
    ui->ramIsMonitoringLabel->setText("");
    ui->resetRAM->setDisabled(true);
}

void MainWindow::on_outputTypeComboBox_currentIndexChanged(int index)
{
    if(index == 0){
        ui->logNameBox->setDisabled(true);
    }
    else{
        ui->logNameBox->setDisabled(false);
    }
}

void MainWindow::on_stopCPU_clicked()
{
    cc.setStopFromUiCpuProcess(true);
}

void MainWindow::on_stopRAM_clicked()
{
    cc.setStopFromUiRamProcess(true);
}

void MainWindow::on_cpuBrowseButton_clicked()
{
    int lastSlashIndex = 0;

    QString filters = "Executive files (*.exe)";
    QString fileName = QFileDialog::getOpenFileName(this, "Open a file", "C://", filters);

    for(int i = 0; i < fileName.size(); i++){
        if(fileName[i] == '/'){
            lastSlashIndex = i;
        }
    }

    int splittingStringStartIndex = fileName.size() - lastSlashIndex;
    fileName = fileName.mid(lastSlashIndex + 1, splittingStringStartIndex);

    ui->acceptableCpuProcessesList->addItem(fileName);
}

void MainWindow::on_acceptableCpuProcessesList_itemClicked(QListWidgetItem *item)
{
    ui->cpuDeleteButton->setDisabled(false);
}

void MainWindow::on_cpuDeleteButton_clicked()
{
    ui->acceptableCpuProcessesList->takeItem(ui->acceptableCpuProcessesList->currentRow());

    if(ui->acceptableCpuProcessesList->count() == 0){
        ui->cpuDeleteButton->setDisabled(true);
    }
}

void MainWindow::on_ramBrowseButton_clicked()
{
    int lastSlashIndex = 0;

    QString filters = "Executive files (*.exe)";
    QString fileName = QFileDialog::getOpenFileName(this, "Open a file", "C://", filters);

    for(int i = 0; i < fileName.size(); i++){
        if(fileName[i] == '/'){
            lastSlashIndex = i;
        }
    }

    int splittingStringStartIndex = fileName.size() - lastSlashIndex;
    fileName = fileName.mid(lastSlashIndex + 1, splittingStringStartIndex);

    qDebug() << "ui->acceptableCpuProcessesList->item(0)->text()";

    ui->acceptableRamProcessesList->addItem(fileName);

    qDebug() << ui->acceptableRamProcessesList->item(0)->text();
}

void MainWindow::on_ramDeleteButton_clicked()
{
    ui->acceptableRamProcessesList->takeItem(ui->acceptableRamProcessesList->currentRow());

    if(ui->acceptableRamProcessesList->count() == 0){
        ui->ramDeleteButton->setDisabled(true);
    }
}

void MainWindow::on_acceptableRamProcessesList_itemClicked(QListWidgetItem *item)
{
     ui->ramDeleteButton->setDisabled(false);
}

//*********************END:MONITORING TOOLS**********************//

//*********************BEGIN:MONITORING TOOLS GLOBAL**********************//

void MainWindow::on_applyButtonMtg_clicked(){
    int loadProc = 0, loadRam = 0;
    loadProc = QVariant(ui->acceptaleCpuLoadTxtBox_mtg->text()).toInt();
    loadRam = QVariant(ui->acceptaleRamLoadTxtBox_mtg->text()).toInt();

    initConnections();

    if(loadProc > 100 || loadProc < 0){
        QMessageBox box;
        QString msgStr = "Incorrect input, acceptable CPU load can not be ";
        msgStr.append(ui->acceptaleCpuLoadTxtBox_mtg->text());
        box.setStyleSheet("background-color: rgb(54, 54, 54); color : white");
        box.setText(msgStr);
        box.exec();
    }
    else if(loadRam > 100 || loadRam < 0){
        QMessageBox box;
        QString msgStr = "Incorrect input, acceptable RAM load can not be ";
        msgStr.append(ui->acceptaleRamLoadTxtBox_mtg->text());
        box.setStyleSheet("background-color: rgb(54, 54, 54); color : white");
        box.setText(msgStr);
        box.exec();
    }
    else{
        cc.setOutputTypeMtg(ui->outputTypeComboBox_Mtg->currentIndex());
        if(ui->outputTypeComboBox_Mtg->currentIndex() == 1){
            QString logName;

            logName = ui->logNameBox->text();
            if(ui->monitorCpuCheckBox_mtg && !ui->monitorRamCheckBox_mtg){
                 cc.createFile(0, true, logName);
            }
            else if(!ui->monitorCpuCheckBox_mtg && ui->monitorRamCheckBox_mtg){

                 cc.createFile(1, true, logName);
            }
            else if(ui->monitorCpuCheckBox_mtg && ui->monitorRamCheckBox_mtg){
                 cc.createFile(2, true, logName);
            }
        }

        if(ui->monitorCpuCheckBox_mtg->isChecked()){
            cc.setUserAcceptableCpuLoadOverall(loadProc);
            ui->cpuIsMonitoringLabel_Mtg->setText("Monitoring...");
            ui->cpuIsMonitoringLabel_Mtg->setStyleSheet("color: rgb(43, 117, 34);");
            ui->stopCPU_Mtg->setDisabled(false);

            cc.monitoringCpuOverallStart();
        }
        if(ui->monitorRamCheckBox_mtg->isChecked()){
            cc.setUserAcceptableRamLoadOverall(loadRam);
            ui->ramIsMonitoringLabel_Mtg->setText("Monitoring...");
            ui->ramIsMonitoringLabel_Mtg->setStyleSheet("color: rgb(43, 117, 34);");
            ui->stopRAM_Mtg->setDisabled(false);

            cc.monitoringRamOverallStart();
        }
    }
}

void MainWindow::on_stopCPU_Mtg_clicked()
{
    cc.setStopFromUiCpuLoadOverall(true);
}

void MainWindow::on_resetCPU_Mtg_clicked()
{
    ui->hintResLabel_Mtg->setText("Press \"Apply\" to start again.");
    ui->cpuIsMonitoringLabel_Mtg->setText("");
    ui->resetCPU_Mtg->setDisabled(true);
}


void MainWindow::on_stopRAM_Mtg_clicked()
{
   cc.setStopFromUiRamLoadOverall(true);
}

void MainWindow::on_resetRAM_Mtg_clicked()
{
    ui->hintResLabel_Mtg->setText("Press \"Apply\" to start again.");
    ui->ramIsMonitoringLabel_Mtg->setText("");
    ui->resetRAM_Mtg->setDisabled(true);
}

void MainWindow::on_monitorCpuCheckBox_mtg_clicked()
{
    if(ui->monitorCpuCheckBox_mtg->isChecked()){
       ui->acceptaleCpuLoadTxtBox_mtg->setDisabled(false);
    }
    else{
       ui->acceptaleCpuLoadTxtBox_mtg->setDisabled(true);
    }
}

void MainWindow::on_monitorRamCheckBox_mtg_clicked()
{
    if(ui->monitorRamCheckBox_mtg->isChecked()){
       ui->acceptaleRamLoadTxtBox_mtg->setDisabled(false);
    }
    else{
       ui->acceptaleRamLoadTxtBox_mtg->setDisabled(true);
    }
}

//*********************END:MONITORING TOOLS GLOBAL**********************//

//*********************BEGING:EXTERNAL DISPLAY TOOLS**********************//

//CPU TEMP CHECKBOX
void MainWindow::on_onScreenCpuTempCheckBox_clicked()
{
    int i = 0;
    if(ui->onScreenCpuTempCheckBox->isChecked()){
         ui->mainParamComboBox->addItem(ui->cpuTempParam->text());
    }
    else{
        for(i = 0; i < ui->mainParamComboBox->count(); i++){
            if(ui->mainParamComboBox->itemText(i) == ui->cpuTempParam->text()){
                break;
            }
        }
        ui->mainParamComboBox->removeItem(i);
    }
}

//CPU LOAD CHECKBOX
void MainWindow::on_onScreenCpuLoadCheckBox_clicked()
{
    int i = 0;
    if(ui->onScreenCpuLoadCheckBox->isChecked()){
         ui->mainParamComboBox->addItem(ui->cpuLoadParam->text());
    }
    else{
        for(i = 0; i < ui->mainParamComboBox->count(); i++){
            if(ui->mainParamComboBox->itemText(i) == ui->cpuLoadParam->text()){
                break;
            }
        }
        ui->mainParamComboBox->removeItem(i);
    }
}

//RAM LOAD CHECKBOX
void MainWindow::on_onScreenRamLoadCheckBox_clicked()
{
    int i = 0;
    if(ui->onScreenRamLoadCheckBox->isChecked()){
         ui->mainParamComboBox->addItem(ui->ramLoadParam->text());
    }
    else{
        for(i = 0; i < ui->mainParamComboBox->count(); i++){
            if(ui->mainParamComboBox->itemText(i) == ui->ramLoadParam->text()){
                break;
            }
        }
        ui->mainParamComboBox->removeItem(i);
    }
}

//GPU TEMP CHECKBOX
void MainWindow::on_onScreenGpuTempCheckBox_clicked()
{
    int i = 0;
    if(ui->onScreenGpuTempCheckBox->isChecked()){
         ui->mainParamComboBox->addItem(ui->gpuTempParam->text());
    }
    else{
        for(i = 0; i < ui->mainParamComboBox->count(); i++){
            if(ui->mainParamComboBox->itemText(i) == ui->gpuTempParam->text()){
                break;
            }
        }
        ui->mainParamComboBox->removeItem(i);
    }
}

//GPU LOAD CHECKBOX
void MainWindow::on_onScreenGpuLoadCheckBox_clicked()
{
    int i = 0;
    if(ui->onScreenGpuLoadCheckBox->isChecked()){
         ui->mainParamComboBox->addItem(ui->gpuLoadParam->text());
    }
    else{
        for(i = 0; i < ui->mainParamComboBox->count(); i++){
            if(ui->mainParamComboBox->itemText(i) == ui->gpuLoadParam->text()){
                break;
            }
        }
        ui->mainParamComboBox->removeItem(i);
    }
}

//MOTHERBOARD TEMP CHECKBOX
void MainWindow::on_onScreenMotherboardTempCheckBox_clicked()
{
    int i = 0;
    if(ui->onScreenMotherboardTempCheckBox->isChecked()){
         ui->mainParamComboBox->addItem(ui->motherboardTempParam->text());
    }
    else{
        for(i = 0; i < ui->mainParamComboBox->count(); i++){
            if(ui->mainParamComboBox->itemText(i) == ui->motherboardTempParam->text()){
                break;
            }
        }
        ui->mainParamComboBox->removeItem(i);
    }
}

//STORAGE TEMP CHECKBOX
void MainWindow::on_onScreenStorageTempCheckBox_clicked()
{
    int i = 0;
    if(ui->onScreenStorageTempCheckBox->isChecked()){
         ui->mainParamComboBox->addItem(ui->storageTempParam->text());
    }
    else{
        for(i = 0; i < ui->mainParamComboBox->count(); i++){
            if(ui->mainParamComboBox->itemText(i) == ui->storageTempParam->text()){
                break;
            }
        }
        ui->mainParamComboBox->removeItem(i);
    }
}

//UPTIME CHECKBOX
void MainWindow::on_onScreenUpTimeCheckBox_clicked()
{
    int i = 0;
    if(ui->onScreenUpTimeCheckBox->isChecked()){
         ui->mainParamComboBox->addItem(ui->uptimeParam->text());
    }
    else{
        for(i = 0; i < ui->mainParamComboBox->count(); i++){
            if(ui->mainParamComboBox->itemText(i) == ui->uptimeParam->text()){
                break;
            }
        }
        ui->mainParamComboBox->removeItem(i);
    }
}

//*********************END:EXTERNAL DISPLAY TOOLS**********************//
