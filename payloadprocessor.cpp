#include "payloadprocessor.h"
#include <QtEndian>
//#include <qDebug>

PayloadProcessor::PayloadProcessor(const QString &id, QObject *parent)
    : QObject(parent), m_id(id)
{
}

PayloadProcessor::~PayloadProcessor(){

}

//---------------------------------------------------------------------------------------
void PayloadProcessor::initPort(const QString &portName)
{
    if (m_serial && m_serial->isOpen()) {
        //emit debugMessage("Port already open.");
        return;
    }

    if (m_serial) {
        m_serial->deleteLater();
        m_serial = nullptr;
    }

    m_serial = new QSerialPort(portName, this);
    m_serial->setPortName(portName);
    m_serial->setBaudRate(QSerialPort::Baud115200);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setStopBits(QSerialPort::OneStop);

    if (!m_serial->open(QIODevice::ReadWrite)) {
        emit serialError(m_serial->errorString());
        m_serial->deleteLater();
        m_serial = nullptr;
        emit serialOpened(false);
        return;
    }

    //emit debugMessage("Open Port OK");

    connect(m_serial, &QSerialPort::readyRead,
            this, &PayloadProcessor::readData, Qt::DirectConnection);

    connect(m_serial, &QSerialPort::errorOccurred,
            this, [this](QSerialPort::SerialPortError err){
        if (err != QSerialPort::NoError && m_serial) {
            emit serialError(m_serial->errorString());
        }
    });

    emit serialOpened(true);

    /*
    void setHeight(int height);
    void setFallDuraion(int duration);
    void setAngle(int angle);

    void setPresence(bool checked);
    void setStandStill(bool checked);
    void setFallDetector(bool checked);
    void setTraceTracking(bool checked);
*/

    //Prepare radar
    //QTimer::singleShot(2000, this, [this](){ //qDebug() << "Set FallDuraion";      setFallDuraion(3);     });
    //QTimer::singleShot(3000, this, [this](){ //qDebug() << "Set Angle";            setAngle(30);          });
    //QTimer::singleShot(4000, this, [this](){ //qDebug() << "Set Presence";         setPresence(true);     });
    //QTimer::singleShot(5000, this, [this](){ //qDebug() << "Set StandStill";       setStandStill(true);   });
    //QTimer::singleShot(6000, this, [this](){ //qDebug() << "Set FallDetector";     setFallDetector(true); });
    //QTimer::singleShot(7000, this, [this](){ //qDebug() << "Set TraceTracking";    setTraceTracking(true);});
    //QTimer::singleShot(1000, this, [this](){ //qDebug() << "Set height";           setHeight(96);         });

}

//---------------------------------------------------------------------------------------
void PayloadProcessor::closePort()
{
    if (m_serial) {
        if (m_serial->isOpen())
            m_serial->close();
        m_serial->deleteLater();
        m_serial = nullptr;
    }
}

//---------------------------------------------------------------------------------------
void PayloadProcessor::readData()
{
    if (!m_serial) return;

    const QByteArray data = m_serial->readAll();
    if (data.isEmpty()) return;

    m_buffer.append(data);

    while (true) {
        int startIdx = m_buffer.indexOf(QByteArray::fromHex("5359")); // "53 59"
        if (startIdx < 0) break;

        int endIdx = m_buffer.indexOf(QByteArray::fromHex("5443"), startIdx + 2); // "54 43"
        if (endIdx < 0) break;

        int frameLen = endIdx + 2 - startIdx;
        if (frameLen <= 0 || endIdx + 2 > m_buffer.size())
            break;

        QByteArray frame = m_buffer.mid(startIdx, frameLen);
        m_buffer.remove(0, endIdx + 2);

        if (frame.size() < 6)
            continue;

        quint8 cs = calcChecksum(frame.mid(0, frame.size() - 3));
        quint8 recvCs = static_cast<quint8>(frame.at(frame.size() - 3));

        if (cs != recvCs) {
            //emit debugMessage("[ERR] checksum mismatch");
            continue;
        }

        QByteArray payload = frame.mid(2, frame.size() - 5);
        enqueuePayload(payload);
    }
}

//---------------------------------------------------------------------------------------
void PayloadProcessor::enqueuePayload(const QByteArray &payload)
{
    m_queue.enqueue(payload);
    processQueue();
}

//---------------------------------------------------------------------------------------
void PayloadProcessor::prepareRadar(const QString portName)
{

}

