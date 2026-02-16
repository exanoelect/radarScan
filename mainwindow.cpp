#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtMultimedia/QSoundEffect>
#include <QUrl>
#include <QTime>
#include <QElapsedTimer>
#include <QSharedPointer>
#include "configmanager.h"

// Jika qcustomplot butuh include spesifik, sudah di header

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initSound();
    initGraphics();
    initSocketIO();
    initRadar();

    m_gpio = new gpio();
    m_gpio->setupGPIO();
    m_gpio->setColor(COLOR_WHITE);

    m_volume = new volume();
    m_brightness = new brightness();
}

//---------------------------------------------------------------------------------------
MainWindow::~MainWindow()
{

    if (m_serial) {
        if (m_serial->isOpen()) m_serial->close();
        m_serial->deleteLater();
        m_serial = nullptr;
    }

    delete ui;
}

//---------------------------------------------------------------------------------------
void MainWindow::initSound()
{
    m_audioThread = new QThread(this);
    m_audioWorker = new AudioWorker();
    m_audioWorker->moveToThread(m_audioThread);

    connect(m_audioThread, &QThread::started,
            m_audioWorker, &AudioWorker::init);   // <-- penting!

    connect(m_audioThread, &QThread::finished,
            m_audioWorker, &QObject::deleteLater);

    m_audioThread->start();
}

//---------------------------------------------------------------------------------------
void MainWindow::initSocketIO()
{
    client = new SocketIOClient();

    connect(client, &SocketIOClient::eventReceived,
            this,   &MainWindow::onSocketEventReceived);

    //connect(client, &SocketIOClient::deviceready,
    //        this,   &SocketIOClient:sendDeviceReady);

    // Worker setup
    m_workerThread = new QThread(this);
    m_worker = new SocketEventWorker();
    m_worker->moveToThread(m_workerThread);

    connect(m_workerThread, &QThread::started,
            m_worker, &SocketEventWorker::process);

    // Hubungkan signal worker ke aksi UI / device
    connect(m_worker, &SocketEventWorker::listenStateChanged,
            this, &MainWindow::onListenStateChanged);
    connect(m_worker, &SocketEventWorker::talkingStateChanged,
            this, &MainWindow::onTalkingStateChanged);
    connect(m_worker, &SocketEventWorker::volumeGetRequested,
            this, &MainWindow::onVolumeGetRequested);
    connect(m_worker, &SocketEventWorker::volumeSetRequested,
            this, &MainWindow::onVolumeSetRequested);
    connect(m_worker, &SocketEventWorker::pingDeviceUpRequested,
            this, &MainWindow::onPingDeviceUpRequested);
    connect(m_worker, &SocketEventWorker::sleepRequested,
            this, &MainWindow::onSleepRequested);
    connect(m_worker, &SocketEventWorker::brightnessSetRequested,
            this, &MainWindow::onBrightnessSetRequested);

    //Add on
    connect(m_worker, &SocketEventWorker::volumeIncreaseReq,
            this, &MainWindow::onVolumeIncreaseReq);
    connect(m_worker, &SocketEventWorker::volumeDecreaseReq,
            this, &MainWindow::onVolumeDecreaseReq);
    connect(m_worker, &SocketEventWorker::brightnessIncreaseReq,
            this, &MainWindow::onBrihtnessIncreaseReq);
    connect(m_worker, &SocketEventWorker::brightnessDecreaseReq,
            this, &MainWindow::onBrightnessDecreaseReq);
    connect(m_worker, &SocketEventWorker::brightnessGetRequested,
            this, &MainWindow::onBrightnessGetRequested);

    //Sound
    connect(m_worker, &SocketEventWorker::incidentFall,
            this, &MainWindow::onIncidentFallOccur);
    connect(m_worker, &SocketEventWorker::incidentHelp,
            this, &MainWindow::onIncidentIamnotOK);
    connect(m_worker, &SocketEventWorker::incidentIamOK,
            this, &MainWindow::onIncidentIamOK);
    connect(m_worker, &SocketEventWorker::incidentIamnotOK,
            this, &MainWindow::onIncidentIamnotOK);


    m_workerThread->start();

    QString serverIp = ConfigManager::getServerIp();
    int serverPort = ConfigManager::getServerPort();

    qDebug() << "Server IP:" << serverIp;
    qDebug() << "Server Port:" << serverPort;

    //client->connectToServer("192.168.1.27", 3000);
    client->connectToServer(serverIp, serverPort);
}

//---------------------------------------------------------------------------------------
void MainWindow::initGraphics()
{
    setupRealtimeDataMotion(ui->plottsgram);
    setupRealtimeDataVelocity(ui->plottsVelocity);
    setupPlotRadar(ui->plotRadar);

    setupRealtimeDataMotion2(ui->plottsgram2);
    setupRealtimeDataVelocity2(ui->plottsVelocity2);
    setupPlotRadar2(ui->plotRadar2);
}


