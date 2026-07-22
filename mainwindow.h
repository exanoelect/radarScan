#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <cstdint>
#include <functional>

// Qt Core
#include <QBuffer>
#include <QByteArray>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QIODevice>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QNetworkInformation>
#include <QProcess>
#include <QQueue>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QThread>
#include <QTimer>
#include <QVector>
#include <QtEndian>
#include <QtMath>

// Qt GUI / Widgets
#include <QFileDialog>
#include <QListWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QScreen> // Qt6 replacement for QDesktopWidget
#include <QVBoxLayout>
#include <QWidget>

// Qt Network / Multimedia / Serial Port
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QWebSocket>
#include <QtMultimedia/qaudio.h>

// Project headers
#include "AudioHealthChecker.h"
#include "Pzem004Tv30Qt.h"
#include "VolumeMonitor.h"
#include "audioworker.h"
#include "bme280worker.h"
#include "brightness.h"
#include "cputemperatureworker.h"
#include "gpio.h"
#include "networkmonitor.h"
#include "payloadprocessor.h"
#include "qcustomplot.h"
#include "radar.h"
#include "socketeventworker.h"
#include "socketioclient.h"
#include "systemdmonitorqt.h"
#include "utilities.h"
#include "volume.h"

#ifdef Q_OS_LINUX
extern "C" {
#include <gpiod.h>
}
#endif

// Optional features
// #include <QMqttClient>
// #define AUTOSTART_ONRPI 1
// #define MQTT_FITUR 1

#pragma pack(push, 1)
struct WavHeader
{
    char riff[4] = {'R', 'I', 'F', 'F'};
    uint32_t fileSize;

    char wave[4] = {'W', 'A', 'V', 'E'};

    char fmt[4] = {'f', 'm', 't', ' '};
    uint32_t fmtSize = 16;
    uint16_t audioFormat = 1; // PCM
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;

    char data[4] = {'d', 'a', 't', 'a'};
    uint32_t dataSize;
};
#pragma pack(pop)

enum SOCKET_STATE
{
    SOCKET_IDDLE,
    SOCKET_CONNECTING,
    SOCKET_WAITING_CONNECTED,
    SOCKET_CONNECTED,
    SOCKET_SENDING,
    SOCKET_WAITING_SENT,
    SOCKET_SENT,
    SOCKET_CLOSING
};

enum SOCKET_REQUEST
{
    SOCKET_NULL,
    SOCKET_REQ__RADAR_ERROR,
    SOCKET_REQ_FALL
};

enum LED_STRIP_COLOR
{
    COLOR_WHITE,
    COLOR_WHITE_BRIGHT,
    COLOR_WHITE_BLINKY,
    COLOR_RED

    // COLOR_GREEN,
    // COLOR_GREEN_BLINKY,
    // COLOR_BLUE,
    // COLOR_BLUE_BLINKY
};

struct bme280Data
{
    double temperatureC;
    double pressureHpa;
    double humidityPercent;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setupRealtimeDataMotion(QCustomPlot *plottsgram);
    void setupRealtimeDataVelocity(QCustomPlot *plottsVelocity);
    void setupPlotRadar(QCustomPlot *plotRadar);

    void setupRealtimeDataMotion2(QCustomPlot *plottsgram2);
    void setupRealtimeDataVelocity2(QCustomPlot *plottsVelocity2);
    void setupPlotRadar2(QCustomPlot *plotRadar2);

signals:
    // Signals are delivered to worker threads using queued connections.
    void requestSound(int sentenceIndex, QString langIndex);
    void requestBme280Read();
    void requestCpuTemperature();

private slots:
    // ---------------------------------------------------------------------
    // Radar 1
    // ---------------------------------------------------------------------
    void on_btnLoad_clicked();
    void on_btnOpenSerialPort_clicked();
    void on_btnGetProductID_clicked();
    void on_btnGetProductModel_clicked();
    void on_btnGetFirmwareVersion_clicked();
    void on_btnHWModel_clicked();
    void on_btnCmdInitCompleteCek_clicked();
    void on_btnGetAngleInst_clicked();
    void on_btnGetHeightInst_clicked();
    void on_cbPresence_toggled(bool checked);
    void on_cbFallDetection_toggled(bool checked);
    void on_btnGetFallDuration_clicked();
    void on_cbStandStill_toggled(bool checked);
    void on_cbTraceTracking_toggled(bool checked);
    void on_btnSetHeight_clicked();
    void on_btnSetFallDuration_clicked();
    void on_btnSetAngle_clicked();
    void realtimeDataSlot(QString value);
    void realtimeDataVelocity(QString value);

