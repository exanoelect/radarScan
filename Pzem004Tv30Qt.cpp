#include "Pzem004Tv30Qt.h"

#include <QIODevice>
#include <QMetaType>

Pzem004Tv30Qt::Pzem004Tv30Qt(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<Pzem004Tv30Data>("Pzem004Tv30Data");

    m_responseTimer.setSingleShot(true);

    connect(&m_responseTimer,
            &QTimer::timeout,
            this,
            &Pzem004Tv30Qt::onResponseTimeout);
}

bool Pzem004Tv30Qt::open(const QString &portName, quint8 address, int timeoutMs)
{
    if ((address < 0x01) || (address > 0xF8)) {
        setLastError("Invalid PZEM address. Use 0x01..0xF8.");
        return false;
    }

    if (m_serial.isOpen())
        m_serial.close();

    m_address = address;
    m_timeoutMs = timeoutMs;

    m_rxBuffer.clear();
    m_pending = PendingNone;
    m_expectedLength = 0;

    m_serial.setPortName(portName);
    m_serial.setBaudRate(QSerialPort::Baud9600);
    m_serial.setDataBits(QSerialPort::Data8);
    m_serial.setParity(QSerialPort::NoParity);
    m_serial.setStopBits(QSerialPort::OneStop);
    m_serial.setFlowControl(QSerialPort::NoFlowControl);

    if (!m_serial.open(QIODevice::ReadWrite)) {
        setLastError("Failed to open serial port " + portName + ": " + m_serial.errorString());
        return false;
    }

    m_serial.clear(QSerialPort::AllDirections);

    connect(&m_serial,
            &QSerialPort::readyRead,
            this,
            &Pzem004Tv30Qt::onReadyRead,
            Qt::UniqueConnection);

    connect(&m_serial,
            &QSerialPort::errorOccurred,
            this,
            &Pzem004Tv30Qt::onSerialError,
            Qt::UniqueConnection);

    m_lastError.clear();
    return true;
}

void Pzem004Tv30Qt::close()
{
    m_responseTimer.stop();

    m_rxBuffer.clear();
    m_pending = PendingNone;
    m_expectedLength = 0;

    if (m_serial.isOpen())
        m_serial.close();
}

bool Pzem004Tv30Qt::isOpen() const
{
    return m_serial.isOpen();
}

QString Pzem004Tv30Qt::lastError() const
{
    return m_lastError;
}

bool Pzem004Tv30Qt::requestReadAll(QString *error)
{
    if (!m_serial.isOpen()) {
        setLastError("Serial port is not open.", error);
        emit errorOccurred(m_lastError);
        return false;
    }

    if (m_pending != PendingNone) {
        setLastError("PZEM request still pending. Wait for response first.", error);
        emit errorOccurred(m_lastError);
        return false;
    }

    QByteArray request = buildFrame(
        CmdReadInputRegister,
        RegVoltage,
        0x000A,      // 10 register
        m_address
    );

    m_rxBuffer.clear();
    m_pending = PendingReadAll;

    // Response:
    // address + function + byteCount + 20 data bytes + 2 CRC = 25 bytes
    m_expectedLength = 25;

    const qint64 written = m_serial.write(request);

    if (written != request.size()) {
        failPending("Failed to write full PZEM request.");
        if (error)
            *error = m_lastError;
        return false;
    }

    m_serial.flush();
    m_responseTimer.start(m_timeoutMs);

    return true;
}

void Pzem004Tv30Qt::onReadyRead()
{
    QByteArray chunk = m_serial.readAll();

    qDebug().noquote() << "PZEM RX chunk:" << chunk.toHex(' ').toUpper();

    m_rxBuffer.append(chunk);

   // m_rxBuffer.append(chunk);
    //qDebug() << "rcv:" <<
    if (m_pending == PendingNone) {
        // Ada data masuk tapi tidak sedang menunggu response.
        // Bisa saja noise/stale data, jadi dibersihkan.
        m_rxBuffer.clear();
        return;
    }

    // Sinkronisasi awal frame.
    // Kalau ada byte sampah sebelum address PZEM, buang.
    while (!m_rxBuffer.isEmpty() && quint8(m_rxBuffer.at(0)) != m_address) {
        m_rxBuffer.remove(0, 1);
    }

    if (m_rxBuffer.size() < m_expectedLength)
        return;

    QByteArray frame = m_rxBuffer.left(m_expectedLength);
    m_rxBuffer.remove(0, m_expectedLength);

    m_responseTimer.stop();

    if (!checkCrc(frame)) {
        failPending("Invalid PZEM CRC.");
        return;
    }

    emit rawFrameReceived(frame);

    if (m_pending == PendingReadAll) {
        Pzem004Tv30Data data;

        if (!parseReadAllFrame(frame, data)) {
            failPending(m_lastError);
            return;
        }

        m_pending = PendingNone;
        m_expectedLength = 0;

        emit dataReady(data);
    }
}

