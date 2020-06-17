#include "cWorker.h"

cWorker::cWorker(QObject *parent) : QObject(parent)
{
    _abort = false;
    _interrupt = false;
}

void cWorker::requestMethod(cWorker::Method method)
{
    QMutexLocker locker(&mutex);
    _interrupt = true;
    _method = method;
    condition.wakeOne();
}

void cWorker::abort()
{
    QMutexLocker locker(&mutex);
    if(mFile->isOpen()) mFile->close();
    _abort = true;
    condition.wakeOne();
}

void cWorker::doStart()
{
//    mFile = new QFile(mFileName);
    QTextStream file(mFile);
    QString line;
    QByteArray data;
    bool check = false;
    if (mFile->open(QIODevice::ReadOnly)) {
        while (!file.atEnd()) {
            mutex.lock();
            bool abort = _abort;
            bool interrupt = _interrupt;
            mutex.unlock();

            if (abort || interrupt) break;
            check = false;

            data.clear();
            line = file.readLine(30);
            QStringList list = line.split(",");
            if(list.size() == 3)
            {
                quint8 resultCode = 0xFF;
                float x = list.at(0).toFloat();
                float y = list.at(1).toFloat();
                float z = list.at(2).toFloat();

                if (moveToXYZ(x, y, z)) packageData(x, y, z, "Card Detection", data, true);
//                if (moveToXYZ(x, y, z)) resultCode = mCardDetection->cardDetection(0x03, 0x01, 0x06, 0x06, 0x06, 0xFFFF, 500, &data);
//                if (resultCode == 0x00) packageData(x, y, z, "Card Detection", data, true);
//                else packageData(x, y, z, "Card Detection", data, false);
            }
        }
    }
    mFile->close();
}

void cWorker::doStop()
{
    if (mFile->isOpen()) mFile->close();
}

void cWorker::doOpen()
{
//    rfcapi_Init()
}

void cWorker::doClose()
{
//    rfcapi_Deinit()
}

void cWorker::mainLoop()
{
    mCardDetection = new cCardDetection();
    mFile = new QFile(mFileName);
    forever {
        mutex.lock();
        if (!_interrupt && !_abort) {
            condition.wait(&mutex);
        }
        _interrupt = false;

        if (_abort) {
            mutex.unlock();
            emit finished();
            return;
        }

        Method method = _method;
        mutex.unlock();

        switch(method) {
        case Start:
            doStart();
            break;
        case Stop:
            doStop();
            break;
        case Open:
            doOpen();
            break;
        case Close:
            doClose();
            break;
        }
    }
}

void cWorker::packageData(float x, float y, float z, QString function, QByteArray data, bool status)
{
    QVariantMap map;
    map.insert("x", x);
    map.insert("y", y);
    map.insert("z", z);
    map.insert("function", function);
    map.insert("data", data);
    map.insert("pass", status);
    emit valueChanged(map);
}

bool cWorker::equalFloat(float f1, float f2)
{
    if (f1 > f2)
    {
        if (f1 - f2 <= 2) return true;
        else return false;
    }
    else if (f1 < f2)
    {
        if (f2 - f1 <= 2) return true;
        else return false;
    }
    else
    {
        if (f2 == f1) return true;
        else return false;
    }
}

bool cWorker::moveToXYZ(float x, float y, float z)
{
    Pose mPose;
    PTPCmd ptpCmd;
    ptpCmd.ptpMode = PTPMOVJXYZMode;
    ptpCmd.r = mOriginPose.r;
    float tempZ;

    bool isCheck = true;
    bool abort;
    bool interrupt;

    // Step 1: current x, current y, z + 20
    isCheck = true;
    GetPose(mDobotId, &mPose);
    ptpCmd.x = mPose.x;
    ptpCmd.y = mPose.y;
    ptpCmd.z = mPose.z + z + 30;
    tempZ = ptpCmd.z;

    while (SetPTPCmd(mDobotId, &ptpCmd, true, NULL) != DobotCommunicate_NoError) {
        return false;
    }

    while (isCheck) {
        Pose pose;
        GetPose(mDobotId, &pose);
        if (equalFloat(pose.x, ptpCmd.x) &&
            equalFloat(pose.y, ptpCmd.y) &&
            equalFloat(pose.z, ptpCmd.z) &&
            equalFloat(pose.r, ptpCmd.r)) isCheck = false;
    }

    // Step 2: new x, new y, z + 20
    isCheck = true;
    GetPose(mDobotId, &mPose);
    ptpCmd.x = mOriginPose.x + x;
    ptpCmd.y = mOriginPose.y + y;
    ptpCmd.z = tempZ;

    while (SetPTPCmd(mDobotId, &ptpCmd, true, NULL) != DobotCommunicate_NoError) {
        return false;
    }

    while (isCheck) {
        Pose pose;
        GetPose(mDobotId, &pose);
        if (equalFloat(pose.x, ptpCmd.x) &&
            equalFloat(pose.y, ptpCmd.y) &&
            equalFloat(pose.z, ptpCmd.z) &&
            equalFloat(pose.r, ptpCmd.r)) isCheck = false;
    }

    // Step 3: current x, current y, z
    isCheck = true;

    ptpCmd.x = mOriginPose.x + x;
    ptpCmd.y = mOriginPose.y + y;
    ptpCmd.z = mOriginPose.z + z;

    while (SetPTPCmd(mDobotId, &ptpCmd, true, NULL) != DobotCommunicate_NoError) {
        return false;
    }

    while (isCheck) {
        Pose pose;
        GetPose(mDobotId, &pose);
        if (equalFloat(pose.x, ptpCmd.x) &&
            equalFloat(pose.y, ptpCmd.y) &&
            equalFloat(pose.z, ptpCmd.z) &&
            equalFloat(pose.r, ptpCmd.r)) isCheck = false;
    }
    return true;
}

void cWorker::setOriginalCordiante(int dobotId, Pose pose)
{
    mDobotId = dobotId;
    mOriginPose.x = pose.x;
    mOriginPose.y = pose.y;
    mOriginPose.z = pose.z;
    mOriginPose.r = pose.r;
}

void cWorker::setPortReader(QString portName)
{
    mPortReader = portName;
}

void cWorker::setFileName(QString fileName)
{
    mFileName = fileName;
}