//---------------------------------------------------------------------------------------
//#ifdef PLATFORM_LINUX
void MainWindow::initRadar()
{    
    // === Worker A ===
    m_threadA = new QThread(this);
    m_procA = new PayloadProcessor(UART_PORT0);
    m_procA->moveToThread(m_threadA);

    // === Worker B ===
    m_threadB = new QThread(this);
    m_procB = new PayloadProcessor(UART_PORT1);
    m_procB->moveToThread(m_threadB);

    // Connect UI update (dipakai bersama)
    auto connectProcessor = [this](PayloadProcessor *p){

        // =========================
        // Radar point (plot posisi)
        // =========================
        connect(p, &PayloadProcessor::radarPoint, this,
                [=](const QString &src, double x, double y){
                    if (src == UART_PORT0)
                        updateRadarPoint(x, y);
                    else if (src == UART_PORT1)
                        updateRadarPoint2(x, y);
                }, Qt::QueuedConnection);

        // =========================
        // Velocity plot
        // =========================
        connect(p, &PayloadProcessor::velocityUpdate, this,
                [=](const QString &src, const QString &v){
                    if (src == UART_PORT0)
                        drawRealTimeVelocity(v);
                    else if (src == UART_PORT1)
                        drawRealTimeVelocity2(v);
                }, Qt::QueuedConnection);

        // =========================
        // Motion / ETS gram plot
        // =========================
        connect(p, &PayloadProcessor::motionUpdate, this,
                [=](const QString &src, const QString &motion){
                    if (src == UART_PORT0)
                        drawRealTimeetsgram(motion);
                    else if (src == UART_PORT1)
                        drawRealTimeetsgram2(motion);
                }, Qt::QueuedConnection);

        // =========================
        // Fall detected event
        // =========================
        connect(p, &PayloadProcessor::fallDetected, this,
                [=](const QString &src){
                    Q_UNUSED(src);
                    //sound.stop();
                    //sound.play();

                    if (client->isConnected()) {
                        soundPlay(SOUND_FALL_OCCUR);
                        client->emitEventStringMsg("INCIDENT_FALL_DOWN_DETECTED", "");
                    } else {
                        qDebug() << "Socket DC";
                    }
                });

        connect(p, &PayloadProcessor::fallCancel, this,
                [=](const QString &src){
                    Q_UNUSED(src);
                    //sound.stop();
                    //sound.play();

                    if (client->isConnected()) {
                        soundPlay(SOUND_FALL_OCCUR);
                        client->emitEventStringMsg("INCIDENT_FALL_CANCEL", "");
                    } else {
                        qDebug() << "Socket DC";
                    }
                });

        // =========================
        // Debug & serial state
        // =========================
        connect(p, &PayloadProcessor::debugMessage,
                this, [](const QString &msg){ qDebug() << msg; });

        connect(p, &PayloadProcessor::serialOpened,
                this, [=](bool ok){
                    ui->btnOpenSerialPort->setEnabled(!ok);
                    ui->btnLoad->setEnabled(!ok);
                });

        connect(p, &PayloadProcessor::serialError,
                this, [=](const QString &err){
                    qDebug() << "Serial error:" << err;
                });

        // =========================
        // UI update berbasis key + src
        // =========================
        connect(p, &PayloadProcessor::uiUpdate, this,
                [=](const QString &src, const QString &key, const QString &value){

                    if (key == "fallDetection") {
                        if (src == UART_PORT0) ui->leFallDetection->setText(value);
                        else if (src == UART_PORT1) ui->leFallDetection2->setText(value);
                    }
                    else if (key == "fallStateText") {
                        if (src == UART_PORT0) ui->leFallState->setText(value);
                        else if (src == UART_PORT1) ui->leFallState2->setText(value);
                    }
                    else if (key == "fallStateColor") {
                        QString style = (value == "red")
                        ? "background-color: red; color: yellow;"
                        : "background-color: green; color: black;";

                        if (src == UART_PORT0) ui->leFallState->setStyleSheet(style);
                        else if (src == UART_PORT1) ui->leFallState2->setStyleSheet(style);
                    }
                    else if (key == "fallPosX") {
                        if (src == UART_PORT0) ui->leFallPosX->setText(value);
                        else if (src == UART_PORT1) ui->leFallPosX2->setText(value);
                    }
                    else if (key == "fallPosY") {
                        if (src == UART_PORT0) ui->leFallPosY->setText(value);
                        else if (src == UART_PORT1) ui->leFallPosY2->setText(value);
                    }
                    else if (key == "fallDuration") {
                        if (src == UART_PORT0) ui->leFallDuration->setText(value);
                        else if (src == UART_PORT1) ui->leFallDuration2->setText(value);
                    }
                    else if (key == "initStatus") {
                        if (src == UART_PORT0) ui->leInitComplete->setText("Inited");
                        else if (src == UART_PORT1) ui->leInitComplete2->setText("Inited");
                    }
                    else if (key == "angleX") {
                        if (src == UART_PORT0) ui->leAngleXInstallation->setText(value);
                        else if (src == UART_PORT1) ui->leAngleXInstallation2->setText(value);
                    }
                    else if (key == "angleY") {
                        if (src == UART_PORT0) ui->leAngleYInstallation->setText(value);
                        else if (src == UART_PORT1) ui->leAngleYInstallation2->setText(value);
                    }
                    else if (key == "angleZ") {
                        if (src == UART_PORT0) ui->leAngleZInstallation->setText(value);
                        else if (src == UART_PORT1) ui->leAngleZInstallation2->setText(value);
                    }
                    else if (key == "height") {
                        if (src == UART_PORT0) ui->leHeightInstallation->setText(value);
                        else if (src == UART_PORT1) ui->leHeightInstallation2->setText(value);
                    }
                    else if (key == "traceTracking") {
                        if (src == UART_PORT0) ui->leTraceTracking->setText(value);
                        else if (src == UART_PORT1) ui->leTraceTracking2->setText(value);
                    }
                    else if (key == "traceNumber") {
                        if (src == UART_PORT0) ui->leTraceNumber->setText(value);
                        else if (src == UART_PORT1) ui->leTraceNumber2->setText(value);
                    }
                    else if (key == "velocity") {
                        if (src == UART_PORT0) ui->leVelocity->setText(value);
                        else if (src == UART_PORT1) ui->leVelocity2->setText(value);
                    }
                    else if (key == "presence") {
                        if (src == UART_PORT0) ui->lePresence->setText(value);
                        else if (src == UART_PORT1) ui->lePresence2->setText(value);
                    }
                    else if (key == "motionStyle") {
                        QString style;
                        if (value == "presence")
                            style = "background-color: blue; color: yellow;";
                        else if (value == "no_presence")
                            style = "background-color: green; color: black;";
                        else if (value == "motion_high")
                            style = "background-color: grey; color: black;";
                        else if (value == "motion_low")
                            style = "background-color: orange; color: black;";

                        if (src == UART_PORT0) ui->leMotion->setStyleSheet(style);
                        else if (src == UART_PORT1) ui->leMotion2->setStyleSheet(style);
                    }
                    else if (key == "motionValue") {
                        if (src == UART_PORT0) ui->leMotion->setText(value);
                        else if (src == UART_PORT1) ui->leMotion2->setText(value);
                    }
                });
    };

    connectProcessor(m_procA);
    connectProcessor(m_procB);

    m_threadA->start();
    m_threadB->start();

#ifdef PLATFORM_LINUX
    QMetaObject::invokeMethod(m_procA, "initPort",
                              Qt::QueuedConnection,
                              Q_ARG(QString, "/dev/ttyAMA0"));

    QMetaObject::invokeMethod(m_procB, "initPort",
                              Qt::QueuedConnection,
                              Q_ARG(QString, "/dev/ttyAMA1"));
#endif
}
//#endif

//---------------------------------------------------------------------------------------
void MainWindow::fillPortsInfo()
{
    ui->serialPortInfoListBox->clear();
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        QStringList list;
        list << info.portName();
        ui->serialPortInfoListBox->addItem(list.first(), list);
    }

    ui->serialPortInfoListBox->addItem(tr("Custom"));
}

//---------------------------------------------------------------------------------------
void MainWindow::fillPortsInfo2()
{
    ui->serialPortInfoListBox2->clear();
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        QStringList list;
        list << info.portName();
        ui->serialPortInfoListBox2->addItem(list.first(), list);
    }

    ui->serialPortInfoListBox2->addItem(tr("Custom"));
}

//---------------------------------------------------------------------------------------
QByteArray MainWindow::makeFrame(const QByteArray &body)
{
    int s = 0;
    for (unsigned char c : body) s += c;
    unsigned char sum = s & 0xFF;
    QByteArray frame = body;
    frame.append(static_cast<char>(sum));
    frame.append(static_cast<char>(0x54));
    frame.append(static_cast<char>(0x43));
    return frame;
}

//---------------------------------------------------------------------------------------
QString MainWindow::toHexSpace(const QByteArray &data)
{
    QStringList parts;
    for (auto b : data) parts << QString("%1").arg((unsigned char)b, 2, 16, QChar('0')).toUpper();
    return parts.join(' ');
}


