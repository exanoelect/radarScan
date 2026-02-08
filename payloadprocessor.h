#pragma once
#include <QObject>
#include <QQueue>
#include <QByteArray>
#include <QSerialPort>
#include <QVariant>
#include <radar.h>

class PayloadProcessor : public QObject {
    Q_OBJECT
public:
    explicit PayloadProcessor(const QString &id, QObject *parent = nullptr);

public slots:
    void initPort(const QString &portName);
    void readData();
    void closePort();
    void enqueuePayload(const QByteArray &payload);

signals:
    void uiUpdate(const QString &source, const QString &key, const QString &value);
    void radarPoint(const QString &source, double x, double y);
    void velocityUpdate(const QString &source, const QString &v);
    void motionUpdate(const QString &source, const QString &motion);
    void debugMessage(const QString &msg);
    void serialOpened(bool ok);
    void serialError(const QString &err);
    void fallDetected(const QString &source);   // trigger sound / socket


private:
    void processQueue();
    quint8 calcChecksum(const QByteArray &data);

    QString m_id;
    QSerialPort *m_serial = nullptr;
    QByteArray m_buffer;
    QQueue<QByteArray> m_queue;
};