//---------------------------------------------------------------------------------------
void PayloadProcessor::processQueue()
{
    while (!m_queue.isEmpty()) {
        QByteArray payload = m_queue.dequeue();
        if (payload.isEmpty())
            continue;

        quint8 cmd = static_cast<quint8>(payload[0]);
        quint8 sub = (payload.size() > 1) ? static_cast<quint8>(payload[1]) : 0;

        switch (cmd)
        {
        // ======================================================
        // 0x01 — SYSTEM
        // ======================================================
        case 0x01:
            switch (sub) {
            case 0x01:
                //emit debugMessage("System: Heartbeat (reporting)");
                break;
            case 0x02:
                //emit debugMessage("System: Module Reset (distribution/reporting)");
                break;
            default:
                //emit debugMessage("System: Unknown subcmd " + QString::number(sub,16));
                break;
            }
            break;

        // ======================================================
        // 0x02 — PRODUCT INFO
        // ======================================================
        case 0x02:
            switch (sub) {
            case 0xA1:
                if (payload.size() > 10) {
                    QByteArray model = payload.mid(payload.length()-7, 7);
                    emit uiUpdate(m_id, "productModel", QString::fromUtf8(model));
                }
                break;
            case 0xA2:
                if (payload.size() > 6) {
                    quint16 len = qFromLittleEndian<quint16>(
                        reinterpret_cast<const uchar*>(payload.constData() + 2));
                    QByteArray pid = payload.mid(4, len);
                    emit uiUpdate(m_id, "productID", QString::fromUtf8(pid));
                }
                break;
            case 0xA3:
                if (payload.size() >= 8) {
                    QByteArray hwmodel = payload.mid(payload.length()-4, 4);
                    emit uiUpdate(m_id,"hwModel", QString::fromUtf8(hwmodel));
                }
                break;
            case 0xA4:
                if (payload.size() > 6) {
                    QByteArray ver = payload.mid(payload.size() - 16, 16);
                    emit uiUpdate(m_id,"firmwareVersion", QString::fromUtf8(ver));
                }
                break;
            default:
                //emit debugMessage("Product Info: Unknown subcmd " + QString::number(sub,16));
                break;
            }
            break;

        // ======================================================
        // 0x05 — WORKING STATUS / PARAMETER
        // ======================================================
        case 0x05:
            switch (sub) {
            case 0x01:
                //emit debugMessage(m_id + " Working Status: Init complete");
                break;

            case 0x02:
                //emit debugMessage(m_id + " Working Status: Radar fault");
                if (payload.size() > 6){
                    //emit debugMessage(m_id + " Fault code: " +  QString::number(static_cast<quint8>(payload[6])));
                }
                break;

            case 0x81:
                if (payload.size() >= 5){
                    quint8 status = static_cast<quint8>(payload[4]);
                    emit uiUpdate(m_id, "initStatus", status == 1 ? "Inited" : "Uninit");
                }
                break;

            default:
                //emit debugMessage(m_id + " Working Status/Param: Unknown subcmd " +QString::number(sub, 16));
                break;
            }
            break;


        // ======================================================
        // 0x06 — INSTALLATION INFO
        // ======================================================
        case 0x06:
            switch (sub) {

            case 0x81: {   // get angle
                if (payload.size() >= 10) {
                    auto read16BigEndian = [&](int offset) {
                        return (static_cast<uchar>(payload[offset]) << 8) |
                               static_cast<uchar>(payload[offset + 1]);
                    };
                    double x_deg = read16BigEndian(4);
                    double y_deg = read16BigEndian(6);
                    double z_deg = read16BigEndian(8);

                    emit uiUpdate(m_id, "angleX", QString::number(x_deg));
                    emit uiUpdate(m_id, "angleY", QString::number(y_deg));
                    emit uiUpdate(m_id, "angleZ", QString::number(z_deg));

                    //emit debugMessage(m_id + " 06 81 get angle = " + payload.toHex(' ') +
                                    //  " xyz " + QString::number(x_deg) + "-" +
                                    //  QString::number(y_deg) + "-" +
                                    //  QString::number(z_deg));
                }
                break;
            }

            case 0x82: {   // get height
                if (payload.size() >= 6) {
                    uchar low = payload[4];
                    uchar high = payload[5];
                    qint16 height = (low << 8) | high;

                    emit uiUpdate(m_id, "height", QString::number(height));
                    //emit debugMessage(m_id + " 06 82 get height = " + payload.toHex(' ') +
                               //       " height " + QString::number(height));
                }
                break;
            }

            case 0x01: {   // set angle reply
                if (payload.size() >= 10) {
                    auto read16BigEndian = [&](int offset) {
                        return (static_cast<uchar>(payload[offset]) << 8) |
                               static_cast<uchar>(payload[offset + 1]);
                    };
                    double x_deg = read16BigEndian(4);
                    double y_deg = read16BigEndian(6);
                    double z_deg = read16BigEndian(8);

                    emit uiUpdate(m_id, "angleX", QString::number(x_deg));
                    emit uiUpdate(m_id, "angleY", QString::number(y_deg));
                    emit uiUpdate(m_id, "angleZ", QString::number(z_deg));

                    //emit debugMessage(m_id + " 06 01 set angle = " + payload.toHex(' ') +
                     //                 " xyz " + QString::number(x_deg) + "-" +
                     //                 QString::number(y_deg) + "-" +
                     //                 QString::number(z_deg));
                }
                break;
            }

            case 0x04: {   // heartbeat
                if (payload.size() >= 8) {
                    //emit debugMessage(m_id + " 06 04 Heart Beat " + payload.toHex(' '));
                }
                break;
            }

            case 0x02: {   // set height reply
                if (payload.size() >= 6) {
                    uchar low = payload[4];
                    uchar high = payload[5];
                    qint16 height = (low << 8) | high;

                    emit uiUpdate(m_id, "height", QString::number(height));
                    //emit debugMessage(m_id + " 06 02 set height reply = " +
                    //                  payload.toHex(' ') +
                    //                  " height " + QString::number(height));
                }
                break;
            }

            default:
                //emit debugMessage(m_id + " Installation: Unknown subcmd payload " +
                //                  payload.toHex(' '));
                break;
            }
            break;


        // ======================================================
        // 0x07 — RADAR RANGE
        // ======================================================
        case 0x07:
            if (sub == 0x09 && payload.size() >= 14) {
                auto read16 = [&](int offset) {
                    return qFromLittleEndian<qint16>(reinterpret_cast<const uchar*>(payload.constData() + offset));
                };
                //qDebug() << "Radar Range: X+:" << read16(6) << " X-:" << read16(8) << " Y+:" << read16(10) << " Y-:" << read16(12);
            } else {
                //qDebug() << "Radar Range: Unknown subcmd" << QString::number(sub,16);
            }
            break;

        // ======================================================
        // 0x80 — PRESENCE
        // ======================================================
        case 0x80:
            switch (sub) {

            case 0x00: {   // Set Presence Activated
                quint8 val = static_cast<quint8>(payload[4]);
                emit uiUpdate(m_id, "presence", val == 1 ? "ON" : "OFF");
                //emit debugMessage(m_id + " 80 00 Set Presence Activated = " + payload.toHex(' '));
                break;
            }

            case 0x01: {   // Presence Info
                quint8 val = static_cast<quint8>(payload[4]);
                emit uiUpdate(m_id, "motionStyle", val ? "presence" : "no_presence");
                //emit debugMessage(m_id + " 80 01 Presence Info = " + QString::number(static_cast<quint8>(payload[6])));
                break;
            }

            case 0x02: {   // Motion Info
                quint8 val = static_cast<quint8>(payload[4]);
                emit uiUpdate(m_id, "motionStyle", val ? "motion_high" : "motion_low");
                //emit debugMessage(m_id + " 80 02 Motion Info = " + QString::number(static_cast<quint8>(payload[6])));
                break;
            }

            case 0x03: {   // Motion value
                if (payload.size() >= 5) {
                    quint8 param = static_cast<quint8>(payload[4]);
                    emit uiUpdate(m_id, "motionValue", QString::number(param));
                    emit motionUpdate(m_id, QString::number(param));   // for drawRealTimeetsgram
                    //emit debugMessage(m_id + " 80 03 Motion Info = " + QString::number(param));
                }
                break;
            }

            default:
                //emit debugMessage(m_id + " 80 Presence: Unknown subcmd " + QString::number(sub,16));
                break;
            }
            break;


        // ======================================================
        // 0x82 — RADAR FRAME DATA
        // ======================================================
        case 0x82:
            switch (sub) {

            case 0x00: {  // command reply set trace tracking
                quint8 val = static_cast<quint8>(payload[4]);
                emit uiUpdate(m_id, "traceTracking", val == 1 ? "ON" : "OFF");
                //emit debugMessage(m_id + " 82 00 Reply of Trace Cmd = " + payload.toHex(' '));
                break;
            }

            case 0x01: {  // trace number reply
                quint8 val = static_cast<quint8>(payload[4]);
                emit uiUpdate(m_id, "traceNumber", QString::number(val));
                //emit debugMessage(m_id + " 82 01 Trace Numbers of = " + payload.toHex(' '));
                break;
            }

            case 0x02:{
                 if(!fpsTimer.isValid())
                     fpsTimer.start();

                 frameCount++;

                 if(fpsTimer.elapsed() >= 1000){
                     //qDebug() << "Tracking FPS =" << frameCount;
                     frameCount = 0;
                     fpsTimer.restart();
                 }

                 //qDebug() << "82 02 Trace Tracking Info =" << payload.toHex(' ');

                 auto read16u = [&](int offset) -> qint16 {
                     quint16 raw = (quint16(payload[offset]) << 8) |
                                   quint8(payload[offset + 1]);
                     return qint16(raw);
                 };

                 auto decodeCoord = [&](int offset) -> qint16 {
                     quint16 raw = read16u(offset);

                     bool negative = (raw & 0x8000);
                     qint16 value  = raw & 0x7FFF;

                     return negative ? -value : value;
                 };

                 //uint8_t  type_code = payload[0];
                 //uint8_t  subtype   = payload[1];
                 uint16_t len       = read16u(2);

                 int offset = 4;
                 const int targetSize = 11;

                 int targetCount = len / targetSize;

                 //qDebug() << "Len =" << len<< "Target Count =" << targetCount;

                 //---------------------------------
                 // Parse semua target dalam frame
                 //---------------------------------

                 for (int i = 0; i < targetCount; i++){
                     if ((offset + targetSize) > payload.size()){
                         //qDebug() << "Payload too short";
                         break;
                     }

                     uint8_t trackId = payload[offset + 0];

                     qint16 x = decodeCoord(offset + 3);
                     qint16 y = decodeCoord(offset + 5);

                     qint16 h = read16u(offset + 7);

                     qint16 vel = decodeCoord(offset + 9);

                     //---------------------------------
                     // Sanity check
                     //---------------------------------

                     if ((qAbs(x) > 5000) || (qAbs(y) > 5000)){
                         //qDebug() << "Invalid coordinate received" << x << y;

                         offset += targetSize;
                         continue;
                     }

                     //---------------------------------
                     // Cari target berdasarkan Track ID
                     //---------------------------------

                     TargetInfo *t = nullptr;

                     for(int k=0; k<TARGET_COUNT_SIZE; k++){
                         if(targets[k].valid &&
                            targets[k].trackId == trackId){
                             t = &targets[k];
                             break;
                         }
                     }

                     //---------------------------------
                     // Target baru
                     //---------------------------------

                     if(t == nullptr){
                         for(int k=0; k<TARGET_COUNT_SIZE; k++){
                             if(!targets[k].valid){
                                 t = &targets[k];

                                 t->valid = true;
                                 t->trackId = trackId;

                                 t->fallScore = 0;
                                 t->state = StateUnknown;

                                 t->historyCount = 0;

                                 t->lowHeightActive = false;
                                 t->fallCandidateActive = false;

                                 t->lowHeightTimer.invalidate();
                                 t->fallTimer.invalidate();

                                 t->hiddenStableActive = false;
                                 t->hiddenCandidateActive = false;
                                 t->hiddenCandidateTimer.invalidate();
                                 t->hiddenStableTimer.invalidate();

                                 memset(t->x,        0, sizeof(t->x));
                                 memset(t->y,        0, sizeof(t->y));
                                 memset(t->height,   0, sizeof(t->height));
                                 memset(t->velocity, 0, sizeof(t->velocity));
                                 memset(t->motion,   0, sizeof(t->motion));

                                 t->lastSeenMs = QDateTime::currentMSecsSinceEpoch();

                                 //qDebug() << "New Target:" << trackId;

                                 break;
                             }
                         }
                     }

                     //---------------------------------
                     // Tidak ada slot kosong
                     //---------------------------------

                     if(t == nullptr){
                         //qDebug() << "No free target slot";

                         offset += targetSize;
                         continue;
                     }

                     //---------------------------------
                     // Hitung movement
                     //---------------------------------

                     qint32 movement = 0;

                     if(t->historyCount > 0){
                         qint16 prevX = t->x[HISTORY_SIZE - 1];
                         qint16 prevY = t->y[HISTORY_SIZE - 1];

                         double dx = double(x) - double(prevX);
                         double dy = double(y) - double(prevY);

                         movement = qRound(std::sqrt(dx*dx + dy*dy));
                     }

                     //---------------------------------
                     // Update history
                     //---------------------------------
                     shiftHistory(t->x, x);
                     shiftHistory(t->y, y);
                     shiftHistory(t->height, h);
                     shiftHistory(t->velocity, vel);
                     shiftHistory(t->motion, movement);

                     if(t->historyCount < HISTORY_SIZE)
                         t->historyCount++;

                     t->lastSeenMs = QDateTime::currentMSecsSinceEpoch();

                     if(t->historyCount >= 5){
                         //---------------------------------
                         // Fall Detection
                         //---------------------------------

                         decisionFall(*t);
                     }

                     //---------------------------------
                     // Debug
                     //---------------------------------
                     //qDebug()<< "ID:" << t->trackId<< "X:" << x << "Y:" << y<< "M:" << movement<< " H:" << h<< "Vel:"   << vel << "Move:"  << movement<< "Score:" << t->fallScore<< "State:" << t->state;

                     //---------------------------------
                     // UI
                     //---------------------------------

                     //updateRadarPoint();
                     offset += targetSize;
                 }

                 //---------------------------------
                 // Bersihkan target timeout
                 //---------------------------------

                 qint64 now = QDateTime::currentMSecsSinceEpoch();
                 TargetInfo *activeTarget = nullptr;
                 int maxActivity = -1;
                 const qint64 TARGET_TIMEOUT_MS = 6000;

                 for(int i=0; i<TARGET_COUNT_SIZE; i++) {
                     if(!targets[i].valid)
                         continue;

                     if(now - targets[i].lastSeenMs > TARGET_TIMEOUT_MS){
                         //---------------------------------
                         // Hapus target
                         //---------------------------------

                         //qDebug()<< "Target timeout:"<< targets[i].trackId;

                         resetFallState(targets[i]);
                         targets[i].valid = false;

                         continue;
                     }

                     int activity = 0;
                     //qDebug() << "ID:" << targets[i].trackId << "Activity:" << activity;

                     for(int j=0; j<HISTORY_SIZE; j++){
                         activity += targets[i].motion[j];
                         activity += qAbs(targets[i].velocity[j]) * 5;
                     }

                     if(activity > maxActivity){
                         maxActivity = activity;
                         activeTarget = &targets[i];
                     }
                 }

                 bool anyFallen = false;
                 bool anyFalling = false;

                 for(int i = 0; i < TARGET_COUNT_SIZE; i++){
                     if(!targets[i].valid)
                         continue;

                     if(targets[i].state == StateLying)
                         anyFallen = true;

                     if(targets[i].state == StateFalling)
                         anyFalling = true;
                 }

                 break;
             }

            case 0x80: {
                //emit debugMessage(m_id + " 82 80 Trace Tracking Info = " + payload.toHex(' '));
                break;
            }

            case 0x81: {
                //emit debugMessage(m_id + " 82 81 Trace Numbers = " + payload.toHex(' '));
                break;
            }

            case 0x82: {
                //emit debugMessage(m_id + " 82 82 Trace Info query = " + payload.toHex(' '));
                break;
            }

            default:
                //emit debugMessage(m_id + " 82 xx Trace Info query = " + payload.toHex(' '));
                break;
            }
            break;


        // ======================================================
        // 0x83 — FALL DETECTION
        // ======================================================
        case 0x83:
            switch (sub) {
            case 0x00: { // Fall detection enable/disable
                quint8 val = static_cast<quint8>(payload[4]);
                //radarFrame.traceTracking = (val == 1);
                emit uiUpdate(m_id, "fallDetection", val ? "ON" : "OFF");
                break;
            }

            case 0x01: { // Fall state
                quint8 val = static_cast<quint8>(payload[4]);
                //radarFrame.fallState = (val == 1);

                emit uiUpdate(m_id, "fallStateText", val ? "FALLEN" : "NOT FALLEN");
                emit uiUpdate(m_id, "fallStateColor", val ? "red" : "green");

                //qDebug() << "83 01 Fall =" << payload;

                if (val == 1) {
                    emit fallDetected(m_id);  // UI thread will handle sound & socket
                }
                break;
            }

            case 0x05: { // Standstill state
                quint8 val = static_cast<quint8>(payload[4]);
                //radarFrame.standStillState = (val == 1);

                emit uiUpdate(m_id, "standStillText", val ? "EXIST" : "NO STAND STILL");
                emit uiUpdate(m_id, "standStillColor", val ? "blue" : "grey");

                //qDebug() << "83 05 standstill =" << payload;
                break;
            }

            case 0x0B: { // Standstill setting
                quint8 val = static_cast<quint8>(payload[4]);
                //radarFrame.standStillState = (val == 1);

                emit uiUpdate(m_id, "getStandStill", val ? "ON" : "OFF");
                //qDebug() << "83 0B standstill setting =" << payload;
                break;
            }

            case 0x0C: { // Fall duration (short)
                if (payload.size() >= 8) {
                    quint8 duration = static_cast<quint8>(payload[7]);
                    //qDebug() << "0C Fall: Duration Setting =" << duration;
                }
                break;
            }

            case 0x8C: { // Fall duration (32-bit)
                if (payload.size() >= 8) {
                    QByteArray durationBytes = payload.mid(4, 4);
                    quint32 duration =
                        (static_cast<quint8>(durationBytes[0]) << 24) |
                        (static_cast<quint8>(durationBytes[1]) << 16) |
                        (static_cast<quint8>(durationBytes[2]) << 8)  |
                        (static_cast<quint8>(durationBytes[3]));

                    emit uiUpdate(m_id, "fallDuration", QString::number(duration));
                    //qDebug() << "8C Fall: Duration Setting =" << duration;
                }
                break;
            }

            case 0x16: { // Fall position
                if (payload.size() >= 8) {
                    quint16 posX = (static_cast<quint8>(payload[4]) << 8) |
                                   static_cast<quint8>(payload[5]);

                    quint16 posY = (static_cast<quint8>(payload[6]) << 8) |
                                   static_cast<quint8>(payload[7]);

                    //radarFrame.fallPosX = posX;
                    //radarFrame.fallPosY = posY;

                    emit uiUpdate(m_id, "fallPosX", QString::number(posX));
                    emit uiUpdate(m_id, "fallPosY", QString::number(posY));
                    emit radarPoint(m_id, posX, posY);

                    //qDebug() << "Fall Position -> X:" << posX << "Y:" << posY;
                }
                break;
            }

            case 0x17: { // Fall cancel position
                if (payload.size() >= 8) {
                    quint16 posX = (static_cast<quint8>(payload[4]) << 8) |
                                   static_cast<quint8>(payload[5]);

                    quint16 posY = (static_cast<quint8>(payload[6]) << 8) |
                                   static_cast<quint8>(payload[7]);

                    //radarFrame.fallPosX = posX;
                    //radarFrame.fallPosY = posY;

                    emit uiUpdate(m_id, "fallPosX", QString::number(posX));
                    emit uiUpdate(m_id, "fallPosY", QString::number(posY));
                    emit radarPoint(m_id, posX, posY);

                    emit uiUpdate(m_id, "fallStateText", "FALLEN");
                    emit uiUpdate(m_id, "fallStateColor", "grey");

                    //qDebug() << "Fall Cancel Position -> X:" << posX << "Y:" << posY;
                    //qDebug() << "83 17 Fall Cancel=" << payload;

                    //emit fallDetected(m_id); // still trigger alert if needed
                    emit fallCancel(m_id);
                }
                break;
            }

            default:
                //qDebug() << "Fall: Unknown subcmd" << QString::number(sub, 16);
                break;
            }
            break;

        default:
            break;
        }
    }
}