//---------------------------------------------------------------------------------------
void MainWindow::on_btnLoad_clicked()
{
    fillPortsInfo();
    ui->btnOpenSerialPort->setEnabled(true);

    //Test koordinat
    //updateRadarPoint(30, -45);
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnOpenSerialPort_clicked()
{
    if (ui->serialPortInfoListBox->currentText().isEmpty()) return;

    const QString portName = ui->serialPortInfoListBox->currentText();
    QMetaObject::invokeMethod(m_procA, "initPort",
                              Qt::QueuedConnection,
                              Q_ARG(QString, portName));

//#ifndef AUTOSTART_ONRPI
    //init_port();
//#endif
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnLoad2_clicked()
{
    fillPortsInfo2();
    ui->btnOpenSerialPort2->setEnabled(true);

    //Test koordinat
    //updateRadarPoint(30, -45);
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnOpenSerialPort2_clicked()
{
    if (ui->serialPortInfoListBox2->currentText().isEmpty()) return;

    const QString portName = ui->serialPortInfoListBox2->currentText();
    QMetaObject::invokeMethod(m_procB, "initPort",
                              Qt::QueuedConnection,
                              Q_ARG(QString, portName));

//#ifndef AUTOSTART_ONRPI
//    init_port2();
//#endif
}

//---------------------------------------------------------------------------------------
quint8 MainWindow::calcChecksum(const QByteArray &frame)
{
    quint16 sum = 0;
    for (char c : frame)
        sum += static_cast<quint8>(c);
    return static_cast<quint8>(sum & 0xFF);
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnGetProductID_clicked()
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_PRODUCT_ID);
    qDebug() << "Sending frame get Product Id:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnGetProductModel_clicked()
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_PRODUCT_MODEL);
    qDebug() << "Sending frame get Production:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnGetFirmwareVersion_clicked()
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_FIRMWARE_VERSION);
    qDebug() << "Sending frame get Firmware:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnGetProductID2_clicked()
{
    if (!m_serial2) return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_PRODUCT_ID);
    qDebug() << "Sending frame get Product Id2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnGetProductModel2_clicked()
{
    if (!m_serial2) return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_PRODUCT_MODEL);
    qDebug() << "Sending frame get Production2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnGetFirmwareVersion2_clicked()
{
    if (!m_serial2) return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_FIRMWARE_VERSION);
    qDebug() << "Sending frame get Firmware2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnHWModel_clicked()
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_HARDWARE_MODEL);
    qDebug() << "Sending frame HW Model:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnCmdInitCompleteCek_clicked()
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_CEK_INITIALIZATION_COMPLETE);
    qDebug() << "Sending frame Cek Init Complete:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnHWModel2_clicked()
{
    if (!m_serial2) return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_HARDWARE_MODEL);
    qDebug() << "Sending frame HW Model2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnCmdInitCompleteCek2_clicked()
{
    if (!m_serial2) return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_CEK_INITIALIZATION_COMPLETE);
    qDebug() << "Sending frame Cek Init Complete2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnGetAngleInst_clicked()
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_ANGLE_INST_QUERY);
    qDebug() << "Sending frame get Angle:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnGetHeightInst_clicked()
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_HEIGHT_INST_QUERY);
    qDebug() << "Sending frame Get Height:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnGetAngleInst2_clicked()
{
    if (!m_serial2) return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_ANGLE_INST_QUERY);
    qDebug() << "Sending frame get Angle2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnGetHeightInst2_clicked()
{
    if (!m_serial2) return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_HEIGHT_INST_QUERY);
    qDebug() << "Sending frame Get Height:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_cbPresence_toggled(bool checked)
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = checked ? makeFrame(CMD_SET_PRESENCE_ON) : makeFrame(CMD_SET_PRESENCE_OFF);
    qDebug() << "Sending frame set Presence:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_cbFallDetection_toggled(bool checked)
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = checked ? makeFrame(CMD_SET_FALL_DETECTION_ON) : makeFrame(CMD_SET_FALL_DETECTION_OFF);
    qDebug() << "Sending frame Set Fall Duration:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnGetFallDuration_clicked()
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_FALL_DURATION);
    qDebug() << "Sending frame Get Fall Duration:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_cbStandStill_toggled(bool checked)
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = checked ? makeFrame(CMD_SET_STAND_STILLON) : makeFrame(CMD_SET_STAND_STILLOFF);
    qDebug() << "Sending frame StandStill:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_cbTraceTracking_toggled(bool checked)
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = checked ? makeFrame(CMD_SET_TRACE_TRACKING_ON) : makeFrame(CMD_SET_TRACE_TRACKING_OFF);
    qDebug() << "Sending frame Trace Tracking:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_cbPresence2_toggled(bool checked)
{
    if (!m_serial2) return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = checked ? makeFrame(CMD_SET_PRESENCE_ON) : makeFrame(CMD_SET_PRESENCE_OFF);
    qDebug() << "Sending frame set Presence2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_cbFallDetection2_toggled(bool checked)
{
    if (!m_serial2) return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = checked ? makeFrame(CMD_SET_FALL_DETECTION_ON) : makeFrame(CMD_SET_FALL_DETECTION_OFF);
    qDebug() << "Sending frame Set Fall Duration2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnGetFallDuration2_clicked()
{
    if (!m_serial2) return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_FALL_DURATION);
    qDebug() << "Sending frame Get Fall Duration2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_cbStandStill2_toggled(bool checked)
{
    if (!m_serial2) return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = checked ? makeFrame(CMD_SET_STAND_STILLON) : makeFrame(CMD_SET_STAND_STILLOFF);
    qDebug() << "Sending frame StandStill2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_cbTraceTracking2_toggled(bool checked)
{
    if (!m_serial2) return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = checked ? makeFrame(CMD_SET_TRACE_TRACKING_ON) : makeFrame(CMD_SET_TRACE_TRACKING_OFF);
    qDebug() << "Sending frame Trace Tracking2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnSetHeight_clicked()
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);

    int height = ui->leSetHeight->text().toInt();
    QByteArray cmd = CMD_SET_HEIGHT;

    quint8 hb = static_cast<quint8>((height >> 8) & 0xFF);
    quint8 lb = static_cast<quint8>(height & 0xFF);

    cmd.append(static_cast<char>(hb));
    cmd.append(static_cast<char>(lb));

    QByteArray frame = makeFrame(cmd);
    qDebug() << "Sending frame cmd Height2:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnSetHeight2_clicked()
{
    if (!m_serial2) return;
    m_serial2->clear(QSerialPort::Input);

    int height = ui->leSetHeight2->text().toInt();
    QByteArray cmd = CMD_SET_HEIGHT;

    quint8 hb = static_cast<quint8>((height >> 8) & 0xFF);
    quint8 lb = static_cast<quint8>(height & 0xFF);

    cmd.append(static_cast<char>(hb));
    cmd.append(static_cast<char>(lb));

    QByteArray frame = makeFrame(cmd);
    qDebug() << "Sending frame cmd Height2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnSetFallDuration_clicked()
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);

    uint32_t duration = ui->leSetFallDuration->text().toUInt();

    QByteArray cmd = CMD_SET_FALL_DURATION;

    // 4 byte (big-endian)
    quint8 b1 = static_cast<quint8>((duration >> 24) & 0xFF);
    quint8 b2 = static_cast<quint8>((duration >> 16) & 0xFF);
    quint8 b3 = static_cast<quint8>((duration >> 8) & 0xFF);
    quint8 b4 = static_cast<quint8>(duration & 0xFF);

    cmd.append(static_cast<char>(b1));
    cmd.append(static_cast<char>(b2));
    cmd.append(static_cast<char>(b3));
    cmd.append(static_cast<char>(b4));

    QByteArray frame = makeFrame(cmd);

    qDebug() << "Sending frame cmd Set Fall Duration:" << toHexSpace(frame);

    m_serial->write(frame);
    m_serial->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnSetAngle_clicked()
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);

    uint16_t angleX = ui->leAngleX->text().toUInt();
    uint16_t angleY = ui->leAngleY->text().toUInt();
    uint16_t angleZ = ui->leAngleZ->text().toUInt();

    QByteArray cmd = CMD_SET_ANGLE_INST;   // ini harus berisi 06 01 sesuai protokol kamu

    // X axis (2 byte big-endian)
    cmd.append(static_cast<char>((angleX >> 8) & 0xFF));   // HB
    cmd.append(static_cast<char>(angleX & 0xFF));          // LB

    // Y axis
    cmd.append(static_cast<char>((angleY >> 8) & 0xFF));
    cmd.append(static_cast<char>(angleY & 0xFF));

    // Z axis
    cmd.append(static_cast<char>((angleZ >> 8) & 0xFF));
    cmd.append(static_cast<char>(angleZ & 0xFF));

    // Generate full frame (prefix, length, checksum, suffix)
    QByteArray frame = makeFrame(cmd);

    qDebug() << "Sending frame SetAngle:" << toHexSpace(frame);

    m_serial->write(frame);
    m_serial->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnSetFallDuration2_clicked()
{
    if (!m_serial2) return;
    m_serial2->clear(QSerialPort::Input);

    uint32_t duration = ui->leSetFallDuration->text().toUInt();

    QByteArray cmd = CMD_SET_FALL_DURATION;

    // 4 byte (big-endian)
    quint8 b1 = static_cast<quint8>((duration >> 24) & 0xFF);
    quint8 b2 = static_cast<quint8>((duration >> 16) & 0xFF);
    quint8 b3 = static_cast<quint8>((duration >> 8) & 0xFF);
    quint8 b4 = static_cast<quint8>(duration & 0xFF);

    cmd.append(static_cast<char>(b1));
    cmd.append(static_cast<char>(b2));
    cmd.append(static_cast<char>(b3));
    cmd.append(static_cast<char>(b4));

    QByteArray frame = makeFrame(cmd);

    qDebug() << "Sending frame cmd Set Fall Duration:" << toHexSpace(frame);

    m_serial2->write(frame);
    m_serial2->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::on_btnSetAngle2_clicked()
{
    if (!m_serial2) return;
    m_serial2->clear(QSerialPort::Input);

    uint16_t angleX = ui->leAngleX2->text().toUInt();
    uint16_t angleY = ui->leAngleY2->text().toUInt();
    uint16_t angleZ = ui->leAngleZ2->text().toUInt();

    QByteArray cmd = CMD_SET_ANGLE_INST;   // ini harus berisi 06 01 sesuai protokol kamu

    // X axis (2 byte big-endian)
    cmd.append(static_cast<char>((angleX >> 8) & 0xFF));   // HB
    cmd.append(static_cast<char>(angleX & 0xFF));          // LB

    // Y axis
    cmd.append(static_cast<char>((angleY >> 8) & 0xFF));
    cmd.append(static_cast<char>(angleY & 0xFF));

    // Z axis
    cmd.append(static_cast<char>((angleZ >> 8) & 0xFF));
    cmd.append(static_cast<char>(angleZ & 0xFF));

    // Generate full frame (prefix, length, checksum, suffix)
    QByteArray frame = makeFrame(cmd);

    qDebug() << "Sending frame SetAngle2:" << toHexSpace(frame);

    m_serial2->write(frame);
    m_serial2->flush();
}

//---------------------------------------------------------------------------------------
void MainWindow::updateRadarPoint(double x, double y)
{
    radarPoint->data()->clear();
    radarPoint->addData(x, y);

    //ui->plotRadar->rescaleAxes();
    ui->plotRadar->replot();
}

//---------------------------------------------------------------------------------------
void MainWindow::init_radar()
{

}

//---------------------------------------------------------------------------------------
void MainWindow::updateRadarPoint2(double x, double y)
{
    radarPoint2->data()->clear();
    radarPoint2->addData(x, y);

    //ui->plotRadar2->rescaleAxes();
    ui->plotRadar2->replot();
}

//---------------------------------------------------------------------------------------
void MainWindow::setupPlotTs()
{
    // Add two graphs and configure appearance
    ui->plottsgram->addGraph();
    ui->plottsgram->graph(0)->setPen(QPen(Qt::yellow));
    ui->plottsgram->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));

    ui->plottsgram->addGraph();
    ui->plottsgram->graph(1)->setPen(QPen(Qt::red));

    // Configure top and right axes (ticks only, no labels)
    ui->plottsgram->xAxis2->setVisible(true);
    ui->plottsgram->xAxis2->setTickLabels(false);
    ui->plottsgram->yAxis2->setVisible(true);
    ui->plottsgram->yAxis2->setTickLabels(false);

    connect(ui->plottsgram, &QCustomPlot::afterReplot, [=]() {
        ui->plottsgram->xAxis2->setRange(ui->plottsgram->xAxis->range());
        ui->plottsgram->yAxis2->setRange(ui->plottsgram->yAxis->range());
    });

    // Enable interactions
    ui->plottsgram->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

}

//---------------------------------------------------------------------------------------
void MainWindow::setupPlotTsVelocity()
{
    // Add two graphs and configure appearance
    ui->plottsVelocity->addGraph();
    ui->plottsVelocity->graph(0)->setPen(QPen(Qt::yellow));
    ui->plottsVelocity->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));

    ui->plottsVelocity->addGraph();
    ui->plottsVelocity->graph(1)->setPen(QPen(Qt::red));

    // Configure top and right axes (ticks only, no labels)
    ui->plottsVelocity->xAxis2->setVisible(true);
    ui->plottsVelocity->xAxis2->setTickLabels(false);
    ui->plottsVelocity->yAxis2->setVisible(true);
    ui->plottsVelocity->yAxis2->setTickLabels(false);

    connect(ui->plottsVelocity, &QCustomPlot::afterReplot, [=]() {
        ui->plottsVelocity->xAxis2->setRange(ui->plottsVelocity->xAxis->range());
        ui->plottsVelocity->yAxis2->setRange(ui->plottsVelocity->yAxis->range());
    });

    // Enable interactions
    ui->plottsVelocity->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

