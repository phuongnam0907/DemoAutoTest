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
    moveToXYZ(0, 0, 0);
    mFile = new QFile(mFileName);
    QTextStream file(mFile);
    QString line;
    QString data;
    bool isDetected = false;
    float currentZ = 1;
    float percentOk = 0.0;
    mListResult.clear();
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

                            resultCode = mCardDetection->cardDetection(0x01, 0x01, 0x06, 0x06, 0x06, 0xFFFF, 100, &data);
                            if (resultCode == 0x00) countOk++;
                            mRFControl->rfReset();
                        }

                         percentOk = (float)countOk/(float)repeat;

                         if (percentOk >= 0.9) packageData(x, y, z + MAX_SIZE_Z, "Card Detection", data, true);
                         else packageData(x, y, z + MAX_SIZE_Z, "Card Detection", data, false);
                    }
                }
                currentZ = z;

                if (countOk != 0){
                    isDetected = true;
                }

                if (isDetected == true)
                {
                    cExportClass myClass;
                    myClass.setX((float)x/10);
                    myClass.setY((float)y/10);
                    myClass.setZ((float)(z + MAX_SIZE_Z)/10);
                    if (percentOk >= 0.9){
                        myClass.setResult(true);
                    } else {
                        myClass.setResult(false);
                    }
                    mListResult.append(myClass);
                }
            }
        }
    }
    mFile->close();
    moveToXYZ(0, 0, 0);

    emit finishTest();
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

void cWorker::doExport()
{
    exportReport();
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
        case Export:
            doExport();
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

    // Step 1: current x, current y, z + 5
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

    // Step 2: new x, new y, z + 5
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

void cWorker::setFileExportName(QString fileName)
{
    mFileExportName = fileName;
}

void cWorker::exportReport(){
    if (mListResult.isEmpty()){
        QFile exportFile(mFileExportName);
        if (exportFile.open(QIODevice::WriteOnly | QIODevice::Text)){
            QTextStream out(&exportFile);
            out << "Z,\"(0,0)\"" << endl;
            out << "0,F" << endl;
            exportFile.close();
        }
    } else {
        QStringList strList, strTitle;
        int i, cycle = 0;
        float firstZ = mListResult[0].getZ();
        QString fileName = mFileExportName;

        i = 0;
        while(true){
            if(firstZ == mListResult[i].getZ()) cycle++;
            else break;
            i++;
        }

        strTitle.append("Z");
        for (i = 0; i < cycle; i++) {
            QString strTemp = "\"(" + QString::number(mListResult[i].getX()) + ((mListResult[i].getX() != 0)?"cm":"") + "," +
                    QString::number(mListResult[i].getY()) + ((mListResult[i].getY() != 0)?"cm":"") + ")\"";
            strTitle.append(strTemp);
        }

        QFile exportFile(fileName);
        if (exportFile.open(QIODevice::WriteOnly | QIODevice::Text)){
            QTextStream out(&exportFile);
            out << strTitle.join(",") << endl;
            i = 0;
            while (i < mListResult.size())
            {
                if (mListResult[i].getResult()) strList.append("P");
                else strList.append("F");
                
                if (((firstZ != mListResult[i].getZ()) && ((i+1)%cycle == 0)) || ((i+1)%cycle == 0) || ((i+1) == mListResult.size())) {
                    out << mListResult[i].getZ() << "," << strList.join(",") << endl;
                    strList.clear();
                    firstZ = mListResult[i].getZ();
                }
                i++;
            }
            exportFile.close();
        }
    }
}