//---------------------------------------------------------------------------------------
quint8 PayloadProcessor::calcChecksum(const QByteArray &data)
{
    quint8 cs = 0;
    for (auto b : data)
        cs += static_cast<quint8>(b);
    return cs;
}

//---------------------------------------------------------------------------------------
void PayloadProcessor::sendCmdRadar(QByteArray cmd)
{

}

//---------------------------------------------------------------------------------------
QByteArray PayloadProcessor::makeFrame(const QByteArray &body)
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
QString PayloadProcessor::toHexSpace(const QByteArray &data)
{
    QStringList parts;
    for (auto b : data) parts << QString("%1").arg((unsigned char)b, 2, 16, QChar('0')).toUpper();
    return parts.join(' ');
}

//---------------------------------------------------------------------------------------
void PayloadProcessor::setHeight(int height)
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);

    //int height = ui->leSetHeight2->text().toInt();
    QByteArray cmd = CMD_SET_HEIGHT;

    quint8 hb = static_cast<quint8>((height >> 8) & 0xFF);
    quint8 lb = static_cast<quint8>(height & 0xFF);

    cmd.append(static_cast<char>(hb));
    cmd.append(static_cast<char>(lb));

    QByteArray frame = makeFrame(cmd);
    //qDebug() << "Sending frame cmd Height:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