//---------------------------------------------------------------------------------------
void MainWindow::setupPlotTs2()
{
    // Add two graphs and configure appearance
    ui->plottsgram2->addGraph();
    ui->plottsgram2->graph(0)->setPen(QPen(Qt::yellow));
    ui->plottsgram2->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));

    ui->plottsgram2->addGraph();
    ui->plottsgram2->graph(1)->setPen(QPen(Qt::red));

    // Configure top and right axes (ticks only, no labels)
    ui->plottsgram2->xAxis2->setVisible(true);
    ui->plottsgram2->xAxis2->setTickLabels(false);
    ui->plottsgram2->yAxis2->setVisible(true);
    ui->plottsgram2->yAxis2->setTickLabels(false);

    connect(ui->plottsgram2, &QCustomPlot::afterReplot, [=]() {
        ui->plottsgram2->xAxis2->setRange(ui->plottsgram->xAxis->range());
        ui->plottsgram2->yAxis2->setRange(ui->plottsgram->yAxis->range());
    });

    // Enable interactions
    ui->plottsgram2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

}

//---------------------------------------------------------------------------------------
void MainWindow::setupPlotTsVelocity2()
{
    // Add two graphs and configure appearance
    ui->plottsVelocity2->addGraph();
    ui->plottsVelocity2->graph(0)->setPen(QPen(Qt::yellow));
    ui->plottsVelocity2->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));

    ui->plottsVelocity2->addGraph();
    ui->plottsVelocity2->graph(1)->setPen(QPen(Qt::red));

    // Configure top and right axes (ticks only, no labels)
    ui->plottsVelocity2->xAxis2->setVisible(true);
    ui->plottsVelocity2->xAxis2->setTickLabels(false);
    ui->plottsVelocity2->yAxis2->setVisible(true);
    ui->plottsVelocity2->yAxis2->setTickLabels(false);

    connect(ui->plottsVelocity2, &QCustomPlot::afterReplot, [=]() {
        ui->plottsVelocity2->xAxis2->setRange(ui->plottsVelocity->xAxis->range());
        ui->plottsVelocity2->yAxis2->setRange(ui->plottsVelocity->yAxis->range());
    });

    // Enable interactions
    ui->plottsVelocity->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

