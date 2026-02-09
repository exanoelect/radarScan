#include "socketeventworker.h"
#include <QJsonObject>
#include <QDebug>

SocketEventWorker::SocketEventWorker(QObject *parent)
    : QObject(parent), m_running(true)
{
}

//------------------------------------------------------------------------
void SocketEventWorker::stop()
{
    QMutexLocker locker(&m_mutex);
    m_running = false;
    m_cond.wakeAll();
}

//------------------------------------------------------------------------
void SocketEventWorker::enqueue(const QString &eventName, const QJsonValue &data)
{
    QMutexLocker locker(&m_mutex);
    m_queue.enqueue(qMakePair(eventName, data));
    m_cond.wakeOne();
}

//------------------------------------------------------------------------
void SocketEventWorker::process()
{
    while (true) {
        m_mutex.lock();
        while (m_queue.isEmpty() && m_running) {
            m_cond.wait(&m_mutex);   // tidur sampai ada data
        }

        if (!m_running) {
            m_mutex.unlock();
            break;
        }

        auto event = m_queue.dequeue();
        m_mutex.unlock();

        const QString &eventName = event.first;
        const QJsonValue &data = event.second;

        qDebug() << "Worker processing event:" << eventName << "data:" << data;

        if (eventName == "LISTEN" && data.isString()) {
            emit listenStateChanged(data.toString());
        }
        else if (eventName == "TALKING" && data.isString()) {
            emit talkingStateChanged(data.toString());
        }
        else if (eventName == "VOLUME_GET") {
            emit volumeGetRequested();
        }
        else if (eventName == "VOLUME_SET") {
            int vt = 0;
            if (data.isDouble()) vt = data.toInt();
            else if (data.isString()) vt = data.toString().toInt();
            else if (data.isObject()) {
                QJsonObject obj = data.toObject();
                vt = obj.value("level").toInt(0);
            }
            emit volumeSetRequested(vt);
        }
        else if (eventName == "PING_DEVICE_UP") {
            emit pingDeviceUpRequested();
        }
        else if (eventName == "SLEEP") {
            emit sleepRequested();
        }
        else if (eventName == "SCREEN_BRIGHTNESS_SET") {
            int bst = 0;
            if (data.isDouble()) bst = data.toInt();
            else if (data.isString()) bst = data.toString().toInt();
            else if (data.isObject()) {
                QJsonObject obj = data.toObject();
                bst = obj.value("level").toInt(0);
            }
            emit brightnessSetRequested(bst);
        }else if(eventName == "INCREASE_VOLUME"){
            emit volumeIncreaseReq();
        }else if(eventName == "DECREASE_VOLUME"){
            emit volumeDecreaseReq();
        }else if(eventName == "INCREASE_BRIGHTNESS"){
            emit brightnessIncreaseReq();
        }else if(eventName == "DECREASE_BRIGHTNESS"){
            emit brightnessDecreaseReq();
        }
    }
}
