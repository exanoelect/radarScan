#include "bme280worker.h"

#include <QByteArray>
#include <QThread>

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstring>

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace {
constexpr quint8 RegChipId = 0xD0;
constexpr quint8 ExpectedChipId = 0x60;
constexpr quint8 RegReset = 0xE0;
constexpr quint8 ResetCommand = 0xB6;
constexpr quint8 RegCtrlHum = 0xF2;
constexpr quint8 RegStatus = 0xF3;
constexpr quint8 RegCtrlMeas = 0xF4;
constexpr quint8 RegConfig = 0xF5;
constexpr quint8 RegData = 0xF7;
}

Bme280Worker::Bme280Worker(const QString &device,
                           quint8 address,
                           QObject *parent)
    : QObject(parent),
      m_device(device),
      m_address(address)
{
}

void Bme280Worker::readSensor()
{
    double temperatureC = 0.0;
    double pressureHpa = 0.0;
    double humidityPercent = 0.0;
    QString errorMessage;

    if (readBme280(temperatureC,
                   pressureHpa,
                   humidityPercent,
                   errorMessage)) {
        emit readingReady(
            temperatureC,
            pressureHpa,
            humidityPercent
        );
    } else {
        emit errorOccurred(errorMessage);
    }

    emit readFinished();
}

