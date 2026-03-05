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

    void sleepRequested();
    void wakeupRequested();
    void pingDeviceUpRequested();
    void listenStateChanged(const QString &state);
    void talkingStateChanged(const QString &state);

    void incidentFall();
    void incidentHelp();
    void incidentIamOK();
    void incidentIamnotOK();

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
