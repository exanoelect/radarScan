#pragma once

#include <QString>

class AudioTestWavGenerator
{
public:
    static bool generate(const QString &outputPath, QString *error = nullptr);

private:
    static void appendSilence(QVector<double> &samples,
                              double durationSec,
                              int sampleRate);

    static void appendSine(QVector<double> &samples,
                           double freqHz,
                           double durationSec,
                           double amplitude,
                           int sampleRate);

    static bool writePcm16MonoWav(const QString &path,
                                  const QVector<double> &samples,
                                  int sampleRate,
                                  QString *error);
};