//---------------------------------------------------------------------------------------
void PayloadProcessor::setFallDuraion(int duration)
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);

    //uint32_t duration = ui->leSetFallDuration->text().toUInt();

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

    //qDebug() << "Sending frame cmd Set Fall Duration:" << toHexSpace(frame);

    m_serial->write(frame);
    m_serial->flush();
}

//---------------------------------------------------------------------------------------
void PayloadProcessor::setAngle(int angle)
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);

    uint16_t angleX = 0; //ui->leAngleX2->text().toUInt();
    uint16_t angleY = 0; //ui->leAngleY2->text().toUInt();
    uint16_t angleZ = angle; //ui->leAngleZ2->text().toUInt();

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

    //qDebug() << "Sending frame SetAngle:" << toHexSpace(frame);

    m_serial->write(frame);
    m_serial->flush();
}

//---------------------------------------------------------------------------------------
void PayloadProcessor::setPresence(bool checked)
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = checked ? makeFrame(CMD_SET_PRESENCE_ON) : makeFrame(CMD_SET_PRESENCE_OFF);
    //qDebug() << "Sending frame set Presence:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

//---------------------------------------------------------------------------------------
void PayloadProcessor::setStandStill(bool checked)
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = checked ? makeFrame(CMD_SET_STAND_STILLON) : makeFrame(CMD_SET_STAND_STILLOFF);
    //qDebug() << "Sending frame StandStill:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

