#pragma once

#include <QObject>
#include <QQueue>
#include <QPair>
#include <QMutex>
#include <QWaitCondition>
#include <QJsonValue>

class SocketEventWorker : public QObject
{
    Q_OBJECT
public:
    explicit SocketEventWorker(QObject *parent = nullptr);
    ~SocketEventWorker();

    void enqueue(const QString &eventName, const QJsonValue &data);
    void stop();

signals:
    void volumeGetRequested();
    void volumeSetRequested(int value);

    void volumeIncreaseReq();
    void volumeDecreaseReq();

    void brightnessSetRequested(int value);
    void brightnessGetRequested();

    void brightnessIncreaseReq();
    void brightnessDecreaseReq();

    //Robot mode
    void modeSleep();
    void modeWakeUp();
    void pingDeviceUp();
    void modeListen();
    void modeRecording();
    void modeWaiting();
    void modeTalking();
    void speechModuleReady();

    //Language
    void langCurrent(); //LANGUAGE_CURRENT
    void langGet(); //LANGUAGE_GET
    void langSet(); //LANGUAGE_SET
    void langAckSet(); //ACK_LANGUAGE_SET

    //Fall
    void incidentFall();
    void incidentHelp();
    void incidentIamOK();
    void incidentIamnotOK();
    void incidentFallEventDetected();
    void incidentFallWakeUpByFallDetection();
    void incidentFallAckFallEventDetected();
    void incidentFallNoResponse();
    void incidentFallHelpEventDetected();
    void incidentFallOKEventDetected();
    void incidentFallCompleted();
    void incidentFallIamnotOK();
    void incidentFallIamOK();

    //Alarm
    void alarmRing();         //ALARM_RING
    void alarmStop();         //ALARM_STOP
    void alarmSnooze();       //ALARM_SNOOZE
    void alarmWakeUp();       //WAKE_UP_BY_ALARM
    void alarmStopButton();   //ALARM_STOP_BUTTON
    void alarmSnoozeButton(); //ALARM_SNOOZE_BUTTON

    //Wifi
    void wifiOn();
    void wifiOff();
    void wifiGetSsid();
    void wifiScanSsidReqReceived();
    void wifiDisconnectCurrentSsid();
    void wifiSsidList();
    void wifiSsidListComplete();
    void wifiConnect(const QString &ssid,const QString &pwd);
    void wifiForget(const QString &ssid);

    //utility
    void rpiRestart();
    void rpiShutdown();
    void tzSetReq(QString tz);
    void tzGetReq();

public slots:
    void process();

private:
    QQueue<QPair<QString, QJsonValue>> m_queue;
    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_running;
};
