#include "cWorker.h"

cWorker::cWorker(QObject *parent) : QObject(parent)
{
    _abort = false;
    _interrupt = false;
}

void cWorker::requestMethod(cWorker::Method method, QString filName)
{
    QMutexLocker locker(&mutex);
    _interrupt = true;
    _method = method;
    mFileName = filName;
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
    while (true) {
        mutex.lock();
        bool abort = _abort;
        bool interrupt = _interrupt;
        mutex.unlock();

        if (abort || interrupt) break;

//        if (mCommunicationInterfaces->openPort(mPortName, mBaudrate) == 0) mIsOpen = true;
//        else mIsOpen = false;
//        isOpenPort = true;
//        emit on_signal_isOpenPort(mIsOpen);
    }
}

void cWorker::doStop()
{

}

void cWorker::mainLoop()
{
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
        }
    }
}

QVariantMap cWorker::packageData(float x, float y, float z, QString function, QByteArray data, bool status)
{
    QVariantMap map;
    map.insert("x", x);
    map.insert("y", y);
    map.insert("z", z);
    map.insert("function", function);
    map.insert("data", data);
    map.insert("pass", status);
    return map;
}