//---------------------------------------------------------------------------------------
void MainWindow::realtimeDataSlot(QString value)
{
    static QTime timeStart = QTime::currentTime();
    double key = timeStart.msecsTo(QTime::currentTime()) / 1000.0; // seconds
    static double lastPointKey = 0;

    if (key - lastPointKey > 0.002) { // at most add point every 2 ms
        double filteredValue = value.toDouble();
        ui->plottsgram->graph(0)->addData(key, filteredValue);
        lastPointKey = key;
    }

    ui->plottsgram->xAxis->setRange(key, 8, Qt::AlignRight);
    ui->plottsgram->graph(0)->rescaleValueAxis(true);

    static QElapsedTimer replotTimer;
    if (!replotTimer.isValid()) replotTimer.start();

    if (replotTimer.elapsed() >= 50) { // Refresh setiap 50 ms
        ui->plottsgram->replot();
        replotTimer.restart();
    }
}

//---------------------------------------------------------------------------------------
void MainWindow::realtimeDataVelocity(QString value)
{
    static QTime timeStart = QTime::currentTime();
    double key = timeStart.msecsTo(QTime::currentTime()) / 1000.0; // seconds
    static double lastPointKey = 0;

    if (key - lastPointKey > 0.002) { // at most add point every 2 ms
        double filteredValue = value.toDouble();
        ui->plottsVelocity->graph(0)->addData(key, filteredValue);
        lastPointKey = key;
    }

    ui->plottsVelocity->xAxis->setRange(key, 8, Qt::AlignRight);
    ui->plottsVelocity->graph(0)->rescaleValueAxis(true);

    static QElapsedTimer replotTimer;
    if (!replotTimer.isValid()) replotTimer.start();

    if (replotTimer.elapsed() >= 50) { // Refresh setiap 50 ms
        ui->plottsVelocity->replot();
        replotTimer.restart();
    }
}

//---------------------------------------------------------------------------------------
void MainWindow::realtimeDataSlot2(QString value)
{
    static QTime timeStart = QTime::currentTime();
    double key = timeStart.msecsTo(QTime::currentTime()) / 1000.0; // seconds
    static double lastPointKey = 0;

    if (key - lastPointKey > 0.002) { // at most add point every 2 ms
        double filteredValue = value.toDouble();
        ui->plottsgram2->graph(0)->addData(key, filteredValue);
        lastPointKey = key;
    }

    ui->plottsgram2->xAxis->setRange(key, 8, Qt::AlignRight);
    ui->plottsgram2->graph(0)->rescaleValueAxis(true);

    static QElapsedTimer replotTimer;
    if (!replotTimer.isValid()) replotTimer.start();

    if (replotTimer.elapsed() >= 50) { // Refresh setiap 50 ms
        ui->plottsgram2->replot();
        replotTimer.restart();
    }
}

//---------------------------------------------------------------------------------------
void MainWindow::realtimeDataVelocity2(QString value)
{
    static QTime timeStart = QTime::currentTime();
    double key = timeStart.msecsTo(QTime::currentTime()) / 1000.0; // seconds
    static double lastPointKey = 0;

    if (key - lastPointKey > 0.002) { // at most add point every 2 ms
        double filteredValue = value.toDouble();
        ui->plottsVelocity2->graph(0)->addData(key, filteredValue);
        lastPointKey = key;
    }

    ui->plottsVelocity2->xAxis->setRange(key, 8, Qt::AlignRight);
    ui->plottsVelocity2->graph(0)->rescaleValueAxis(true);

    static QElapsedTimer replotTimer;
    if (!replotTimer.isValid()) replotTimer.start();

    if (replotTimer.elapsed() >= 50) { // Refresh setiap 50 ms
        ui->plottsVelocity2->replot();
        replotTimer.restart();
    }
}

//---------------------------------------------------------------------------------------
void MainWindow::drawRealTimeetsgram(QString motion)
{
    realtimeDataSlot(motion);
}

//---------------------------------------------------------------------------------------
void MainWindow::drawRealTimeVelocity(QString velocity)
{
    realtimeDataVelocity(velocity);
}

//---------------------------------------------------------------------------------------
void MainWindow::drawRealTimeetsgram2(QString motion)
{
    realtimeDataSlot2(motion);
}

//---------------------------------------------------------------------------------------
void MainWindow::drawRealTimeVelocity2(QString velocity)
{
    realtimeDataVelocity2(velocity);
}

//---------------------------------------------------------------------------------------
void MainWindow::soundPlay(int request)
{
    switch (request){
    case SOUND_FALL_OCCUR:
        qDebug() << "Request 1 received";
        QMetaObject::invokeMethod(m_audioWorker, "enqueueSound",
                                  Qt::QueuedConnection,
                                  Q_ARG(int, SOUND_FALL_OCCUR));
        break;
    case SOUND_HELP:
        qDebug() << "Request 2 received";
        QMetaObject::invokeMethod(m_audioWorker, "enqueueSound",
                                  Qt::QueuedConnection,
                                  Q_ARG(int, SOUND_HELP));
        break;
    case SOUND_IAM_OK:
        qDebug() << "Request 3 received";
        QMetaObject::invokeMethod(m_audioWorker, "enqueueSound",
                                  Qt::QueuedConnection,
                                  Q_ARG(int, SOUND_IAM_OK));
        break;
    default:
        break;
    }
}

//---------------------------------------------------------------------------------------
void MainWindow::setupRealtimeDataMotion(QCustomPlot *plottsgram)
{
    demoName = "Real Time Data Demo";

    plottsgram->setNotAntialiasedElements(QCP::aeAll);
    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    plottsgram->xAxis->setTickLabelFont(font);
    plottsgram->yAxis->setTickLabelFont(font);
    plottsgram->legend->setFont(font);

    plottsgram->addGraph();
    plottsgram->graph(0)->setPen(QPen(QColor(40, 255, 255)));

    plottsgram->setBackground(QBrush(QColor(30, 30, 30)));
    plottsgram->axisRect()->setBackground(QBrush(QColor(45, 45, 45)));

    QPen axisPen(QColor(250, 250, 250));
    QPen tickPen(QColor(250, 250, 250));
    QFont labelFont("Arial", 10);

    plottsgram->xAxis->setBasePen(axisPen);
    plottsgram->xAxis->setTickPen(tickPen);
    plottsgram->xAxis->setSubTickPen(tickPen);
    plottsgram->xAxis->setTickLabelColor(QColor(250, 250, 250));
    plottsgram->xAxis->setLabelColor(QColor(250, 250, 250));
    plottsgram->xAxis->setLabelFont(labelFont);

    plottsgram->yAxis->setBasePen(axisPen);
    plottsgram->yAxis->setTickPen(tickPen);
    plottsgram->yAxis->setSubTickPen(tickPen);
    plottsgram->yAxis->setTickLabelColor(QColor(250, 250, 250));
    plottsgram->yAxis->setLabelColor(QColor(250, 250, 250));
    plottsgram->yAxis->setLabelFont(labelFont);

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    plottsgram->xAxis->setRange(0, 10000);
    plottsgram->axisRect()->setupFullAxesBox();
    plottsgram->yAxis->setRange(0, 40);

    // Synchronize top/right axes with bottom/left
    //connect(plottsgram->xAxis, &QCPAxis::rangeChanged, plottsgram->xAxis2, &QCPAxis::setRange);
    //connect(plottsgram->yAxis, &QCPAxis::rangeChanged, plottsgram->yAxis2, &QCPAxis::setRange);
    // Synchronize top/right axes with bottom/left
    //plottsgram->xAxis->setRangeReplotPolicy(QCPAxis::rpImmediate);
    plottsgram->xAxis2->setRange(plottsgram->xAxis->range());
    plottsgram->yAxis2->setRange(plottsgram->yAxis->range());

    // Set update function saat plot direplot
    // Sinkronisasi top/right axes dengan bottom/left
    connect(plottsgram, &QCustomPlot::afterReplot, [=](){
        plottsgram->xAxis2->setRange(plottsgram->xAxis->range());
        plottsgram->yAxis2->setRange(plottsgram->yAxis->range());
    });

    // Vertical Text - left blank intentionally
}

