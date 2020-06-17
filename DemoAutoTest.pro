#-------------------------------------------------
#
# Project created by QtCreator 2020-06-15T15:54:15
#
#-------------------------------------------------

QT       += core gui network xml serialport printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DemoAutoTest
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    cWorker.cpp \
    cCardDetection.cpp \
    ../../xmsdk/src/mlsBluefinSerial/src/mlsCmdParser.c \
    ../../xmsdk/src/mlsBluefinSerial/src/mlsComm.c \
    ../../xmsdk/src/mlsBluefinSerial/src/mlsReaderHandler.c \
    ../../xmsdk/src/mlsBluefinSerial/src/mlsSerialDebugPrint.c \
    ../../xmsdk/src/mlsBluefinSerial/src/mlsSerialPorting.c \
    ../../xmsdk/src/mlsBluefinSerial/src/winQueue.c \
    ../../xmsdk/src/mlsBluefinSerial/src/winSerial.c \
    ../../xmsdk/src/mlsBluefinSerial/src/mlsHouseKeepingService.c \
    ../../xmsdk/src/mlsRFCAPI/src/rfcapiinfineonmyd.c \
    ../../xmsdk/src/mlsAppCAPI/src/mlsappcapi.c \
    ../../xmsdk/src/mlsRFCAPI/src/rfcapiinit.c

HEADERS  += mainwindow.h \
    cWorker.h \
    cCardDetection.h \
    ../../xmsdk/src/include/mlsCompileSwitches.h \
    ../../xmsdk/src/include/mlsErrors.h \
    ../../xmsdk/src/include/mlsInclude.h \
    ../../xmsdk/src/include/mlsMacros.h \
    ../../xmsdk/src/include/mlsTypes.h \
    ../../xmsdk/src/mlsBluefinSerial/inc/mlsCmdParser.h \
    ../../xmsdk/src/mlsBluefinSerial/inc/mlsComm.h \
    ../../xmsdk/src/mlsBluefinSerial/inc/mlsReaderHandler.h \
    ../../xmsdk/src/mlsBluefinSerial/inc/mlsSerialDebugPrint.h \
    ../../xmsdk/src/mlsBluefinSerial/inc/mlsSerialPorting.h \
    ../../xmsdk/src/mlsBluefinSerial/inc/winQueue.h \
    ../../xmsdk/src/mlsBluefinSerial/inc/winSerial.h \
    ../../xmsdk/src/mlsBluefinSerial/inc/mlsHouseKeepingService.h \
    ../../xmsdk/src/mlsBluefinSerial/inc/mlsRfApi.h \
    ../../xmsdk/src/mlsAppCAPI/inc/mlsappcapi.h \
    ../../xmsdk/src/mlsRFCAPI/inc/mifaredesfireerrorcode.h \
    ../../xmsdk/src/mlsRFCAPI/inc/resultcode.h \
    ../../xmsdk/src/mlsRFCAPI/inc/rfcapiinit.h

FORMS    += mainwindow.ui

INCLUDEPATH += ../../xmsdk/src/include \
     ../../xmsdk/src/mlsBluefinSerial/inc \
     ../../xmsdk/src/mlsRFCAPI/inc \

win32: LIBS += -L$$PWD/DobotDll/ -lDobotDll

INCLUDEPATH += $$PWD/DobotDll
DEPENDPATH += $$PWD/DobotDll

win32:DEFINES += PLATFORM_SEL_WINDOWS=1 SUPPORT_DUMP_DEBUG_DATA=0 BUILD_MODE_HOST=1 QT_CREATOR SERIAL_READ_NON_BLOCKING=1 QT_DEBUG_PRINT
else:unix:DEFINES += USE_DF_TIMER PLATFORM_SEL_LINUX=1 SUPPORT_DUMP_DEBUG_DATA=0 BUILD_MODE_HOST=1 QT_CREATOR BUILD_ENV_LINUX_PC=1 SERIAL_READ_NON_BLOCKING=1 QT_DEBUG_PRINT
