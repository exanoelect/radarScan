#include "cputemperatureworker.h"

#include <QFile>

CpuTemperatureWorker::CpuTemperatureWorker(QObject *parent)
    : QObject(parent)
{
}

void CpuTemperatureWorker::readTemperature()
{
    QFile file(QStringLiteral("/sys/class/thermal/thermal_zone0/temp"));

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit errorOccurred(
            QStringLiteral("Gagal membuka file temperature CPU: %1")
                .arg(file.errorString())
        );
        emit readFinished();
        return;
    }

    const QByteArray rawData = file.readAll().trimmed();
    file.close();

    bool ok = false;
    const double milliCelsius = rawData.toDouble(&ok);

    if (!ok) {
        emit errorOccurred(
            QStringLiteral("Data temperature CPU tidak valid: %1")
                .arg(QString::fromUtf8(rawData))
        );
        emit readFinished();
        return;
    }

    emit temperatureReady(milliCelsius / 1000.0);
    emit readFinished();
}