//---------------------------------------------------------------------------------------
void MainWindow::setupRealtimeDataVelocity(QCustomPlot *plottsVelocity)
{
    demoName = "Real Time Data Demo";

    ui->plottsVelocity->setNotAntialiasedElements(QCP::aeAll);

    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    ui->plottsVelocity->xAxis->setTickLabelFont(font);
    ui->plottsVelocity->yAxis->setTickLabelFont(font);
    ui->plottsVelocity->legend->setFont(font);

    ui->plottsVelocity->addGraph();
    ui->plottsVelocity->graph(0)->setPen(QPen(QColor(40, 255, 255)));

    ui->plottsVelocity->setBackground(QBrush(QColor(30, 30, 30)));
    ui->plottsVelocity->axisRect()->setBackground(QBrush(QColor(45, 45, 45)));

    QPen axisPen(QColor(250, 250, 250));
    QPen tickPen(QColor(250, 250, 250));
    QFont labelFont("Arial", 10);

    ui->plottsVelocity->xAxis->setBasePen(axisPen);
    ui->plottsVelocity->xAxis->setTickPen(tickPen);
    ui->plottsVelocity->xAxis->setSubTickPen(tickPen);
    ui->plottsVelocity->xAxis->setTickLabelColor(QColor(250, 250, 250));
    ui->plottsVelocity->xAxis->setLabelColor(QColor(250, 250, 250));
    ui->plottsVelocity->xAxis->setLabelFont(labelFont);

    ui->plottsVelocity->yAxis->setBasePen(axisPen);
    ui->plottsVelocity->yAxis->setTickPen(tickPen);
    ui->plottsVelocity->yAxis->setSubTickPen(tickPen);
    ui->plottsVelocity->yAxis->setTickLabelColor(QColor(250, 250, 250));
    ui->plottsVelocity->yAxis->setLabelColor(QColor(250, 250, 250));
    ui->plottsVelocity->yAxis->setLabelFont(labelFont);

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->plottsVelocity->xAxis->setRange(0, 10000);
    ui->plottsVelocity->axisRect()->setupFullAxesBox();
    ui->plottsVelocity->yAxis->setRange(0, 40);


    ui->plottsVelocity->xAxis2->setRange(ui->plottsVelocity->xAxis->range());
    ui->plottsVelocity->yAxis2->setRange(ui->plottsVelocity->yAxis->range());

    // Set update function saat plot direplot
    // Sinkronisasi top/right axes dengan bottom/left
    connect(ui->plottsVelocity, &QCustomPlot::afterReplot, [=](){
        ui->plottsVelocity->xAxis2->setRange(ui->plottsVelocity->xAxis->range());
        ui->plottsVelocity->yAxis2->setRange(ui->plottsVelocity->yAxis->range());
    });

    // Vertical Text - left blank intentionally
}

//---------------------------------------------------------------------------------------
void MainWindow::setupPlotRadar(QCustomPlot *plotRadar)
{
    //-------------------------- Inisialisasi plotRadar --------------------------
    ui->plotRadar->xAxis->setVisible(true);   // tampilkan axes
    ui->plotRadar->yAxis->setVisible(true);
    ui->plotRadar->xAxis->setRange(-500, 500);
    ui->plotRadar->yAxis->setRange(-500, 500);

    // Grid kotak-kotak putus-putus
    ui->plotRadar->xAxis->grid()->setVisible(true);
    ui->plotRadar->yAxis->grid()->setVisible(true);
    ui->plotRadar->xAxis->grid()->setPen(QPen(Qt::gray, 1, Qt::DashLine));
    ui->plotRadar->yAxis->grid()->setPen(QPen(Qt::gray, 1, Qt::DashLine));

    // Bikin garis utama di tengah (X=0 dan Y=0)
    QCPItemStraightLine *xLine = new QCPItemStraightLine(ui->plotRadar);
    xLine->point1->setCoords(0, -500);
    xLine->point2->setCoords(0, 500);
    xLine->setPen(QPen(Qt::gray, 1, Qt::DashLine));

    QCPItemStraightLine *yLine = new QCPItemStraightLine(ui->plotRadar);
    yLine->point1->setCoords(-500, 0);
    yLine->point2->setCoords(500, 0);
    yLine->setPen(QPen(Qt::gray, 1, Qt::DashLine));

    // Bikin graph untuk titik radar
    radarPoint = ui->plotRadar->addGraph();
    radarPoint->setLineStyle(QCPGraph::lsNone);
    radarPoint->setScatterStyle(QCPScatterStyle(
        QCPScatterStyle::ssCircle,
        QPen(Qt::red),
        QBrush(Qt::red),
        8
        ));
}


//---------------------------------------------------------------------------------------
void MainWindow::setupRealtimeDataMotion2(QCustomPlot *plottsgram2)
{
    demoName = "Real Time Data Demo";

    plottsgram2->setNotAntialiasedElements(QCP::aeAll);
    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    plottsgram2->xAxis->setTickLabelFont(font);
    plottsgram2->yAxis->setTickLabelFont(font);
    plottsgram2->legend->setFont(font);

    plottsgram2->addGraph();
    plottsgram2->graph(0)->setPen(QPen(QColor(40, 255, 255)));

    plottsgram2->setBackground(QBrush(QColor(30, 30, 30)));
    plottsgram2->axisRect()->setBackground(QBrush(QColor(45, 45, 45)));

    QPen axisPen(QColor(250, 250, 250));
    QPen tickPen(QColor(250, 250, 250));
    QFont labelFont("Arial", 10);

    plottsgram2->xAxis->setBasePen(axisPen);
    plottsgram2->xAxis->setTickPen(tickPen);
    plottsgram2->xAxis->setSubTickPen(tickPen);
    plottsgram2->xAxis->setTickLabelColor(QColor(250, 250, 250));
    plottsgram2->xAxis->setLabelColor(QColor(250, 250, 250));
    plottsgram2->xAxis->setLabelFont(labelFont);

    plottsgram2->yAxis->setBasePen(axisPen);
    plottsgram2->yAxis->setTickPen(tickPen);
    plottsgram2->yAxis->setSubTickPen(tickPen);
    plottsgram2->yAxis->setTickLabelColor(QColor(250, 250, 250));
    plottsgram2->yAxis->setLabelColor(QColor(250, 250, 250));
    plottsgram2->yAxis->setLabelFont(labelFont);

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    plottsgram2->xAxis->setRange(0, 10000);
    plottsgram2->axisRect()->setupFullAxesBox();
    plottsgram2->yAxis->setRange(0, 40);

    // Synchronize top/right axes with bottom/left
    //connect(plottsgram->xAxis, &QCPAxis::rangeChanged, plottsgram->xAxis2, &QCPAxis::setRange);
    //connect(plottsgram->yAxis, &QCPAxis::rangeChanged, plottsgram->yAxis2, &QCPAxis::setRange);
    // Synchronize top/right axes with bottom/left
    //plottsgram->xAxis->setRangeReplotPolicy(QCPAxis::rpImmediate);
    plottsgram2->xAxis->setRange(plottsgram2->xAxis2->range());
    plottsgram2->yAxis->setRange(plottsgram2->yAxis2->range());

    // Set update function saat plot direplot
    // Sinkronisasi top/right axes dengan bottom/left
    connect(plottsgram2, &QCustomPlot::afterReplot, [=](){
        plottsgram2->xAxis->setRange(plottsgram2->xAxis->range());
        plottsgram2->yAxis->setRange(plottsgram2->yAxis->range());
    });

    // Vertical Text - left blank intentionally
}