    // ---------------------------------------------------------------------
    // Radar 2
    // ---------------------------------------------------------------------
    void on_btnLoad2_clicked();
    void on_btnOpenSerialPort2_clicked();
    void on_btnGetProductID2_clicked();
    void on_btnGetProductModel2_clicked();
    void on_btnGetFirmwareVersion2_clicked();
    void on_btnHWModel2_clicked();
    void on_btnCmdInitCompleteCek2_clicked();
    void on_btnGetAngleInst2_clicked();
    void on_btnGetHeightInst2_clicked();
    void on_cbPresence2_toggled(bool checked);
    void on_cbFallDetection2_toggled(bool checked);
    void on_btnGetFallDuration2_clicked();
    void on_cbStandStill2_toggled(bool checked);
    void on_cbTraceTracking2_toggled(bool checked);
    void on_btnSetHeight2_clicked();
    void on_btnSetFallDuration2_clicked();
    void on_btnSetAngle2_clicked();
    void realtimeDataSlot2(QString value);
    void realtimeDataVelocity2(QString value);

    // ---------------------------------------------------------------------
    // Socket.IO and device communication
    // ---------------------------------------------------------------------
    void onSocketEventReceived(const QString &eventName, const QJsonValue &data);
    void onDeviceReadyConnected(int vol, int bright);
    void on_btnConnect_clicked();
    void on_btnFallSimulation_clicked();
    void on_btnEmitEvenwAck_clicked();
    void on_btnEmitListeningOn_clicked();
    void on_btnPing_clicked();
    void on_btnLogin_clicked();

    // ---------------------------------------------------------------------
    // LED, brightness, and volume
    // ---------------------------------------------------------------------
    void on_btnColor1_clicked();
    void on_btnColor2_clicked();
    void on_btnColor3_clicked();
    void on_btnColor4_clicked();
    void on_hsBrightness_valueChanged(int value);
    void on_btnGetBrightness_clicked();
    void on_btnsetBrightness_clicked();
    void on_hsVol_valueChanged(int value);
    void on_btnGetVol_clicked();
    void on_btnsetVol_clicked();
    void onVolumeGetRequested();
    void onVolumeSetRequested(int value);
    void onVolumeIncreaseReq();
    void onVolumeDecreaseReq();
    void onVolumeChanged(int percent);
    void onBrightnessSetRequested(int value);
    void onBrightnessGetRequested();
    void onBrihtnessIncreaseReq();
    void onBrightnessDecreaseReq();
    void slotGpioTimer();

    // ---------------------------------------------------------------------
    // Robot speech and operating states
    // ---------------------------------------------------------------------
    void onListenStateChanged();
    void onTalkingStateChanged();
    void onWaiting();
    void onRecording();
    void onPingDeviceUpRequested();
    void onSleepRequested();
    void onWakeUpRequested();
    void onSpeechModuleReady();

    // ---------------------------------------------------------------------
    // Incident and fall handling
    // ---------------------------------------------------------------------
    void onIncidentFallEventDetected();
    void onIncidentFallCancel();
    void onIncidentFallWakeUpByFallDetection();
    void onIncidentAckFallEventDetected();
    void onIncidentFallNoResponse();
    void onIncidentIamnotOK();
    void onIncidentIamOK();
    void onIncidentFallHelpEventDetected();
    void onIncidentFallOKEventDetected();
    void onIncidentFallCompleted();
    void slotTimerSendFallEvent();
    void slotTimerHeartBeat();
    void onRadarHeartBeatDetected();

