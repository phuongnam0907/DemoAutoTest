#ifndef CWORKER_H
#define CWORKER_H

#include <QObject>
#include <QMutex>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QStringList>
#include <QWaitCondition>
#include <QTimer>
#include <QEventLoop>
#include <QThread>
#include <QDebug>
#include <QVariantMap>
#include <cCardDetection.h>
#include <rfcapiinit.h>
#include "DobotType.h"
#include "DobotDll.h"

#define SERIAL_PREFIX   "\\\\.\\"
#define FAIL            0xFF
#define OK              0x00

class cWorker : public QObject
{
    Q_OBJECT

public:
    explicit cWorker(QObject *parent = 0);
    /**
     * @brief This enum describes the various available methods
     */
    enum Method {
        Start = 1,
        Stop = 2,
        Open = 3,
        Close = 4
    };
    /**
     * @brief Requests for the method @em method to be executed
     *
     * This method will defines #_method and set #_abort to interrupt current method.
     * It is thread safe as it uses #mutex to protect access to #_method and #_abort variable.
     */
    void requestMethod(Method method);
    /**
     * @brief Requests the process to abort
     *
     * It is thread safe as it uses #mutex to protect access to #_abort variable.
     */
    void abort();
private:
    /**
     * @brief Currently requested method
     */
    Method _method;
    /**
     * @brief Process is aborted when @em true
     */
    bool _abort;
    /**
     * @brief Current method is interrupted when @em true
     */
    bool _interrupt;
    /**
     * @brief Protects access to #_abort
     */
    QMutex mutex;
    /**
     * @brief Condition used to wait for a new request to be called in the #mainLoop()
     */
    QWaitCondition condition;
    /**
     * @brief 3rd method which could be called
     *
     * Counts 30 sec in this example.
     * Counting is interrupted if #_abort or #_interrupt is set to true.
     */
    void doStart();
    /**
     * @brief 3rd method which could be called
     *
     * Counts 30 sec in this example.
     * Counting is interrupted if #_abort or #_interrupt is set to true.
     */
    void doStop();
    /**
     * @brief 3rd method which could be called
     *
     * Counts 30 sec in this example.
     * Counting is interrupted if #_abort or #_interrupt is set to true.
     */
    void doOpen();
    /**
     * @brief 3rd method which could be called
     *
     * Counts 30 sec in this example.
     * Counting is interrupted if #_abort or #_interrupt is set to true.
     */
    void doClose();

signals:
    /**
     * @brief This signal is emitted when counted value is changed (every sec)
     */
    void valueChanged(QVariantMap map);
    /**
     * @brief This signal is emitted when process is finished (either by counting 60 sec or being aborted)
     */
    void finished();
public slots:
    /**
     * @brief Waits for a method to be requested then executes it, then waits again
     */
    void mainLoop();

private:

    QString mFileName;
    int mDobotId;
    Pose mOriginPose;
    QFile *mFile;
    cCardDetection *mCardDetection;
    QString mPortReader;
    char *portname;

    void packageData(float x, float y, float z, QString function, QString data, bool status);

    bool equalFloat(float f1, float f2);

    bool moveToXYZ(float x, float y, float z);

public:
    void setOriginalCordiante(int dobotId, Pose pose);
    void setPortReader(QString portName);
    void setFileName(QString fileName);
};

#endif // CWORKER_H