//---------------------------------------------------------------------------------------
void MainWindow::setupRealtimeDataVelocity2(QCustomPlot *plottsVelocity2)
{
    demoName = "Real Time Data Demo";

    ui->plottsVelocity2->setNotAntialiasedElements(QCP::aeAll);

    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    ui->plottsVelocity2->xAxis->setTickLabelFont(font);
    ui->plottsVelocity2->yAxis->setTickLabelFont(font);
    ui->plottsVelocity2->legend->setFont(font);

    ui->plottsVelocity2->addGraph();
    ui->plottsVelocity2->graph(0)->setPen(QPen(QColor(40, 255, 255)));

    ui->plottsVelocity2->setBackground(QBrush(QColor(30, 30, 30)));
    ui->plottsVelocity2->axisRect()->setBackground(QBrush(QColor(45, 45, 45)));

    QPen axisPen(QColor(250, 250, 250));
    QPen tickPen(QColor(250, 250, 250));
    QFont labelFont("Arial", 10);

    ui->plottsVelocity2->xAxis->setBasePen(axisPen);
    ui->plottsVelocity2->xAxis->setTickPen(tickPen);
    ui->plottsVelocity2->xAxis->setSubTickPen(tickPen);
    ui->plottsVelocity2->xAxis->setTickLabelColor(QColor(250, 250, 250));
    ui->plottsVelocity2->xAxis->setLabelColor(QColor(250, 250, 250));
    ui->plottsVelocity2->xAxis->setLabelFont(labelFont);

    ui->plottsVelocity2->yAxis->setBasePen(axisPen);
    ui->plottsVelocity2->yAxis->setTickPen(tickPen);
    ui->plottsVelocity2->yAxis->setSubTickPen(tickPen);
    ui->plottsVelocity2->yAxis->setTickLabelColor(QColor(250, 250, 250));
    ui->plottsVelocity2->yAxis->setLabelColor(QColor(250, 250, 250));
    ui->plottsVelocity2->yAxis->setLabelFont(labelFont);

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->plottsVelocity2->xAxis->setRange(0, 10000);
    ui->plottsVelocity2->axisRect()->setupFullAxesBox();
    ui->plottsVelocity2->yAxis->setRange(0, 40);


    ui->plottsVelocity2->xAxis->setRange(ui->plottsVelocity2->xAxis->range());
    ui->plottsVelocity2->yAxis->setRange(ui->plottsVelocity2->yAxis->range());

    // Set update function saat plot direplot
    // Sinkronisasi top/right axes dengan bottom/left
    connect(ui->plottsVelocity2, &QCustomPlot::afterReplot, [=](){
        ui->plottsVelocity2->xAxis->setRange(ui->plottsVelocity2->xAxis->range());
        ui->plottsVelocity2->yAxis->setRange(ui->plottsVelocity2->yAxis->range());
    });

    // Vertical Text - left blank intentionally
}

//---------------------------------------------------------------------------------------
void MainWindow::setupPlotRadar2(QCustomPlot *plotRadar2)
{
    //-------------------------- Inisialisasi plotRadar --------------------------
    ui->plotRadar2->xAxis->setVisible(true);   // tampilkan axes
    ui->plotRadar2->yAxis->setVisible(true);
    ui->plotRadar2->xAxis->setRange(-500, 500);
    ui->plotRadar2->yAxis->setRange(-500, 500);

    // Grid kotak-kotak putus-putus
    ui->plotRadar2->xAxis->grid()->setVisible(true);
    ui->plotRadar2->yAxis->grid()->setVisible(true);
    ui->plotRadar2->xAxis->grid()->setPen(QPen(Qt::gray, 1, Qt::DashLine));
    ui->plotRadar2->yAxis->grid()->setPen(QPen(Qt::gray, 1, Qt::DashLine));

    // Bikin garis utama di tengah (X=0 dan Y=0)
    QCPItemStraightLine *xLine = new QCPItemStraightLine(ui->plotRadar);
    xLine->point1->setCoords(0, -500);
    xLine->point2->setCoords(0, 500);
    xLine->setPen(QPen(Qt::gray, 1, Qt::DashLine));

    QCPItemStraightLine *yLine = new QCPItemStraightLine(ui->plotRadar);
    yLine->point1->setCoords(-500, 0);
    yLine->point2->setCoords(500, 0);
    yLine->setPen(QPen(Qt::gray, 1, Qt::DashLine));

    // Bikin graph untuk titik radar
    radarPoint2 = ui->plotRadar2->addGraph();
    radarPoint2->setLineStyle(QCPGraph::lsNone);
    radarPoint2->setScatterStyle(QCPScatterStyle(
        QCPScatterStyle::ssCircle,
        QPen(Qt::red),
        QBrush(Qt::red),
        8
        ));
}


//------------------------------------------------------------------------
void MainWindow::on_btnPlaySound_clicked()
{
    soundPlay(SOUND_FALL_OCCUR);
}

//------------------------------------------------------------------------
void MainWindow::onSocketEventReceived(const QString &eventName, const QJsonValue &data)
{
    qDebug() << "UI received event:" << eventName << "data:" << data;
    //m_eventQueue.enqueue(qMakePair(eventName, data));

    qDebug() << "UI received event:" << eventName << "data:" << data;
    m_worker->enqueue(eventName, data);
}

//------------------------------------------------------------------------
void MainWindow::onDeviceReadyConnected(int vol, int bright)
{

}

//------------------------------------------------------------------------------------------------------------------------
void MainWindow::on_btnColor1_clicked()
{
    m_gpio->setColor(COLOR_WHITE);
}

//------------------------------------------------------------------------
void MainWindow::on_btnColor2_clicked()
{
    m_gpio->setColor(COLOR_WHITE_BLINKY);
}

//------------------------------------------------------------------------
void MainWindow::on_btnColor3_clicked()
{
    m_gpio->setColor(COLOR_RED);

}

//------------------------------------------------------------------------
void MainWindow::on_btnColor4_clicked()
{
    m_gpio->setColor(COLOR_RED_BLINKY);
}

//------------------------------------------------------------------------
void MainWindow::on_hsBrightness_valueChanged(int value)
{
    ui->lBrightness->setText(QString::number(value));
}

//------------------------------------------------------------------------
void MainWindow::on_btnGetBrightness_clicked()
{
    ui->leBrightness->setText(QString::number(m_brightness->getBrightness()));
}

