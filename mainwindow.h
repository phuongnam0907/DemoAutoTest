#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QTimer>
#include <QDateTime>

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

private:
    Ui::MainWindow *ui;
    QString mLogData;
    void updateLogData(float x, float y, float z, QString function, bool status);
    void updateLogFile(float x, float y, float z, QString function, bool status);
    void onConnection();
    bool connectStatus = false;
    QTimer *m_searchTime;
    QDateTime currentTime;
};

#endif // MAINWINDOW_H
