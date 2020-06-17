#ifndef CWORKER_H
#define CWORKER_H

#include <QObject>
#include <QMutex>
#include <QFile>
#include <QIODevice>
#include <QWaitCondition>
#include <QTimer>
#include <QEventLoop>
#include <QThread>
#include <QDebug>
#include <QVariantMap>

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
        Stop = 2
    };
    /**
     * @brief Requests for the method @em method to be executed
     *
     * This method will defines #_method and set #_abort to interrupt current method.
     * It is thread safe as it uses #mutex to protect access to #_method and #_abort variable.
     */
    void requestMethod(Method method, QString filName);
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

    QVariantMap packageData(float x, float y, float z, QString function, QByteArray data, bool status);

};

#endif // CWORKER_H
