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
        else if (eventName == "SOUND_VOLUME_GET") {
            emit volumeGetRequested();
        }
        else if (eventName == "SOUND_VOLUME_SET") {
            int vt = 0;
            if (data.isDouble()) vt = data.toInt();
            else if (data.isString()) vt = data.toString().toInt();
            else if (data.isObject()) {
                QJsonObject obj = data.toObject();
                vt = obj.value("level").toInt(0);
            }
            emit volumeSetRequested(vt);
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
        }else if(eventName == "SCREEN_BRIGHTNESS_GET"){
            emit brightnessGetRequested();
        }
        else if (eventName == "PING_DEVICE_UP") {
            emit pingDeviceUpRequested();
        }
        else if (eventName == "SLEEP") {
            emit sleepRequested();
        }else if(eventName == "INCIDENT_HELP_EVENT_DETECTED"){
            emit incidentFall();
        }else if(eventName == "INCIDENT_NOT_OK_EVENT_DETECTED"){
            emit incidentIamnotOK();
        }else if (eventName == "i_am_ok"){
            emit incidentIamOK();
        }
        //WIFI
        else if (eventName == "WIFI_ON") {
            qDebug() << "Wifi request:" << eventName;
            emit wifiOn();
        }
        else if (eventName == "WIFI_OFF") {
            qDebug() << "Wifi request:" << eventName;
            emit wifiOff();
        }
        else if (eventName == "disconnect_wifi") {
            qDebug() << "Wifi disconnect from current ssid :" << eventName;
            emit wifiDisconnectCurrentSsid();
        }
        else if (eventName == "get_wifi_status") {
            qDebug() << "Wifi request:" << eventName;
            emit wifiGetSsid();
        }
        else if (eventName == "scan_wifi_stream") {
            qDebug() << "Wifi scan re received:" << eventName;
            emit wifiScanSsidReqReceived();
        }
        //else if (eventName == "WIFI_SSID_LIST") {
        //    qDebug() << "Wifi request:" << eventName;
        //    emit wifiSsidListComplete();//wifiSsidList();
        //}
        else if (eventName.startsWith("connect_wifi")) {
          if (data.isObject()) {
                QJsonObject obj = data.toObject();
                QString ssid = obj["ssid"].toString();
                QString pwd = obj["pwd"].toString();
                qDebug() << "Wifi request:" << eventName << obj;
                emit wifiConnect(ssid,pwd);
            }
        }
        else if (eventName.startsWith("WIFI_SSID_FORGET")) {
            if (data.isObject()) {
                  QJsonObject obj = data.toObject();
                  QString ssid = obj["ssid"].toString();
                  qDebug() << "Wifi request:" << eventName << obj;
                  emit wifiForget(ssid);
            }
        }
        else if (eventName == "DEVICE_RESTART") {
            qDebug() << "Rpi restart" << eventName;
            emit rpiRestart();
        }
        else if (eventName == "DEVICE_OFF") {
            qDebug() << "Rpi OFF" << eventName;
            emit rpiShutdown();
        }
    }
}