//------------------------------------------------------------------------
void MainWindow::on_btnsetBrightness_clicked()
{
   if(m_brightness->setBrightnessPercent(ui->hsBrightness->value())){
       qDebug() << "Success Set brightness " << ui->hsBrightness->value();
   }else{
       qDebug() << "Fail Set brightness " << ui->hsBrightness->value();
   }
}

//------------------------------------------------------------------------
void MainWindow::on_btnGetVol_clicked()
{
    ui->leVol->setText(QString::number(m_volume->getVolumePercent()));
}

//------------------------------------------------------------------------
void MainWindow::on_hsVol_valueChanged(int value)
{
    ui->lVol->setText(QString::number(value));
}

//------------------------------------------------------------------------
void MainWindow::on_btnsetVol_clicked()
{
    if(m_volume->setVolumePercent(ui->hsVol->value())){
        qDebug() << "Success Set Volume " << ui->hsVol->value();
    }else{
        qDebug() << "Fail Set Volume " << ui->hsVol->value();
    }
}

//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------
void MainWindow::on_btnConnect_clicked()
{
    client->connectToServer("192.168.1.100", 3000);
}

//------------------------------------------------------------------------
void MainWindow::on_btnFallSimulation_clicked()
{
    if(client->isConnected()){
        client->emitEventStringMsg("INCIDENT_FALL_DOWN_DETECTED","");
    }else{
        qDebug() << " Socket DC";
    }
}

//------------------------------------------------------------------------
void MainWindow::onListenStateChanged(const QString &state)
{
    qDebug() << "UI Process LISTEN:" << state;

    if (state == "ON") m_gpio->setColor(COLOR_GREEN);
    else if (state == "OFF") m_gpio->setColor(COLOR_WHITE);
}

//------------------------------------------------------------------------
void MainWindow::onTalkingStateChanged(const QString &state)
{
    qDebug() << "UI Process TALKING:" << state;
    if (state == "ON") m_gpio->setColor(COLOR_RED_BLINKY);
    else if (state == "OFF") m_gpio->setColor(COLOR_GREEN);
}

//------------------------------------------------------------------------
void MainWindow::onVolumeGetRequested()
{
    qDebug() << "UI Process VOL get req";
    int currentVol = m_volume->getVolumePercent();
    client->emitEventStringMsg("VOLUME_GET_ACK", QString::number(currentVol));

}

//------------------------------------------------------------------------
void MainWindow::onVolumeSetRequested(int vt)
{
    qDebug() << "UI vol Set:" << vt;
    if (vt > 0 && m_volume->setVolumePercent(vt)) {
        qDebug() << "UI vol successfully set to" << vt;
    } else {
        qDebug() << "fail setVol" << vt;
    }
}

//------------------------------------------------------------------------
void MainWindow::onPingDeviceUpRequested()
{
    qDebug() << "UI PingDeviceUpReq";
    int brightGet = m_brightness->setBrightnessPercent(80);
    client->emitEventStringMsg("PING_DEVICE_UP_FRONTEND", QString::number(brightGet));
}

//------------------------------------------------------------------------
void MainWindow::onSleepRequested()
{
    qDebug() << "UI SleepReq";
    int getBright = m_brightness->getBrightness();
    client->emitEventStringMsg("SLEEP_FRONTEND", QString::number(getBright));
}

//------------------------------------------------------------------------
void MainWindow::onBrightnessSetRequested(int bst)
{
    qDebug() << "Brightness Set:" << bst;
    if (bst > 0 && m_brightness->setBrightness(bst)) {
        qDebug() << "Brightness successfully set to" << bst;
        bst = m_brightness->getBrightness();
        qDebug() << "prepare emit brightness set ack " << bst;
        //client->emitEventStringMsg("BRIGHTNESS_GET_ACK",QString::number(bst));
    }
}

//------------------------------------------------------------------------
void MainWindow::onBrightnessGetRequested()
{
    qDebug() << "UI Brightness get:";
    int bst = m_brightness->getBrightness();
    //if (bst > 0 && setBrightnessPercent(bst)) {
    qDebug() << "UI emit Brightness get start" << bst;
    client->emitEventStringMsg("BRIGHTNESS_GET_ACK",QString::number(bst));
    qDebug() << "UI emit Brightness get end" << bst;
    //}
}

//------------------------------------------------------------------------
void MainWindow::onVolumeIncreaseReq()
{
    qDebug() << "UI onVolumeIncreaseReq";
    int currentVol = m_volume->getVolumePercent();
    currentVol = currentVol + 5;
    if((currentVol > 20) && (currentVol <= 99)){
        if(m_volume->setVolumePercent(currentVol)){
            qDebug() << "UI succes Inc Vol " << currentVol;
        }else{
            qDebug() << "UI fail Inc Vol " << currentVol;
        }
    }
}

//------------------------------------------------------------------------
void MainWindow::onVolumeDecreaseReq()
{
    qDebug() << "UI onVolumeDecreaseReq";
    int currentVol = m_volume->getVolumePercent();
    currentVol = currentVol - 5;
    if((currentVol > 20) && (currentVol <= 99)){
        if(m_volume->setVolumePercent(currentVol)){
            qDebug() << "UI succes Inc Vol " << currentVol;
        }else{
            qDebug() << "UI fail Inc Vol " << currentVol;
        }
    }
}

//------------------------------------------------------------------------
void MainWindow::onBrihtnessIncreaseReq()
{
    qDebug() << "UI onBrihtnessIncreaseReq";
    int currentBrightness = m_brightness->getBrightness();
    currentBrightness = currentBrightness + 5;
    if((currentBrightness > 20) && (currentBrightness <= 255)){
        if(m_brightness->setBrightness(currentBrightness)){
            qDebug() << "UI succes Inc Brightness " << currentBrightness;
        }else{
            qDebug() << "UI fail Inc  " << currentBrightness;
        }
    }else{
        qDebug() << "UI fail Inc out of range  " << currentBrightness;
    }
}

//------------------------------------------------------------------------
void MainWindow::onBrightnessDecreaseReq()
{
    qDebug() << "UI onBrightnessDecreaseReq";
    int currentBrightness = m_brightness->getBrightness();
    currentBrightness = currentBrightness - 5;
    if((currentBrightness > 20) && (currentBrightness <= 255)){
        if(m_brightness->setBrightness(currentBrightness)){
            qDebug() << "UI succes Inc Brightness " << currentBrightness;
        }else{
            qDebug() << "UI fail Inc v " << currentBrightness;
        }
    }else{
        qDebug() << "UI fail Inc out of range  " << currentBrightness;
    }
}

//------------------------------------------------------------------------
void MainWindow::onIncidentFallOccur()
{
    soundPlay(SOUND_FALL_OCCUR);
}

//------------------------------------------------------------------------
void MainWindow::onIncidentFallCancel()
{

}

//------------------------------------------------------------------------
void MainWindow::onIncidentIamnotOK()
{
    soundPlay(SOUND_HELP);
}

//------------------------------------------------------------------------
void MainWindow::onIncidentIamOK()
{
    soundPlay(SOUND_IAM_OK);
}

//------------------------------------------------------------------------
void MainWindow::on_btnPlayFall_clicked()
{
    soundPlay(SOUND_FALL_OCCUR);
}


//------------------------------------------------------------------------
void MainWindow::on_btnPlayHelp_clicked()
{
    soundPlay(SOUND_HELP);
}

//------------------------------------------------------------------------
void MainWindow::on_btnPlayIamOK_clicked()
{
    soundPlay(SOUND_IAM_OK);
}

