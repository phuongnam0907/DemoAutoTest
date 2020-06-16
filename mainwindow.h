#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QThread>
#include <QMessageBox>
#include "DobotDll.h"
#include "DobotType.h"

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

private:
    Ui::MainWindow *ui;
    QString mLogData;
    void updateLogData(float x, float y, float z, QString function, QString data, bool status);
    void updateLogFile(QString time, float x, float y, float z, QString function, QString data, bool status);
    void onConnection();
    void initDobot();

    bool connectStatus = false;
    QTimer *m_searchTime;
    QDateTime currentTime;
    QFile *tempFile;
    QString mFileName;
    char mList[100];
    int dobotId;
    bool equalFloat(float f1, float f2);
    bool moveToXYZ(float x, float y, float z);

    QString getCurrentTime();
    Pose *mPose;
};

#endif // MAINWINDOW_H