//---------------------------------------------------------------------------------------
void PayloadProcessor::setFallDetector(bool checked)
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = checked ? makeFrame(CMD_SET_FALL_DETECTION_ON) : makeFrame(CMD_SET_FALL_DETECTION_OFF);
    //qDebug() << "Sending frame Set Fall Duration:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}

//---------------------------------------------------------------------------------------
void PayloadProcessor::setTraceTracking(bool checked)
{
    if (!m_serial) return;
    m_serial->clear(QSerialPort::Input);
    QByteArray frame = checked ? makeFrame(CMD_SET_TRACE_TRACKING_ON) : makeFrame(CMD_SET_TRACE_TRACKING_OFF);
    //qDebug() << "Sending frame Trace Tracking:" << toHexSpace(frame);
    m_serial->write(frame);
    m_serial->flush();
}
//---------------------------------------------------------------------------------------
bool PayloadProcessor::isFallCandidate(const TargetInfo &t)
{
    if(t.historyCount < HISTORY_SIZE){
        return false;
    }

    int heightDrop = 0;

    bool rapidHeightDrop = isHeightTrendDecreasing(t, &heightDrop);

    qint32 velocityTotal = 0;
    qint32 totalMovement = 0;

    for(int i = 0; i < HISTORY_SIZE; i++){
        velocityTotal += qAbs(t.velocity[i]);
        totalMovement += t.motion[i];
    }

    bool fastMotion = (velocityTotal > 5);
    bool movedEnough = (totalMovement > 2);

    return rapidHeightDrop &&
           (fastMotion || movedEnough);
}