    // ---------------------------------------------------------------------
    // Language
    // ---------------------------------------------------------------------
    void onlangCurrent(QString langstr);

#ifdef Q_OS_LINUX
    // ---------------------------------------------------------------------
    // Wi-Fi and Raspberry Pi system controls
    // ---------------------------------------------------------------------
    void onWifiOnRequest();
    void onWifiOffRequest();
    void onwifiScanSsidReqReceived();
    void onWifiGetSsidRequest();
    void onWifiSsidListRequest();
    void onWifiSsidListRequestComplete();
    void onWifiSSidListReady(QStringList ssidList);
    void onWifiSSidListReadyComplete(QList<WifiAP> wifiList);
    void onWifiSSidListReadyCompleteRequest(QList<WifiAP> wifiList);
    void onCurrentSSidRequest();
    void onWifiConnectRequest(const QString &ssid, const QString &pwd);
    void onWifiForgetRequest(const QString &ssid);
    void onCurrentWifiInfoReady(QJsonObject obj);
    void onWifiConnected(bool success,
                         const QString &ssid,
                         const QString &ip,
                         const QString gateway);
    void onWifiDisconnectRequest();
    void onwifiDisconnectResult(bool success, QString ssid, QString message);
    void onWifiEnabled(bool on);
    void onWifiDeleted(bool success, QString ssid, QString message);
    void onWifiProgress(int state, QString stateText);
    void onWifiConnectFinished(bool success,
                               QString ssid,
                               QString ip,
                               QString gateway);

    void on_btnScanWifiList_clicked();
    void on_btnGetSSID_clicked();
    void on_btnWifiCon_clicked();
    void on_btnWifiOff_clicked();
    void on_btnWifiOn_clicked();
    void on_btnForget_clicked();
    void on_btnRestart_clicked();
    void on_btnShutdown_clicked();
    void on_btnSetTZ_clicked();

    void onMonitorWlan0Connected();
    void onMonitorWlan0Disconnected();
    void onMonitorWlan0WifiSignalLost();
    void onMonitorWlan0networkInterfaceDown();
    void onMonitorWlan0ipAddressChanged(QString ip);

    void onRpiRestart();
    void onRpiShutdown();
    void onTzSetReq(QString tz);
    void onTzGetReq();
#endif

    // ---------------------------------------------------------------------
    // Audio controls and recording
    // ---------------------------------------------------------------------
    void on_btnPlaySound_clicked();
    void on_btnPlayFall_clicked();
    void on_btnPlayHelp_clicked();
    void on_btnPlayIamOK_clicked();
    void on_btnRec_pressed();
    void on_btnRec_released();
    void on_btnRec_clicked();
    void on_btnPlayRec_clicked();
    void readMore();
    void handleAudioData();
    void handleFinished();

    // ---------------------------------------------------------------------
    // Power, environmental sensors, and health information
    // ---------------------------------------------------------------------
    void onPzemDataReadyComplete(Pzem004Tv30Data data);
    void onPowerInfoReq();
    void onAudioInfoReq();
    void onBme280ReadingReady(double temperatureC,
                              double pressureHpa,
                              double humidityPercent);
    void onBme280Error(const QString &message);
    void onCpuTemperatureReady(double temperatureC);
    void onCpuTemperatureError(const QString &message);

    // ---------------------------------------------------------------------
    // Sound worker results
    // ---------------------------------------------------------------------
    void onSoundFinished(int sentenceIndex, QString langIndex);
    void onSoundFailed(int sentenceIndex,
                       QString langIndex,
                       QString errorMessage);
    void onUploadFailed();

private:
    // ---------------------------------------------------------------------
    // UI and general state
    // ---------------------------------------------------------------------
    Ui::MainWindow *ui;
    QString demoName;
    QString lang;
    QString wifiState = "";
    int m_volCurrent;

    // ---------------------------------------------------------------------
    // Socket.IO and payload workers
    // ---------------------------------------------------------------------
    SocketIOClient *client;
    SocketEventWorker *m_worker;
    QThread *m_workerThread;

    QThread *m_threadA;
    QThread *m_threadB;
    PayloadProcessor *m_procA;
    PayloadProcessor *m_procB;

    // ---------------------------------------------------------------------
    // Audio worker
    // ---------------------------------------------------------------------
    QThread *m_audioThread;
    AudioWorker *m_audioWorker;
    AudioHealthChecker m_audioCheck;

#ifdef Q_OS_LINUX
    // ---------------------------------------------------------------------
    // Linux-specific services and GPIO
    // ---------------------------------------------------------------------
    VolumeMonitor *m_volumeMonitor;
    gpio *m_gpio;
    volume *m_volume;
    brightness *m_brightness;
    utilities *m_utility;
    systemdmonitorqt *systemdymon;
    QTimer *gpioTimer;
    QElapsedTimer gpioElapsedTimer;

    //static constexpr quint8 PWM_PIN = 2;       // GPIO22
    static constexpr quint8 PWM_PIN = 1;       // GPIO17

