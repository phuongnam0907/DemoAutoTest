#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#define LOG_DETAIL_FILE_NAME   "temp_logdata_styl.txt"
#define LOG_SUMMARY_FILE_NAME  "summary_logdata_styl.txt"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("Automation Test");

    ui->comboBox_2->addItem("115200");
    ui->comboBox_3->addItem("115200");

    m_searchTime = new QTimer(this);
    connect(m_searchTime, SIGNAL(timeout()), this, SLOT(searchPortDobot()));
    m_searchTime->setSingleShot(true);
    m_searchTime->setInterval(1000);
    m_searchTime->start();

    tempFile = new QFile(LOG_DETAIL_FILE_NAME);
    sumFile = new QFile(LOG_SUMMARY_FILE_NAME);

    worker = new cWorker();
    thread = new QThread();

    worker->moveToThread(thread);
    connect(thread, SIGNAL(started()), worker, SLOT(mainLoop()));
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()), Qt::DirectConnection);
    thread->start();

    connect(worker, SIGNAL(valueChanged(QVariantMap)), this, SLOT(on_slot_receiveResult(QVariantMap)));
    isConnectUI();
}

MainWindow::~MainWindow()
{
    tempFile->remove();
    sumFile->remove();
    worker->abort();
    thread->wait();
    delete thread;
    delete worker;
    delete ui;
}
void MainWindow::on_connectButton_clicked()
{
    onConnection();
    if (connectStatus) {
        ui->connectButton->setText("Disconnect");
        updateLogData(-999,-999,-999,"Connect Dobot Port " + mCurrentPortDobot,"",connectStatus);
    }
    else {
        ui->connectButton->setText("Connect");
        updateLogData(-999,-999,-999,"Disconnect Dobot Port " + mCurrentPortDobot,"",!connectStatus);
    }
    isConnectUI();
}

void MainWindow::searchPortDobot(void)
{
    char lis[100];
    SearchDobot(lis, 100);
    if (strcmp(mList,lis) != 0) {
        strcpy(mList,lis);
        QStringList comList = QString(lis).split(" ");
        ui->comboBox->clear();
        ui->comboBox_4->clear();
        for (int i = 0; i < comList.size(); ++i) ui->comboBox->addItem(comList.at(i));
        for (int i = 0; i < comList.size(); ++i) ui->comboBox_4->addItem(comList.at(i));
    }
    m_searchTime->start();

}

void MainWindow::updateLogData(float x, float y, float z, QString function, QString data, bool status)
{
    QString Time = getCurrentTime();
    if (mLogData.size() > 40000) mLogData.clear();
    mLogData += "<span><font size=4><font color='blue'><b>" + Time + "</b></font></font></span>" + "<br>";
    if ((x <= -999) || (y <= -999) || (z <= -999)) mLogData += function + " ==> ";
    else mLogData += "X: " + QString::number(x) + " | Y: " + QString::number(y) + " | Z: " + QString::number(z) + " | " + function + " ==> ";
    if(status) mLogData += "<span><font color='green'><b>PASSED</b></font></span><br>";
    else mLogData += "<span><font color='red'><b>FAILED</b></font></span><br>";
    if (!data.isEmpty()) mLogData += "<b>Data:</b> " + data + "<br>";
    ui->textEdit->setText(mLogData);
    ui->textEdit->moveCursor(QTextCursor::End);
    updateLogFile(Time, x, y, z, function, data, status);
    updateLogSummary(Time, function, status);
}

void MainWindow::updateLogFile(QString time, float x, float y, float z, QString function, QString data, bool status)
{
    QTextStream out(tempFile);
    QString firstData;
    if ((x <= -999) || (y <= -999) || (z <= -999)) firstData = time + " " + function;
    else firstData = time + " X:" + QString::number(x) + " Y:" + QString::number(y) + " Z:" + QString::number(z) + " " + function + " " + data;
    if (status) firstData += " PASSED";
    else firstData += " FAILED";
    if(tempFile->open(QIODevice::ReadWrite)){
        out.readAll();
        out << firstData << endl;
    }
    tempFile->close();
}

void MainWindow::updateLogSummary(QString time, QString function, bool status)
{
    QTextStream out(sumFile);
    QString firstData = time + " - " + function;
    if (status) firstData += " - PASSED";
    else firstData += " - FAILED";
    if(sumFile->open(QIODevice::ReadWrite)){
        out.readAll();
        out << firstData << endl;
    }
    sumFile->close();
}

