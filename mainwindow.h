#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QVector>
#include <QRandomGenerator>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QTimer>
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QDir>
#include <QListWidget>
#include <QQueue>
#include <QtEndian>
#include <QScreen>   // Qt6 pengganti QDesktopWidget
#include "qcustomplot.h"
#include <QWebSocket>
#include <QTimer>
#include <QDebug>
#include <QProcess>
#include <QTimer>
#include <QElapsedTimer>
#include <QWebSocket>
#include <QtMultimedia/qaudio.h>
#include <socketeventworker.h>
#include <QThread>
#include <radar.h>
#include <socketioclient.h>
#include <payloadprocessor.h>
#include <QThread>
#include <audioworker.h>
#include <gpio.h>
#include <volume.h>
#include <brightness.h>
#include <utilities.h>
#include <QDateTime>
#include <QDebug>
#include <QNetworkInformation>
#include <VolumeMonitor.h>
#include <networkmonitor.h>
#include <QIODevice>
#include <QVBoxLayout>
#include <QWidget>
#include <QtMath>
#include <QDebug>
#include <qdebug.h>
#include <QFile>
#include <QBuffer>
#include <QTimer>
#include <QFile>
#include <systemdmonitorqt.h>
#include <QTimer>
#include "Pzem004Tv30Qt.h"
#include "AudioHealthChecker.h"
#include <QRegularExpression>
#include <functional>

//#include <QMqttClient>

//#define AUTOSTART_ONRPI 1
//#define MQTT_FITUR 1

#ifdef Q_OS_LINUX
extern "C" {
#include <gpiod.h>
}
#endif

#pragma pack(push, 1)
struct WavHeader {
    char riff[4] = {'R','I','F','F'};
    uint32_t fileSize;

    char wave[4] = {'W','A','V','E'};

    char fmt[4] = {'f','m','t',' '};
    uint32_t fmtSize = 16;
    uint16_t audioFormat = 1; // PCM
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;

    char data[4] = {'d','a','t','a'};
    uint32_t dataSize;
};
#pragma pack(pop)



enum SOCKET_STATE{
    SOCKET_IDDLE,
    SOCKET_CONNECTING,
    SOCKET_WAITING_CONNECTED,
    SOCKET_CONNECTED,

    SOCKET_SENDING,
    SOCKET_WAITING_SENT,
    SOCKET_SENT,
    SOCKET_CLOSING
};

enum SOCKET_REQUEST{
    SOCKET_NULL,
    SOCKET_REQ__RADAR_ERROR,
    SOCKET_REQ_FALL
};

enum LED_STRIP_COLOR{
    COLOR_WHITE,
    COLOR_WHITE_BRIGHT,
    COLOR_WHITE_BLINKY,
    COLOR_RED
    //COLOR_GREEN,
    //COLOR_GREEN_BLINKY,
    //COLOR_BLUE,
    //COLOR_BLUE_BLINKY
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

public slots:
    //void updatePlot(const QVector<double> &values);   //

signals:
    /*
     * Signal ini dikirim dari GUI thread menuju AudioWorker thread.
     * Karena memakai Qt::QueuedConnection, pemanggilannya non-blocking.
     */
    void requestSound(int sentenceIndex, QString langIndex);

private slots:
    void on_btnLoad_clicked();
    void on_btnOpenSerialPort_clicked();

    void on_btnLoad2_clicked();
    void on_btnOpenSerialPort2_clicked();

    void on_btnGetProductID_clicked();
    void on_btnGetProductModel_clicked();
    void on_btnGetFirmwareVersion_clicked();

    void on_btnGetProductID2_clicked();
    void on_btnGetProductModel2_clicked();
    void on_btnGetFirmwareVersion2_clicked();

    void realtimeDataSlot(QString value);
    void realtimeDataVelocity(QString value);

    void realtimeDataSlot2(QString value);
    void realtimeDataVelocity2(QString value);

