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

public slots:
    void process();

private:
    QQueue<QPair<QString, QJsonValue>> m_queue;
    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_running;
};