void MainWindow::onConnection()
{
    char fwType[20];
    char version[20];
    mCurrentPortDobot = ui->comboBox->currentText();
    //connect dobot
    if (!connectStatus) {
        if (ConnectDobot(ui->comboBox->currentText().toLatin1().data(), 115200, fwType, version, &dobotId) != DobotConnect_NoError) {
            QMessageBox::information(this, tr("Error"), tr("Connect dobot error!!!"), QMessageBox::Ok);
            return;
        }
        connectStatus = true;
        initDobot();
    } else {
        connectStatus = false;
        DisconnectDobot(dobotId);
    }
}

void MainWindow::initDobot()
{
    //Command timeout
    SetCmdTimeout(dobotId, 3000);
    //clear old commands and set the queued command running
    SetQueuedCmdClear(dobotId);
    SetQueuedCmdStartExec(dobotId);

    char deviceSN[64];
    GetDeviceSN(dobotId, deviceSN, sizeof(deviceSN));
//    ui->deviceSNLabel->setText(deviceSN);
    qDebug() << "deviceSN: " << deviceSN;

    char deviceName[64];
    GetDeviceName(dobotId, deviceName, sizeof(deviceName));
//    ui->DeviceNameLabel->setText(deviceName);
    qDebug() << "deviceName: " << deviceName;

    uint8_t majorVersion, minorVersion, revision;
    GetDeviceVersion(dobotId, &majorVersion, &minorVersion, &revision);
//    ui->DeviceInfoLabel->setText(QString::number(majorVersion) + "." + QString::number(minorVersion) + "." + QString::number(revision));
    qDebug() << "version: " << QString::number(majorVersion) + "." + QString::number(minorVersion) + "." + QString::number(revision);

    QTextStream out(tempFile);
    if(tempFile->open(QIODevice::WriteOnly)){
        out << "Serial Number: " << deviceSN << endl;
        out << "Device Name: " << deviceName << endl;
        out << "Version: " << QString::number(majorVersion) + "." + QString::number(minorVersion) + "." + QString::number(revision) << endl;
        ui->textEdit->clear();
        mLogData = "Serial Number: <b>" + QString::fromLatin1(deviceSN) + "</b><br>";
        mLogData += "Device Name: <b>" +  QString::fromLatin1(deviceName) + "</b><br>";
        mLogData += "Version: <b>" + QString::number(majorVersion) + "." + QString::number(minorVersion) + "." + QString::number(revision) + "</b><br>";
        mLogData += "================================<br>";
    }
    tempFile->close();

    //set the end effector parameters
    EndEffectorParams endEffectorParams;
    memset(&endEffectorParams, 0, sizeof(endEffectorParams));
    endEffectorParams.xBias = 71.6f;
    SetEndEffectorParams(dobotId, &endEffectorParams, false, NULL);

    JOGJointParams jogJointParams;
    for (int i = 0; i < 4; i++) {
        jogJointParams.velocity[i] = 100;
        jogJointParams.acceleration[i] = 100;
    }
    SetJOGJointParams(dobotId, &jogJointParams, false, NULL);

    JOGCoordinateParams jogCoordinateParams;
    for (int i = 0; i < 4; i++) {
        jogCoordinateParams.velocity[i] = 100;
        jogCoordinateParams.acceleration[i] = 100;
    }
    SetJOGCoordinateParams(dobotId, &jogCoordinateParams, false, NULL);

    JOGCommonParams jogCommonParams;
    jogCommonParams.velocityRatio = 50;
    jogCommonParams.accelerationRatio = 50;
    SetJOGCommonParams(dobotId, &jogCommonParams, false, NULL);

    PTPJointParams ptpJointParams;
    for (int i = 0; i < 4; i++) {
        ptpJointParams.velocity[i] = 100;
        ptpJointParams.acceleration[i] = 100;
    }
    SetPTPJointParams(dobotId, &ptpJointParams, false, NULL);

    PTPCoordinateParams ptpCoordinateParams;
    ptpCoordinateParams.xyzVelocity = 100;
    ptpCoordinateParams.xyzAcceleration = 100;
    ptpCoordinateParams.rVelocity = 100;
    ptpCoordinateParams.rAcceleration = 100;
    SetPTPCoordinateParams(dobotId, &ptpCoordinateParams, false, NULL);

    PTPJumpParams ptpJumpParams;
    ptpJumpParams.jumpHeight = 20;
    ptpJumpParams.zLimit = 150;
    SetPTPJumpParams(dobotId, &ptpJumpParams, false, NULL);
}