    void on_btnHWModel_clicked();
    void on_btnCmdInitCompleteCek_clicked();

    void on_btnHWModel2_clicked();
    void on_btnCmdInitCompleteCek2_clicked();

    void on_btnGetAngleInst_clicked();
    void on_btnGetHeightInst_clicked();

    void on_btnGetAngleInst2_clicked();
    void on_btnGetHeightInst2_clicked();

    void on_cbPresence_toggled(bool checked);
    void on_cbFallDetection_toggled(bool checked);
    void on_btnGetFallDuration_clicked();
    void on_cbStandStill_toggled(bool checked);
    void on_cbTraceTracking_toggled(bool checked);

    void on_cbPresence2_toggled(bool checked);
    void on_cbFallDetection2_toggled(bool checked);
    void on_btnGetFallDuration2_clicked();
    void on_cbStandStill2_toggled(bool checked);
    void on_cbTraceTracking2_toggled(bool checked);

    void on_btnSetHeight_clicked();
    void on_btnSetHeight2_clicked();

    void on_btnSetFallDuration_clicked();
    void on_btnSetAngle_clicked();

    void on_btnSetFallDuration2_clicked();
    void on_btnSetAngle2_clicked();

    //void on_btnTestFall_clicked();
    //void on_btnTestFall2_clicked();
    void on_btnPlaySound_clicked();

    void onSocketEventReceived(const QString &eventName, const QJsonValue &data);
    void onDeviceReadyConnected(int vol, int bright);

    void on_btnColor1_clicked();
    void on_btnColor2_clicked();
    void on_btnColor3_clicked();
    void on_btnColor4_clicked();

    void on_hsBrightness_valueChanged(int value);
    void on_btnGetBrightness_clicked();
    void on_btnsetBrightness_clicked();
    void on_btnGetVol_clicked();

    void on_hsVol_valueChanged(int value);
    void on_btnsetVol_clicked();

    void on_btnConnect_clicked();
    void on_btnFallSimulation_clicked();

    void onListenStateChanged();
    void onTalkingStateChanged();
    void onWaiting();
    void onRecording();
    void onVolumeGetRequested();
    void onVolumeSetRequested(int value);
    void onPingDeviceUpRequested();
    void onSleepRequested();
    void onWakeUpRequested();
    void onSpeechModuleReady();
    void onBrightnessSetRequested(int value);
    void onBrightnessGetRequested();

    void onVolumeIncreaseReq();
    void onVolumeDecreaseReq();
    void onVolumeChanged(int percent);

    //void onVolumeChanged(int percent);

    void onBrihtnessIncreaseReq();
    void onBrightnessDecreaseReq();

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

    //Language
    void onlangCurrent(QString langstr);

    //Wifi
#ifdef Q_OS_LINUX
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

    void onWifiConnectRequest(const QString &ssid,const QString &pwd);
    void onWifiForgetRequest(const QString &ssid);
    //void onSsidReady(QString ssid);
    void onCurrentWifiInfoReady(QJsonObject obj);
    void onWifiConnected(bool success,
                         const QString &ssid,
                         const QString &ip,
                         const QString gateway);
    void onWifiDisconnectRequest();
    void onwifiDisconnectResult(bool success,
                                QString ssid,
                                QString message);
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

    void on_btnPlayFall_clicked();
    void on_btnPlayHelp_clicked();
    void on_btnPlayIamOK_clicked();

    void on_btnEmitEvenwAck_clicked();
    void on_btnEmitListeningOn_clicked();
    void readMore();
    void on_btnRec_pressed();
    void on_btnRec_released();
    void handleAudioData();

    void on_btnPlayRec_clicked();
    void handleFinished();


    void on_btnRec_clicked();
    void on_btnPing_clicked();

    //PZEM
    void onPzemDataReadyComplete(Pzem004Tv30Data data);

    //DEvice
    void onPowerInfoReq();
    void onAudioInfoReq();