bool Bme280Worker::readBme280(double &temperatureC,
                              double &pressureHpa,
                              double &humidityPercent,
                              QString &errorMessage)
{
    const QByteArray devicePath = m_device.toLocal8Bit();

    const int fd = ::open(
        devicePath.constData(),
        O_RDWR | O_CLOEXEC
    );

    if (fd < 0) {
        errorMessage =
            QStringLiteral("Gagal membuka %1: %2")
                .arg(
                    m_device,
                    QString::fromLocal8Bit(
                        std::strerror(errno)
                    )
                );
        return false;
    }

    auto closeDevice = [&fd]() {
        ::close(fd);
    };

    if (::ioctl(
            fd,
            I2C_SLAVE,
            static_cast<int>(m_address)
        ) < 0) {
        errorMessage =
            QStringLiteral(
                "Gagal memilih alamat I2C 0x%1: %2"
            )
                .arg(
                    m_address,
                    2,
                    16,
                    QLatin1Char('0')
                )
                .arg(
                    QString::fromLocal8Bit(
                        std::strerror(errno)
                    )
                );

        closeDevice();
        return false;
    }

    quint8 chipId = 0;

    if (!readRegisters(
            fd,
            RegChipId,
            &chipId,
            1,
            errorMessage
        )) {
        closeDevice();
        return false;
    }

    if (chipId != ExpectedChipId) {
        errorMessage =
            QStringLiteral(
                "Perangkat pada 0x%1 bukan BME280. "
                "Chip ID terbaca 0x%2."
            )
                .arg(
                    m_address,
                    2,
                    16,
                    QLatin1Char('0')
                )
                .arg(
                    chipId,
                    2,
                    16,
                    QLatin1Char('0')
                );

        closeDevice();
        return false;
    }

    if (!writeRegister(
            fd,
            RegReset,
            ResetCommand,
            errorMessage
        )) {
        closeDevice();
        return false;
    }

    QThread::msleep(10);

    bool calibrationReady = false;

    for (int attempt = 0; attempt < 200; ++attempt) {
        quint8 status = 0;

        if (!readRegisters(
                fd,
                RegStatus,
                &status,
                1,
                errorMessage
            )) {
            closeDevice();
            return false;
        }

        if ((status & 0x01U) == 0U) {
            calibrationReady = true;
            break;
        }

        QThread::msleep(5);
    }

    if (!calibrationReady) {
        errorMessage =
            QStringLiteral(
                "Timeout menunggu calibration data BME280."
            );

        closeDevice();
        return false;
    }

    Calibration calibration;

    if (!readCalibration(
            fd,
            calibration,
            errorMessage
        )) {
        closeDevice();
        return false;
    }

    // Humidity oversampling x1.
    if (!writeRegister(
            fd,
            RegCtrlHum,
            0x01,
            errorMessage
        )) {
        closeDevice();
        return false;
    }

    // Standby 1000 ms, filter off.
    if (!writeRegister(
            fd,
            RegConfig,
            0xA0,
            errorMessage
        )) {
        closeDevice();
        return false;
    }

    // Temperature x1, pressure x1, normal mode.
    if (!writeRegister(
            fd,
            RegCtrlMeas,
            0x27,
            errorMessage
        )) {
        closeDevice();
        return false;
    }

    QThread::msleep(50);

    quint8 data[8] = {};

    if (!readRegisters(
            fd,
            RegData,
            data,
            8,
            errorMessage
        )) {
        closeDevice();
        return false;
    }

    closeDevice();

    const qint32 rawPressure =
        (static_cast<qint32>(data[0]) << 12) |
        (static_cast<qint32>(data[1]) << 4) |
        (static_cast<qint32>(data[2]) >> 4);

    const qint32 rawTemperature =
        (static_cast<qint32>(data[3]) << 12) |
        (static_cast<qint32>(data[4]) << 4) |
        (static_cast<qint32>(data[5]) >> 4);

    const qint32 rawHumidity =
        (static_cast<qint32>(data[6]) << 8) |
        static_cast<qint32>(data[7]);

    const double temperatureVar1 =
        (
            static_cast<double>(rawTemperature) / 16384.0 -
            static_cast<double>(calibration.t1) / 1024.0
        ) * static_cast<double>(calibration.t2);

    const double temperaturePart =
        static_cast<double>(rawTemperature) / 131072.0 -
        static_cast<double>(calibration.t1) / 8192.0;

    const double temperatureVar2 =
        temperaturePart *
        temperaturePart *
        static_cast<double>(calibration.t3);

    const double tFine =
        temperatureVar1 + temperatureVar2;

    temperatureC = tFine / 5120.0;

    double pressureVar1 =
        tFine / 2.0 - 64000.0;

    double pressureVar2 =
        pressureVar1 *
        pressureVar1 *
        static_cast<double>(calibration.p6) /
        32768.0;

    pressureVar2 +=
        pressureVar1 *
        static_cast<double>(calibration.p5) *
        2.0;

    pressureVar2 =
        pressureVar2 / 4.0 +
        static_cast<double>(calibration.p4) *
        65536.0;

    pressureVar1 =
        (
            static_cast<double>(calibration.p3) *
            pressureVar1 *
            pressureVar1 /
            524288.0 +
            static_cast<double>(calibration.p2) *
            pressureVar1
        ) / 524288.0;

    pressureVar1 =
        (
            1.0 +
            pressureVar1 / 32768.0
        ) *
        static_cast<double>(calibration.p1);

    if (std::abs(pressureVar1) < 1e-12) {
        errorMessage =
            QStringLiteral(
                "Pembagian nol saat menghitung pressure."
            );
        return false;
    }

    double pressurePa =
        1048576.0 -
        static_cast<double>(rawPressure);

    pressurePa =
        (
            pressurePa -
            pressureVar2 / 4096.0
        ) *
        6250.0 /
        pressureVar1;

    pressureVar1 =
        static_cast<double>(calibration.p9) *
        pressurePa *
        pressurePa /
        2147483648.0;

    pressureVar2 =
        pressurePa *
        static_cast<double>(calibration.p8) /
        32768.0;

    pressurePa +=
        (
            pressureVar1 +
            pressureVar2 +
            static_cast<double>(calibration.p7)
        ) / 16.0;

    pressureHpa = pressurePa / 100.0;

    double humidity = tFine - 76800.0;

    humidity =
        (
            static_cast<double>(rawHumidity) -
            (
                static_cast<double>(calibration.h4) *
                64.0 +
                static_cast<double>(calibration.h5) /
                16384.0 *
                humidity
            )
        ) *
        (
            static_cast<double>(calibration.h2) /
            65536.0 *
            (
                1.0 +
                static_cast<double>(calibration.h6) /
                67108864.0 *
                humidity *
                (
                    1.0 +
                    static_cast<double>(calibration.h3) /
                    67108864.0 *
                    humidity
                )
            )
        );

    humidity *=
        (
            1.0 -
            static_cast<double>(calibration.h1) *
            humidity /
            524288.0
        );

    humidityPercent =
        std::clamp(
            humidity,
            0.0,
            100.0
        );

    if (!std::isfinite(temperatureC) ||
        !std::isfinite(pressureHpa) ||
        !std::isfinite(humidityPercent)) {
        errorMessage =
            QStringLiteral(
                "Hasil perhitungan BME280 tidak valid."
            );
        return false;
    }

    return true;
}