    static constexpr qint64 PWM_PERIOD_US = 1000000; // 1 Hz

    quint8 pwmDutyPercent = 0;
    qint64 pwmHighTimeUs = 0;

    bool pwmOutputState = false;

    void requestPWM(quint8 persen);

    gpiod_line *line17;
    gpiod_line *line27;
    gpiod_line *line22;
#endif

    // ---------------------------------------------------------------------
    // Radar coordinates, graphs, serial ports, and frames
    // ---------------------------------------------------------------------
    float radarX = 0;
    float radarY = 0;
    float radar2X = 0;
    float radar2Y = 0;

    QCPGraph *radarPoint;
    QCPGraph *radarPoint2;

    QSerialPort *m_serial = nullptr;
    QByteArray m_buffer;
    QSerialPort *m_serial2 = nullptr;
    QByteArray m_buffer2;

    FrameRadarData radarFrame;
    FrameRadarData radarFrame2;
    bool fallEmergency = false;

    // ---------------------------------------------------------------------
    // Timers and heartbeat state
    // ---------------------------------------------------------------------
    QTimer *timerSendFallevent = nullptr;
    QTimer *timerHeartBeatCounter;
    bool fallEventAckReceived = false;
    quint8 radar1UartHeartBeatCounter;
    quint8 radar2UartHeartBeatCounter;

    // ---------------------------------------------------------------------
    // BME280 and CPU temperature workers
    // ---------------------------------------------------------------------
    QThread *m_bmeThread = nullptr;
    Bme280Worker *m_bmeWorker = nullptr;
    bme280Data mbme280data;

    QThread *m_cpuTemperatureThread = nullptr;
    CpuTemperatureWorker *m_cpuTemperatureWorker = nullptr;
    double cpuTempC;

    // ---------------------------------------------------------------------
    // PZEM
    // ---------------------------------------------------------------------
    Pzem004Tv30Qt *m_pzem = nullptr;

    // ---------------------------------------------------------------------
    // Initialization
    // ---------------------------------------------------------------------
    void initSound();
    void initGraphics();
    void initRadar();
    void initSocketIO();
    void initAudioSystem();
    void initPzem();
    void initBME280();
    void initCpuTemp();
    void initUtility();

    // ---------------------------------------------------------------------
    // Radar and plot helpers
    // ---------------------------------------------------------------------
    void init_radar();
    void updateRadarPoint(double x, double y);
    void updateRadarPoint2(double x, double y);

    void fillPortsInfo();
    void fillPortsInfo2();

    quint8 calcChecksum(const QByteArray &frame);
    QByteArray makeFrame(const QByteArray &body);
    QString toHexSpace(const QByteArray &data);

    void setupPlotTs();
    void setupPlotTsVelocity();
    void setupPlotTs2();
    void setupPlotTsVelocity2();

    void drawRealTimeetsgram(QString motion);
    void drawRealTimeVelocity(QString velocity);
    void drawRealTimeetsgram2(QString motion);
    void drawRealTimeVelocity2(QString velocity);

    // ---------------------------------------------------------------------
    // Sound and recording helpers
    // ---------------------------------------------------------------------
    void soundPlay(int request, const QString &lang = "sv");
    void startRecording();
    void stopRecording();
    void loadWav(const QString &path);
    void processBuffer();
    double calculateDb(const QByteArray &data);

    void runAudioHealthRecordTest();
    void getMicTargetFromWpctlStatusAsync(
        std::function<void(const QString &micTarget,
                           const QString &err)> callback);

    QString parseMicTargetFromWpctlStatus(
        const QString &output,
        QString *err = nullptr) const;

    void getAudioTargetsFromWpctlStatusAsync(
        std::function<void(const QString &micTarget,
                           const QString &speakerTarget,
                           const QString &err)> callback);

    bool parseAudioTargetsFromWpctlStatus(const QString &output,
                                          QString *micTarget,
                                          QString *speakerTarget,
                                          QString *err = nullptr) const;

    // ---------------------------------------------------------------------
    // System, network, and application helpers
    // ---------------------------------------------------------------------
    QString runCommand(const QString &cmd);
    void stopAllThreads();
    void stopAllProcesses();
    void restartApp();
    void getLangCommand();
    void getCputemp();
};

#endif // MAINWINDOW_H
