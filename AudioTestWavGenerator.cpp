#include "AudioTestWavGenerator.h"

#include <QFile>
#include <QDataStream>
#include <QVector>
#include <QtMath>
#include <algorithm>

static constexpr int SAMPLE_RATE = 16000;
static constexpr double PI_D = 3.14159265358979323846;

static void writeFourCC(QDataStream &out, const char *id)
{
    out.writeRawData(id, 4);
}

static double clampSample(double v)
{
    if (v > 1.0) return 1.0;
    if (v < -1.0) return -1.0;
    return v;
}

void AudioTestWavGenerator::appendSilence(QVector<double> &samples,
                                          double durationSec,
                                          int sampleRate)
{
    const int n = qRound(durationSec * sampleRate);

    for (int i = 0; i < n; ++i) {
        samples.append(0.0);
    }
}

void AudioTestWavGenerator::appendSine(QVector<double> &samples,
                                       double freqHz,
                                       double durationSec,
                                       double amplitude,
                                       int sampleRate)
{
    const int n = qRound(durationSec * sampleRate);

    // Fade in/out 10 ms supaya tidak ada bunyi "click"
    const int fadeSamples = qRound(0.01 * sampleRate);

    for (int i = 0; i < n; ++i) {
        double gain = 1.0;

        if (i < fadeSamples) {
            gain = static_cast<double>(i) / static_cast<double>(fadeSamples);
        }

        const int remaining = n - 1 - i;

        if (remaining < fadeSamples) {
            gain = std::min(gain,
                            static_cast<double>(remaining) / static_cast<double>(fadeSamples));
        }

        const double t = static_cast<double>(i) / static_cast<double>(sampleRate);
        const double sample = amplitude * gain * qSin(2.0 * PI_D * freqHz * t);

        samples.append(sample);
    }
}

bool AudioTestWavGenerator::writePcm16MonoWav(const QString &path,
                                              const QVector<double> &samples,
                                              int sampleRate,
                                              QString *error)
{
    QFile file(path);

    if (!file.open(QIODevice::WriteOnly)) {
        if (error) {
            *error = "Failed to open WAV file for writing: " + file.errorString();
        }
        return false;
    }

    const quint16 audioFormat = 1; // PCM
    const quint16 numChannels = 1;
    const quint16 bitsPerSample = 16;
    const quint16 blockAlign = numChannels * bitsPerSample / 8;
    const quint32 byteRate = sampleRate * blockAlign;
    const quint32 dataSize = samples.size() * blockAlign;
    const quint32 riffSize = 36 + dataSize;

    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);

    // RIFF header
    writeFourCC(out, "RIFF");
    out << riffSize;
    writeFourCC(out, "WAVE");

    // fmt chunk
    writeFourCC(out, "fmt ");
    out << quint32(16);          // PCM fmt chunk size
    out << audioFormat;          // PCM = 1
    out << numChannels;          // mono
    out << quint32(sampleRate);   // 16000
    out << byteRate;
    out << blockAlign;
    out << bitsPerSample;

    // data chunk
    writeFourCC(out, "data");
    out << dataSize;

    for (double v : samples) {
        const double clipped = clampSample(v);
        const qint16 pcm = static_cast<qint16>(qRound(clipped * 32767.0));
        out << pcm;
    }

    file.close();
    return true;
}

bool AudioTestWavGenerator::generate(const QString &outputPath, QString *error)
{
    QVector<double> samples;
    samples.reserve(SAMPLE_RATE * 8);

    // 1. Silence awal untuk noise floor
    appendSilence(samples, 0.5, SAMPLE_RATE);

    // 2. Sync beep 1000 Hz, agak keras supaya gampang dideteksi
    appendSine(samples, 1000.0, 0.12, 0.70, SAMPLE_RATE);

    // 3. Silence setelah sync
    appendSilence(samples, 0.5, SAMPLE_RATE);

    // 4. Tone sweep manual
    // Amplitude 0.25 kira-kira sekitar -12 dBFS, aman dari clipping
    const QVector<double> freqs = {
        250.0,
        500.0,
        1000.0,
        2000.0,
        4000.0
    };

    for (double f : freqs) {
        appendSine(samples, f, 1.0, 0.25, SAMPLE_RATE);
        appendSilence(samples, 0.2, SAMPLE_RATE);
    }

    // 5. Silence akhir
    appendSilence(samples, 0.5, SAMPLE_RATE);

    return writePcm16MonoWav(outputPath, samples, SAMPLE_RATE, error);
}