    void on_btnLogin_clicked();

    //Sound
    void onSoundFinished(int sentenceIndex, QString langIndex);
    void onSoundFailed(
        int sentenceIndex,
        QString langIndex,
        QString errorMessage
        );

private:
    Ui::MainWindow *ui;
    QString demoName;

    SocketIOClient *client;
    SocketEventWorker *m_worker;
    QThread *m_workerThread;

    QThread *m_threadA;
    QThread *m_threadB;

    PayloadProcessor *m_procA;
    PayloadProcessor *m_procB;

    QThread *m_audioThread;
    AudioWorker *m_audioWorker;

#ifdef Q_OS_LINUX
    VolumeMonitor *m_volumeMonitor;

    gpio *m_gpio;
    volume *m_volume;
    brightness *m_brightness;
    utilities *m_utility;
#endif


    int m_volCurrent;
    //int brightnessCurrent;

    float radarX = 0;
    float radarY = 0;

    float radar2X = 0;
    float radar2Y = 0;

    QCPGraph *radarPoint;
    void updateRadarPoint(double x, double y);
    void init_radar();

    QCPGraph *radarPoint2;
    void updateRadarPoint2(double x, double y);
    //void init_radar2();

    QSerialPort *m_serial = nullptr;
    QByteArray m_buffer;

    QSerialPort *m_serial2 = nullptr;
    QByteArray m_buffer2;

    QTimer *timerSendFallevent = nullptr;
    bool fallEventAckReceived = false;

    QString radarReportInfo;

#ifdef Q_OS_LINUX
    gpiod_line *line17;
    gpiod_line *line27;
    gpiod_line *line22;
#endif

    FrameRadarData radarFrame;
    FrameRadarData radarFrame2;

    quint8 calcChecksum(const QByteArray &frame);

    void fillPortsInfo();
    void fillPortsInfo2();

    QByteArray makeFrame(const QByteArray &body);
    QString toHexSpace(const QByteArray &data);

    void initSound();
    void initGraphics();

    void initRadar();
    void initSocketIO();

    void setupPlotTs();
    void setupPlotTsVelocity();

    void setupPlotTs2();
    void setupPlotTsVelocity2();

    void drawRealTimeetsgram(QString motion);
    void drawRealTimeVelocity(QString velocity);

    void drawRealTimeetsgram2(QString motion);
    void drawRealTimeVelocity2(QString velocity);

    void soundPlay(int request, const QString &lang = "sv");

    //Wifi healthy
    QString runCommand(const QString &cmd);
    QString wifiState = "";

    void startRecording();
    void stopRecording();

    void loadWav(const QString &path);
    void processBuffer();

    // Audio
    QString lang;
    AudioHealthChecker m_audioCheck;
   // AudioHealthChecker.Report mReport;
    void runAudioHealthRecordTest();
    void getMicTargetFromWpctlStatusAsync(
        std::function<void(const QString &micTarget,
                           const QString &err)> callback
        );

    QString parseMicTargetFromWpctlStatus(
        const QString &output,
        QString *err = nullptr) const;

    void getAudioTargetsFromWpctlStatusAsync(
        std::function<void(const QString &micTarget,
                           const QString &speakerTarget,
                           const QString &err)> callback
        );

    bool parseAudioTargetsFromWpctlStatus(const QString &output,
                                          QString *micTarget,
                                          QString *speakerTarget,
                                          QString *err = nullptr) const;

    // Function hitung dB
    double calculateDb(const QByteArray &data);
    void initAudioSystem();

    //PZEM
    Pzem004Tv30Qt *m_pzem = nullptr;
    void initPzem(void);

    //Monitoring system
#ifdef Q_OS_LINUX
    systemdmonitorqt *systemdymon;
#endif

    void stopAllThreads();
    void stopAllProcesses();
    void restartApp();

    //Language Info request
    void getLangCommand();
};

#endif // MAINWINDOW_H
