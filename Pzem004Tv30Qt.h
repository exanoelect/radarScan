#ifndef PZEM004TV30QT_H
#define PZEM004TV30QT_H

#include <QObject>
#include <QSerialPort>
#include <QByteArray>
#include <QString>
#include <QTimer>
#include <QtGlobal>
#include <qdebug.h>

#include <limits>

struct Pzem004Tv30Data
{
    double voltage = std::numeric_limits<double>::quiet_NaN();    // V
    double current = std::numeric_limits<double>::quiet_NaN();    // A
    double power = std::numeric_limits<double>::quiet_NaN();      // W
    double energy = std::numeric_limits<double>::quiet_NaN();     // kWh
    double frequency = std::numeric_limits<double>::quiet_NaN();  // Hz
    double powerFactor = std::numeric_limits<double>::quiet_NaN();
    bool alarm = false;
};


Q_DECLARE_METATYPE(Pzem004Tv30Data)

class Pzem004Tv30Qt : public QObject
{
    Q_OBJECT

public:


    static constexpr quint8 DefaultAddress = 0xF8;
    static constexpr int DefaultBaudRate = 9600;
    static constexpr int DefaultTimeoutMs = 1000;

    explicit Pzem004Tv30Qt(QObject *parent = nullptr);

    bool open(const QString &portName,
              quint8 address = DefaultAddress,
              int timeoutMs = DefaultTimeoutMs);

    void close();
    bool isOpen() const;

    bool requestReadAll(QString *error = nullptr);

    QString lastError() const;

signals:
    void dataReady(const Pzem004Tv30Data &data);
    void errorOccurred(const QString &message);
    void rawFrameReceived(const QByteArray &frame);
    //void dataReqComplete(Pzem004Tv30Data mData);

private slots:
    void onReadyRead();
    void onResponseTimeout();
    void onSerialError(QSerialPort::SerialPortError error);

private:
    enum PendingCommand {
        PendingNone,
        PendingReadAll
    };

    static constexpr quint8 CmdReadInputRegister = 0x04;
    static constexpr quint16 RegVoltage = 0x0000;

    QSerialPort m_serial;
    QTimer m_responseTimer;

    QByteArray m_rxBuffer;

    quint8 m_address = DefaultAddress;
    int m_timeoutMs = DefaultTimeoutMs;

    PendingCommand m_pending = PendingNone;
    int m_expectedLength = 0;

    QString m_lastError;
    Pzem004Tv30Data pzemData;

    QByteArray buildFrame(quint8 function,
                          quint16 regAddress,
                          quint16 value,
                          quint8 slaveAddress) const;

    bool parseReadAllFrame(const QByteArray &frame, Pzem004Tv30Data &data);

    void failPending(const QString &message);
    void setLastError(const QString &message, QString *error = nullptr);

    static quint16 crc16Modbus(const QByteArray &data);
    static void appendCrc(QByteArray &frame);
    static bool checkCrc(const QByteArray &frame);

    static quint16 u16be(const QByteArray &data, int index);
    static quint32 u32Pzem(const QByteArray &data, int indexLowReg);
};

#endif // PZEM004TV30QT_H
