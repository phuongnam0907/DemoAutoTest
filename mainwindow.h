#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
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

private:
    Ui::MainWindow *ui;
    QString mLogData;
    void updateLogData(float x, float y, float z, QString function, QString data, bool status);
    void updateLogFile(QString time, float x, float y, float z, QString function, QString data, bool status);
    void onConnection();
    bool connectStatus = false;
    QTimer *m_searchTime;
    QDateTime currentTime;
    QFile *tempFile;
    QString mFileName;
};

#endif // MAINWINDOW_H
