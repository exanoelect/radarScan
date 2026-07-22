#include "socketeventworker.h"
#include <QJsonObject>
#include <QDebug>

SocketEventWorker::SocketEventWorker(QObject *parent)
    : QObject(parent), m_running(true)
{
}

SocketEventWorker::~SocketEventWorker(){

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

        //Robot mode
        if (eventName == "LISTENING") {
            emit modeListen();
        }else if (eventName == "TALKING") {
            emit modeTalking();
        }else if (eventName == "PING_DEVICE_UP") {
            emit pingDeviceUp();
        }else if (eventName == "SLEEP") {
            emit modeSleep();
        }else if(eventName == "WAKE_UP"){
            emit modeWakeUp();
        }else if (eventName == "WAITING") {
            emit modeWaiting();
        }else if (eventName == "RECORDING") {
            emit modeRecording();
        }else if(eventName == "SPEECH_MODULE_READY"){
            emit speechModuleReady();
        }else if(eventName == "UPLOAD_FAILED"){
            emit modeUploadFailed();
        }

        //Brightness, Volume
        else if (eventName == "VOLUME_SET_REQUEST") {
            emit volumeGetRequested();
        }else if (eventName == "VOLUME_SET") {
            int vt = 0;
            if (data.isDouble()) vt = data.toInt();
            else if (data.isString()) vt = data.toString().toInt();
            else if (data.isObject()) {
                QJsonObject obj = data.toObject();
                vt = obj.value("level").toInt(0);
            }
            emit volumeSetRequested(vt);
        }else if (eventName == "SCREEN_BRIGHTNESS_SET") {
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
        }else if(eventName == "SCREEN_BRIGHTNESS_REQUEST"){
            emit brightnessGetRequested();
        }

        //ALARM
        else if (eventName == "ALARM_RING") {
            emit alarmRing();
        }else if (eventName == "ALARM_STOP") {
            emit alarmStop();
        }else if (eventName == "ALARM_SNOOZE") {
            emit  alarmSnooze();
        }else if (eventName == "WAKE_UP_BY_ALARM") {
            emit  alarmWakeUp();
        }else if (eventName == "ALARM_STOP_BUTTON") {
            emit  alarmStopButton();
        }else if (eventName == "ALARM_SNOOSE_BUTTON") {
            emit  alarmSnoozeButton();
        }

        //LAnguage
        else if (eventName == "LANGUAGE_CURRENT") {
            if (data.isObject()) {
                QJsonObject obj = data.toObject();
                QString lang = obj["lang"].toString();
                qDebug() << "lang current:" << eventName << obj;
                emit langCurrent(lang);
            }
        }else if(eventName == "LANGUAGE_GET"){
            emit langGet();
        }else if(eventName == "LANGUAGE_SET"){
            if (data.isObject()) {
                QJsonObject obj = data.toObject();
                QString ret = obj["success"].toString();
                QString lang = obj["lang"].toString();
                qDebug() << "lang current:" << eventName << obj;
                emit langSet(lang);
            }
        }else if(eventName == "ACK_LANGUAGE_SET"){
            emit langAckSet();
        }

        //FALL EVENT DETECTED
        else if(eventName == "INCIDENT_FALL_EVENT_DETECTED"){    //1
            emit incidentFallEventDetected();
        }else if(eventName == "WAKE_UP_BY_FALL_DETECTION"){      //2
            emit incidentFallWakeUpByFallDetection();
        }else if(eventName == "ACK_FALL_EVENT_DETECTED"){        //3
            emit incidentAckFallEventDetected();
        }else if(eventName == "INCIDENT_FALL_DOWN_NO_RESPONSE"){  //4
            emit incidentFallNoResponse();
        }else if(eventName == "INCIDENT_HELP_EVENT_DETECTED"){   //5
            emit incidentFallHelpEventDetected();
        }else if(eventName == "INCIDENT_OK_EVENT_DETECTED"){     //6
            emit incidentFallOKEventDetected();
        }else if(eventName == "INCIDENT_COMPLETED"){             //7
            emit incidentFallCompleted();
        }else if(eventName == "INCIDENT_NOT_OK_EVENT_DETECTED"){
            emit incidentFallIamnotOK();
        }else if (eventName == "i_am_ok"){
            emit incidentFallIamOK();
        }

        //WIFI ori
        else if (eventName == "WIFI_ON") {
            qDebug() << "Wifi request:" << eventName;
            emit wifiOn();
        }else if (eventName == "WIFI_OFF") {
            qDebug() << "Wifi request:" << eventName;
            emit wifiOff();
        }else if (eventName == "disconnect_wifi") {
            qDebug() << "Wifi disconnect from current ssid :" << eventName;
            emit wifiDisconnectCurrentSsid();
        }else if (eventName == "get_wifi_status") {
            qDebug() << "Wifi request:" << eventName;
            emit wifiGetSsid();
        }else if (eventName == "scan_wifi_stream") {
            qDebug() << "Wifi scan re received:" << eventName;
            emit wifiScanSsidReqReceived();
        }else if (eventName.startsWith("connect_wifi")) {
          if (data.isObject()) {
                QJsonObject obj = data.toObject();
                QString ssid = obj["ssid"].toString();
                QString pwd = obj["password"].toString();
                qDebug() << "Wifi request:" << eventName << obj;
                emit wifiConnect(ssid,pwd);
            }
        }else if (eventName.startsWith("FORGET_WIFI")) {
            if (data.isObject()) {
                  QJsonObject obj = data.toObject();
                  QString ssid = obj["ssid"].toString();
                  qDebug() << "Wifi request:" << eventName << obj;
                  emit wifiForget(ssid);
            }
        }

        //Wifi CAPITAL
        else if (eventName == "DISCONNECT_WIFI") {
                qDebug() << "Wifi disconnect from current ssid :" << eventName;
                emit wifiDisconnectCurrentSsid();
        }else if (eventName == "GET_WIFI_STATUS") {
                  qDebug() << "Wifi request:" << eventName;
                  emit wifiGetSsid();
        }else if (eventName == "SCAN_WIFI_STREAM") {
                  qDebug() << "Wifi scan re received:" << eventName;
                  emit wifiScanSsidReqReceived();
        }else if (eventName.startsWith("CONNECT_WIFI")) {
                if (data.isObject()) {
                    QJsonObject obj = data.toObject();
                    QString ssid = obj["ssid"].toString();
                    QString pwd = obj["password"].toString();
                    qDebug() << "Wifi request:" << eventName << obj;
                    emit wifiConnect(ssid,pwd);
                }
        }else if (eventName.startsWith("WIFI_SSID_FORGET")) {
                if (data.isObject()) {
                    QJsonObject obj = data.toObject();
                    QString ssid = obj["ssid"].toString();
                    qDebug() << "Wifi request:" << eventName << obj;
                    emit wifiForget(ssid);
                    }
        } else if (eventName == "wifi_on") {
                   qDebug() << "Wifi request:" << eventName;
                   emit wifiOn();
        }else if (eventName == "wifi_off") {
            qDebug() << "Wifi request:" << eventName;
            emit wifiOff();
        }

        //TimeZone
        else if (eventName.startsWith("TIMEZONE_SET")) {
            if (data.isObject()) {
                  QJsonObject obj = data.toObject();
                  QString tz = obj["timezone"].toString();
                  qDebug() << "timezone set request:" << eventName << obj;
                  emit tzSetReq(tz);
            }
        }else if (eventName.startsWith("TIMEZONE_GET")) {
            if (data.isObject()) {
                  qDebug() << "timezone get request:" << eventName;
                  emit tzGetReq();
            }
        }

        //Utility
        else if (eventName == "DEVICE_RESTART") {
            qDebug() << "Rpi restart" << eventName;
            emit rpiRestart();
        }
        else if (eventName == "DEVICE_OFF") {
            qDebug() << "Rpi OFF" << eventName;
            emit rpiShutdown();
        }

        //Device Status health
        else if (eventName == "DEVICE_STATUS_GET") {
            qDebug() << "dev status get";
            emit audioRadarInfoRequest();
        }else if(eventName == "DEVICE_POWER_GET"){
            qDebug() << "dev power get";
            emit powerInfoRequest();
        }
    }
}
