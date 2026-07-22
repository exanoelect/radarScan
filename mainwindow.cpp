#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "bme280worker.h"
#include "configmanager.h"
#include "cputemperatureworker.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QSharedPointer>
#include <QThread>
#include <QTime>
#include <QUrl>
#include <QtCore/QDateTime>
#include <QtMqtt/QMqttClient>
#include <QtMultimedia/QSoundEffect>

// Jika qcustomplot butuh include spesifik, sudah di header

// =============================================================================
// Application lifecycle
// =============================================================================
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug() << "Begin Setup";

    initSound();
    initGraphics();
    initSocketIO();
    initRadar();
    initBME280();
    initCpuTemp();
    initPzem();
    initUtility();

    fallEventAckReceived = false;
#ifdef Q_OS_LINUX
    m_gpio->setColor(COLOR_WHITE);
    //requestPWM(15);
#endif
}

// -----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    if (m_serial) {
        if (m_serial->isOpen())
            m_serial->close();
        m_serial->deleteLater();
        m_serial = nullptr;
    }

    if (m_audioThread) {
        m_audioThread->quit();
        m_audioThread->wait();
    }

    if (m_pzem) {
        m_pzem->deleteLater();
        m_pzem = nullptr;
    }

    if (client) {
        client->deleteLater();
        client = nullptr;
    }

    if (m_worker) {
        m_worker->deleteLater();
        m_worker = nullptr;
    }

    if (m_threadA) {
        m_threadA->deleteLater();
        m_threadA = nullptr;
    }

    if (m_threadB) {
        m_threadB->deleteLater();
        m_threadB = nullptr;
    }

    if (m_procA) {
        m_procA->deleteLater();
        m_procA = nullptr;
    }

    if (m_procB) {
        m_procB->deleteLater();
        m_procB = nullptr;
    }

    if (m_audioWorker) {
        m_audioWorker->deleteLater();
        m_audioWorker = nullptr;
    }

    if (m_bmeThread) {
        m_bmeThread->quit();
        m_bmeThread->wait();
    }

    if (m_cpuTemperatureThread) {
        m_cpuTemperatureThread->quit();
        m_cpuTemperatureThread->wait();
    }
    // audio->stop();
    delete ui;
}

// =============================================================================
// Audio playback worker
// =============================================================================
void MainWindow::initSound()
{
    m_audioThread = new QThread(this);
    m_audioWorker = new AudioWorker();

    /*
     * Setelah moveToThread(), slot milik m_audioWorker akan dieksekusi
     * pada m_audioThread apabila dipanggil dengan QueuedConnection.
     */
    m_audioWorker->moveToThread(m_audioThread);

    /*
     * AudioWorker::init() dijalankan setelah audio thread mulai.
     *
     * Di dalam init(), QProcess dibuat. Karena dibuat dari audio thread,
     * QProcess juga mempunyai thread affinity ke audio thread.
     */
    connect(m_audioThread, &QThread::started, m_audioWorker, &AudioWorker::init);

    /*
     * MainWindow -> AudioWorker.
     *
     * Explicit Qt::QueuedConnection memastikan soundPlay() tidak
     * menjalankan enqueueSound() langsung di GUI thread.
     */
    connect(this, &MainWindow::requestSound, m_audioWorker, &AudioWorker::enqueueSound, Qt::QueuedConnection);

    /*
     * AudioWorker -> MainWindow.
     *
     * Karena kedua object berada di thread berbeda, AutoConnection
     * otomatis menjadi QueuedConnection.
     */
    connect(m_audioWorker, &AudioWorker::finishedPlaying, this, &MainWindow::onSoundFinished);

    connect(m_audioWorker, &AudioWorker::playbackFailed, this, &MainWindow::onSoundFailed);

    /*
     * Worker dihapus setelah audio thread selesai.
     */
    connect(m_audioThread, &QThread::finished, m_audioWorker, &QObject::deleteLater);

    /*
     * Agar pointer tidak dipakai lagi setelah object dihancurkan.
     */
    connect(m_audioWorker, &QObject::destroyed, this, [this]() {
        m_audioWorker = nullptr;
    });

    m_audioThread->start();
}

// =============================================================================
// Socket.IO and event worker
// =============================================================================
void MainWindow::initSocketIO()
{
    client = new SocketIOClient();

    connect(client, &SocketIOClient::eventReceived, this, &MainWindow::onSocketEventReceived);

    // connect(client, &SocketIOClient::deviceready,
    //         this,   &SocketIOClient:sendDeviceReady);

    // Worker setup
    m_workerThread = new QThread(this);
    m_worker = new SocketEventWorker();
    m_worker->moveToThread(m_workerThread);

    connect(m_workerThread, &QThread::started, m_worker, &SocketEventWorker::process);

    // Hubungkan signal worker ke aksi UI / device
    connect(m_worker, &SocketEventWorker::modeListen, this, &MainWindow::onListenStateChanged);
    connect(m_worker, &SocketEventWorker::modeTalking, this, &MainWindow::onTalkingStateChanged);
    connect(m_worker, &SocketEventWorker::modeWaiting, this, &MainWindow::onWaiting);
    connect(m_worker, &SocketEventWorker::modeRecording, this, &MainWindow::onRecording);
    connect(m_worker, &SocketEventWorker::modeUploadFailed, this, &MainWindow::onUploadFailed);
    connect(m_worker, &SocketEventWorker::volumeGetRequested, this, &MainWindow::onVolumeGetRequested);
    connect(m_worker, &SocketEventWorker::volumeSetRequested, this, &MainWindow::onVolumeSetRequested);
    connect(m_worker, &SocketEventWorker::pingDeviceUp, this, &MainWindow::onPingDeviceUpRequested);
    connect(m_worker, &SocketEventWorker::modeSleep, this, &MainWindow::onSleepRequested);
    connect(m_worker, &SocketEventWorker::modeWakeUp, this, &MainWindow::onWakeUpRequested);
    connect(m_worker, &SocketEventWorker::speechModuleReady, this, &MainWindow::onSpeechModuleReady);
    connect(
        m_worker, &SocketEventWorker::brightnessSetRequested, this, &MainWindow::onBrightnessSetRequested);

    // Add on
    connect(m_worker, &SocketEventWorker::volumeIncreaseReq, this, &MainWindow::onVolumeIncreaseReq);
    connect(m_worker, &SocketEventWorker::volumeDecreaseReq, this, &MainWindow::onVolumeDecreaseReq);
    connect(m_worker, &SocketEventWorker::brightnessIncreaseReq, this, &MainWindow::onBrihtnessIncreaseReq);
    connect(m_worker, &SocketEventWorker::brightnessDecreaseReq, this, &MainWindow::onBrightnessDecreaseReq);
    connect(
        m_worker, &SocketEventWorker::brightnessGetRequested, this, &MainWindow::onBrightnessGetRequested);

    // Fall
    connect(m_worker, &SocketEventWorker::incidentHelp, this, &MainWindow::onIncidentIamnotOK);
    connect(m_worker, &SocketEventWorker::incidentFallIamOK, this, &MainWindow::onIncidentIamOK);
    connect(m_worker,
            &SocketEventWorker::incidentFallEventDetected,
            this,
            &::MainWindow::onIncidentFallEventDetected);
    connect(m_worker,
            &SocketEventWorker::incidentAckFallEventDetected,
            this,
            &MainWindow::onIncidentAckFallEventDetected);
    connect(m_worker,
            &SocketEventWorker::incidentFallWakeUpByFallDetection,
            this,
            &MainWindow::onIncidentFallWakeUpByFallDetection);
    connect(
        m_worker, &SocketEventWorker::incidentFallNoResponse, this, &MainWindow::onIncidentFallNoResponse);
    connect(m_worker,
            &SocketEventWorker::incidentFallHelpEventDetected,
            this,
            &MainWindow::onIncidentFallHelpEventDetected);
    connect(m_worker,
            &SocketEventWorker::incidentFallOKEventDetected,
            this,
            &MainWindow::onIncidentFallOKEventDetected);
    connect(m_worker, &SocketEventWorker::incidentFallCompleted, this, &MainWindow::onIncidentFallCompleted);

    // Get Language current
    connect(m_worker, &SocketEventWorker::langCurrent, this, &MainWindow::onlangCurrent);

    // Wifi
#ifdef Q_OS_LINUX
    connect(m_worker, &SocketEventWorker::wifiOn, this, &MainWindow::onWifiOnRequest);   // Async
    connect(m_worker, &SocketEventWorker::wifiOff, this, &MainWindow::onWifiOffRequest); // Async
    connect(m_worker,
            &SocketEventWorker::wifiScanSsidReqReceived,
            this,
            &MainWindow::onwifiScanSsidReqReceived); // Async

    // connect(m_worker, &SocketEventWorker::wifiGetSsid,
    //         this, &MainWindow::onWifiGetSsidRequest);  //Async
    connect(m_worker, &SocketEventWorker::wifiGetSsid, this, &MainWindow::onWifiGetSsidRequest); // Async
    connect(m_worker,
            &SocketEventWorker::wifiSsidListComplete,
            this,
            &MainWindow::onWifiSsidListRequestComplete); // Async
    connect(m_worker, &SocketEventWorker::wifiForget, this, &MainWindow::onWifiForgetRequest);
    connect(m_worker, &SocketEventWorker::wifiConnect, this, &MainWindow::onWifiConnectRequest); // Async
    connect(m_worker,
            &SocketEventWorker::wifiDisconnectCurrentSsid,
            this,
            &MainWindow::onWifiDisconnectRequest); // Async

    // Utility
    connect(m_worker, &SocketEventWorker::rpiRestart, this, &MainWindow::onRpiRestart);
    connect(m_worker, &SocketEventWorker::rpiShutdown, this, &MainWindow::onRpiShutdown);
    connect(m_worker, &SocketEventWorker::tzSetReq, this, &MainWindow::onTzSetReq);
    connect(m_worker, &SocketEventWorker::tzGetReq, this, &MainWindow::onTzGetReq);

    connect(m_worker, &SocketEventWorker::powerInfoRequest, this, &MainWindow::onPowerInfoReq);
    connect(m_worker, &SocketEventWorker::audioRadarInfoRequest, this, &MainWindow::onAudioInfoReq);

#endif

    m_workerThread->start();

    QString serverIp =
        ConfigManager::getServerIp(); //"203.194.114.21"; ////"https://elderly-care-socket-io-server.online";
    int serverPort = ConfigManager::getServerPort(); // 4000;//

    qDebug() << "Server IP:" << serverIp;
    qDebug() << "Server Port:" << serverPort;

    // client->connectToServer("192.168.1.27", 3000);
    // client->connectToServer(serverIp);//, serverPort);
    client->connectToServer(serverIp, serverPort);

#ifdef Q_OS_LINUX
    connect(client, &SocketIOClient::connected, this, &MainWindow::onCurrentSSidRequest);
#endif
}

// =============================================================================
// Graphics initialization
// =============================================================================
void MainWindow::initGraphics()
{
    setupRealtimeDataMotion(ui->plottsgram);
    setupRealtimeDataVelocity(ui->plottsVelocity);
    setupPlotRadar(ui->plotRadar);

    setupRealtimeDataMotion2(ui->plottsgram2);
    setupRealtimeDataVelocity2(ui->plottsVelocity2);
    setupPlotRadar2(ui->plotRadar2);
}

