#include "payloadprocessor.h"
#include <QtEndian>
#include <QDebug>

PayloadProcessor::PayloadProcessor(const QString &id, QObject *parent)
    : QObject(parent), m_id(id)
{
}

//---------------------------------------------------------------------------------------
void PayloadProcessor::initPort(const QString &portName)
{
    if (m_serial && m_serial->isOpen()) {
        emit debugMessage("Port already open.");
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

    emit debugMessage("Open Port OK");

    connect(m_serial, &QSerialPort::readyRead,
            this, &PayloadProcessor::readData, Qt::DirectConnection);

    connect(m_serial, &QSerialPort::errorOccurred,
            this, [this](QSerialPort::SerialPortError err){
        if (err != QSerialPort::NoError && m_serial) {
            emit serialError(m_serial->errorString());
        }
    });

    emit serialOpened(true);
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
            emit debugMessage("[ERR] checksum mismatch");
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
                emit debugMessage("System: Heartbeat (reporting)");
                break;
            case 0x02:
                emit debugMessage("System: Module Reset (distribution/reporting)");
                break;
            default:
                emit debugMessage("System: Unknown subcmd " + QString::number(sub,16));
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
                emit debugMessage("Product Info: Unknown subcmd " + QString::number(sub,16));
                break;
            }
            break;

        // ======================================================
        // 0x05 — WORKING STATUS / PARAMETER
        // ======================================================
        case 0x05:
            switch (sub) {
            case 0x01:
                emit debugMessage(m_id + " Working Status: Init complete");
                break;

            case 0x02:
                emit debugMessage(m_id + " Working Status: Radar fault");
                if (payload.size() > 6){
                    emit debugMessage(m_id + " Fault code: " +
                                      QString::number(static_cast<quint8>(payload[6])));
                }
                break;

            case 0x81:
                if (payload.size() >= 5){
                    quint8 status = static_cast<quint8>(payload[4]);
                    emit uiUpdate(m_id, "initStatus", status == 1 ? "Inited" : "Uninit");
                }
                break;

            default:
                emit debugMessage(m_id + " Working Status/Param: Unknown subcmd " +
                                  QString::number(sub, 16));
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

                    emit debugMessage(m_id + " 06 81 get angle = " + payload.toHex(' ') +
                                      " xyz " + QString::number(x_deg) + "-" +
                                      QString::number(y_deg) + "-" +
                                      QString::number(z_deg));
                }
                break;
            }

            case 0x82: {   // get height
                if (payload.size() >= 6) {
                    uchar low = payload[4];
                    uchar high = payload[5];
                    qint16 height = (low << 8) | high;

                    emit uiUpdate(m_id, "height", QString::number(height));
                    emit debugMessage(m_id + " 06 82 get height = " + payload.toHex(' ') +
                                      " height " + QString::number(height));
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

                    emit debugMessage(m_id + " 06 01 set angle = " + payload.toHex(' ') +
                                      " xyz " + QString::number(x_deg) + "-" +
                                      QString::number(y_deg) + "-" +
                                      QString::number(z_deg));
                }
                break;
            }

            case 0x04: {   // heartbeat
                if (payload.size() >= 8) {
                    emit debugMessage(m_id + " 06 04 Heart Beat " + payload.toHex(' '));
                }
                break;
            }

            case 0x02: {   // set height reply
                if (payload.size() >= 6) {
                    uchar low = payload[4];
                    uchar high = payload[5];
                    qint16 height = (low << 8) | high;

                    emit uiUpdate(m_id, "height", QString::number(height));
                    emit debugMessage(m_id + " 06 02 set height reply = " +
                                      payload.toHex(' ') +
                                      " height " + QString::number(height));
                }
                break;
            }

            default:
                emit debugMessage(m_id + " Installation: Unknown subcmd payload " +
                                  payload.toHex(' '));
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
                qDebug() << "Radar Range: X+:" << read16(6) << " X-:" << read16(8)
                         << " Y+:" << read16(10) << " Y-:" << read16(12);
            } else {
                qDebug() << "Radar Range: Unknown subcmd" << QString::number(sub,16);
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
                emit debugMessage(m_id + " 80 00 Set Presence Activated = " + payload.toHex(' '));
                break;
            }

            case 0x01: {   // Presence Info
                quint8 val = static_cast<quint8>(payload[4]);
                emit uiUpdate(m_id, "motionStyle", val ? "presence" : "no_presence");
                emit debugMessage(m_id + " 80 01 Presence Info = " + QString::number(static_cast<quint8>(payload[6])));
                break;
            }

            case 0x02: {   // Motion Info
                quint8 val = static_cast<quint8>(payload[4]);
                emit uiUpdate(m_id, "motionStyle", val ? "motion_high" : "motion_low");
                emit debugMessage(m_id + " 80 02 Motion Info = " + QString::number(static_cast<quint8>(payload[6])));
                break;
            }

            case 0x03: {   // Motion value
                if (payload.size() >= 5) {
                    quint8 param = static_cast<quint8>(payload[4]);
                    emit uiUpdate(m_id, "motionValue", QString::number(param));
                    emit motionUpdate(m_id, QString::number(param));   // for drawRealTimeetsgram
                    emit debugMessage(m_id + " 80 03 Motion Info = " + QString::number(param));
                }
                break;
            }

            default:
                emit debugMessage(m_id + " 80 Presence: Unknown subcmd " + QString::number(sub,16));
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
                emit debugMessage(m_id + " 82 00 Reply of Trace Cmd = " + payload.toHex(' '));
                break;
            }

            case 0x01: {  // trace number reply
                quint8 val = static_cast<quint8>(payload[4]);
                emit uiUpdate(m_id, "traceNumber", QString::number(val));
                emit debugMessage(m_id + " 82 01 Trace Numbers of = " + payload.toHex(' '));
                break;
            }

            case 0x02: {  // trace tracking info
                emit debugMessage(m_id + " 82 02 Trace Tracking Info = " + payload.toHex(' '));

                auto read16s = [&](int offset) -> qint16 {
                    quint16 raw = (quint16(payload[offset]) << 8) |
                                  quint8(payload[offset + 1]);
                    return qint16(raw);
                };

                uint8_t  type_code = payload[0];
                uint8_t  subtype   = payload[1];
                uint16_t len       = read16s(2);

                int offset = 4;   // first target starts at 4
                const int targetSize = 11;
                int targetCount = len / targetSize;

                for (int i = 0; i < targetCount; i++) {

                    uint8_t index        = payload[offset + 0];
                    uint8_t target_size  = payload[offset + 1];
                    uint8_t target_feat  = payload[offset + 2];

                    qint16 x_pos    = read16s(offset + 3);
                    qint16 y_pos    = read16s(offset + 5);
                    qint16 height   = read16s(offset + 7);
                    qint16 velocity = read16s(offset + 9);

                    // Filter invalid velocity
                    if (velocity <= -32000 || velocity >= 32000) {
                        velocity = 0;
                    }

                    // Filter invalid coordinates
                    if (x_pos <= -32000 || x_pos >= 32000 ||
                        y_pos <= -32000 || y_pos >= 32000)
                    {
                        emit debugMessage(m_id + " Invalid coordinate received, skipping "
                                          + QString::number(x_pos) + "-" + QString::number(y_pos));
                        offset += targetSize;
                        continue;
                    }

                    // Kirim ke UI (per port)
                    emit radarPoint(m_id,
                                    static_cast<double>(x_pos),
                                    static_cast<double>(y_pos));

                    emit velocityUpdate(m_id, QString::number(velocity));
                    emit uiUpdate(m_id, "velocity", QString::number(velocity));
                    emit uiUpdate(m_id, "fallPosX", QString::number(x_pos));
                    emit uiUpdate(m_id, "fallPosY", QString::number(y_pos));

                    emit debugMessage(m_id + " Target " + QString::number(i) +
                                      " tc:" + QString::number(targetCount) +
                                      " Index:" + QString::number(index) +
                                      " Size:" + QString::number(target_size) +
                                      " Feat:" + QString::number(target_feat) +
                                      " X:" + QString::number(x_pos) +
                                      " Y:" + QString::number(y_pos) +
                                      " Z:" + QString::number(height) +
                                      " Vel:" + QString::number(velocity));

                    offset += targetSize;
                }
                break;
            }

            case 0x80: {
                emit debugMessage(m_id + " 82 80 Trace Tracking Info = " + payload.toHex(' '));
                break;
            }

            case 0x81: {
                emit debugMessage(m_id + " 82 81 Trace Numbers = " + payload.toHex(' '));
                break;
            }

            case 0x82: {
                emit debugMessage(m_id + " 82 82 Trace Info query = " + payload.toHex(' '));
                break;
            }

            default:
                emit debugMessage(m_id + " 82 xx Trace Info query = " + payload.toHex(' '));
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

                qDebug() << "83 01 Fall =" << payload;

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

                qDebug() << "83 05 standstill =" << payload;
                break;
            }

            case 0x0B: { // Standstill setting
                quint8 val = static_cast<quint8>(payload[4]);
                //radarFrame.standStillState = (val == 1);

                emit uiUpdate(m_id, "getStandStill", val ? "ON" : "OFF");
                qDebug() << "83 0B standstill setting =" << payload;
                break;
            }

            case 0x0C: { // Fall duration (short)
                if (payload.size() >= 8) {
                    quint8 duration = static_cast<quint8>(payload[7]);
                    qDebug() << "0C Fall: Duration Setting =" << duration;
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
                    qDebug() << "8C Fall: Duration Setting =" << duration;
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

                    qDebug() << "Fall Position -> X:" << posX << "Y:" << posY;
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

                    qDebug() << "Fall Cancel Position -> X:" << posX << "Y:" << posY;
                    qDebug() << "83 17 Fall Cancel=" << payload;

                    emit fallDetected(m_id); // still trigger alert if needed
                }
                break;
            }

            default:
                qDebug() << "Fall: Unknown subcmd" << QString::number(sub, 16);
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
