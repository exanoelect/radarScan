#pragma once
#include <QObject>
#include <QQueue>
#include <QByteArray>
#include <QSerialPort>
#include <QVariant>
#include <radar.h>
#include <QTimer>
#include <QElapsedTimer>
#include <QDateTime>

constexpr int HISTORY_SIZE = 40;
constexpr int TARGET_COUNT_SIZE = 20;

enum TargetState
{
    StateUnknown = 0,
    StateStanding,
    StateSitting,
    StateLying,
    StateFalling
};

struct TargetInfo
{
    bool valid;

    uint16_t trackId;

    qint16 x[HISTORY_SIZE];
    qint16 y[HISTORY_SIZE];
    qint16 height[HISTORY_SIZE];
    qint16 velocity[HISTORY_SIZE];
    qint16 motion[HISTORY_SIZE];
    //qint16 movement[HISTORY_SIZE];

    uint8_t historyCount;

    qint16 fallScore;
    quint8 state;

    qint64 lastSeenMs;

    bool lowHeightActive;
    QElapsedTimer lowHeightTimer;

    bool fallCandidateActive;
    QElapsedTimer fallTimer;

    bool hiddenStableActive = false;
    bool hiddenCandidateActive = false;
    QElapsedTimer hiddenCandidateTimer;
    QElapsedTimer hiddenStableTimer;
};


class PayloadProcessor : public QObject {
    Q_OBJECT
public:
    explicit PayloadProcessor(const QString &id, QObject *parent = nullptr);
    ~PayloadProcessor();

public slots:
    void initPort(const QString &portName);
    void readData();
    void closePort();
    void enqueuePayload(const QByteArray &payload);
    void prepareRadar(const QString portName);

signals:
    void uiUpdate(const QString &source, const QString &key, const QString &value);
    void radarPoint(const QString &source, double x, double y);
    void velocityUpdate(const QString &source, const QString &v);
    void motionUpdate(const QString &source, const QString &motion);
    void debugMessage(const QString &msg);
    void serialOpened(bool ok);
    void serialError(const QString &err);
    void fallDetected(const QString &source);   // trigger sound / socket
    void fallCancel(const QString &source);     //ga jadi fall

private:
    void processQueue();
    quint8 calcChecksum(const QByteArray &data);
    void sendCmdRadar(QByteArray cmd);

    QByteArray makeFrame(const QByteArray &body);
    QString toHexSpace(const QByteArray &data);

    void setHeight(int height);
    void setFallDuraion(int duration);
    void setAngle(int angle);

    void setPresence(bool checked);
    void setStandStill(bool checked);
    void setFallDetector(bool checked);
    void setTraceTracking(bool checked);

    QString m_id;
    QSerialPort *m_serial = nullptr;
    QByteArray m_buffer;
    QQueue<QByteArray> m_queue;

    TargetInfo targets[TARGET_COUNT_SIZE];

    //Fall algorithm
    bool isFallCandidate(const TargetInfo &t);
    bool isHeightTrendDecreasing(const TargetInfo &t, int *dropOut);
    void decisionFall(TargetInfo &t);
    void shiftHistory(qint16 data[HISTORY_SIZE], qint16 value);
    void resetFallState(TargetInfo &t);
    QString historyToString(const qint16 data[]);
    bool isStanding(const TargetInfo &t);
    bool isLostAfterHeightDrop(const TargetInfo &t);
    bool fallReported;

    QElapsedTimer fpsTimer;
    int frameCount = 0;
};
