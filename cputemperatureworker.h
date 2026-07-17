#ifndef CPUTEMPERATUREWORKER_H
#define CPUTEMPERATUREWORKER_H

#include <QObject>
#include <QString>

class CpuTemperatureWorker final : public QObject
{
    Q_OBJECT

public:
    explicit CpuTemperatureWorker(QObject *parent = nullptr);

public slots:
    void readTemperature();

signals:
    void temperatureReady(double temperatureC);
    void errorOccurred(const QString &message);
    void readFinished();
};

#endif // CPUTEMPERATUREWORKER_H