//---------------------------------------------------------------------------------------
bool PayloadProcessor::isHeightTrendDecreasing(const TargetInfo &t, int *dropOut)
{
    constexpr int N = HISTORY_SIZE;

    constexpr int HEIGHT_DROP_THRESHOLD = 50;
    constexpr int SEGMENT_WINDOW = 8;
    constexpr int SEGMENT_TOLERANCE = 10;

    constexpr double MIN_NEGATIVE_SLOPE = -0.7;
    constexpr int MAX_UP_RATIO_PERCENT = 35;

    if(t.historyCount < HISTORY_SIZE)
        return false;

    qint64 sumX = 0;
    qint64 sumY = 0;
    qint64 sumXY = 0;
    qint64 sumX2 = 0;

    qint32 downSum = 0;
    qint32 upSum = 0;

    for(int i = 0; i < N; i++){
        const int h = t.height[i];

        if(h <= 0)
            return false;

        sumX  += i;
        sumY  += h;
        sumXY += i * h;
        sumX2 += i * i;

        if(i > 0){
            const int diff = t.height[i] - t.height[i - 1];

            if(diff < 0)
                downSum += -diff;
            else if(diff > 0)
                upSum += diff;
        }
    }

    auto avgRange = [&](int start, int count) -> int {
        qint32 sum = 0;

        for(int i = start; i < start + count; i++){
            if(t.height[i] <= 0)
                return -1;

            sum += t.height[i];
        }

        return sum / count;
    };

    const int earlyAvg = avgRange(0, SEGMENT_WINDOW);
    const int midAvg   = avgRange((N - SEGMENT_WINDOW) / 2, SEGMENT_WINDOW);
    const int lateAvg  = avgRange(N - SEGMENT_WINDOW, SEGMENT_WINDOW);

    if(earlyAvg <= 0 || midAvg <= 0 || lateAvg <= 0)
        return false;

    const int heightDrop = earlyAvg - lateAvg;

    if(dropOut)
        *dropOut = heightDrop;

    const double denominator =
        static_cast<double>(N) * sumX2 -
        static_cast<double>(sumX) * sumX;

    if(denominator == 0)
        return false;

    const double slope =
        (static_cast<double>(N) * sumXY -
         static_cast<double>(sumX) * sumY) / denominator;

    const bool enoughDrop =
        heightDrop > HEIGHT_DROP_THRESHOLD;

    const bool segmentTrendDown =
        (earlyAvg + SEGMENT_TOLERANCE >= midAvg) &&
        (midAvg   + SEGMENT_TOLERANCE >= lateAvg);

    const bool dominantDownMovement =
        (downSum > 0) &&
        (upSum * 100 <= downSum * MAX_UP_RATIO_PERCENT);

    const bool negativeSlope =
        slope < MIN_NEGATIVE_SLOPE;

    return enoughDrop &&
           segmentTrendDown &&
           dominantDownMovement &&
           negativeSlope;
}