// -----------------------------------------------------------------------------
// #ifdef Q_OS_LINUX
// =============================================================================
// Radar workers and UART processing
// =============================================================================
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

    // Retry send fall event
    timerSendFallevent = new QTimer(this);
    connect(timerSendFallevent, &QTimer::timeout, this, &MainWindow::slotTimerSendFallEvent);

    // Timer counter heartbeat
    timerHeartBeatCounter = new QTimer(this);
    connect(timerHeartBeatCounter, &QTimer::timeout, this, &MainWindow::slotTimerHeartBeat);
    timerHeartBeatCounter->start(60000);

    // Connect UI update (dipakai bersama)
    auto connectProcessor = [this](PayloadProcessor *p) {
        // =========================
        // Radar Heart Beat
        // =========================
        connect(
            p,
            &PayloadProcessor::heartBeat,
            this,
            [=](const QString &src) {
                if (src == UART_PORT0) {
                    // radar1ReportInfo = "serialRadar1Normal";
                    radar1UartHeartBeatCounter = 0;
                } else if (src == UART_PORT1) {
                    // radar2ReportInfo = "serialRadar2Normal";
                    radar2UartHeartBeatCounter = 0;
                }
            },
            Qt::QueuedConnection);

        // =========================
        // Radar point (plot posisi)
        // =========================
        connect(
            p,
            &PayloadProcessor::radarPoint,
            this,
            [=](const QString &src, double x, double y) {
                if (src == UART_PORT0) {
                    updateRadarPoint(x, y);
                    radar1UartHeartBeatCounter = 0;
                    // radar1ReportInfo = "serialRadar1Normal";
                } else if (src == UART_PORT1) {
                    updateRadarPoint2(x, y);
                    radar2UartHeartBeatCounter = 0;
                    // radar1ReportInfo = "serialRadar2Normal";
                }
            },
            Qt::QueuedConnection);

        // =========================
        // Velocity plot
        // =========================
        connect(
            p,
            &PayloadProcessor::velocityUpdate,
            this,
            [=](const QString &src, const QString &v) {
                if (src == UART_PORT0) {
                    drawRealTimeVelocity(v);
                    radar1UartHeartBeatCounter = 0;
                    // radar1ReportInfo = "serialRadar1Normal";
                } else if (src == UART_PORT1) {
                    drawRealTimeVelocity2(v);
                    radar2UartHeartBeatCounter = 0;
                    // radar2ReportInfo = "serialRadar2Normal";
                }
            },
            Qt::QueuedConnection);

        // =========================
        // Motion / ETS gram plot
        // =========================
        connect(
            p,
            &PayloadProcessor::motionUpdate,
            this,
            [=](const QString &src, const QString &motion) {
                if (src == UART_PORT0) {
                    drawRealTimeetsgram(motion);
                    radar1UartHeartBeatCounter = 0;
                    // radar1ReportInfo = "serialRadar1Normal";
                } else if (src == UART_PORT1) {
                    drawRealTimeetsgram2(motion);
                    radar2UartHeartBeatCounter = 0;
                    // radar2ReportInfo = "serialRadar2Normal";
                }
            },
            Qt::QueuedConnection);

        // =========================
        // Fall detected event
        // =========================
        connect(p, &PayloadProcessor::fallDetected, this, [=](const QString &src) {
            Q_UNUSED(src);
            // sound.stop();
            // sound.play();
#ifdef Q_OS_LINUX
            m_gpio->setColor(COLOR_RED);
            //requestPWM(45);
            fallEmergency = true;
            // radar1ReportInfo = "serialNormal";
#endif

            //if (client->isConnected()) {
                // soundPlay(SOUND_FALL_OCCUR);
                QString timestamp = QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss");
                QJsonObject obj;
                obj["datetime"] = timestamp;
                fallEventAckReceived = false;
                client->emitEventStringMsgJsoned("INCIDENT_FALL_EVENT_DETECTED", obj);

                // increase brightness
                if (m_brightness->setBrightnessPercent(90)) {
                    qDebug() << "Success Set brightness " << 70;
                } else {
                    qDebug() << "Fail Set brightness " << 70;
                }

            //} else {
            //    qDebug() << "Socket DC";
            //}
            timerSendFallevent->start(1000); // Aktifkan send fall event repeat
            soundPlay(SOUND_FALL_OCCUR, lang);
        });

        connect(p, &PayloadProcessor::fallCancel, this, [=](const QString &src) {
            Q_UNUSED(src);
            // radar1ReportInfo = "serialRadar1Normal";

            // sound.stop();
            // sound.play();

            /*soundPlay(SOUND_FALL_OCCUR, lang);
            if (client->isConnected()) {
                //soundPlay(SOUND_FALL_OCCUR);
                client->emitEventStringMsgJsoned("INCIDENT_FALL_CANCEL", "");
            } else {
                qDebug() << "Socket DC";
            }*/
        });

        // =========================
        // Debug & serial state
        // =========================
        connect(p, &PayloadProcessor::debugMessage, this, [](const QString &msg) {
            qDebug() << msg;
            // radar1ReportInfo = "serialNormal";
        });

        connect(p, &PayloadProcessor::serialOpened, this, [=](bool ok) {
            ui->btnOpenSerialPort->setEnabled(!ok);
            ui->btnLoad->setEnabled(!ok);
            // radar1ReportInfo = "serialRadar1Normal";
        });

        connect(p, &PayloadProcessor::serialError, this, [=](const QString &err) {
            qDebug() << "Serial error:" << err;
            // radar2ReportInfo = "serialRadar2Normal";
        });

        // =========================
        // UI update berbasis key + src
        // =========================
        connect(p,
                &PayloadProcessor::uiUpdate,
                this,
                [=](const QString &src, const QString &key, const QString &value) {
                    if (key == "fallDetection") {
                        if (src == UART_PORT0)
                            ui->leFallDetection->setText(value);
                        else if (src == UART_PORT1)
                            ui->leFallDetection2->setText(value);
                    } else if (key == "fallStateText") {
                        if (src == UART_PORT0)
                            ui->leFallState->setText(value);
                        else if (src == UART_PORT1)
                            ui->leFallState2->setText(value);
                    } else if (key == "fallStateColor") {
                        QString style = (value == "red") ? "background-color: red; color: yellow;"
                                                         : "background-color: green; color: black;";

                        if (src == UART_PORT0)
                            ui->leFallState->setStyleSheet(style);
                        else if (src == UART_PORT1)
                            ui->leFallState2->setStyleSheet(style);
                    } else if (key == "fallPosX") {
                        if (src == UART_PORT0)
                            ui->leFallPosX->setText(value);
                        else if (src == UART_PORT1)
                            ui->leFallPosX2->setText(value);
                    } else if (key == "fallPosY") {
                        if (src == UART_PORT0)
                            ui->leFallPosY->setText(value);
                        else if (src == UART_PORT1)
                            ui->leFallPosY2->setText(value);
                    } else if (key == "fallDuration") {
                        if (src == UART_PORT0)
                            ui->leFallDuration->setText(value);
                        else if (src == UART_PORT1)
                            ui->leFallDuration2->setText(value);
                    } else if (key == "initStatus") {
                        if (src == UART_PORT0)
                            ui->leInitComplete->setText("Inited");
                        else if (src == UART_PORT1)
                            ui->leInitComplete2->setText("Inited");
                    } else if (key == "angleX") {
                        if (src == UART_PORT0)
                            ui->leAngleXInstallation->setText(value);
                        else if (src == UART_PORT1)
                            ui->leAngleXInstallation2->setText(value);
                    } else if (key == "angleY") {
                        if (src == UART_PORT0)
                            ui->leAngleYInstallation->setText(value);
                        else if (src == UART_PORT1)
                            ui->leAngleYInstallation2->setText(value);
                    } else if (key == "angleZ") {
                        if (src == UART_PORT0)
                            ui->leAngleZInstallation->setText(value);
                        else if (src == UART_PORT1)
                            ui->leAngleZInstallation2->setText(value);
                    } else if (key == "height") {
                        if (src == UART_PORT0)
                            ui->leHeightInstallation->setText(value);
                        else if (src == UART_PORT1)
                            ui->leHeightInstallation2->setText(value);
                    } else if (key == "traceTracking") {
                        if (src == UART_PORT0)
                            ui->leTraceTracking->setText(value);
                        else if (src == UART_PORT1)
                            ui->leTraceTracking2->setText(value);
                    } else if (key == "traceNumber") {
                        if (src == UART_PORT0)
                            ui->leTraceNumber->setText(value);
                        else if (src == UART_PORT1)
                            ui->leTraceNumber2->setText(value);
                    } else if (key == "velocity") {
                        if (src == UART_PORT0)
                            ui->leVelocity->setText(value);
                        else if (src == UART_PORT1)
                            ui->leVelocity2->setText(value);
                    } else if (key == "presence") {
                        if (src == UART_PORT0)
                            ui->lePresence->setText(value);
                        else if (src == UART_PORT1)
                            ui->lePresence2->setText(value);
                    } else if (key == "motionStyle") {
                        QString style;
                        if (value == "presence")
                            style = "background-color: blue; color: yellow;";
                        else if (value == "no_presence")
                            style = "background-color: green; color: black;";
                        else if (value == "motion_high")
                            style = "background-color: grey; color: black;";
                        else if (value == "motion_low")
                            style = "background-color: orange; color: black;";

                        if (src == UART_PORT0)
                            ui->leMotion->setStyleSheet(style);
                        else if (src == UART_PORT1)
                            ui->leMotion2->setStyleSheet(style);
                    } else if (key == "motionValue") {
                        if (src == UART_PORT0)
                            ui->leMotion->setText(value);
                        else if (src == UART_PORT1)
                            ui->leMotion2->setText(value);
                    }
                });
    };

    connectProcessor(m_procA);
    connectProcessor(m_procB);

    m_threadA->start();
    m_threadB->start();

#ifdef Q_OS_LINUX
    QMetaObject::invokeMethod(m_procA, "initPort", Qt::QueuedConnection, Q_ARG(QString, UART_PORT0));

    QMetaObject::invokeMethod(m_procB, "initPort", Qt::QueuedConnection, Q_ARG(QString, UART_PORT1));
#endif
}

// #endif

// =============================================================================
// Serial-port helpers and radar commands
// =============================================================================
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

// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
QByteArray MainWindow::makeFrame(const QByteArray &body)
{
    int s = 0;
    for (unsigned char c : body)
        s += c;
    unsigned char sum = s & 0xFF;
    QByteArray frame = body;
    frame.append(static_cast<char>(sum));
    frame.append(static_cast<char>(0x54));
    frame.append(static_cast<char>(0x43));
    return frame;
}

