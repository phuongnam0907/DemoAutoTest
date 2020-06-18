#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QVariantMap>
#include <QTimer>
#include <QDateTime>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QThread>
#include <QMessageBox>
#include "cConvert.h"
#include "DobotDll.h"
#include "DobotType.h"
#include "cWorker.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_connectButton_clicked();
    void searchPortDobot(void);

    void on_saveButton_clicked();

    void on_loadButton_clicked();

    void on_setButton_clicked();

    void on_startButton_clicked();

    void on_stopButton_clicked();

    void on_slot_receiveResult(QVariantMap map);
    void on_portButton_clicked();

private:
    Ui::MainWindow *ui;
    QString mLogData;
    void updateLogData(float x, float y, float z, QString function, QString data, bool status);
    void updateLogFile(QString time, float x, float y, float z, QString function, QString data, bool status);
    void onConnection();
    void initDobot();
    void isConnectUI();

    bool connectStatus = false;
    bool connectReader = false;
    QTimer *m_searchTime;
    QDateTime currentTime;
    QFile *tempFile;
    QString mFileName;
    char mList[100];
    int dobotId;

    QString getCurrentTime();
    Pose *mPose;
    /**
     * @brief Thread object which will let us manipulate the running thread
     */
    QThread *thread;
    /**
     * @brief Object which contains methods that should be runned in another thread
     */
    cWorker* worker;
};

#endif // MAINWINDOW_H