//---------------------------------------------------------------------------------------
void PayloadProcessor::decisionFall(TargetInfo &t)
{
    if(!t.valid)
        return;

    //---------------------------------
    // Ambil data terbaru
    //---------------------------------

    const int idx = HISTORY_SIZE - 1;

    qint16 hNew = t.height[idx];
    //qint16 velLast = qAbs(t.velocity[idx]);
    //qint32 moveLast = qAbs(t.motion[idx]);

    bool normalCandidate = isFallCandidate(t);
    bool lowHeight = (hNew < 30);

    //---------------------------------
    // kandidat jatuh
    //---------------------------------

    if(normalCandidate){
        //qDebug() << "Nearly down....";
        if(!t.fallCandidateActive){
            t.fallCandidateActive = true;
            t.fallTimer.restart();
            //qDebug() << "!t.fallCandidateActive";
        }
        t.fallScore += 20;
        if(t.fallScore > 100){
            t.fallScore = 100;
            //qDebug() << "fallScore 100";
        }
        t.state = StateFalling;
    }else{
        if(t.fallScore > 0){
            //qDebug() << "fallScore -- " << t.fallScore;
            t.fallScore -= 2;
            if(t.fallScore < 0)
               t.fallScore = 0;
        }
        //qDebug() << "No Candidate...";
    }

    //---------------------------------
    // posisi rendah
    //---------------------------------

    if(lowHeight){
        //qDebug() << "lowHeight detected";
        if(!t.lowHeightActive){
            t.lowHeightActive = true;
            t.lowHeightTimer.restart();
            //qDebug() << "lowHeight true";
        }
    }else{
        t.lowHeightActive = false;
        t.lowHeightTimer.invalidate();
        //qDebug() << "lowHeight false";
    }

    //---------------------------------
    // reset kandidat jika score habis
    //---------------------------------

    if(!normalCandidate &&
       !lowHeight &&
       t.fallScore == 0){
       t.fallCandidateActive = false;
       t.fallTimer.invalidate();

        //qDebug() << "reset candidate";
    }

    //---------------------------------
    // konfirmasi jatuh normal
    //---------------------------------

    if(t.fallCandidateActive &&
       t.lowHeightActive &&
       t.fallTimer.isValid() &&
       t.lowHeightTimer.isValid() &&
       t.fallScore >= 40){

        qint64 fallMs = t.fallTimer.elapsed();
        qint64 lowMs = t.lowHeightTimer.elapsed();

        //qDebug() << "konfirmasi hampir jatuh "<< fallMs<< "-"<< lowMs;

        if(fallMs > 300 &&
           lowMs > 5000){
            //emit fallAlarm(t.trackId);
            emit fallDetected(QString(t.trackId));  // UI thread will handle sound & socket
            //qDebug() << "FALL CONFIRMED "<< t.trackId;
            resetFallState(t);
            t.state = StateLying;
            return;
        }
    }

    //---------------------------------
    // Hidden Fall Detection
    //---------------------------------

    /*

    bool hiddenTrigger = isLostAfterHeightDrop(t);
    bool stableHidden = (hNew < 50 &&
                         velLast == 0 &&
                         moveLast == 0);

    if(hiddenTrigger &&
        !t.hiddenCandidateActive){
        t.hiddenCandidateActive = true;
        t.hiddenCandidateTimer.restart();
        t.hiddenStableTimer.invalidate();

        //qDebug() << "Hidden candidate started"
                 //<< t.trackId
                 //<< "hNew" << hNew
                 //<< "vel" << velLast
                 //<< "motion" << moveLast;
    }

    if(t.hiddenCandidateActive){
        const qint64 HIDDEN_CONFIRM_MS = 3000;
        const qint64 HIDDEN_TIMEOUT_MS = 6000;

        //---------------------------------
        // Jika objek tetap hidden stable
        //---------------------------------

        if(stableHidden){
            if(!t.hiddenStableTimer.isValid()){
                t.hiddenStableTimer.restart();

                //qDebug() << "Hidden stable timer started" << t.trackId;
            }

            qint64 hiddenStableMs = t.hiddenStableTimer.elapsed();

            //qDebug() << "Hidden stable duration"
                     //<< hiddenStableMs
                    // << "ms";

            if(hiddenStableMs > HIDDEN_CONFIRM_MS){
                //emit fallAlarm(t.trackId);
                emit fallDetected(QString(t.trackId));  // UI thread will handle sound & socket

                //qDebug() << "FALL CONFIRMED HIDDEN"
                         //<< t.trackId
                        // << "hNew" << hNew
                        // << "vel" << velLast
                        // << "motion" << moveLast;

                resetFallState(t);
                t.state = StateLying;
                return;
            }

            //---------------------------------
            // Jika objek bergerak lagi / tidak stabil
            //---------------------------------

        }else{
            t.hiddenStableTimer.invalidate();

            qint64 hiddenCandidateMs = t.hiddenCandidateTimer.elapsed();

            if(hiddenCandidateMs > HIDDEN_TIMEOUT_MS){
                //qDebug() << "Hidden fall cancelled by timeout"   << t.trackId;

                resetFallState(t);
                return;
            }
        }
    }
    */

    //---------------------------------
    // UI for debug
    //---------------------------------

    /*
    if(t.state == StateLying){
        ui->leFallStateAlgorithm->setStyleSheet(
            "background-color: red; color: yellow;");

        ui->leFallStateAlgorithm->setText(
            "FALLEN");

    }else if(isStanding(t)){
        ui->leFallStateAlgorithm->setStyleSheet(
            "background-color: green; color: black;");

        ui->leFallStateAlgorithm->setText(
            "NOTFALL");

    }else if(t.state < StateLying){
        ui->leFallStateAlgorithm->setStyleSheet(
            "background-color: green; color: black;");

        ui->leFallStateAlgorithm->setText(
            "NOTFALLEN");
    }
    */
}