void Pzem004Tv30Qt::onResponseTimeout()
{
    failPending(
        QString("PZEM response timeout. RX %1/%2 bytes.")
            .arg(m_rxBuffer.size())
            .arg(m_expectedLength)
    );
}

void Pzem004Tv30Qt::onSerialError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError)
        return;

    const QString message = "Serial error: " + m_serial.errorString();

    m_lastError = message;
    emit errorOccurred(message);
}

bool Pzem004Tv30Qt::parseReadAllFrame(const QByteArray &frame, Pzem004Tv30Data &data)
{
    if (frame.size() != 25) {
        setLastError("Invalid PZEM response length.");
        return false;
    }

    if (quint8(frame[0]) != m_address) {
        setLastError("Invalid PZEM slave address.");
        return false;
    }

    if (quint8(frame[1]) != CmdReadInputRegister) {
        setLastError("Invalid PZEM function code.");
        return false;
    }

    if (quint8(frame[2]) != 20) {
        setLastError("Invalid PZEM byte count.");
        return false;
    }

    data.voltage     = u16be(frame, 3) / 10.0;
    data.current     = u32Pzem(frame, 5) / 1000.0;
    data.power       = u32Pzem(frame, 9) / 10.0;
    data.energy      = u32Pzem(frame, 13) / 1000.0;
    data.frequency   = u16be(frame, 17) / 10.0;
    data.powerFactor = u16be(frame, 19) / 100.0;
    data.alarm       = u16be(frame, 21) != 0x0000;

    return true;
}

QByteArray Pzem004Tv30Qt::buildFrame(quint8 function,
                                     quint16 regAddress,
                                     quint16 value,
                                     quint8 slaveAddress) const
{
    QByteArray frame;
    frame.reserve(8);

    frame.append(char(slaveAddress));
    frame.append(char(function));

    frame.append(char((regAddress >> 8) & 0xFF));
    frame.append(char(regAddress & 0xFF));

    frame.append(char((value >> 8) & 0xFF));
    frame.append(char(value & 0xFF));

    appendCrc(frame);

    return frame;
}

void Pzem004Tv30Qt::failPending(const QString &message)
{
    m_responseTimer.stop();

    m_pending = PendingNone;
    m_expectedLength = 0;
    m_rxBuffer.clear();

    setLastError(message);
    emit errorOccurred(message);
}

void Pzem004Tv30Qt::setLastError(const QString &message, QString *error)
{
    m_lastError = message;

    if (error)
        *error = message;
}

quint16 Pzem004Tv30Qt::crc16Modbus(const QByteArray &data)
{
    quint16 crc = 0xFFFF;

    for (char ch : data) {
        crc ^= quint8(ch);

        for (int i = 0; i < 8; ++i) {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }

    return crc;
}

void Pzem004Tv30Qt::appendCrc(QByteArray &frame)
{
    const quint16 crc = crc16Modbus(frame);

    // Modbus CRC: low byte dulu, lalu high byte.
    frame.append(char(crc & 0xFF));
    frame.append(char((crc >> 8) & 0xFF));
}

bool Pzem004Tv30Qt::checkCrc(const QByteArray &frame)
{
    if (frame.size() < 4)
        return false;

    const QByteArray payload = frame.left(frame.size() - 2);
    const quint16 calculated = crc16Modbus(payload);

    const quint16 received =
        quint8(frame[frame.size() - 2]) |
        (quint16(quint8(frame[frame.size() - 1])) << 8);

    return calculated == received;
}

quint16 Pzem004Tv30Qt::u16be(const QByteArray &data, int index)
{
    return (quint16(quint8(data[index])) << 8) |
            quint16(quint8(data[index + 1]));
}

quint32 Pzem004Tv30Qt::u32Pzem(const QByteArray &data, int indexLowReg)
{
    const quint32 lowReg = u16be(data, indexLowReg);
    const quint32 highReg = u16be(data, indexLowReg + 2);

    // Format PZEM: low register dulu, lalu high register.
    return lowReg | (highReg << 16);
}