bool Bme280Worker::readCalibration(
    int fd,
    Calibration &calibration,
    QString &errorMessage
)
{
    quint8 block1[26] = {};
    quint8 block2[7] = {};

    if (!readRegisters(
            fd,
            0x88,
            block1,
            26,
            errorMessage
        )) {
        return false;
    }

    if (!readRegisters(
            fd,
            0xE1,
            block2,
            7,
            errorMessage
        )) {
        return false;
    }

    calibration.t1 =
        toUnsigned16(block1[0], block1[1]);

    calibration.t2 =
        toSigned16(block1[2], block1[3]);

    calibration.t3 =
        toSigned16(block1[4], block1[5]);

    calibration.p1 =
        toUnsigned16(block1[6], block1[7]);

    calibration.p2 =
        toSigned16(block1[8], block1[9]);

    calibration.p3 =
        toSigned16(block1[10], block1[11]);

    calibration.p4 =
        toSigned16(block1[12], block1[13]);

    calibration.p5 =
        toSigned16(block1[14], block1[15]);

    calibration.p6 =
        toSigned16(block1[16], block1[17]);

    calibration.p7 =
        toSigned16(block1[18], block1[19]);

    calibration.p8 =
        toSigned16(block1[20], block1[21]);

    calibration.p9 =
        toSigned16(block1[22], block1[23]);

    calibration.h1 = block1[25];

    calibration.h2 =
        toSigned16(block2[0], block2[1]);

    calibration.h3 = block2[2];

    const quint16 h4 =
        (
            static_cast<quint16>(block2[3]) << 4
        ) |
        (
            static_cast<quint16>(block2[4]) & 0x0F
        );

    const quint16 h5 =
        (
            static_cast<quint16>(block2[5]) << 4
        ) |
        (
            static_cast<quint16>(block2[4]) >> 4
        );

    calibration.h4 = toSigned12(h4);
    calibration.h5 = toSigned12(h5);
    calibration.h6 = static_cast<qint8>(block2[6]);

    if (calibration.p1 == 0) {
        errorMessage =
            QStringLiteral(
                "Calibration pressure BME280 tidak valid."
            );
        return false;
    }

    return true;
}

bool Bme280Worker::readRegisters(
    int fd,
    quint8 startRegister,
    quint8 *buffer,
    int length,
    QString &errorMessage
)
{
    quint8 registerAddress = startRegister;

    i2c_msg messages[2] = {};

    messages[0].addr = m_address;
    messages[0].flags = 0;
    messages[0].len = 1;
    messages[0].buf = &registerAddress;

    messages[1].addr = m_address;
    messages[1].flags = I2C_M_RD;
    messages[1].len = static_cast<__u16>(length);
    messages[1].buf = buffer;

    i2c_rdwr_ioctl_data transaction {};
    transaction.msgs = messages;
    transaction.nmsgs = 2;

    if (::ioctl(
            fd,
            I2C_RDWR,
            &transaction
        ) < 0) {
        errorMessage =
            QStringLiteral(
                "Gagal membaca register 0x%1: %2"
            )
                .arg(
                    startRegister,
                    2,
                    16,
                    QLatin1Char('0')
                )
                .arg(
                    QString::fromLocal8Bit(
                        std::strerror(errno)
                    )
                );

        return false;
    }

    return true;
}

bool Bme280Worker::writeRegister(
    int fd,
    quint8 reg,
    quint8 value,
    QString &errorMessage
)
{
    quint8 buffer[2] = {
        reg,
        value
    };

    i2c_msg message {};
    message.addr = m_address;
    message.flags = 0;
    message.len = 2;
    message.buf = buffer;

    i2c_rdwr_ioctl_data transaction {};
    transaction.msgs = &message;
    transaction.nmsgs = 1;

    if (::ioctl(
            fd,
            I2C_RDWR,
            &transaction
        ) < 0) {
        errorMessage =
            QStringLiteral(
                "Gagal menulis register 0x%1: %2"
            )
                .arg(
                    reg,
                    2,
                    16,
                    QLatin1Char('0')
                )
                .arg(
                    QString::fromLocal8Bit(
                        std::strerror(errno)
                    )
                );

        return false;
    }

    return true;
}

quint16 Bme280Worker::toUnsigned16(
    quint8 lsb,
    quint8 msb
)
{
    return static_cast<quint16>(
        (
            static_cast<quint16>(msb) << 8
        ) |
        static_cast<quint16>(lsb)
    );
}

qint16 Bme280Worker::toSigned16(
    quint8 lsb,
    quint8 msb
)
{
    return static_cast<qint16>(
        toUnsigned16(lsb, msb)
    );
}

qint16 Bme280Worker::toSigned12(
    quint16 value
)
{
    value &= 0x0FFF;

    if ((value & 0x0800U) != 0U) {
        return static_cast<qint16>(
            value | 0xF000U
        );
    }

    return static_cast<qint16>(value);
}
