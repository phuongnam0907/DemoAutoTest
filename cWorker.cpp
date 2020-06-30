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
    _abort = true;
    condition.wakeOne();
}

void cWorker::doStart()
{
    mFile = new QFile(mFileName);
    QTextStream file(mFile);
    QString line;
    QString data;
    bool isDetected = false;
    float currentZ = 1;
    float percentOk = 0.0;
    if (mFile->open(QIODevice::ReadOnly)) {
        while (!file.atEnd()) {
            mutex.lock();
            bool abort = _abort;
            bool interrupt = _interrupt;
            mutex.unlock();

            if (abort || interrupt) break;

            data = "";
            line = file.readLine(30);
            QStringList list = line.split(",");
            if(list.size() == 4)
            {
                quint8 resultCode = FAIL;
                float x = list.at(0).toFloat();
                float y = list.at(1).toFloat();
                float z = list.at(2).toFloat();
                int loopTimes = list.at(3).toInt();
                int repeat = loopTimes;
                int countOk = 0;

                if ((currentZ != z) || (isDetected == true)) {
                    if (moveToXYZ(x, y, z)) {
                        while ((loopTimes--)) {
                            mutex.lock();
                            bool abort = _abort;
                            bool interrupt = _interrupt;
                            mutex.unlock();

                            if (abort || interrupt) break;

                            resultCode = mCardDetection->cardDetection(0x03, 0x01, 0x06, 0x06, 0x06, 0xFFFF, 100, &data);
                            if (resultCode == 0x00) countOk++;
                            mRFControl->rfReset();
                        }

                        percentOk = (float)countOk/(float)repeat;

                        if (percentOk < 0.5) packageData(x, y, z, "Card Detection", data, true);
                        else packageData(x, y, z, "Card Detection", data, false);
                    }
                }
                currentZ = z;

                if (countOk != 0){
                    isDetected = true;
                }

            }
        }
    }
    mFile->close();
    moveToXYZ(0, 0, 0);

    emit finishTest();
    //    QFile fi(desktopPath + "/AutoTestReport_123.csv");
    //    if (fi.open(QIODevice::WriteOnly | QIODevice::Text)){
    //        QTextStream out(&fi);
    //        out << "hello112321";
    //        fi.close();
    //    }

}

void cWorker::doStop()
{
    if (mFile->isOpen()) mFile->close();
}

void cWorker::doOpen()
{
    quint8 resultCode = FAIL;

    resultCode = rfcapi_Init(portname, 0x02);

    packageData(-999,-999,-999, "Open Port Reader " + mPortReader, "", true);
}

void cWorker::doClose()
{
    quint8 resultCode = FAIL;

    resultCode = rfcapi_Deinit(portname);

    packageData(-999,-999,-999, "Close Port Reader " + mPortReader, "", true);
}

void cWorker::mainLoop()
{
    mCardDetection = new cCardDetection();
    mRFControl = new cRFControl();
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

void cWorker::packageData(float x, float y, float z, QString function, QString data, bool status)
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

    // Step 1: current x, current y, z + 20
    isCheck = true;
    GetPose(mDobotId, &mPose);
    ptpCmd.x = mPose.x;
    ptpCmd.y = mPose.y;
    ptpCmd.z = mPose.z + 5;
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

    QString PortName = SERIAL_PREFIX + mPortReader;
    static QByteArray sArray;
    sArray = PortName.toLatin1();
    portname = sArray.data();
}

void cWorker::setFileName(QString fileName)
{
    mFileName = fileName;
}
