#ifndef BME280WORKER_H
#define BME280WORKER_H

#include <QObject>
#include <QString>
#include <QtGlobal>

class Bme280Worker final : public QObject
{
    Q_OBJECT

public:
    explicit Bme280Worker(
        const QString &device = QStringLiteral("/dev/i2c-1"),
        quint8 address = 0x76,
        QObject *parent = nullptr
    );

public slots:
    void readSensor();

signals:
    void readingReady(double temperatureC,
                      double pressureHpa,
                      double humidityPercent);

    void errorOccurred(const QString &message);

    void readFinished();

private:
    struct Calibration {
        quint16 t1 = 0;
        qint16 t2 = 0;
        qint16 t3 = 0;

        quint16 p1 = 0;
        qint16 p2 = 0;
        qint16 p3 = 0;
        qint16 p4 = 0;
        qint16 p5 = 0;
        qint16 p6 = 0;
        qint16 p7 = 0;
        qint16 p8 = 0;
        qint16 p9 = 0;

        quint8 h1 = 0;
        qint16 h2 = 0;
        quint8 h3 = 0;
        qint16 h4 = 0;
        qint16 h5 = 0;
        qint8 h6 = 0;
    };

    bool readBme280(double &temperatureC,
                    double &pressureHpa,
                    double &humidityPercent,
                    QString &errorMessage);

    bool readCalibration(int fd,
                         Calibration &calibration,
                         QString &errorMessage);

    bool readRegisters(int fd,
                       quint8 startRegister,
                       quint8 *buffer,
                       int length,
                       QString &errorMessage);

    bool writeRegister(int fd,
                       quint8 reg,
                       quint8 value,
                       QString &errorMessage);

    static quint16 toUnsigned16(quint8 lsb, quint8 msb);
    static qint16 toSigned16(quint8 lsb, quint8 msb);
    static qint16 toSigned12(quint16 value);

    QString m_device;
    quint8 m_address = 0x76;
};

#endif // BME280WORKER_H
