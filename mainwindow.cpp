#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#define LOG_FILE_NAME   "temp_logdata_styl.txt"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->comboBox_2->addItem("115200");

    m_searchTime = new QTimer(this);
    connect(m_searchTime, SIGNAL(timeout()), this, SLOT(searchPortDobot()));
    m_searchTime->setSingleShot(true);
    m_searchTime->setInterval(1000);
    m_searchTime->start();

    tempFile = new QFile(LOG_FILE_NAME);


}

MainWindow::~MainWindow()
{
    tempFile->remove();
    delete ui;
}
void MainWindow::on_connectButton_clicked()
{
    onConnection();
    if (connectStatus) {
        ui->connectButton->setText("Disconnect");
        updateLogData(0,0,0,"Disconnect","Disconnect",connectStatus);
        ui->comboBox->setEnabled(false);
        ui->comboBox_2->setEnabled(false);
    }
    else {
        ui->connectButton->setText("Connect");
        updateLogData(0,0,0,"Connect","Connect",connectStatus);
        ui->comboBox->setEnabled(true);
        ui->comboBox_2->setEnabled(true);
    }
}

void MainWindow::searchPortDobot(void)
{
    char lis[100];
    SearchDobot(lis, 100);
    if (strcmp(mList,lis) != 0) {
        strcpy(mList,lis);
        QStringList comList = QString(lis).split(" ");
        ui->comboBox->clear();
        for (int i = 0; i < comList.size(); ++i) ui->comboBox->addItem(comList.at(i));
    }
    m_searchTime->start();

}

void MainWindow::updateLogData(float x, float y, float z, QString function, QString data, bool status)
{
    QString Time = getCurrentTime();
    if (mLogData.size() > 40000) mLogData.clear();
    mLogData += "<span><font size=4><font color='blue'><b>" + Time + "</b></font></font></span>" + "<br>";
    mLogData += "X: " + QString::number(x) + " | Y: " + QString::number(y) + " | Z: " + QString::number(z) + " | " + function + ": " + data + "<br>";
//    if(status) mLogData += "<span><font color='green'><b>PASSED</b></font></span><br>";
//    else mLogData += "<span><font color='red'><b>FAIL</b></font></span><br>";
    ui->textEdit->setText(mLogData);
    ui->textEdit->moveCursor(QTextCursor::End);
    updateLogFile(Time, x, y, z, function, data, status);
}

void MainWindow::updateLogFile(QString time, float x, float y, float z, QString function, QString data, bool status)
{
    QTextStream out(tempFile);
    QString firstData = time + " X:" + QString::number(x) + " Y:" + QString::number(y) + " Z:" + QString::number(z) + " " + function + " " + data;
//    if (status) firstData += " PASSED";
//    else firstData += " FAILED";
    if(tempFile->open(QIODevice::ReadWrite)){
        out.readAll();
        out << firstData << endl;
    }
    tempFile->close();
}

void MainWindow::onConnection()
{
    char fwType[20];
    char version[20];

    //connect dobot
    if (!connectStatus) {
        if (ConnectDobot(ui->comboBox->currentText().toLatin1().data(), 115200, fwType, version, &dobotId) != DobotConnect_NoError) {
            QMessageBox::information(this, tr("Error"), tr("Connect dobot error!!!"), QMessageBox::Ok);
            return;
        }

        qDebug() << "dobotId" << dobotId;
        connectStatus = true;
        initDobot();

        qDebug() << "connect success!!!";
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

bool MainWindow::equalFloat(float f1, float f2)
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

bool MainWindow::moveToXYZ(float x, float y, float z)
{
    fasd
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
    QFile::copy(LOG_FILE_NAME, fileName);
    QMessageBox msg;
    msg.setWindowTitle("Save file log");

    msg.setText("Save file successfully!!!");
    msg.exec();
}

void MainWindow::on_loadButton_clicked()
{
    QString filter = "Text (*.txt)";
    mFileName = QFileDialog::getOpenFileName(this, "Open file", QDir::homePath(), filter);
    ui->textEdit_2->setText(mFileName);
}

void MainWindow::on_setButton_clicked()
{
    Pose pose;
    GetPose(dobotId, &pose);
    mPose->x = pose.x;
    mPose->y = pose.y;
    mPose->z = pose.z;
    mPose->r = pose.r;
    updateLogData(pose.x, pose.y, pose.z, "Get Data", "Get Data", true);
    updateLogData(mPose->x, mPose->y, mPose->z, "Set Data", "Set Data", true);
}

void MainWindow::on_startButton_clicked()
{
    float abc = 40;
    float cba = 30;
    bool isCheck = true;
    while (true) {
        isCheck = true;

        PTPCmd ptpCmd;
        ptpCmd.ptpMode = PTPMOVJXYZMode;
        ptpCmd.x = mPose->x + abc;
        ptpCmd.y = mPose->y;
        ptpCmd.z = mPose->z;
        ptpCmd.r = mPose->r;
        qDebug() << "debug number 1";

        while (SetPTPCmd(dobotId, &ptpCmd, true, NULL) != DobotCommunicate_NoError) {}

        while (isCheck) {
            Pose pose;
            GetPose(dobotId, &pose);
            if (equalFloat(pose.x, ptpCmd.x) &&
                equalFloat(pose.x, ptpCmd.x) &&
                equalFloat(pose.x, ptpCmd.x) &&
                equalFloat(pose.x, ptpCmd.x)) isCheck = false;
        }
        qDebug() << "debug number 4";
        abc *= -1;

        isCheck = true;

        PTPCmd xptpCmd;
        xptpCmd.ptpMode = PTPMOVJXYZMode;
        xptpCmd.x = ptpCmd.x;
        xptpCmd.y = mPose->y;
        xptpCmd.z = mPose->z + cba;
        xptpCmd.r = mPose->r;
        qDebug() << "debug number 1";

        while (SetPTPCmd(dobotId, &xptpCmd, true, NULL) != DobotCommunicate_NoError) {}

        while (isCheck) {
            Pose pose;
            GetPose(dobotId, &pose);
            if (equalFloat(pose.x, xptpCmd.x) &&
                equalFloat(pose.x, xptpCmd.x) &&
                equalFloat(pose.x, xptpCmd.x) &&
                equalFloat(pose.x, xptpCmd.x)) isCheck = false;
        }
        qDebug() << "debug number 4";
        cba *= -1;
    }
}

void MainWindow::on_stopButton_clicked()
{

}
