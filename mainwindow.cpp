#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->comboBox_2->addItem("115200");

    m_searchTime = new QTimer(this);
    connect(m_searchTime, SIGNAL(timeout()), this, SLOT(searchPortDobot()));
    m_searchTime->setSingleShot(true);
    m_searchTime->setInterval(1);
    m_searchTime->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_connectButton_clicked()
{
    if (!connectStatus) {
        ui->connectButton->setText("Disconnect");
        connectStatus = true;
        updateLogData(0,0,0,"Disconnect",connectStatus);


    }
    else {
        ui->connectButton->setText("Connect");
        connectStatus = false;
        updateLogData(0,0,0,"Connect",connectStatus);
    }
}

void MainWindow::searchPortDobot()
{
//    char lis[100];
//    SearchDobot(lis, 100);
//    ui->lbSearchResult->setText(QString(lis));
//    QStringList comList = QString(lis).split(" ");
//    if(QString(lis).isEmpty() || comList.size() < 2){
//        m_searchTime->start();
//        return;
//    }

//    qDebug() << "searchDobot1" << comList;

//    for (int i = 0; i < comList.size(); ++i)
//    {
//        ConnectTest *ttttt = new ConnectTest();
//        qDebug() << comList.at(i);
//        ttttt->setCom(comList.at(i));
//        QThread *trhd = new QThread();
//        trhd->start();
//        ttttt->moveToThread(trhd);
//        QMetaObject::invokeMethod(ttttt, "onStart", Qt::QueuedConnection);
//    }
    ui->comboBox->addItem("COM22");
}

void MainWindow::updateLogData(float x, float y, float z, QString function, bool status){
    currentTime = QDateTime::currentDateTime();
    QString Time = QString("%1").arg(currentTime.time().hour(), 2, 10, QChar('0')) + ":"
            + QString("%1").arg(currentTime.time().minute(), 2, 10, QChar('0')) + ":"
            + QString("%1").arg(currentTime.time().second(), 2, 10, QChar('0')) + ":"
            + QString("%1").arg(currentTime.time().msec(), 3, 10, QChar('0'));
    if (mLogData.size() > 40000) mLogData.clear();
    mLogData += "<span><font size=4><font color='blue'><b>" + Time + "</b></font></font></span>" + "<br>";
    mLogData += "X: " + QString::number(x) + " | Y: " + QString::number(y) + " | Z: " + QString::number(z) + " | " + function + ": ";
    if(status) mLogData += "<span><font color='green'><b>PASSED</b></font></span><br>";
    else mLogData += "<span><font color='red'><b>FAIL</b></font></span><br>";
    ui->textEdit->setText(mLogData);
    ui->textEdit->moveCursor(QTextCursor::End);
}

void MainWindow::updateLogFile(float x, float y, float z, QString function, bool status)
{

}

void MainWindow::onConnection()
{
    char fwType[20];
    char version[20];

    //connect dobot
//    if (!connectStatus) {
//        if (ConnectDobot( ui->combox1->currentText().toLatin1().data(), 115200, fwType, version, &dobotId) != DobotConnect_NoError) {
//            QMessageBox::information(this, tr("error"), tr("Connect dobot error!!!"), QMessageBox::Ok);
//            return;
//        }
//        qDebug() << "dobotId" << dobotId;
//        connectStatus = true;
//        refreshBtn();
//        initDobot();

//        QTimer *getPoseTimer = findChild<QTimer *>("getPoseTimer");
//        getPoseTimer->start(200);
//        qDebug() << "connect success!!!";
//    } else {
//        QTimer *getPoseTimer = findChild<QTimer *>("getPoseTimer");
//        getPoseTimer->stop();
//        connectStatus = false;
//        refreshBtn();
//        DisconnectDobot(dobotId);
//    }
}
