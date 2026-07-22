#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <qdebug.h>
#include <QDebug>

class AudioHealthChecker
{
public:
    struct ToneResult {
        double frequencyHz = 0.0;
        double levelDbfs = -120.0;
        double relativeTo1kDb = 0.0;
        double dropoutPercent = 0.0;
    };

    struct Report {
        bool ok = false;
        QString verdict;

        double peakDbfs = -120.0;
        double clippingPercent = 0.0;
        double noiseFloorDbfs = -120.0;
        double level1kDbfs = -120.0;
        double snr1kDb = 0.0;
        double latencyMs = 0.0;
        double thd1kDb = -120.0;

        QVector<ToneResult> tones;
        QStringList notes;
        QStringList diagnose;
    };

    Report audioReport;

    bool createTestWav(const QString &path, QString *error = nullptr);

    bool runPipeWireTest(const QString &testWavPath,
                         const QString &recordedWavPath,
                         const QString &inputTarget,
                         const QString &outputTarget,
                         QString *error = nullptr);

    Report analyzeRecording(const QString &recordedWavPath, QString *error = nullptr);
    void printAudioHealthReport(const AudioHealthChecker::Report &report);
    void printAudioHealthReportCompact(const AudioHealthChecker::Report &report);
    QString formatReportCompact(const AudioHealthChecker::Report &report);

private:
    struct WavData {
        int sampleRate = 0;
        int channels = 0;
        QVector<double> samples;
    };

    struct ToneSpec {
        double frequencyHz = 0.0;
        double startSec = 0.0;
        double durationSec = 0.0;
    };

    static QVector<ToneSpec> toneSpecs();

    static bool writePcm16MonoWav(const QString &path,
                                  const QVector<double> &samples,
                                  int sampleRate,
                                  QString *error);

    static bool readPcm16Wav(const QString &path,
                             WavData &wav,
                             QString *error);

    static double rmsDbfs(const QVector<double> &x, int start, int count);
    static double peakDbfs(const QVector<double> &x);
    static double toneLevelDbfs(const QVector<double> &x,
                                int start,
                                int count,
                                int sampleRate,
                                double freqHz);
    static double dropoutPercent(const QVector<double> &x,
                                 int start,
                                 int count,
                                 int sampleRate);

    static int findSyncStartSample(const QVector<double> &x, int sampleRate);
    static double thd1kDb(const QVector<double> &x,
                          int start,
                          int count,
                          int sampleRate);

    static double dbfs(double linearRms);
};
