#include "mainwindow.h"

#include "countingcenter.h"

#include <QApplication>

#include <windows.h>

DWORD WINAPI testThread(LPVOID lpParam);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);   

    MainWindow w;
    w.show();   

    return a.exec();
}

