QT       += core gui
QT       += core gui charts
QT       += widgets

QMAKE_CXXFLAGS += -clr
QMAKE_CXXFLAGS_STL_ON -= -EHsc
QMAKE_CXXFLAGS_EXCEPTIONS_ON -= -EHsc

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    countingcenter.cpp \
    cpuusage.cpp \
    hardwareinformationcenter.cpp \
    main.cpp \
    mainwindow.cpp \
    smarthandle.cpp

HEADERS += \
    countingcenter.h \
    cpuusage.h \
    hardwareinformationcenter.h \
    mainwindow.h \
    smarthandle.h

FORMS += \
    mainwindow.ui



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