// -----------------------------------------------------------------------------
QString MainWindow::toHexSpace(const QByteArray &data)
{
    QStringList parts;
    for (auto b : data)
        parts << QString("%1").arg((unsigned char)b, 2, 16, QChar('0')).toUpper();
    return parts.join(' ');
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnLoad_clicked()
{
    fillPortsInfo();
    ui->btnOpenSerialPort->setEnabled(true);

    // Test koordinat
    // updateRadarPoint(30, -45);
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnOpenSerialPort_clicked()
{
    if (ui->serialPortInfoListBox->currentText().isEmpty())
        return;

    const QString portName = ui->serialPortInfoListBox->currentText();
    QMetaObject::invokeMethod(m_procA, "initPort", Qt::QueuedConnection, Q_ARG(QString, portName));

    // #ifndef AUTOSTART_ONRPI
    // init_port();
    // #endif
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnLoad2_clicked()
{
    fillPortsInfo2();
    ui->btnOpenSerialPort2->setEnabled(true);

    // Test koordinat
    // updateRadarPoint(30, -45);
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnOpenSerialPort2_clicked()
{
    if (ui->serialPortInfoListBox2->currentText().isEmpty())
        return;

    const QString portName = ui->serialPortInfoListBox2->currentText();
    QMetaObject::invokeMethod(m_procB, "initPort", Qt::QueuedConnection, Q_ARG(QString, portName));

    // #ifndef AUTOSTART_ONRPI
    //     init_port2();
    // #endif
}

// -----------------------------------------------------------------------------
quint8 MainWindow::calcChecksum(const QByteArray &frame)
{
    quint16 sum = 0;
    for (char c : frame)
        sum += static_cast<quint8>(c);
    return static_cast<quint8>(sum & 0xFF);
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnGetProductID_clicked()
{
    if (!m_serial)
        return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_PRODUCT_ID);
    qDebug() << "Sending frame get Product Id:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnGetProductModel_clicked()
{
    if (!m_serial)
        return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_PRODUCT_MODEL);
    qDebug() << "Sending frame get Production:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnGetFirmwareVersion_clicked()
{
    if (!m_serial)
        return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_FIRMWARE_VERSION);
    qDebug() << "Sending frame get Firmware:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnGetProductID2_clicked()
{
    if (!m_serial2)
        return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_PRODUCT_ID);
    qDebug() << "Sending frame get Product Id2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnGetProductModel2_clicked()
{
    if (!m_serial2)
        return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_PRODUCT_MODEL);
    qDebug() << "Sending frame get Production2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnGetFirmwareVersion2_clicked()
{
    if (!m_serial2)
        return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_FIRMWARE_VERSION);
    qDebug() << "Sending frame get Firmware2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnHWModel_clicked()
{
    if (!m_serial)
        return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_HARDWARE_MODEL);
    qDebug() << "Sending frame HW Model:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnCmdInitCompleteCek_clicked()
{
    if (!m_serial)
        return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_CEK_INITIALIZATION_COMPLETE);
    qDebug() << "Sending frame Cek Init Complete:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnHWModel2_clicked()
{
    if (!m_serial2)
        return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_HARDWARE_MODEL);
    qDebug() << "Sending frame HW Model2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnCmdInitCompleteCek2_clicked()
{
    if (!m_serial2)
        return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_CEK_INITIALIZATION_COMPLETE);
    qDebug() << "Sending frame Cek Init Complete2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnGetAngleInst_clicked()
{
    if (!m_serial)
        return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_ANGLE_INST_QUERY);
    qDebug() << "Sending frame get Angle:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnGetHeightInst_clicked()
{
    if (!m_serial)
        return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_HEIGHT_INST_QUERY);
    qDebug() << "Sending frame Get Height:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnGetAngleInst2_clicked()
{
    if (!m_serial2)
        return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_ANGLE_INST_QUERY);
    qDebug() << "Sending frame get Angle2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnGetHeightInst2_clicked()
{
    if (!m_serial2)
        return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_HEIGHT_INST_QUERY);
    qDebug() << "Sending frame Get Height:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_cbPresence_toggled(bool checked)
{
    if (!m_serial)
        return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = checked ? makeFrame(CMD_SET_PRESENCE_ON) : makeFrame(CMD_SET_PRESENCE_OFF);
    qDebug() << "Sending frame set Presence:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_cbFallDetection_toggled(bool checked)
{
    if (!m_serial)
        return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = checked ? makeFrame(CMD_SET_FALL_DETECTION_ON) : makeFrame(CMD_SET_FALL_DETECTION_OFF);
    qDebug() << "Sending frame Set Fall Duration:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnGetFallDuration_clicked()
{
    if (!m_serial)
        return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_FALL_DURATION);
    qDebug() << "Sending frame Get Fall Duration:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_cbStandStill_toggled(bool checked)
{
    if (!m_serial)
        return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = checked ? makeFrame(CMD_SET_STAND_STILLON) : makeFrame(CMD_SET_STAND_STILLOFF);
    qDebug() << "Sending frame StandStill:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_cbTraceTracking_toggled(bool checked)
{
    if (!m_serial)
        return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = checked ? makeFrame(CMD_SET_TRACE_TRACKING_ON) : makeFrame(CMD_SET_TRACE_TRACKING_OFF);
    qDebug() << "Sending frame Trace Tracking:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_cbPresence2_toggled(bool checked)
{
    if (!m_serial2)
        return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = checked ? makeFrame(CMD_SET_PRESENCE_ON) : makeFrame(CMD_SET_PRESENCE_OFF);
    qDebug() << "Sending frame set Presence2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_cbFallDetection2_toggled(bool checked)
{
    if (!m_serial2)
        return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = checked ? makeFrame(CMD_SET_FALL_DETECTION_ON) : makeFrame(CMD_SET_FALL_DETECTION_OFF);
    qDebug() << "Sending frame Set Fall Duration2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnGetFallDuration2_clicked()
{
    if (!m_serial2)
        return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = makeFrame(CMD_GET_FALL_DURATION);
    qDebug() << "Sending frame Get Fall Duration2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_cbStandStill2_toggled(bool checked)
{
    if (!m_serial2)
        return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = checked ? makeFrame(CMD_SET_STAND_STILLON) : makeFrame(CMD_SET_STAND_STILLOFF);
    qDebug() << "Sending frame StandStill2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_cbTraceTracking2_toggled(bool checked)
{
    if (!m_serial2)
        return;
    m_serial2->clear(QSerialPort::Input);
    QByteArray frame = checked ? makeFrame(CMD_SET_TRACE_TRACKING_ON) : makeFrame(CMD_SET_TRACE_TRACKING_OFF);
    qDebug() << "Sending frame Trace Tracking2:" << toHexSpace(frame);
    m_serial2->write(frame);
    m_serial2->flush();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnSetHeight_clicked()
{
    if (!m_serial)
        return;
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

// -----------------------------------------------------------------------------
void MainWindow::on_btnSetHeight2_clicked()
{
    if (!m_serial2)
        return;
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

// -----------------------------------------------------------------------------
void MainWindow::on_btnSetFallDuration_clicked()
{
    if (!m_serial)
        return;
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

// -----------------------------------------------------------------------------
void MainWindow::on_btnSetAngle_clicked()
{
    if (!m_serial)
        return;
    m_serial->clear(QSerialPort::Input);

    uint16_t angleX = ui->leAngleX->text().toUInt();
    uint16_t angleY = ui->leAngleY->text().toUInt();
    uint16_t angleZ = ui->leAngleZ->text().toUInt();

    QByteArray cmd = CMD_SET_ANGLE_INST; // ini harus berisi 06 01 sesuai protokol kamu

    // X axis (2 byte big-endian)
    cmd.append(static_cast<char>((angleX >> 8) & 0xFF)); // HB
    cmd.append(static_cast<char>(angleX & 0xFF));        // LB

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

// -----------------------------------------------------------------------------
void MainWindow::on_btnSetFallDuration2_clicked()
{
    if (!m_serial2)
        return;
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

// -----------------------------------------------------------------------------
void MainWindow::on_btnSetAngle2_clicked()
{
    if (!m_serial2)
        return;
    m_serial2->clear(QSerialPort::Input);

    uint16_t angleX = ui->leAngleX2->text().toUInt();
    uint16_t angleY = ui->leAngleY2->text().toUInt();
    uint16_t angleZ = ui->leAngleZ2->text().toUInt();

    QByteArray cmd = CMD_SET_ANGLE_INST; // ini harus berisi 06 01 sesuai protokol kamu

    // X axis (2 byte big-endian)
    cmd.append(static_cast<char>((angleX >> 8) & 0xFF)); // HB
    cmd.append(static_cast<char>(angleX & 0xFF));        // LB

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

// =============================================================================
// Radar and real-time plots
// =============================================================================
void MainWindow::updateRadarPoint(double x, double y)
{
    radarPoint->data()->clear();
    radarPoint->addData(x, y);

    // ui->plotRadar->rescaleAxes();
    ui->plotRadar->replot();
}

// -----------------------------------------------------------------------------
void MainWindow::init_radar() {}

// -----------------------------------------------------------------------------
void MainWindow::updateRadarPoint2(double x, double y)
{
    radarPoint2->data()->clear();
    radarPoint2->addData(x, y);

    // ui->plotRadar2->rescaleAxes();
    ui->plotRadar2->replot();
}

// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
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
    if (!replotTimer.isValid())
        replotTimer.start();

    if (replotTimer.elapsed() >= 50) { // Refresh setiap 50 ms
        ui->plottsgram->replot();
        replotTimer.restart();
    }
}

// -----------------------------------------------------------------------------
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
    if (!replotTimer.isValid())
        replotTimer.start();

    if (replotTimer.elapsed() >= 50) { // Refresh setiap 50 ms
        ui->plottsVelocity->replot();
        replotTimer.restart();
    }
}

// -----------------------------------------------------------------------------
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
    if (!replotTimer.isValid())
        replotTimer.start();

    if (replotTimer.elapsed() >= 50) { // Refresh setiap 50 ms
        ui->plottsgram2->replot();
        replotTimer.restart();
    }
}

// -----------------------------------------------------------------------------
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
    if (!replotTimer.isValid())
        replotTimer.start();

    if (replotTimer.elapsed() >= 50) { // Refresh setiap 50 ms
        ui->plottsVelocity2->replot();
        replotTimer.restart();
    }
}

// -----------------------------------------------------------------------------
void MainWindow::drawRealTimeetsgram(QString motion)
{
    realtimeDataSlot(motion);
}

// -----------------------------------------------------------------------------
void MainWindow::drawRealTimeVelocity(QString velocity)
{
    realtimeDataVelocity(velocity);
}

// -----------------------------------------------------------------------------
void MainWindow::drawRealTimeetsgram2(QString motion)
{
    realtimeDataSlot2(motion);
}

// -----------------------------------------------------------------------------
void MainWindow::drawRealTimeVelocity2(QString velocity)
{
    realtimeDataVelocity2(velocity);
}

// -----------------------------------------------------------------------------
void MainWindow::soundPlay(int request, const QString &lang)
{
    QString requestName;

    switch (request) {
        case SOUND_FALL_OCCUR:
            requestName = QStringLiteral("SOUND_FALL_OCCUR");
            break;

        case SOUND_HELP:
            requestName = QStringLiteral("SOUND_HELP");
            break;

        case SOUND_IAM_OK:
            requestName = QStringLiteral("SOUND_IAM_OK");
            break;

        case SOUND_RECORD:
            requestName = QStringLiteral("SOUND_RECORD");
            break;

        case SOUND_WAITING:
            requestName = QStringLiteral("SOUND_WAITING");
            break;

        case SOUND_HELPYOU:
            requestName = QStringLiteral("SOUND_HELPYOU");
            break;

        case SOUND_LOGIN:
            requestName = QStringLiteral("SOUND_LOGIN");
            break;

        case SOUND_UPLOAD_FAILED:
            requestName = QStringLiteral("SOUND_UPLOAD_FAILED");
            break;

        default:
            qWarning() << "Unknown sound request:" << request;
            return;
    }

    /*
     * requestToFile() hanya menerima:
     * - sv
     * - id
     * - en
     */
    if ((lang != "sv") && (lang != "id") && (lang != "en")) {
        qWarning() << "Unsupported audio language:" << lang;
        return;
    }

    if (!m_audioThread || !m_audioThread->isRunning() || !m_audioWorker) {
        qWarning() << "Audio worker is not running";
        return;
    }

    qDebug() << "Sound request received:" << requestName << "id:" << request << "lang:" << lang;

    // Emit event PLAYING_SOUND
    //if (client->isConnected()) {
        QJsonObject obj;
        client->emitEventStringMsgJsoned("PLAYING_SOUND", obj);
    //}

    /*
     * Delay 500 ms secara non-blocking.
     *
     * Event loop Qt tetap berjalan.
     * GUI tidak freeze dan thread tidak ditahan.
     */
    QTimer::singleShot(1000, this, [this, request, lang, requestName]() {
        /*
         * Periksa kembali karena selama delay 500 ms,
         * thread atau worker mungkin sudah dihentikan.
         */
        if (!m_audioThread || !m_audioThread->isRunning() || !m_audioWorker) {
            qWarning() << "Audio worker is no longer running:" << requestName;
            return;
        }

        qDebug() << "Sending delayed sound request:" << requestName << "id:" << request << "lang:" << lang;

        emit requestSound(request, lang);
    });
}

// -----------------------------------------------------------------------------
QString MainWindow::runCommand(const QString &cmd)
{
    QProcess process;
    process.start("bash", QStringList() << "-c" << cmd);
    process.waitForFinished();
    //  connect(proc, &QProcess::finished, proc, &QObject::deleteLater);

    QString output = process.readAllStandardOutput();
    QString error = process.readAllStandardError();

    if (!error.isEmpty()) {
        qDebug() << "Error:" << error;
    }

    return output.trimmed();
}

// -----------------------------------------------------------------------------
void MainWindow::startRecording()
{
    /*
    qDebug() << "Recording WAV started";
    recordedDataSize = 0;

    QAudioDevice inputDevice = QMediaDevices::defaultAudioInput();

    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    if (!inputDevice.isFormatSupported(format)) {
        qDebug() << "⚠️ Using preferred format";
        format = inputDevice.preferredFormat();
    }

    audi = new QAudioSource(inputDevice, format, this);
    audi->setBufferSize(8192);

    file.setFileName("record.wav");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "❌ File open failed";
        return;
    }

    // ✅ Header WAV valid
    WavHeader header;

    header.numChannels = format.channelCount();
    header.sampleRate = format.sampleRate();
    header.bitsPerSample = 16;

    header.byteRate = header.sampleRate * header.numChannels * header.bitsPerSample / 8;
    header.blockAlign = header.numChannels * header.bitsPerSample / 8;

    header.dataSize = 0;
    header.fileSize = 36 + header.dataSize;

    // ✅ Tulis header awal
    file.write(reinterpret_cast<const char*>(&header), sizeof(WavHeader));

    ioDevice = audi->start();

    connect(ioDevice, &QIODevice::readyRead,
            this, &MainWindow::handleAudioData);
*/
}

// -----------------------------------------------------------------------------

void MainWindow::handleAudioData()
{
    /*
    QByteArray data = ioDevice->readAll();

    file.write(data);

    header.dataSize += data.size();
*/
}

// -----------------------------------------------------------------------------
/*
void MainWindow::handleStateChanged(QAudio::State state)
{
    if (state == QAudio::StoppedState) {
        if (audi->error() != QAudio::NoError) {
            qDebug() << "❌ Audio error:" << audi->error();
        } else {
            qDebug() << "✅ Recording stopped normally";
        }
    }
}
*/

// -----------------------------------------------------------------------------
void MainWindow::stopRecording()
{
    /*
    audi->stop();

    // Update header
    file.seek(0);

    WavHeader header;

    header.numChannels = 1;
    header.sampleRate = 44100;
    header.bitsPerSample = 16;

    header.byteRate = header.sampleRate * header.numChannels * header.bitsPerSample / 8;
    header.blockAlign = header.numChannels * header.bitsPerSample / 8;

    header.dataSize = recordedDataSize;
    header.fileSize = 36 + header.dataSize;

    file.write(reinterpret_cast<const char*>(&header), sizeof(WavHeader));

    file.close();

    qDebug() << "Recording stopped, file size:" << recordedDataSize;
*/
}

// -----------------------------------------------------------------------------
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
    // connect(plottsgram->xAxis, &QCPAxis::rangeChanged, plottsgram->xAxis2, &QCPAxis::setRange);
    // connect(plottsgram->yAxis, &QCPAxis::rangeChanged, plottsgram->yAxis2, &QCPAxis::setRange);
    // Synchronize top/right axes with bottom/left
    // plottsgram->xAxis->setRangeReplotPolicy(QCPAxis::rpImmediate);
    plottsgram->xAxis2->setRange(plottsgram->xAxis->range());
    plottsgram->yAxis2->setRange(plottsgram->yAxis->range());

    // Set update function saat plot direplot
    // Sinkronisasi top/right axes dengan bottom/left
    connect(plottsgram, &QCustomPlot::afterReplot, [=]() {
        plottsgram->xAxis2->setRange(plottsgram->xAxis->range());
        plottsgram->yAxis2->setRange(plottsgram->yAxis->range());
    });

    // Vertical Text - left blank intentionally
}

// -----------------------------------------------------------------------------
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
    connect(ui->plottsVelocity, &QCustomPlot::afterReplot, [=]() {
        ui->plottsVelocity->xAxis2->setRange(ui->plottsVelocity->xAxis->range());
        ui->plottsVelocity->yAxis2->setRange(ui->plottsVelocity->yAxis->range());
    });

    // Vertical Text - left blank intentionally
}

// -----------------------------------------------------------------------------
void MainWindow::setupPlotRadar(QCustomPlot *plotRadar)
{
    // ----------------------------------------------------------------------------- Inisialisasi plotRadar --------------------------
    ui->plotRadar->xAxis->setVisible(true); // tampilkan axes
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
    radarPoint->setScatterStyle(
        QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::red), QBrush(Qt::red), 8));
}

// -----------------------------------------------------------------------------
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
    // connect(plottsgram->xAxis, &QCPAxis::rangeChanged, plottsgram->xAxis2, &QCPAxis::setRange);
    // connect(plottsgram->yAxis, &QCPAxis::rangeChanged, plottsgram->yAxis2, &QCPAxis::setRange);
    // Synchronize top/right axes with bottom/left
    // plottsgram->xAxis->setRangeReplotPolicy(QCPAxis::rpImmediate);
    plottsgram2->xAxis->setRange(plottsgram2->xAxis2->range());
    plottsgram2->yAxis->setRange(plottsgram2->yAxis2->range());

    // Set update function saat plot direplot
    // Sinkronisasi top/right axes dengan bottom/left
    connect(plottsgram2, &QCustomPlot::afterReplot, [=]() {
        plottsgram2->xAxis->setRange(plottsgram2->xAxis->range());
        plottsgram2->yAxis->setRange(plottsgram2->yAxis->range());
    });

    // Vertical Text - left blank intentionally
}

// -----------------------------------------------------------------------------
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
    connect(ui->plottsVelocity2, &QCustomPlot::afterReplot, [=]() {
        ui->plottsVelocity2->xAxis->setRange(ui->plottsVelocity2->xAxis->range());
        ui->plottsVelocity2->yAxis->setRange(ui->plottsVelocity2->yAxis->range());
    });

    // Vertical Text - left blank intentionally
}

// -----------------------------------------------------------------------------
void MainWindow::setupPlotRadar2(QCustomPlot *plotRadar2)
{
    // ----------------------------------------------------------------------------- Inisialisasi plotRadar --------------------------
    ui->plotRadar2->xAxis->setVisible(true); // tampilkan axes
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
    radarPoint2->setScatterStyle(
        QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::red), QBrush(Qt::red), 8));
}

// =============================================================================
// GUI controls and robot states
// =============================================================================
void MainWindow::on_btnPlaySound_clicked()
{
    // soundPlay(SOUND_FALL_OCCUR, lang);
    soundPlay(SOUND_WAITING, lang);
}

// -----------------------------------------------------------------------------
void MainWindow::onSocketEventReceived(const QString &eventName, const QJsonValue &data)
{
    qDebug() << "UI received event:" << eventName << "data:" << data;
    // m_eventQueue.enqueue(qMakePair(eventName, data));

    qDebug() << "UI received event:" << eventName << "data:" << data;
    m_worker->enqueue(eventName, data);
}

// -----------------------------------------------------------------------------
void MainWindow::onDeviceReadyConnected(int vol, int bright) {}

// -----------------------------------------------------------------------------
void MainWindow::on_btnColor1_clicked()
{
#ifdef Q_OS_LINUX
    //m_gpio->setColor(COLOR_WHITE);
    if(!fallEmergency) m_gpio->setColor(COLOR_WHITE); //requestPWM(15);
#endif
#ifdef MQTT_FITUR
    if (publishMessage("ledcolor", "white")) {
        qDebug() << "white ok";
    } else {
        qDebug() << "white fail";
    }
#endif
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnColor2_clicked()
{
#ifdef Q_OS_LINUX
    //m_gpio->setColor(COLOR_WHITE_BLINKY);
    if(!fallEmergency) m_gpio->setColor(COLOR_WHITE_BLINKY); //requestPWM(25);
#endif
#ifdef MQTT_FITUR
    if (publishMessage("ledcolor", "blinky")) {
        qDebug() << "blinky ok";
    } else {
        qDebug() << "blinky fail";
    }
#endif
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnColor3_clicked()
{
#ifdef Q_OS_LINUX
    //m_gpio->setColor(COLOR_WHITE_BRIGHT);
    if(!fallEmergency) m_gpio->setColor(COLOR_WHITE_BRIGHT); //requestPWM(35);
#endif
#ifdef MQTT_FITUR
    if (publishMessage("ledcolor", "bright")) {
        qDebug() << "bright ok";
    } else {
        qDebug() << "bright fail";
    }
#endif
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnColor4_clicked()
{
#ifdef Q_OS_LINUX
   // m_gpio->setColor(COLOR_RED);
    if(!fallEmergency) m_gpio->setColor(COLOR_RED); //requestPWM(45);
#endif
#ifdef MQTT_FITUR
    if (publishMessage("ledcolor", "red")) {
        qDebug() << "red ok";
    } else {
        qDebug() << "red fail";
    }
#endif
}

// -----------------------------------------------------------------------------
void MainWindow::on_hsBrightness_valueChanged(int value)
{
    ui->lBrightness->setText(QString::number(value));
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnGetBrightness_clicked()
{
// ui->leBrightness->setText(QString::number(m_brightness->getBrightness()));
#ifdef Q_OS_LINUX
    ui->leBrightness->setText(QString::number(m_brightness->getBrightnessPercent()));
#endif
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnsetBrightness_clicked()
{
#ifdef Q_OS_LINUX
    if (m_brightness->setBrightnessPercent(ui->hsBrightness->value())) {
        qDebug() << "Success Set brightness " << ui->hsBrightness->value();
    } else {
        qDebug() << "Fail Set brightness " << ui->hsBrightness->value();
    }
#endif
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnGetVol_clicked()
{
    // ui->leVol->setText(QString::number(m_volume->getVolumePercent()));
    // QString volStr = QString::number(m_volume->getVolumePercent());
    // ui->leVol->setText(volStr);
    // ui->hsVol->setValue(volStr.toInt());
}

// -----------------------------------------------------------------------------
void MainWindow::on_hsVol_valueChanged(int value)
{
    ui->lVol->setText(QString::number(value));
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnsetVol_clicked()
{
    // if(m_volume->setVolumePercent(ui->hsVol->value())){
    /*if(m_volumeMonitor->setVolumePercent(ui->hsVol->value())){
        qDebug() << "Success Set Volume " << ui->hsVol->value();
        ui->leVol->setText(QString::number(ui->hsVol->value()));
    }else{
        qDebug() << "Fail Set Volume " << ui->hsVol->value();
    }
    */

    qDebug() << "Hs Set Volume " << ui->hsVol->value();
#ifdef Q_OS_LINUX
    m_volumeMonitor->setVolumePercent(ui->hsVol->value());
#endif
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnConnect_clicked()
{
    client->connectToServer("192.168.1.100", 3000);
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnFallSimulation_clicked()
{
    //if (client->isConnected()) {
        QString timestamp = QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss");
        QJsonObject obj;
        obj["datetime"] = timestamp;
        client->emitEventStringMsgJsoned("INCIDENT_FALL_EVENT_DETECTED", obj);
        fallEventAckReceived = false;
#ifdef Q_OS_LINUX
        //m_gpio->setColor(COLOR_RED);
        m_gpio->setColor(COLOR_RED); //requestPWM(45);
#endif
        fallEmergency = true;
        timerSendFallevent->start(1000); // Aktifkan send fall event repeat
        soundPlay(SOUND_FALL_OCCUR, lang);
    //} else {
    //    qDebug() << " Socket DC";
    //}
}

// -----------------------------------------------------------------------------
void MainWindow::onListenStateChanged()
{
    qDebug() << "UI Process LISTENING"; // << state;
#ifdef Q_OS_LINUX
    //m_gpio->setColor(COLOR_WHITE);
    if(!fallEmergency) m_gpio->setColor(COLOR_WHITE); //requestPWM(15);
#endif
#ifdef MQTT_FITUR
    if (publishMessage("ledcolor", "bright")) {
        qDebug() << "bright ok";
    } else {
        qDebug() << "bright fail";
    }
#endif

    // if (state == "ON") //m_gpio->setColor(COLOR_WHITE_BRIGHT);
    // else if (state == "OFF") //m_gpio->setColor(COLOR_WHITE);
}

// -----------------------------------------------------------------------------
void MainWindow::onTalkingStateChanged()
{
    qDebug() << "UI Process TALKING"; // << state;
#ifdef Q_OS_LINUX
    //m_gpio->setColor(COLOR_WHITE_BLINKY);
    if(!fallEmergency) m_gpio->setColor(COLOR_WHITE_BLINKY); //requestPWM(35);
#endif
#ifdef MQTT_FITUR
    if (publishMessage("ledcolor", "blinky")) {
        qDebug() << "blinky ok";
    } else {
        qDebug() << "blinky fail";
    }
#endif
    /*
    if (state == "ON") {
        qDebug() << "Talking on:" << state;
        //m_gpio->setColor(COLOR_WHITE_BLINKY);
    }
    else if (state == "OFF") {
        qDebug() << "Talking off:" << state;
        //m_gpio->setColor(COLOR_WHITE);
    }
    */
}

// -----------------------------------------------------------------------------
void MainWindow::onWaiting()
{
    qDebug() << "UI Process Waiting"; // << state;
#ifdef Q_OS_LINUX
    //m_gpio->setColor(COLOR_WHITE_BLINKY);
    if(!fallEmergency) m_gpio->setColor(COLOR_WHITE_BLINKY); //requestPWM(35);
    soundPlay(SOUND_WAITING, lang);
#endif
}

// -----------------------------------------------------------------------------
void MainWindow::onRecording()
{
    qDebug() << "UI Process Waiting"; // << state;
#ifdef Q_OS_LINUX
    //m_gpio->setColor(COLOR_WHITE_BRIGHT);
    if(!fallEmergency) m_gpio->setColor(COLOR_WHITE_BRIGHT); //requestPWM(25);
#endif
}

// -----------------------------------------------------------------------------
void MainWindow::onVolumeGetRequested()
{
    qDebug() << "UI Process VOL get req";
    // int currentVol = m_volume->getVolumePercent();
    client->emitEventStringMsgJsoned("VOLUME_SET_REQUEST",
                                     QString::number(m_volCurrent)); // QString::number(currentVol));
}

// -----------------------------------------------------------------------------
void MainWindow::onVolumeSetRequested(int vt)
{
    qDebug() << "UI vol Set:" << vt;
    // if (vt > 0 && m_volume->setVolumePercent(vt)) {
#ifdef Q_OS_LINUX
    if (vt > 0) {
        m_volumeMonitor->setVolumePercent(vt);
        qDebug() << "UI vol successfully set to" << vt;
    } else {
        qDebug() << "fail setVol" << vt;
    }
#endif
}

// -----------------------------------------------------------------------------
void MainWindow::onPingDeviceUpRequested()
{
    qDebug() << "UI PingDeviceUpReq";
#ifdef Q_OS_LINUX
    int brightGet = m_brightness->setBrightnessPercent(80);
    client->emitEventStringMsgJsoned("PING_DEVICE_UP_FRONTEND", QString::number(brightGet));
#else
    client->emitEventStringMsgJsoned("PING_DEVICE_UP_FRONTEND", "70");
#endif
}

// -----------------------------------------------------------------------------
void MainWindow::onSleepRequested()
{
    qDebug() << "UI SleepReq";
#ifdef Q_OS_LINUX
    int getBright = m_brightness->getBrightnessPercent();
    client->emitEventStringMsgJsoned("SLEEP_FRONTEND", QString::number(getBright));

    // Reduce brightness
    if(m_brightness->setBrightnessPercent(10)){
        qDebug() << "Success Set brightness " << 1;
    }else{
        qDebug() << "Fail Set brightness " << 1;
    }

    //REduce led
    //if(!fallEmergency) requestPWM(0);

    // Reduce Volume
    // m_volumeMonitor->setVolumePercent(30);
    // qDebug() << "Vol set to " << 30;
#endif
}

// -----------------------------------------------------------------------------
void MainWindow::onWakeUpRequested()
{
    qDebug() << "UI Wakeup";
#ifdef Q_OS_LINUX
    soundPlay(SOUND_LOGIN, lang);

    int getBright = m_brightness->getBrightnessPercent();
    client->emitEventStringMsgJsoned("WAKE_UP", QString::number(getBright));

    // increase brightness
    if (m_brightness->setBrightnessPercent(90)) {
        qDebug() << "Success Set brightness " << 70;
    } else {
        qDebug() << "Fail Set brightness " << 70;
    }

    soundPlay(SOUND_HELPYOU, lang);

    //increase led
    //if(!fallEmergency) requestPWM(15);

#endif

    // increase Volume
    // m_volumeMonitor->setVolumePercent(98);
    // qDebug() << "Vol set to " << 98;
}

// -----------------------------------------------------------------------------
void MainWindow::onSpeechModuleReady()
{
    qDebug() << "Speech Module Ready notify";
#ifdef Q_OS_LINUX
   // m_gpio->setColor(COLOR_WHITE);
    if(!fallEmergency) m_gpio->setColor(COLOR_WHITE); //requestPWM(15);
#endif
#ifdef MQTT_FITUR
    if (publishMessage("ledcolor", "white")) {
        qDebug() << "white ok";
    } else {
        qDebug() << "white fail";
    }
#endif
}

// -----------------------------------------------------------------------------
void MainWindow::onBrightnessSetRequested(int bst)
{
    qDebug() << "Brightness Set:" << bst;
#ifdef Q_OS_LINUX
    if (bst > 0 && m_brightness->setBrightnessPercent(bst)) {
        qDebug() << "Brightness successfully set to" << bst;
        bst = m_brightness->getBrightnessPercent();
        qDebug() << "prepare emit brightness set ack " << bst;
        // client->emitEventStringMsg("BRIGHTNESS_GET_ACK",QString::number(bst));
    }
#endif
}

// -----------------------------------------------------------------------------
void MainWindow::onBrightnessGetRequested()
{
    qDebug() << "UI Brightness get";
#ifdef Q_OS_LINUX
    int bst = m_brightness->getBrightnessPercent();
    // if (bst > 0 && setBrightnessPercent(bst)) {
    qDebug() << "UI emit Brightness get start" << bst;
    client->emitEventStringMsgJsoned("SCREEN_BRIGHTNESS_REQUEST", QString::number(bst));
    qDebug() << "UI emit Brightness get end" << bst;
#endif
    client->emitEventStringMsgJsoned("SCREEN_BRIGHTNESS_REQUEST", "70");
    //}
}

// -----------------------------------------------------------------------------
void MainWindow::onVolumeIncreaseReq()
{
    qDebug() << "UI onVolumeIncreaseReq";
    int currentVol = m_volCurrent; // m_volume->getVolumePercent();
    currentVol = currentVol + 5;
    if ((currentVol > 20) && (currentVol <= 99)) {
        // if(m_volumeMonitor->setVolumePercent(currentVol)){
#ifdef Q_OS_LINUX
        m_volumeMonitor->setVolumePercent(currentVol);
        qDebug() << "UI succes Inc Vol " << currentVol;
#endif
    }
}

// -----------------------------------------------------------------------------
void MainWindow::onVolumeDecreaseReq()
{
    qDebug() << "UI onVolumeDecreaseReq";
    int currentVol = m_volCurrent; // m_volume->getVolumePercent();
    currentVol = currentVol - 5;
    if ((currentVol > 20) && (currentVol <= 99)) {
        // if(m_volumeMonitor->setVolumePercent(currentVol)){
#ifdef Q_OS_LINUX
        m_volumeMonitor->setVolumePercent(currentVol);
#endif
        qDebug() << "UI succes Inc Vol " << currentVol;
    }
}

// -----------------------------------------------------------------------------
void MainWindow::onVolumeChanged(int percent)
{
    m_volCurrent = percent;
    ui->hsVol->setValue(percent);
    ui->leVol->setText(QString::number(percent));
}

// -----------------------------------------------------------------------------
/*void MainWindow::onVolumeChanged(int percent)
{
    qDebug() << "Volume changed cuk:" << percent;
    ui->leVol->setText(QString::number(percent) + "kod");
}
*/

// -----------------------------------------------------------------------------
void MainWindow::onBrihtnessIncreaseReq()
{
    qDebug() << "UI onBrihtnessIncreaseReq";
#ifdef Q_OS_LINUX
    int currentBrightness = m_brightness->getBrightnessPercent();
    currentBrightness = currentBrightness + 5;
    if ((currentBrightness > 20) && (currentBrightness <= 100)) {
        if (m_brightness->setBrightnessPercent(currentBrightness)) {
            qDebug() << "UI succes Inc Brightness " << currentBrightness;
        } else {
            qDebug() << "UI fail Inc  " << currentBrightness;
        }
    } else {
        qDebug() << "UI fail Inc out of range  " << currentBrightness;
    }
#endif
}

// -----------------------------------------------------------------------------
void MainWindow::onBrightnessDecreaseReq()
{
    qDebug() << "UI onBrightnessDecreaseReq";
#ifdef Q_OS_LINUX
    int currentBrightness = m_brightness->getBrightnessPercent();
    currentBrightness = currentBrightness - 5;
    if ((currentBrightness > 20) && (currentBrightness <= 100)) {
        if (m_brightness->setBrightnessPercent(currentBrightness)) {
            qDebug() << "UI succes Inc Brightness " << currentBrightness;
        } else {
            qDebug() << "UI fail Inc v " << currentBrightness;
        }
    } else {
        qDebug() << "UI fail Inc out of range  " << currentBrightness;
    }
#endif
}

// =============================================================================
//      GPIO TIMER
// =============================================================================
void MainWindow::slotGpioTimer()
{
    if (!m_gpio) {
        return;
    }

    const qint64 elapsedUs = gpioElapsedTimer.nsecsElapsed() / 1000;

    const qint64 positionUs = elapsedUs % PWM_PERIOD_US;

    bool requestedState;

    if(pwmDutyPercent == 0){
        requestedState = false;
    }else if(pwmDutyPercent >= 100){
        requestedState = true;
    }else{
        requestedState = positionUs < pwmHighTimeUs;
    }

    if(requestedState == pwmOutputState){
        return;
    }

    pwmOutputState = requestedState;
    m_gpio->setPin(PWM_PIN, requestedState ? 1 : 0);
}

// =============================================================================
// Fall-incident handling
// =============================================================================
void MainWindow::onIncidentFallCancel() {}

// -----------------------------------------------------------------------------
void MainWindow::onIncidentIamnotOK()
{
    soundPlay(SOUND_HELP, lang);
}

// -----------------------------------------------------------------------------
void MainWindow::onIncidentIamOK()
{
    soundPlay(SOUND_IAM_OK, lang);
}

// -----------------------------------------------------------------------------
void MainWindow::onIncidentFallEventDetected()
{
    // fallEventAckReceived = true;
    fallEmergency = true;
}

// -----------------------------------------------------------------------------
void MainWindow::onIncidentFallWakeUpByFallDetection()
{
    // increase brightness
    if (m_brightness->setBrightnessPercent(90)) {
        qDebug() << "Success Set brightness " << 70;
    } else {
        qDebug() << "Fail Set brightness " << 70;
    }

    //LEd red
    //requestPWM(45);
    m_gpio->setColor(COLOR_RED);
}

// -----------------------------------------------------------------------------
void MainWindow::onIncidentAckFallEventDetected()
{
    fallEventAckReceived = true;
    qDebug() << "ACK_FALL_EVENT_DETECTED";
}

// -----------------------------------------------------------------------------
void MainWindow::onIncidentFallNoResponse()
{
    // fallEventAckReceived = true;
    soundPlay(SOUND_HELP, lang);
}

// -----------------------------------------------------------------------------
void MainWindow::onIncidentFallHelpEventDetected()
{
    // fallEventAckReceived = true;
    soundPlay(SOUND_HELP, lang);
}

// -----------------------------------------------------------------------------
void MainWindow::onIncidentFallOKEventDetected()
{
    // fallEventAckReceived = true;
    soundPlay(SOUND_IAM_OK, lang);
}

// -----------------------------------------------------------------------------
void MainWindow::onIncidentFallCompleted()
{
    fallEventAckReceived = true;
    fallEmergency = false;
#ifdef Q_OS_LINUX
    m_gpio->setColor(COLOR_WHITE);
    //requestPWM(15);
#endif
}

// =============================================================================
// Timers and language state
// =============================================================================
void MainWindow::slotTimerSendFallEvent()
{
    if (!fallEventAckReceived) { // Ack belum diterima, ulangi kirim event fall
        //if (client->isConnected()) {
            // soundPlay(SOUND_FALL_OCCUR);
            QString timestamp = QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss");
            QJsonObject obj;
            obj["datetime"] = timestamp;
            client->emitEventStringMsgJsoned("INCIDENT_FALL_EVENT_DETECTED", obj);
        //} else {
        //    qDebug() << "Socket DC";
        //}
    } else {
        timerSendFallevent->stop(); // Ack sudah diterima, stop
    }
}

// -----------------------------------------------------------------------------
void MainWindow::slotTimerHeartBeat()
{
    radar1UartHeartBeatCounter++;
    radar2UartHeartBeatCounter++;
}

// -----------------------------------------------------------------------------
void MainWindow::onlangCurrent(QString langstr)
{
    lang = langstr;
    qDebug() << "Lang current info " << lang;
}

#ifdef Q_OS_LINUX
// =============================================================================
// Wi-Fi and Raspberry Pi controls
// =============================================================================
void MainWindow::onWifiOnRequest()
{
    m_utility->nmcliWifiOn();
}

// -----------------------------------------------------------------------------
void MainWindow::onWifiOffRequest()
{
    m_utility->nmcliWifiOff();
}

// -----------------------------------------------------------------------------
void MainWindow::onwifiScanSsidReqReceived()
{
    if (!client || !client->isConnected())
        return;

    QString isoMs = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

    QJsonObject obj;
    obj["timestamp"] = isoMs;

    client->emitEventStringMsgJsoned("WIFI_SCAN_STARTED", obj);

    qDebug() << "masuk cuk";

    m_utility->nmcliGetWifiListComplete();
}

// -----------------------------------------------------------------------------
void MainWindow::onWifiGetSsidRequest()
{
    //if (client->isConnected()) {
        // m_utility->nmcliGetSSID();
        qDebug() << "Get current wifi ssid status";
        m_utility->nmcliGetCurrentWifiInfo();
        // qDebug() << "wifiCurrent ";
        // client->emitEventStringMsgJsoned("SSID_GET",wifiCurrent);
    //} else {
    //    qDebug() << "Socket DC";
    //}
}

// -----------------------------------------------------------------------------
void MainWindow::onWifiSsidListRequest()
{
    //if (client->isConnected()) {
        // QStringList wifiList = m_utility->nmcliGetWifiList();
        m_utility->nmcliGetWifiListSSid();

        // qDebug() << "Wifi List " << wifiList;
        // QJsonObject obj;
        // obj["ssids"] = QJsonArray::fromStringList(wifiList);
        // client->emitEventStringMsgJsoned("SSID_LIST",obj);
    //} else {
    //    qDebug() << "Socket DC";
    //}
}

// -----------------------------------------------------------------------------
void MainWindow::onWifiSsidListRequestComplete()
{
    //if (client->isConnected()) {
        // QStringList wifiList = m_utility->nmcliGetWifiList();
        m_utility->nmcliGetWifiListComplete();

        // qDebug() << "Wifi List " << wifiList;
        // QJsonObject obj;
        // obj["ssids"] = QJsonArray::fromStringList(wifiList);
        // client->emitEventStringMsgJsoned("SSID_LIST",obj);
   // } else {
        qDebug() << "Socket DC";
   // }
}

// -----------------------------------------------------------------------------
void MainWindow::onWifiSSidListReady(QStringList ssidList)
{
    //if (client->isConnected()) {
        // QStringList wifiList = m_utility->nmcliGetWifiList();

        qDebug() << "Wifi List " << ssidList;
        QJsonObject obj;
        obj["ssids"] = QJsonArray::fromStringList(ssidList);
        client->emitEventStringMsgJsoned("SSID_LIST", obj);
    //} else {
    //    qDebug() << "Socket DC";
    //}
}

// -----------------------------------------------------------------------------
void MainWindow::onWifiSSidListReadyComplete(QList<WifiAP> wifiList)
{
    if (!client || !client->isConnected())
        return;

    // QJsonArray array;
    int ssidCountFound = 0;

    for (const WifiAP &ap : wifiList) {
        QJsonObject obj;
        obj["ssid"] = ap.ssid;
        obj["signal"] = ap.signalDbm;
        obj["secured"] = true; // ap.security;
        obj["channel"] = ap.channel;
        obj["frequency"] = ap.band;

        client->emitEventStringMsgJsoned("WIFI_NETWORK_FOUND", obj);

        ssidCountFound++;
        // array.append(obj);
    }

    // QString isoMs = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);

    qint64 isoMs = QDateTime::currentMSecsSinceEpoch();

    QJsonObject obj;
    obj["total"] = ssidCountFound;
    obj["timestamp"] = isoMs;

    client->emitEventStringMsgJsoned("WIFI_SCAN_COMPLETED", obj);
}

// -----------------------------------------------------------------------------
void MainWindow::onWifiSSidListReadyCompleteRequest(QList<WifiAP> wifiList)
{
    if (!client || !client->isConnected())
        return;

    m_utility->nmcliGetWifiListComplete();
}

// -----------------------------------------------------------------------------
void MainWindow::onCurrentSSidRequest()
{
    qDebug() << "reconnect, get current ssid.... ";
    m_worker->wifiGetSsid();
}

// -----------------------------------------------------------------------------
void MainWindow::onWifiConnectRequest(const QString &ssid, const QString &pwd)
{
    if (client->isConnected()) {
        QJsonObject obj;
        obj["ssid"] = ssid;
        // obj["password"] = pwd;
        obj["status"] = "connecting";
        client->emitEventStringMsgJsoned("WIFI_CONNECTING", obj);
        qDebug() << "NMCLI connect wifi ssid " << ssid << " pwd " << pwd;
        m_utility->nmcliConnectToWiFi(ssid, pwd);
    } else {
        qDebug() << "Socket DC";
        client->emitEventStringMsgJsoned("WIFI_CONNECTION_FAILED", "socketio_closed");
    }
}

// -----------------------------------------------------------------------------
void MainWindow::onWifiForgetRequest(const QString &ssid)
{
    if (client->isConnected()) {
        m_utility->nmcliForgetConnection(ssid);
    } else {
        qDebug() << "Socket DC";
        client->emitEventStringMsgJsoned("SIO DC", "");
    }
}

/*
// -----------------------------------------------------------------------------
void MainWindow::onSsidReady(QString ssid){
    qDebug() << "SSID aktif:" << ssid;

    // kirim ke socket di sini
    if (client->isConnected()) {
        client->emitEventStringMsgJsoned("wifi_status",ssid);
    } else {
        qDebug() << "Socket DC";
    }
}*/

// -----------------------------------------------------------------------------
void MainWindow::onCurrentWifiInfoReady(QJsonObject obj)
{
    if (!client || !client->isConnected())
        return;

    client->emitEventStringMsgJsoned("WIFI_STATUS", obj);
}

// -----------------------------------------------------------------------------
void MainWindow::onWifiConnected(bool success, const QString &ssid, const QString &ip, const QString gateway)
{
    if (!success) {
        QJsonObject obj;
        obj["error"] = ssid;
        client->emitEventStringMsgJsoned("WIFI_CONNECTION_FAILED", obj);
        return;
    }

    // QString msg = QString("%1 (%2)").arg(ssid, ip);

    if (client->isConnected()) {
        QJsonObject obj;
        obj["ssid"] = ssid;
        obj["ip"] = ip;
        obj["gateway"] = gateway;
        client->emitEventStringMsgJsoned("WIFI_CONNECTED", obj);
    } else {
        qDebug() << "Socket DC";
    }
}

// -----------------------------------------------------------------------------
void MainWindow::onWifiDisconnectRequest()
{
    if (client->isConnected()) {
        qDebug() << "DC current SSid Request";
        m_utility->nmcliDisconnectCurrentWifi();
    } else {
        qDebug() << "Socket DC";
    }
}

// -----------------------------------------------------------------------------
void MainWindow::onwifiDisconnectResult(bool success, QString ssid, QString message)
{
    if (success) {
        qDebug() << "WiFi disconnected:" << ssid;
        if (client->isConnected()) {
            QJsonObject obj;
            obj["ssid"] = ssid;
            obj["disconnectedBy"] = "user";
            client->emitEventStringMsgJsoned("WIFI_DISCONNECTED", obj);
        }
    } else {
        qDebug() << "Disconnect failed:" << message;
        if (client->isConnected()) {
            client->emitEventStringMsgJsoned("WIFI_DISCONNECTED", message);
        }
    }
}

// -----------------------------------------------------------------------------
void MainWindow::onWifiEnabled(bool on)
{
    if (on) {
        qDebug() << "Wifi Enable OK";
        client->emitEventStringMsgJsoned("WIFI_ENABLED_SUCCESS", "");
    } else {
        qDebug() << "Wifi Enable Fail";
        client->emitEventStringMsgJsoned("WIFI_ENABLED_FAIL", "");
    }
}

// -----------------------------------------------------------------------------
void MainWindow::onWifiDeleted(bool success, QString ssid, QString message)
{
    if (success) {
        qDebug() << "SSID deleted " << ssid;
        client->emitEventStringMsgJsoned("SSID_DELETED_OK", ssid);
    } else {
        qDebug() << "Wifi Enable Fail";
        client->emitEventStringMsgJsoned("SSID_DELETED_FAIL", ssid);
    }
}

// -----------------------------------------------------------------------------
void MainWindow::onWifiProgress(int state, QString stateText)
{
    qDebug() << "WiFi State:" << state << stateText;

    if (client->isConnected()) {
        int percent = 0;
        QString progressText = m_utility->deviceStateToString(state);

        switch (state) {
            case 30:
                percent = 0;
                break;
            case 40:
                percent = 10;
                break;
            case 50:
                percent = 25;
                break;
            case 60:
                percent = 40;
                break;
            case 70:
                percent = 60;
                break;
            case 80:
                percent = 75;
                break;
            case 90:
                percent = 90;
                break;
            case 100:
                percent = 100;
                progressText = "connected";
                break;

            case 120: // Failed
                percent = 0;
                progressText = "failed";
                break;

            default:
                percent = 0;
                break;
        }

        // kirim ke frontend

        QJsonObject obj;
        obj["stage"] = progressText;
        obj["progress"] = percent;

        client->emitEventStringMsgJsoned("WIFI_CONNECTION_PROGRESS", obj);
        if (state == 100) {
            qDebug() << "new wifi ssid, recon app....";
            QString serverIp = "203.194.114.21"; // ConfigManager::getServerIp();
                                                 // //"https://elderly-care-socket-io-server.online";
            int serverPort = 4000; // ConfigManager::getServerPort();

            qDebug() << "Server IP:" << serverIp;
            qDebug() << "Server Port:" << serverPort;

            // client->connectToServer("192.168.1.27", 3000);
            // client->connectToServer(serverIp);//, serverPort);
            client->connectToServer(serverIp, serverPort);
            // client->connectToServer("");//, serverPort);
            // restartApp();
        }
    }
}

// -----------------------------------------------------------------------------
void MainWindow::onWifiConnectFinished(bool success, QString ssid, QString ip, QString gateway)
{
    QJsonObject obj;

    obj["success"] = success;
    obj["ssid"] = ssid;
    obj["ip"] = ip;
    obj["gateway"] = gateway;

    if (client->isConnected()) {
        client->emitEventStringMsgJsoned("WIFI_CONNECT_RESULT", obj);
    }

    if (success)
        qDebug() << "Connected to" << ssid << "IP:" << ip << "GW:" << gateway;
    else
        qDebug() << "WiFi Connect Failed";
}

// -----------------------------------------------------------------------------
void MainWindow::onMonitorWlan0Connected() {}

// -----------------------------------------------------------------------------
void MainWindow::onMonitorWlan0Disconnected()
{
    if (client->isConnected()) {
        QJsonObject obj;
        obj["error"] = "Wlan0 Disconnected";
        client->emitEventStringMsgJsoned("WIFI_CONNECTION_FAILED", obj);
    }
}

// -----------------------------------------------------------------------------
void MainWindow::onMonitorWlan0WifiSignalLost()
{
    if (client->isConnected()) {
        QJsonObject obj;
        obj["error"] = "signal lost";
        client->emitEventStringMsgJsoned("WIFI_CONNECTION_FAILED", obj);
    }
}

// -----------------------------------------------------------------------------
void MainWindow::onMonitorWlan0networkInterfaceDown()
{
    if (client->isConnected()) {
        QJsonObject obj;
        obj["error"] = "NetworkInterfaceDown";
        client->emitEventStringMsgJsoned("WIFI_CONNECTION_FAILED", obj);
    }
}

// -----------------------------------------------------------------------------
void MainWindow::onMonitorWlan0ipAddressChanged(QString ip)
{
    if (client->isConnected()) {
        QJsonObject obj;
        obj["error"] = ip;
        client->emitEventStringMsgJsoned("WIFI_CONNECTION_FAILED", obj);
    }
}

// -----------------------------------------------------------------------------
void MainWindow::onRpiRestart()
{
    if (client->isConnected()) {
        QString timestamp = QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss");
        QJsonObject obj;
        obj["datetime"] = timestamp;
        client->emitEventStringMsgJsoned("DEVICE_RESTART", obj);
        m_utility->rpiRestart();
    }
}

// -----------------------------------------------------------------------------
void MainWindow::onRpiShutdown()
{
    if (client->isConnected()) {
        QString timestamp = QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss");
        QJsonObject obj;
        obj["datetime"] = timestamp;
        client->emitEventStringMsgJsoned("DEVICE_OFF", obj);
        m_utility->rpiShutdown();
    }
}

// -----------------------------------------------------------------------------
void MainWindow::onTzSetReq(QString tz)
{
    if (m_utility->setTimezone(tz)) {
        // if(m_utility->setTimezone("Europe/Stockholm")){
        qDebug() << "Set TZ to SW OK";
        if (client->isConnected()) {
            client->emitEventStringMsgJsoned("TIMEZONE", tz);
            m_utility->rpiShutdown();
        }
    } else {
        qDebug() << "Set TZ to SW Fail";
    }
}

// -----------------------------------------------------------------------------
void MainWindow::onTzGetReq()
{
    QString tz = m_utility->getTimeZone();
    // if(m_utility->setTimezone("Europe/Stockholm")){
    if (tz != "") {
        qDebug() << "Set TZ to SW OK";
        if (client->isConnected()) {
            client->emitEventStringMsgJsoned("TIMEZONE", tz);
            m_utility->getTimeZone();
        }
    } else {
        qDebug() << "TZ N/A";
    }
}
#endif
// -----------------------------------------------------------------------------
void MainWindow::on_btnPlayFall_clicked()
{
    soundPlay(SOUND_FALL_OCCUR, lang);
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnPlayHelp_clicked()
{
    soundPlay(SOUND_HELP, lang);
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnPlayIamOK_clicked()
{
    soundPlay(SOUND_IAM_OK, lang);
}

#ifdef Q_OS_LINUX
// -----------------------------------------------------------------------------
void MainWindow::on_btnScanWifiList_clicked()
{
    qDebug() << "SSID List ";
    m_utility->nmcliGetWifiListSSid();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnGetSSID_clicked()
{
    qDebug() << "SSID get ";
    m_utility->nmcliGetSSID();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnWifiCon_clicked()
{
    m_utility->nmcliConnectToWiFi("Parametrik 5G-01", "tabassam");
    //    qDebug() << "Sukses";
    //}else{
    //    qDebug() << "Gagal ";
    //}
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnWifiOff_clicked()
{
    m_utility->nmcliWifiOff();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnWifiOn_clicked()
{
    m_utility->nmcliWifiOn();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnForget_clicked()
{
    m_utility->nmcliForgetConnection("Parametrik 5G-01");
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnRestart_clicked()
{
    m_utility->rpiRestart();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnShutdown_clicked()
{
    m_utility->rpiShutdown();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnSetTZ_clicked()
{
    if (m_utility->setTimezone("Asia/Jakarta")) {
        // if(m_utility->setTimezone("Europe/Stockholm")){
        qDebug() << "Set TZ to SW OK";
    } else {
        qDebug() << "Set TZ to SW Fail";
    }
}
#endif

// -----------------------------------------------------------------------------
void MainWindow::on_btnEmitEvenwAck_clicked()
{
    QJsonObject data;
    data["username"] = "wahyu";
    data["password"] = "123456";

    client->emitEventWithAck(
        "login",
        data,
        [](bool ok, QJsonValue response) {
            if (!ok) {
                qDebug() << "ACK timeout or error:" << response;
                return;
            }

            qDebug() << "ACK received:" << response;

            if (response.isObject()) {
                QJsonObject obj = response.toObject();
                qDebug() << "Server message:" << obj;
            }
        },
        5000 // timeout 5 detik
    );
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnEmitListeningOn_clicked()
{
    if (client->isConnected()) {
        // QString timestamp = QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss");
        // QJsonObject obj;
        // obj["datetime"] = timestamp;
        QString msg = "ON";
        client->emitEventStringMsgJsoned("LISTENING", msg);
    } else {
        qDebug() << " Socket DC";
    }
}

// =============================================================================
// Legacy audio recording helpers
// =============================================================================
void MainWindow::readMore()
{
    /*
    QByteArray data = device->readAll();

    //qDebug() << "Raw first bytes:" << data.left(8).toHex();

    if (data.isEmpty()) return;

    QAudioFormat format = audio->format();

    double level = 0;

    if (format.sampleFormat() == QAudioFormat::Int16) {
        const int16_t *samples = reinterpret_cast<const int16_t*>(data.constData());
        int count = data.size() / sizeof(int16_t);

        int16_t maxVal = 0;
        for (int i = 0; i < count; ++i)
            maxVal = qMax(maxVal, qAbs(samples[i]));

        level = maxVal / 32768.0;
    }
    else if (format.sampleFormat() == QAudioFormat::Float) {
        const float *samples = reinterpret_cast<const float*>(data.constData());
        int count = data.size() / sizeof(float);

        float maxVal = 0;
        for (int i = 0; i < count; ++i)
            maxVal = qMax(maxVal, fabs(samples[i]));

        level = maxVal; // sudah 0.0 – 1.0
    }
    else {
        qDebug() << "Unsupported format!";
        return;
    }

    int barLevel = qBound(0, int(level * 100), 100);

    //qDebug() << barLevel;

    ui->micBar->setValue(barLevel);
*/
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnRec_pressed()
{
    startRecording();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnRec_released()
{
    stopRecording();
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnPlayRec_clicked()
{
    /*
    //soundPlay(SOUND_RECORD);
    //loadWav("/home/pi/qtpro/test12/radarscan/record.wav");
    if (!audioSink || !audioBuffer)
        return;

    // =========================
    // STOP
    // =========================
    if (audioSink->state() == QAudio::ActiveState) {

        audioSink->stop();
        audioBuffer->seek(0);
        levelTimer->stop();

        ui->btnPlayRec->setText("Play");
        return;
    }

    // =========================
    // PLAY
    // =========================
    audioBuffer->seek(0);

    audioSink->start(audioBuffer);

    levelTimer->start(50);

    ui->btnPlayRec->setText("Stop");
*/
}

// -----------------------------------------------------------------------------
void MainWindow::handleFinished()
{
    /*
    if (decoder->error() != QAudioDecoder::NoError) {
        qDebug() << "❌ Decoder error:" << decoder->error();
    } else {
        qDebug() << "✅ Decode finished OK";
    }
*/
}

// -----------------------------------------------------------------------------
// void MainWindow::handleError(QAudioDecoder::Error error)
//{
//    qDebug() << "Decoder error:" << error;
//}

// -----------------------------------------------------------------------------
void MainWindow::loadWav(const QString &path)
{
    /*
    decoder->setSource(QUrl::fromLocalFile(path));
    decoder->start();
    qDebug() << "Start decoding... ";
*/
}

// -----------------------------------------------------------------------------
void MainWindow::processBuffer()
{
    /*
    QAudioBuffer buffer = decoder->read();

    if (!buffer.isValid()) return;

    // ✅ Ensure format is Int16
    if (buffer.format().sampleFormat() != QAudioFormat::Int16)
        return;

    const qint16 *data = buffer.constData<qint16>();
    int samples = buffer.sampleCount();

    if (samples <= 0) return;

    float sum = 0;

    for (int i = 0; i < samples; i++) {
        float v = data[i] / 32768.0f;
        sum += v * v;
    }

    float rms = sqrt(sum / samples);  // 0.0 → ~1.0

    // ✅ Convert to progress bar scale (0–100)
    int value = static_cast<int>(rms * 100.0f);

    // clamp (just in case)
    value = qBound(0, value, 100);

    ui->speakerBar->setValue(value);
    qDebug() << "Buffering...." << value;
*/
}

// -----------------------------------------------------------------------------
double MainWindow::calculateDb(const QByteArray &data)
{
    const int16_t *samples = reinterpret_cast<const int16_t *>(data.constData());
    int sampleCount = data.size() / 2;

    if (sampleCount == 0)
        return -60.0;

    double sum = 0.0;

    for (int i = 0; i < sampleCount; i++) {
        double sample = samples[i] / 32768.0;
        sum += sample * sample;
    }

    double rms = sqrt(sum / sampleCount);

    if (rms < 1e-6)
        return -60.0;

    return 20.0 * log10(rms);
}

// -----------------------------------------------------------------------------
void MainWindow::initAudioSystem() {}

// =============================================================================
// PZEM and process management
// =============================================================================
void MainWindow::initPzem()
{
    /*connect(m_pzem, &Pzem004Tv30Qt::dataReady,
                 this,[this](const Pzem004Tv30Data &data) {
                     //qDebug() << "========== PZEM DATA ==========";
                     //qDebug() << "Voltage   :" << data.voltage << "V";
                     //qDebug() << "Current   :" << data.current << "A";
                     //qDebug() << "Power     :" << data.power << "W";
                     //qDebug() << "Energy    :" << data.energy << "kWh";
                     //qDebug() << "Frequency :" << data.frequency << "Hz";
                     //qDebug() << "PF        :" << data.powerFactor;
                     //qDebug() << "Alarm     :" << data.alarm;


                     // Di sini bisa lanjut emit ke Socket.IO, update UI,
                     // simpan log, atau masuk ke health monitoring robot.
                     emit pzemDataReadyComplete(data);
                 });
                 */

    m_pzem = new Pzem004Tv30Qt(this);

    connect(m_pzem, &Pzem004Tv30Qt::dataReady, this, &MainWindow::onPzemDataReadyComplete);

    connect(m_pzem, &Pzem004Tv30Qt::errorOccurred, this, [](const QString &error) {
        qDebug() << "PZEM error:" << error;
    });

    // connect(this,&MainWindow::pzemDataReadyComplete,
    //              this,&MainWindow::onPzemDataReadyComplete);

    const QString portName = "/dev/ttyAMA2";

    if (!m_pzem->open(portName, 0xF8, 1000)) {
        qDebug() << "Failed to open PZEM:" << m_pzem->lastError();
        return;
    }

    qDebug() << "PZEM serial opened on" << portName;
}

// -----------------------------------------------------------------------------
void MainWindow::stopAllThreads()
{
    QList<QThread *> threads = qApp->findChildren<QThread *>();

    for (QThread *thread : threads) {
        if (thread == QThread::currentThread())
            continue;

        qDebug() << "Stopping thread:" << thread;

        thread->requestInterruption();
        thread->quit();

        if (!thread->wait(3000)) {
            qWarning() << "Thread not stopped:" << thread;
        }
    }
}

// -----------------------------------------------------------------------------
void MainWindow::stopAllProcesses()
{
    QList<QProcess *> processes = qApp->findChildren<QProcess *>();

    for (QProcess *p : processes) {
        p->terminate();

        if (!p->waitForFinished(3000))
            p->kill();
    }
}

// -----------------------------------------------------------------------------
void MainWindow::restartApp()
{
    stopAllProcesses();
    stopAllThreads();

    QString app = QCoreApplication::applicationFilePath();

    QProcess::startDetached(app);

    qApp->quit();
}

// -----------------------------------------------------------------------------
void MainWindow::getLangCommand()
{
    if (client->isConnected()) {
        qDebug() << "Req lang info";
        client->emitEventStringMsgJsoned("LANGUAGE_GET", "");
    } else {
        qDebug() << "Server IO Dc";
    }
}

// =============================================================================
// Sensor workers and utility initialization
// =============================================================================
void MainWindow::initBME280()
{
    m_bmeThread = new QThread(this);

    m_bmeWorker = new Bme280Worker(QStringLiteral("/dev/i2c-1"), 0x76);
    m_bmeWorker->moveToThread(m_bmeThread);

    connect(
        this, &MainWindow::requestBme280Read, m_bmeWorker, &Bme280Worker::readSensor, Qt::QueuedConnection);
    connect(m_bmeWorker, &Bme280Worker::readingReady, this, &MainWindow::onBme280ReadingReady);
    connect(m_bmeWorker, &Bme280Worker::errorOccurred, this, &MainWindow::onBme280Error);
    connect(m_bmeWorker, &Bme280Worker::readFinished, this, [this]() {});
    connect(m_bmeThread, &QThread::finished, m_bmeWorker, &QObject::deleteLater);

    m_bmeThread->start();
}

// -----------------------------------------------------------------------------
void MainWindow::initCpuTemp()
{
    // init cpu temp
    m_cpuTemperatureThread = new QThread(this);
    m_cpuTemperatureWorker = new CpuTemperatureWorker();

    m_cpuTemperatureWorker->moveToThread(m_cpuTemperatureThread);

    connect(this,
            &MainWindow::requestCpuTemperature,
            m_cpuTemperatureWorker,
            &CpuTemperatureWorker::readTemperature,
            Qt::QueuedConnection);
    connect(m_cpuTemperatureWorker,
            &CpuTemperatureWorker::temperatureReady,
            this,
            &MainWindow::onCpuTemperatureReady);
    connect(m_cpuTemperatureWorker,
            &CpuTemperatureWorker::errorOccurred,
            this,
            &MainWindow::onCpuTemperatureError);
    connect(m_cpuTemperatureWorker, &CpuTemperatureWorker::readFinished, this, [this]() {
        // ui->btngetcputemp->setEnabled(true);
        qDebug() << "CPU ready finished ";
    });

    connect(m_cpuTemperatureThread, &QThread::finished, m_cpuTemperatureWorker, &QObject::deleteLater);

    m_cpuTemperatureThread->start();
}

// -----------------------------------------------------------------------------
void MainWindow::getCputemp()
{
    emit requestCpuTemperature();
}

// -----------------------------------------------------------------------------
void MainWindow::initUtility()
{
#ifdef Q_OS_LINUX
    m_gpio = new gpio();    
    if (m_gpio->setupGPIO() < 0) {
        qCritical() << "GPIO initialization failed.";
    }

    //m_gpio->setColor(COLOR_WHITE);
    //gpioTimer = new QTimer(this);
    //gpioTimer->setTimerType(Qt::PreciseTimer);
    //gpioTimer->setInterval(2);
    //connect(gpioTimer,&QTimer::timeout,this,&MainWindow::slotGpioTimer);
    //gpioElapsedTimer.start();
    //gpioTimer->start(2);

    m_volume = new volume();
    m_brightness = new brightness();

    m_utility = new utilities();

    connect(m_utility, &utilities::wifiConnectResult, this, &MainWindow::onWifiConnected);
    connect(m_utility, &utilities::wifiRadioChanged, this, &MainWindow::onWifiEnabled);
    connect(m_utility, &utilities::wifiForgetResult, this, &MainWindow::onWifiDeleted);
    connect(m_utility, &utilities::wifiListReadyComplete, this, &MainWindow::onWifiSSidListReadyComplete);
    connect(m_utility, &utilities::wifiCurrentInfoReady, this, &MainWindow::onCurrentWifiInfoReady);
    connect(m_utility, &utilities::wifiDisconnectResult, this, &MainWindow::onwifiDisconnectResult);

    connect(m_utility, &utilities::wifiConnectProgress, this, &MainWindow::onWifiProgress);
    connect(m_utility, &utilities::wifiConnectResult, this, &MainWindow::onWifiConnectFinished);

    qDebug() << "Start monitoring";
    systemdymon = new systemdmonitorqt("ssh.service", this);
    connect(systemdymon, &systemdmonitorqt::serviceStarted, []() {
        qDebug() << "SSH STARTED";
    });

    connect(systemdymon, &systemdmonitorqt::serviceStopped, []() {
        qDebug() << "SSH STOPPED";
    });

    connect(systemdymon, &systemdmonitorqt::serviceFailed, []() {
        qDebug() << "SSH FAILED";
    });

    qDebug() << "End Monitoring setup ";
#endif
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnRec_clicked() {}

// -----------------------------------------------------------------------------
void MainWindow::on_btnPing_clicked()
{
    // QString isoMs = QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs);
    // QString timestampMs = QString::number(QDateTime::currentMSecsSinceEpoch());

    // qDebug() << timestampMs;
    // qDebug() << "isooooocukkk " << timestampMs;
    m_pzem->requestReadAll();
    runAudioHealthRecordTest();
}

// -----------------------------------------------------------------------------
void MainWindow::onPzemDataReadyComplete(Pzem004Tv30Data data)
{
    qDebug() << "========== PZEM DATA ==========";
    qDebug() << "Voltage   :" << data.voltage << "V";
    qDebug() << "Current   :" << data.current << "A";
    qDebug() << "Power     :" << data.power << "W";
    qDebug() << "Energy    :" << data.energy << "kWh";
    qDebug() << "Frequency :" << data.frequency << "Hz";
    qDebug() << "PF        :" << data.powerFactor;
    qDebug() << "Alarm     :" << data.alarm;

    // Proses lebih lanjut, ke json, kirim ke socketio
    if (client->isConnected()) {
        QJsonObject obj;
        obj["temperature"] = mbme280data.temperatureC;
        obj["pressure"] = mbme280data.pressureHpa;
        obj["humidity"] = mbme280data.humidityPercent;
        obj["cpu_temperature"] = cpuTempC;

        obj["voltage"] = data.voltage;
        obj["current"] = data.current;
        obj["power_cons"] = data.power;
        obj["frquency"] = data.frequency;
        obj["pf"] = data.powerFactor;
        obj["energy"] = data.energy;

        client->emitEventStringMsgJsoned("DEVICE_POWER_INFO", obj);

        // clear
        mbme280data.temperatureC = 0;
        mbme280data.pressureHpa = 0;
        mbme280data.humidityPercent = 0;
        cpuTempC = 0;
    }
}

// -----------------------------------------------------------------------------
void MainWindow::onPowerInfoReq()
{
    emit requestBme280Read();
    emit requestCpuTemperature();

    QTimer::singleShot(1000, this, [this]() {
        m_pzem->requestReadAll();
    });
}

// -----------------------------------------------------------------------------
void MainWindow::onAudioInfoReq()
{
    runAudioHealthRecordTest();
}

// =============================================================================
// Audio health test
// =============================================================================
void MainWindow::runAudioHealthRecordTest()
{
    getAudioTargetsFromWpctlStatusAsync(
        [this](const QString &micTarget, const QString &speakerTarget, const QString &targetErr) {
            if (micTarget.isEmpty() || speakerTarget.isEmpty()) {
                qWarning().noquote() << "Failed to get audio targets from wpctl status:\n" << targetErr;
                return;
            }

            qDebug() << "Using mic target:" << micTarget;
            qDebug() << "Using speaker target KT USB:" << speakerTarget;

            QProcess *recorder = new QProcess(this);
            QProcess *player = new QProcess(this);

            QString wavDir = QDir::homePath() + "/wav";

            QString testFileName = wavDir + "/audio_health_test.wav";
            QString recordFile = wavDir + "/respeaker_testRec.wav";

            QDir().mkpath(wavDir);

            QString err;

            if (!m_audioCheck.createTestWav(testFileName, &err)) {
                qWarning() << "Create test WAV failed:" << err;

                recorder->deleteLater();
                player->deleteLater();
                return;
            } else {
                qDebug() << "generate wav test success" << testFileName;
            }

            QStringList recArgs;
            recArgs << "10s"
                    << "pw-record"
                    << "--target" << micTarget << "--rate" << "16000"
                    << "--channels" << "1"
                    << "--format" << "s16" << recordFile;

            recorder->setProgram("timeout");
            recorder->setArguments(recArgs);
            recorder->setProcessChannelMode(QProcess::MergedChannels);

            connect(recorder, &QProcess::readyReadStandardOutput, this, [recorder]() {
                qDebug().noquote() << "[REC]" << recorder->readAllStandardOutput();
            });

            connect(recorder, &QProcess::started, this, []() {
                qDebug() << "Recorder started non-blocking";
            });

            connect(recorder, &QProcess::errorOccurred, this, [](QProcess::ProcessError error) {
                qWarning() << "Recorder process error:" << error;
            });

            connect(recorder,
                    QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                    this,
                    [=](int exitCode, QProcess::ExitStatus exitStatus) {
                        qDebug() << "Recorder finished. Exit code:" << exitCode
                                 << "Exit status:" << exitStatus;

                        qDebug() << "Recorded file:" << recordFile;

                        if (QFileInfo::exists(recordFile)) {
                            qDebug() << "Recording file exists, ready to analyze.";

                            m_audioCheck.audioReport = m_audioCheck.analyzeRecording(recordFile);

                            QString reportResult = m_audioCheck.formatReportCompact(m_audioCheck.audioReport);

                            qDebug().noquote() << reportResult;
                            if (client->isConnected()) {
                                QString radar1ReportInfo = "radar1Normal";
                                QString radar2ReportInfo = "radar2Normal";

                                if (radar1UartHeartBeatCounter > 5)
                                    radar1ReportInfo = "radar1Error";
                                if (radar2UartHeartBeatCounter > 5)
                                    radar2ReportInfo = "radar2Error";

                                QJsonObject obj;
                                obj["audio_report"] = reportResult;
                                obj["radar_report"] = radar1ReportInfo + "_" + radar2ReportInfo;
                                client->emitEventStringMsgJsoned("DEVICE_STATUS_INFO", obj);
                            }
                        } else {
                            qWarning() << "Recording file not found:" << recordFile;
                        }

                        recorder->deleteLater();
                    });

            recorder->start();

            // =========================
            // PLAY COMMAND:
            //
            // pw-play --target <KT_USB_SINK_ID> audio_health_test.wav
            // =========================

            player->setProgram("pw-play");

            player->setArguments(QStringList() << "--target" << speakerTarget << "audio_health_test.wav");

            player->setWorkingDirectory(wavDir);
            player->setProcessChannelMode(QProcess::MergedChannels);

            connect(player, &QProcess::readyReadStandardOutput, this, [player]() {
                qDebug().noquote() << "[PLAY]" << player->readAllStandardOutput();
            });

            connect(player, &QProcess::started, this, [speakerTarget]() {
                qDebug() << "Player started: pw-play --target" << speakerTarget << "audio_health_test.wav";
            });

            connect(player, &QProcess::errorOccurred, this, [](QProcess::ProcessError error) {
                qWarning() << "Player process error:" << error;
            });

            connect(player,
                    QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                    this,
                    [player](int exitCode, QProcess::ExitStatus exitStatus) {
                        qDebug() << "Player finished. Exit code:" << exitCode << "Exit status:" << exitStatus;

                        player->deleteLater();
                    });

            QTimer::singleShot(300, this, [player]() {
                player->start();
            });
        });
}

// =============================================================================
// PipeWire target discovery
// =============================================================================
void MainWindow::getMicTargetFromWpctlStatusAsync(
    std::function<void(const QString &micTarget, const QString &err)> callback)
{
    QProcess *wpctl = new QProcess(this);
    QByteArray *outputBuffer = new QByteArray();

    wpctl->setProgram("wpctl");
    wpctl->setArguments(QStringList() << "status");
    wpctl->setProcessChannelMode(QProcess::MergedChannels);

    connect(wpctl, &QProcess::readyRead, this, [wpctl, outputBuffer]() {
        outputBuffer->append(wpctl->readAll());
    });

    connect(
        wpctl, &QProcess::errorOccurred, this, [wpctl, outputBuffer, callback](QProcess::ProcessError error) {
            QString err = QString("wpctl process error: %1").arg(error);

            qWarning() << err;

            callback(QString(), err);

            delete outputBuffer;
            wpctl->deleteLater();
        });

    connect(wpctl,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this, wpctl, outputBuffer, callback](int exitCode, QProcess::ExitStatus exitStatus) {
                outputBuffer->append(wpctl->readAll());

                QString output = QString::fromUtf8(*outputBuffer);

                if (exitStatus != QProcess::NormalExit || exitCode != 0) {
                    QString err = "wpctl status failed:\n" + output;

                    qWarning().noquote() << err;

                    callback(QString(), err);

                    delete outputBuffer;
                    wpctl->deleteLater();
                    return;
                }

                QString parseErr;
                QString micTarget = parseMicTargetFromWpctlStatus(output, &parseErr);

                callback(micTarget, parseErr);

                delete outputBuffer;
                wpctl->deleteLater();
            });

    wpctl->start();
}

// -----------------------------------------------------------------------------
QString MainWindow::parseMicTargetFromWpctlStatus(const QString &output, QString *err) const
{
    bool inSources = false;

    QString defaultSourceId;
    QString firstNonMonitorSourceId;
    QStringList detectedSources;

    // Bisa handle format:
    // *   55. ReSpeaker ...
    //     56. Monitor ...
    QRegularExpression sourceRx(R"(^\*?\s*(\d+)\.\s+(.+?)(?:\s+\[|$))");

    QStringList lines = output.split('\n');

    for (const QString &rawLine : lines) {
        QString line = rawLine.trimmed();

        if (line.contains("Sources:")) {
            inSources = true;
            continue;
        }

        if (inSources && (line.contains("Sinks:") || line.contains("Filters:") || line.contains("Streams:") ||
                          line.contains("Video:") || line.contains("Devices:"))) {
            break;
        }

        if (!inSources)
            continue;

        QRegularExpressionMatch match = sourceRx.match(line);
        if (!match.hasMatch())
            continue;

        QString id = match.captured(1).trimmed();
        QString name = match.captured(2).trimmed();

        detectedSources << QString("%1. %2").arg(id, name);

        bool isDefault = rawLine.contains("*");
        bool isMonitor = name.contains("monitor", Qt::CaseInsensitive);

        if (isDefault && !isMonitor) {
            defaultSourceId = id;
        }

        if (firstNonMonitorSourceId.isEmpty() && !isMonitor) {
            firstNonMonitorSourceId = id;
        }

        // Prioritas utama: ReSpeaker / SEEED
        if (name.contains("respeaker", Qt::CaseInsensitive) || name.contains("seeed", Qt::CaseInsensitive) ||
            name.contains("4 mic", Qt::CaseInsensitive) || name.contains("mic array", Qt::CaseInsensitive)) {
            qDebug() << "Matched mic from wpctl status:" << id << name;
            return id;
        }
    }

    if (!defaultSourceId.isEmpty()) {
        qWarning() << "ReSpeaker source not found. Using default source ID:" << defaultSourceId;
        return defaultSourceId;
    }

    if (!firstNonMonitorSourceId.isEmpty()) {
        qWarning() << "Default source not found. Using first non-monitor source ID:"
                   << firstNonMonitorSourceId;
        return firstNonMonitorSourceId;
    }

    if (err) {
        *err = "No valid audio source found from wpctl status.\nDetected sources:\n" +
               detectedSources.join('\n') + "\n\nFull wpctl output:\n" + output;
    }

    return {};
}

// -----------------------------------------------------------------------------
void MainWindow::getAudioTargetsFromWpctlStatusAsync(
    std::function<void(const QString &micTarget, const QString &speakerTarget, const QString &err)> callback)
{
    QProcess *wpctl = new QProcess(this);
    QByteArray *outputBuffer = new QByteArray();

    wpctl->setProgram("wpctl");
    wpctl->setArguments(QStringList() << "status");
    wpctl->setProcessChannelMode(QProcess::MergedChannels);

    connect(wpctl, &QProcess::readyRead, this, [wpctl, outputBuffer]() {
        outputBuffer->append(wpctl->readAll());
    });

    connect(
        wpctl, &QProcess::errorOccurred, this, [wpctl, outputBuffer, callback](QProcess::ProcessError error) {
            if (error == QProcess::FailedToStart) {
                QString err = "Failed to start wpctl status";

                qWarning() << err;

                callback(QString(), QString(), err);

                delete outputBuffer;
                wpctl->deleteLater();
            }
        });

    connect(wpctl,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this, wpctl, outputBuffer, callback](int exitCode, QProcess::ExitStatus exitStatus) {
                outputBuffer->append(wpctl->readAll());

                QString output = QString::fromUtf8(*outputBuffer);

                if (exitStatus != QProcess::NormalExit || exitCode != 0) {
                    QString err = "wpctl status failed:\n" + output;

                    qWarning().noquote() << err;

                    callback(QString(), QString(), err);

                    delete outputBuffer;
                    wpctl->deleteLater();
                    return;
                }

                QString micTarget;
                QString speakerTarget;
                QString parseErr;

                bool ok = parseAudioTargetsFromWpctlStatus(output, &micTarget, &speakerTarget, &parseErr);

                if (!ok) {
                    callback(QString(), QString(), parseErr);
                } else {
                    callback(micTarget, speakerTarget, QString());
                }

                delete outputBuffer;
                wpctl->deleteLater();
            });

    wpctl->start();
}

// -----------------------------------------------------------------------------
bool MainWindow::parseAudioTargetsFromWpctlStatus(const QString &output,
                                                  QString *micTarget,
                                                  QString *speakerTarget,
                                                  QString *err) const
{
    enum class Section
    {
        None,
        Sinks,
        Sources
    };

    Section section = Section::None;

    QString foundMicId;
    QString fallbackDefaultSourceId;
    QString fallbackFirstSourceId;

    QString foundSpeakerId;

    QStringList detectedSources;
    QStringList detectedSinks;

    QRegularExpression nodeRx(R"((\*)?\s*(\d+)\.\s+(.+?)(?:\s+\[|$))");

    const QStringList lines = output.split('\n');

    for (const QString &rawLine : lines) {
        QString line = rawLine.trimmed();

        if (line.contains("Sinks:") && !line.contains("Sink endpoints:")) {
            section = Section::Sinks;
            continue;
        }

        if (line.contains("Sources:") && !line.contains("Source endpoints:")) {
            section = Section::Sources;
            continue;
        }

        if (line.contains("Devices:") || line.contains("Sink endpoints:") ||
            line.contains("Source endpoints:") || line.contains("Filters:") || line.contains("Streams:") ||
            line.contains("Video:")) {
            section = Section::None;
            continue;
        }

        if (section == Section::None)
            continue;

        QRegularExpressionMatch match = nodeRx.match(rawLine);

        if (!match.hasMatch())
            continue;

        const bool isDefault = !match.captured(1).isEmpty();
        const QString id = match.captured(2).trimmed();
        const QString name = match.captured(3).trimmed();

        if (section == Section::Sources) {
            detectedSources << QString("%1. %2").arg(id, name);

            const bool isMonitor = name.contains("monitor", Qt::CaseInsensitive);

            if (isDefault && !isMonitor) {
                fallbackDefaultSourceId = id;
            }

            if (fallbackFirstSourceId.isEmpty() && !isMonitor) {
                fallbackFirstSourceId = id;
            }

            if (name.contains("respeaker", Qt::CaseInsensitive) ||
                name.contains("seeed", Qt::CaseInsensitive) || name.contains("4 mic", Qt::CaseInsensitive) ||
                name.contains("mic array", Qt::CaseInsensitive)) {
                foundMicId = id;
                qDebug() << "Matched ReSpeaker source:" << id << name;
            }
        }

        if (section == Section::Sinks) {
            detectedSinks << QString("%1. %2").arg(id, name);

            // Target speaker wajib KT USB
            if (name.contains("kt usb", Qt::CaseInsensitive) ||
                (name.contains("kt", Qt::CaseInsensitive) && name.contains("usb", Qt::CaseInsensitive))) {
                foundSpeakerId = id;
                qDebug() << "Matched KT USB sink:" << id << name;
            }
        }
    }

    if (foundMicId.isEmpty()) {
        if (!fallbackDefaultSourceId.isEmpty()) {
            foundMicId = fallbackDefaultSourceId;
            qWarning() << "ReSpeaker source not found. Using default source:" << foundMicId;
        } else if (!fallbackFirstSourceId.isEmpty()) {
            foundMicId = fallbackFirstSourceId;
            qWarning() << "ReSpeaker source not found. Using first source:" << foundMicId;
        }
    }

    QStringList errors;

    if (foundMicId.isEmpty()) {
        errors << "No valid microphone source found.";
    }

    if (foundSpeakerId.isEmpty()) {
        errors << "KT USB speaker sink not found.";
    }

    if (!errors.isEmpty()) {
        if (err) {
            *err = errors.join('\n') + "\n\nDetected Sources:\n" + detectedSources.join('\n') +
                   "\n\nDetected Sinks:\n" + detectedSinks.join('\n') + "\n\nFull wpctl output:\n" + output;
        }

        return false;
    }

    if (micTarget)
        *micTarget = foundMicId;

    if (speakerTarget)
        *speakerTarget = foundSpeakerId;

    return true;
}

// -----------------------------------------------------------------------------
void MainWindow::on_btnLogin_clicked()
{
    soundPlay(SOUND_UPLOAD_FAILED, lang);
}

// -----------------------------------------------------------------------------
void MainWindow::onRadarHeartBeatDetected() {}

// =============================================================================
// Sensor and audio callbacks
// =============================================================================
void MainWindow::onBme280ReadingReady(double temperatureC, double pressureHpa, double humidityPercent)
{
    qDebug().noquote() << QStringLiteral("BME280 | Temperature: %1 °C | "
                                         "Pressure: %2 hPa | "
                                         "Humidity: %3 %RH")
                              .arg(temperatureC, 0, 'f', 2)
                              .arg(pressureHpa, 0, 'f', 2)
                              .arg(humidityPercent, 0, 'f', 2);

    mbme280data.temperatureC = temperatureC;
    mbme280data.pressureHpa = pressureHpa;
    mbme280data.humidityPercent = humidityPercent;
}

// -----------------------------------------------------------------------------
void MainWindow::onBme280Error(const QString &message)
{
    qWarning().noquote() << "BME280 error:" << message;
}

// -----------------------------------------------------------------------------
void MainWindow::onSoundFinished(int sentenceIndex, QString langIndex)
{
    qDebug() << "Audio finished:"
             << "sentenceIndex:" << sentenceIndex << "language:" << langIndex;

    if (client->isConnected()) {
        QJsonObject obj;
        client->emitEventStringMsgJsoned("SOUND_PLAYED", obj);
    }

    switch (sentenceIndex) {
        case SOUND_FALL_OCCUR:
            qDebug() << "SOUND_FALL_OCCUR selesai diputar";
            break;

        case SOUND_HELP:
            qDebug() << "SOUND_HELP selesai diputar";
            break;

        case SOUND_IAM_OK:
            qDebug() << "SOUND_IAM_OK selesai diputar";
            break;

        case SOUND_RECORD:
            qDebug() << "SOUND_RECORD selesai diputar";
            break;

        case SOUND_WAITING:
            qDebug() << "SOUND_WAITING selesai diputar";
            break;

        case SOUND_HELPYOU:
            qDebug() << "SOUND_HELPYOU selesai diputar";
            break;

        case SOUND_LOGIN:
            qDebug() << "SOUND_LOGIN selesai diputar";
            break;

        case SOUND_UPLOAD_FAILED:
            qDebug() << "SOUND_UPLOAD_FAILED selesai diputar";
            break;

        default:
            break;
    }
}

// -----------------------------------------------------------------------------
void MainWindow::onSoundFailed(int sentenceIndex, QString langIndex, QString errorMessage)
{
    qWarning() << "Audio playback failed:"
               << "sentenceIndex:" << sentenceIndex << "language:" << langIndex << "error:" << errorMessage;
}

// -----------------------------------------------------------------------------
void MainWindow::onUploadFailed()
{
    soundPlay(SOUND_UPLOAD_FAILED, lang);
}

// -----------------------------------------------------------------------------
void MainWindow::requestPWM(quint8 persen)
{
    if (persen > 100) {
        persen = 100;
    }

    pwmDutyPercent = persen;
    pwmHighTimeUs = (PWM_PERIOD_US * static_cast<qint64>(pwmDutyPercent)) / 100;

    /*
     * Restart periode setiap duty cycle diubah agar perubahan duty cycle
     * langsung diterapkan dari awal periode.
     */
    gpioElapsedTimer.restart();

    if (pwmDutyPercent == 0) {
        pwmOutputState = false;
        m_gpio->setPin(PWM_PIN, 0);
    }else{
        /*
         * Duty cycle 1–100% selalu dimulai dari kondisi HIGH.
         */
        pwmOutputState = true;
        m_gpio->setPin(PWM_PIN, 1);
    }

    qDebug() << "PWM request:" << pwmDutyPercent << "%" << "high time:" << pwmHighTimeUs << "us";
}

// -----------------------------------------------------------------------------
void MainWindow::onCpuTemperatureReady(double temperatureC)
{
    qDebug().noquote() << QStringLiteral("CPU Temperature: %1 °C").arg(temperatureC, 0, 'f', 2);
    cpuTempC = temperatureC;
    // ui->editLog3->insertPlainText("Temperature " + QString::number(temperatureC,'f',2) + " °C\r\n");
}

// -----------------------------------------------------------------------------
void MainWindow::onCpuTemperatureError(const QString &message)
{
    qWarning().noquote() << "CPU temperature error:" << message;
}