//---------------------------------------------------------------------------------------
void PayloadProcessor::shiftHistory(qint16 data[], qint16 value)
{
    for(int i=1; i<HISTORY_SIZE; i++)
        data[i-1] = data[i];

    data[HISTORY_SIZE-1] = value;
}

//---------------------------------------------------------------------------------------
void PayloadProcessor::resetFallState(TargetInfo &t)
{
    t.fallScore = 0;

    t.fallCandidateActive = false;
    t.lowHeightActive = false;

    t.fallTimer.invalidate();
    t.lowHeightTimer.invalidate();

    t.hiddenStableActive = false;
    t.hiddenStableTimer.invalidate();

    t.hiddenCandidateActive = false;
    t.hiddenCandidateTimer.invalidate();
    t.hiddenStableTimer.invalidate();

    t.fallScore = 0;

   // t.lostAfterDropActive = false;
   // t.lostAfterDropTimer.invalidate();

    //if(t.state == StateFalling)
    //    t.state = StateUnknown;

   // ui->leFallStateAlgorithm->setStyleSheet("background-color: green; color: black;");
   // ui->leFallStateAlgorithm->setText("NOTFALL");

}

//---------------------------------------------------------------------------------------
QString PayloadProcessor::historyToString(const qint16 data[])
{
    QString s;

    for(int i=0; i<HISTORY_SIZE; i++)
    {
        s += QString::number(data[i]);

        if(i < HISTORY_SIZE - 1)
            s += ",";
    }

    return s;

}

//---------------------------------------------------------------------------------------
bool PayloadProcessor::isStanding(const TargetInfo &t)
{
    int h = t.height[HISTORY_SIZE - 1];

    return (h > 70);
}

//---------------------------------------------------------------------------------------
bool PayloadProcessor::isLostAfterHeightDrop(const TargetInfo &t)
{
    if(!t.valid)
        return false;

    //---------------------------------
    // Threshold hidden fall
    //---------------------------------

    const int idx =
        HISTORY_SIZE - 1;

    const int HIDDEN_HEIGHT_MAX =
        50;     // area rendah / tertutup sofa

    const int STANDING_HEIGHT_MIN =
        70;     // indikasi sebelumnya masih berdiri / cukup tinggi

    const int MIN_HEIGHT_DROP =
        25;     // minimal penurunan tinggi yang dianggap signifikan

    const int MIN_STANDING_COUNT =
        2;      // minimal jumlah data sebelumnya yang menunjukkan objek tinggi

    //---------------------------------
    // Data terbaru
    //---------------------------------

    int hNew =
        t.height[idx];

    int velLast =
        qAbs(t.velocity[idx]);

    int motionLast =
        qAbs(t.motion[idx]);

    //---------------------------------
    // Kondisi akhir: objek rendah dan benar-benar tidak terbaca gerak
    //---------------------------------
    // Untuk kasus jatuh di belakang sofa, berdasarkan eksperimen:
    // velocity == 0 dan motion == 0 justru menjadi ciri penting.
    //---------------------------------

    bool nowHiddenStable =
        (hNew < HIDDEN_HEIGHT_MAX &&
         velLast == 0 &&
         motionLast == 0);

    if(!nowHiddenStable)
        return false;

    //---------------------------------
    // Cek riwayat sebelumnya:
    // apakah objek sebelumnya pernah cukup tinggi?
    //---------------------------------

    int previousMaxHeight =
        0;

    int standingCount =
        0;

    for(int i = 0; i < idx; i++){
        int h =
            t.height[i];

        if(h > previousMaxHeight)
            previousMaxHeight = h;

        if(h >= STANDING_HEIGHT_MIN)
            standingCount++;
    }

    bool wasStandingBefore =
        (standingCount >= MIN_STANDING_COUNT);

    //---------------------------------
    // Cek apakah terjadi penurunan tinggi yang signifikan
    //---------------------------------

    bool heightDropped =
        ((previousMaxHeight - hNew) >= MIN_HEIGHT_DROP);

    //---------------------------------
    // Hidden fall candidate
    //---------------------------------

    if(wasStandingBefore &&
        heightDropped)
    {
        //qDebug()<< "Hidden fall candidate detected"<< "trackId" << t.trackId<< "prevMaxHeight" << previousMaxHeight<< "hNew" << hNew<< "drop" << (previousMaxHeight - hNew)<< "vel" << velLast<< "motion" << motionLast;

        return true;
    }

    return false;
}