void MainWindow::isConnectUI()
{
    ui->groupBox_2->setEnabled(!connectReader);
    ui->groupBox->setEnabled(connectStatus);
    ui->comboBox->setEnabled(!connectStatus);
    ui->comboBox_2->setEnabled(!connectStatus);
    ui->comboBox_3->setEnabled(!connectReader);
    ui->comboBox_4->setEnabled(!connectReader);
    ui->loadButton->setEnabled(connectStatus && connectReader);
    ui->setButton->setEnabled(connectStatus && connectReader);
    ui->label_2->setEnabled(!connectStatus);
    ui->label_3->setEnabled(!connectStatus);
    ui->label_4->setEnabled(!connectReader);
    ui->label_5->setEnabled(!connectReader);
    ui->textEdit_2->setEnabled(connectStatus && connectReader);
    ui->startButton->setEnabled(connectStatus && connectReader);
    ui->stopButton->setEnabled(connectStatus && connectReader);
    if (!(ui->textEdit_2->toPlainText().isEmpty()) && connectStatus && connectReader) ui->groupBox_4->setEnabled(true);
    else ui->groupBox_4->setEnabled(false);
}

QString MainWindow::getCurrentTime()
{
    currentTime = QDateTime::currentDateTime();
    QString Time = QString("%1").arg(currentTime.time().hour(), 2, 10, QChar('0')) + ":"
            + QString("%1").arg(currentTime.time().minute(), 2, 10, QChar('0')) + ":"
            + QString("%1").arg(currentTime.time().second(), 2, 10, QChar('0')) + ":"
            + QString("%1").arg(currentTime.time().msec(), 3, 10, QChar('0'));
    return Time;
}

void MainWindow::on_saveButton_clicked()
{
    QString filter = "Text (*.txt)";
    QString fileName = QFileDialog::getSaveFileName(this, "Save file", QDir::homePath(), filter);
    if(QFile::copy(LOG_DETAIL_FILE_NAME, fileName)){
        QMessageBox msg;
        msg.setWindowTitle("Save file log");
        msg.setText("Save file successfully!!!");
        msg.exec();
    }
}

void MainWindow::on_loadButton_clicked()
{
    QString filter = "Text (*.txt)";
    mFileName = QFileDialog::getOpenFileName(this, "Open file", QDir::homePath(), filter);
    ui->textEdit_2->setText(mFileName);
    isConnectUI();
}

void MainWindow::on_setButton_clicked()
{
    Pose pose;
    GetPose(dobotId, &pose);
    mPose->x = pose.x;
    mPose->y = pose.y;
    mPose->z = pose.z;
    mPose->r = pose.r;
    updateLogData(mPose->x, mPose->y, mPose->z, "Set Original Cordinate", "", true);
    worker->setOriginalCordiante(dobotId, pose);
}

void MainWindow::on_startButton_clicked()
{
    worker->setFileName(mFileName);
    worker->requestMethod(cWorker::Start);
}

void MainWindow::on_stopButton_clicked()
{
    worker->requestMethod(cWorker::Stop);
}

void MainWindow::on_slot_receiveResult(QVariantMap map)
{
    float x = map.value("x").toFloat();
    float y = map.value("y").toFloat();
    float z = map.value("z").toFloat();
    QString functionName = map.value("function").toString();
    QString data = map.value("data").toString();
    bool isPass = map.value("pass").toBool();

    updateLogData(x, y, z, functionName, data, isPass);
}

void MainWindow::on_portButton_clicked()
{
    if (!connectReader) {
        connectReader = true;
        ui->portButton->setText("Disconnect");
        worker->setPortReader(ui->comboBox_4->currentText());
        worker->requestMethod(cWorker::Open);
    }
    else {
        connectReader = false;
        worker->requestMethod(cWorker::Close);
        ui->portButton->setText("Connect");
    }
    isConnectUI();
}

void MainWindow::on_generalButton_clicked()
{
    QString filter = "Text (*.txt)";
    QString fileName = QFileDialog::getSaveFileName(this, "Save file", QDir::homePath(), filter);
    if(QFile::copy(LOG_SUMMARY_FILE_NAME, fileName)){
        QMessageBox msg;
        msg.setWindowTitle("Save file log");
        msg.setText("Save file successfully!!!");
        msg.exec();
    }
}
