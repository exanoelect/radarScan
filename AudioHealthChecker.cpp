#include "AudioHealthChecker.h"

#include <QFile>
#include <QDataStream>
#include <QProcess>
#include <QThread>
#include <QtMath>
#include <algorithm>
#include <cmath>

static constexpr int TEST_SAMPLE_RATE = 16000;
static constexpr double PI_D = 3.14159265358979323846;

//---------------------------------------------------------------------------------------
static void writeFourCC(QDataStream &out, const char *s)
{
    out.writeRawData(s, 4);
}

//---------------------------------------------------------------------------------------
static double clampSample(double v)
{
    if (v > 1.0) return 1.0;
    if (v < -1.0) return -1.0;
    return v;
}

//---------------------------------------------------------------------------------------
static void appendSilence(QVector<double> &samples, double durationSec, int sampleRate)
{
    const int n = qRound(durationSec * sampleRate);
    for (int i = 0; i < n; ++i)
        samples.append(0.0);
}

//---------------------------------------------------------------------------------------
static void appendSine(QVector<double> &samples,
                       double freqHz,
                       double durationSec,
                       double amplitude,
                       int sampleRate)
{
    const int n = qRound(durationSec * sampleRate);
    const int fadeN = qRound(0.01 * sampleRate); // 10 ms fade

    for (int i = 0; i < n; ++i) {
        double gain = 1.0;

        if (i < fadeN)
            gain = double(i) / double(fadeN);

        const int remain = n - 1 - i;
        if (remain < fadeN)
            gain = std::min(gain, double(remain) / double(fadeN));

        const double t = double(i) / double(sampleRate);
        const double v = amplitude * gain * qSin(2.0 * PI_D * freqHz * t);
        samples.append(v);
    }
}

//---------------------------------------------------------------------------------------
QVector<AudioHealthChecker::ToneSpec> AudioHealthChecker::toneSpecs()
{
    QVector<ToneSpec> specs;

    double t = 0.0;

    t += 0.5;   // silence awal
    t += 0.12;  // sync beep
    t += 0.5;   // silence setelah sync

    const QVector<double> freqs = {250.0, 500.0, 1000.0, 2000.0, 4000.0};

    for (double f : freqs) {
        specs.append({f, t, 1.0});
        t += 1.0;
        t += 0.2;
    }

    return specs;
}

//---------------------------------------------------------------------------------------
bool AudioHealthChecker::createTestWav(const QString &path, QString *error)
{
    QVector<double> samples;

    appendSilence(samples, 0.5, TEST_SAMPLE_RATE);

    // Sync beep dibuat lebih kencang supaya mudah dideteksi.
    appendSine(samples, 1000.0, 0.12, 0.70, TEST_SAMPLE_RATE);

    appendSilence(samples, 0.5, TEST_SAMPLE_RATE);

    // Tone test level aman sekitar -12 dBFS.
    const QVector<double> freqs = {250.0, 500.0, 1000.0, 2000.0, 4000.0};

    for (double f : freqs) {
        appendSine(samples, f, 1.0, 0.25, TEST_SAMPLE_RATE);
        appendSilence(samples, 0.2, TEST_SAMPLE_RATE);
    }

    appendSilence(samples, 0.5, TEST_SAMPLE_RATE);

    return writePcm16MonoWav(path, samples, TEST_SAMPLE_RATE, error);
}

//---------------------------------------------------------------------------------------
bool AudioHealthChecker::writePcm16MonoWav(const QString &path,
                                           const QVector<double> &samples,
                                           int sampleRate,
                                           QString *error)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        if (error) *error = "Failed to open WAV for writing: " + file.errorString();
        return false;
    }

    const quint16 audioFormat = 1; // PCM
    const quint16 channels = 1;
    const quint16 bitsPerSample = 16;
    const quint16 blockAlign = channels * bitsPerSample / 8;
    const quint32 byteRate = sampleRate * blockAlign;
    const quint32 dataBytes = samples.size() * blockAlign;
    const quint32 riffSize = 36 + dataBytes;

    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);

    writeFourCC(out, "RIFF");
    out << riffSize;
    writeFourCC(out, "WAVE");

    writeFourCC(out, "fmt ");
    out << quint32(16);
    out << audioFormat;
    out << channels;
    out << quint32(sampleRate);
    out << byteRate;
    out << blockAlign;
    out << bitsPerSample;

    writeFourCC(out, "data");
    out << dataBytes;

    for (double v : samples) {
        const double c = clampSample(v);
        const qint16 s = qRound(c * 32767.0);
        out << s;
    }

    return true;
}

//---------------------------------------------------------------------------------------
bool AudioHealthChecker::readPcm16Wav(const QString &path,
                                      WavData &wav,
                                      QString *error)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        if (error) *error = "Failed to open WAV: " + file.errorString();
        return false;
    }

    QDataStream in(&file);
    in.setByteOrder(QDataStream::LittleEndian);

    char riff[4];
    char wave[4];

    if (in.readRawData(riff, 4) != 4 || QByteArray(riff, 4) != "RIFF") {
        if (error) *error = "Invalid WAV: missing RIFF";
        return false;
    }

    quint32 riffSize = 0;
    in >> riffSize;
    Q_UNUSED(riffSize);

    if (in.readRawData(wave, 4) != 4 || QByteArray(wave, 4) != "WAVE") {
        if (error) *error = "Invalid WAV: missing WAVE";
        return false;
    }

    quint16 audioFormat = 0;
    quint16 channels = 0;
    quint32 sampleRate = 0;
    quint16 bitsPerSample = 0;
    quint16 blockAlign = 0;

    QByteArray pcmBytes;
    bool hasFmt = false;
    bool hasData = false;

    while (!in.atEnd()) {
        char idRaw[4];

        if (in.readRawData(idRaw, 4) != 4)
            break;

        QByteArray chunkId(idRaw, 4);
        quint32 chunkSize = 0;
        in >> chunkSize;

        const qint64 chunkDataPos = file.pos();

        if (chunkId == "fmt ") {
            quint32 byteRate = 0;

            in >> audioFormat;
            in >> channels;
            in >> sampleRate;
            in >> byteRate;
            in >> blockAlign;
            in >> bitsPerSample;

            Q_UNUSED(byteRate);
            hasFmt = true;
        } else if (chunkId == "data") {
            pcmBytes = file.read(chunkSize);
            hasData = true;
        }

        qint64 nextPos = chunkDataPos + chunkSize;
        if (chunkSize & 1)
            nextPos += 1; // RIFF padding byte

        file.seek(nextPos);

        if (hasFmt && hasData)
            break;
    }

    if (!hasFmt || !hasData) {
        if (error) *error = "Invalid WAV: missing fmt or data chunk";
        return false;
    }

    if (audioFormat != 1 || bitsPerSample != 16) {
        if (error) *error = "Only PCM 16-bit WAV is supported";
        return false;
    }

    if (channels < 1) {
        if (error) *error = "Invalid WAV channel count";
        return false;
    }

    wav.sampleRate = int(sampleRate);
    wav.channels = int(channels);
    wav.samples.clear();

    const int bytesPerSample = bitsPerSample / 8;
    const int frameSize = blockAlign;
    const int frames = pcmBytes.size() / frameSize;

    wav.samples.reserve(frames);

    const uchar *p = reinterpret_cast<const uchar *>(pcmBytes.constData());

    for (int i = 0; i < frames; ++i) {
        double acc = 0.0;

        for (int ch = 0; ch < channels; ++ch) {
            const int idx = i * frameSize + ch * bytesPerSample;
            const quint16 u = quint16(p[idx]) | (quint16(p[idx + 1]) << 8);
            const qint16 s = static_cast<qint16>(u);
            acc += double(s) / 32768.0;
        }

        wav.samples.append(acc / double(channels));
    }

    return true;
}

//---------------------------------------------------------------------------------------
bool AudioHealthChecker::runPipeWireTest(const QString &testWavPath,
                                         const QString &recordedWavPath,
                                         const QString &inputTarget,
                                         const QString &outputTarget,
                                         QString *error)
{
    QProcess recorder;
    QProcess player;

    QStringList recArgs;

    if (!inputTarget.isEmpty()) {
        recArgs << "--target" << inputTarget;
    }

    recArgs << "--rate" << QString::number(TEST_SAMPLE_RATE)
            << "--channels" << "1"
            << "--format" << "s16"
            << recordedWavPath;

    recorder.start("pw-record", recArgs);

    if (!recorder.waitForStarted(3000)) {
        if (error) *error = "Failed to start pw-record";
        return false;
    }

    // Beri waktu recorder siap sebelum playback.
    QThread::msleep(300);

    QStringList playArgs;

    if (!outputTarget.isEmpty()) {
        playArgs << "--target" << outputTarget;
    }

    playArgs << testWavPath;

    player.start("pw-play", playArgs);

    if (!player.waitForStarted(3000)) {
        recorder.terminate();
        recorder.waitForFinished(1000);

        if (error) *error = "Failed to start pw-play";
        return false;
    }

    player.waitForFinished(15000);

    // Tambahkan sedikit tail recording.
    QThread::msleep(500);

    recorder.terminate();

    if (!recorder.waitForFinished(2000)) {
        recorder.kill();
        recorder.waitForFinished(1000);
    }

    return true;
}

//---------------------------------------------------------------------------------------
double AudioHealthChecker::dbfs(double linearRms)
{
    if (linearRms <= 1e-12)
        return -120.0;

    return 20.0 * std::log10(linearRms);
}

//---------------------------------------------------------------------------------------
double AudioHealthChecker::rmsDbfs(const QVector<double> &x, int start, int count)
{
    if (x.isEmpty() || count <= 0)
        return -120.0;

    start = std::max(0, start);
    const int end = std::min(start + count, static_cast<int>(x.size()));
    if (end <= start)
        return -120.0;

    double sum = 0.0;

    for (int i = start; i < end; ++i)
        sum += x[i] * x[i];

    const double rms = std::sqrt(sum / double(end - start));
    return dbfs(rms);
}

//---------------------------------------------------------------------------------------
double AudioHealthChecker::peakDbfs(const QVector<double> &x)
{
    double peak = 0.0;

    for (double v : x)
        peak = std::max(peak, std::abs(v));

    return dbfs(peak);
}

//---------------------------------------------------------------------------------------
double AudioHealthChecker::toneLevelDbfs(const QVector<double> &x,
                                         int start,
                                         int count,
                                         int sampleRate,
                                         double freqHz)
{
    if (x.isEmpty() || count <= 0)
        return -120.0;

    start = std::max(0, start);
    const int end = std::min(start + count, static_cast<int>(x.size()));

    if (end <= start)
        return -120.0;

    double sumCos = 0.0;
    double sumSin = 0.0;

    const int n = end - start;

    for (int i = 0; i < n; ++i) {
        const double t = double(i) / double(sampleRate);
        const double angle = 2.0 * PI_D * freqHz * t;
        const double v = x[start + i];

        sumCos += v * std::cos(angle);
        sumSin += v * std::sin(angle);
    }

    const double peakAmp = 2.0 * std::sqrt(sumCos * sumCos + sumSin * sumSin) / double(n);
    const double rms = peakAmp / std::sqrt(2.0);

    return dbfs(rms);
}

//---------------------------------------------------------------------------------------
double AudioHealthChecker::dropoutPercent(const QVector<double> &x,
                                          int start,
                                          int count,
                                          int sampleRate)
{
    const int win = qRound(0.02 * sampleRate); // 20 ms

    if (win <= 0 || count <= win)
        return 0.0;

    start = std::max(0, start);
    const int end = std::min(start + count, static_cast<int>(x.size()));

    if (end <= start + win)
        return 0.0;

    const double fullRmsDb = rmsDbfs(x, start, end - start);
    const double thresholdDb = fullRmsDb - 12.0;

    int total = 0;
    int bad = 0;

    for (int i = start; i + win < end; i += win) {
        const double wDb = rmsDbfs(x, i, win);
        total++;

        if (wDb < thresholdDb)
            bad++;
    }

    if (total == 0)
        return 0.0;

    return 100.0 * double(bad) / double(total);
}

//---------------------------------------------------------------------------------------
int AudioHealthChecker::findSyncStartSample(const QVector<double> &x, int sampleRate)
{
    const int noiseCount = std::min(qRound(0.35 * sampleRate), static_cast<int>(x.size()));
    const double noiseDb = rmsDbfs(x, 0, noiseCount);
    const double noiseLinear = std::pow(10.0, noiseDb / 20.0);

    const double minThreshold = 0.02;
    const double threshold = std::max(minThreshold, noiseLinear * 5.0);

    const int win = qRound(0.02 * sampleRate); // 20 ms
    const int hop = qRound(0.01 * sampleRate); // 10 ms

    for (int i = 0; i + win < x.size(); i += hop) {
        double sum = 0.0;

        for (int j = 0; j < win; ++j) {
            const double v = x[i + j];
            sum += v * v;
        }

        const double rms = std::sqrt(sum / double(win));

        if (rms > threshold)
            return i;
    }

    return -1;
}

//---------------------------------------------------------------------------------------
double AudioHealthChecker::thd1kDb(const QVector<double> &x,
                                   int start,
                                   int count,
                                   int sampleRate)
{
    const double fundDb = toneLevelDbfs(x, start, count, sampleRate, 1000.0);
    const double fundRms = std::pow(10.0, fundDb / 20.0);
    const double fundPower = fundRms * fundRms;

    if (fundPower <= 1e-12)
        return -120.0;

    double harmonicPower = 0.0;

    for (double h : {2000.0, 3000.0, 4000.0, 5000.0, 6000.0}) {
        if (h >= sampleRate / 2.0)
            continue;

        const double hDb = toneLevelDbfs(x, start, count, sampleRate, h);
        const double hRms = std::pow(10.0, hDb / 20.0);
        harmonicPower += hRms * hRms;
    }

    if (harmonicPower <= 1e-12)
        return -120.0;

    return 10.0 * std::log10(harmonicPower / fundPower);
}

//---------------------------------------------------------------------------------------
AudioHealthChecker::Report AudioHealthChecker::analyzeRecording(const QString &recordedWavPath,
                                                                QString *error)
{
    Report report;

    auto addIssue = [&](const QString &note, const QString &diagnose) {
        report.notes << note;
        report.diagnose << diagnose;
    };

    WavData wav;

    if (!readPcm16Wav(recordedWavPath, wav, error)) {
        report.ok = false;
        report.verdict = "FAIL";

        addIssue(
            "Recording WAV can't be read.",
            "Check whether pw-record finished correctly, the file path is valid, the file permission is allowed, and the WAV format is PCM 16-bit."
        );

        return report;
    }

    if (wav.sampleRate != TEST_SAMPLE_RATE) {
        addIssue(
            QString("Recording sample rate is %1 Hz, expected %2 Hz.")
                .arg(wav.sampleRate)
                .arg(TEST_SAMPLE_RATE),
            "Sample rate mismatch can affect tone detection, latency estimation, and frequency analysis. Use --rate 16000 in pw-record."
        );
    }

    const QVector<double> &x = wav.samples;
    const int sr = wav.sampleRate;

    if (x.isEmpty()) {
        report.ok = false;
        report.verdict = "FAIL";

        addIssue(
            "Recording WAV contains no audio samples.",
            "The recorder may have stopped too early, the input device may be wrong, or pw-record failed to capture audio."
        );

        return report;
    }

    report.peakDbfs = peakDbfs(x);

    int clipped = 0;
    for (double v : x) {
        if (std::abs(v) >= 0.98)
            clipped++;
    }

    report.clippingPercent = 100.0 * double(clipped) / double(x.size());

    const int syncSample = findSyncStartSample(x, sr);

    if (syncSample < 0) {
        report.ok = false;
        report.verdict = "FAIL";

        addIssue(
            "Sync beep was not detected in the recording.",
            "Possible causes: speaker volume is too low, speaker output device is wrong, microphone is muted, microphone target is wrong, or the microphone is too far from the speaker."
        );

        return report;
    }

    const double syncStartRefSec = 0.5;
    const int playbackOffsetSample = syncSample - qRound(syncStartRefSec * sr);
    report.latencyMs = 1000.0 * double(playbackOffsetSample) / double(sr);

    const int noiseStart = std::max(0, playbackOffsetSample);
    const int noiseCount = qRound(0.35 * sr);
    report.noiseFloorDbfs = rmsDbfs(x, noiseStart, noiseCount);

    double level1k = -120.0;

    for (const ToneSpec &spec : toneSpecs()) {
        int segStart = playbackOffsetSample + qRound(spec.startSec * sr);
        int segCount = qRound(spec.durationSec * sr);

        // Trim awal dan akhir supaya fade-in/fade-out tidak ikut dianalisis.
        const int trim = qRound(0.1 * sr);
        segStart += trim;
        segCount -= 2 * trim;

        ToneResult tr;
        tr.frequencyHz = spec.frequencyHz;
        tr.levelDbfs = toneLevelDbfs(x, segStart, segCount, sr, spec.frequencyHz);
        tr.dropoutPercent = dropoutPercent(x, segStart, segCount, sr);

        if (qRound(spec.frequencyHz) == 1000)
            level1k = tr.levelDbfs;

        report.tones.append(tr);
    }

    report.level1kDbfs = level1k;
    report.snr1kDb = report.level1kDbfs - report.noiseFloorDbfs;

    for (ToneResult &tr : report.tones) {
        tr.relativeTo1kDb = tr.levelDbfs - report.level1kDbfs;
    }

    // Cari segment 1 kHz untuk THD kasar.
    for (const ToneSpec &spec : toneSpecs()) {
        if (qRound(spec.frequencyHz) == 1000) {
            int segStart = playbackOffsetSample + qRound(spec.startSec * sr);
            int segCount = qRound(spec.durationSec * sr);

            const int trim = qRound(0.1 * sr);
            segStart += trim;
            segCount -= 2 * trim;

            report.thd1kDb = thd1kDb(x, segStart, segCount, sr);
            break;
        }
    }

    bool fail = false;
    bool warn = false;

    // ================================
    // Main audio level check
    // ================================
    if (report.level1kDbfs < -45.0) {
        fail = true;

        addIssue(
            QString("1 kHz tone level is too low: %1 dBFS.")
                .arg(report.level1kDbfs, 0, 'f', 1),
            "Possible causes: speaker volume is too low, microphone gain is too low, microphone is not capturing audio properly, or the speaker-to-microphone distance is too far."
        );
    } else {
        report.notes << QString("1 kHz tone level is acceptable: %1 dBFS.")
                            .arg(report.level1kDbfs, 0, 'f', 1);
    }

    // ================================
    // SNR check
    // ================================
    if (report.snr1kDb < 18.0) {
        fail = true;

        addIssue(
            QString("SNR is too low: %1 dB.")
                .arg(report.snr1kDb, 0, 'f', 1),
            "Possible causes: room noise is too high, microphone noise floor is high, speaker signal is too weak, or the microphone gain is not properly adjusted."
        );
    } else {
        report.notes << QString("SNR is acceptable: %1 dB.")
                            .arg(report.snr1kDb, 0, 'f', 1);
    }

    // ================================
    // Clipping check
    // ================================
    if (report.clippingPercent > 0.5) {
        fail = true;

        addIssue(
            QString("High clipping detected: %1%.")
                .arg(report.clippingPercent, 0, 'f', 2),
            "Possible causes: speaker volume is too high, microphone gain is too high, or automatic gain control is boosting the input too aggressively."
        );
    } else {
        report.notes << QString("Clipping is acceptable: %1%.")
                            .arg(report.clippingPercent, 0, 'f', 2);
    }

    // ================================
    // Noise floor check
    // ================================
    if (report.noiseFloorDbfs > -35.0) {
        warn = true;

        addIssue(
            QString("Noise floor is quite high: %1 dBFS.")
                .arg(report.noiseFloorDbfs, 0, 'f', 1),
            "Possible causes: room is noisy, microphone gain is too high, fan noise is captured, electrical noise exists, or the microphone is too sensitive."
        );
    } else {
        report.notes << QString("Noise floor is acceptable: %1 dBFS.")
                            .arg(report.noiseFloorDbfs, 0, 'f', 1);
    }

    // ================================
    // Tone response and dropout check
    // ================================
    for (const ToneResult &tr : report.tones) {
        if (tr.frequencyHz >= 500.0 && tr.frequencyHz <= 2000.0) {
            if (std::abs(tr.relativeTo1kDb) > 12.0) {
                warn = true;

                addIssue(
                    QString("Tone %1 Hz differs too much from 1 kHz reference: %2 dB.")
                        .arg(tr.frequencyHz, 0, 'f', 0)
                        .arg(tr.relativeTo1kDb, 0, 'f', 1),
                    "Possible causes: speaker frequency response is uneven, microphone frequency response is uneven, speaker position is not ideal, or the test environment has strong acoustic reflection."
                );
            } else {
                report.notes << QString("Tone %1 Hz response is acceptable: %2 dB relative to 1 kHz.")
                                    .arg(tr.frequencyHz, 0, 'f', 0)
                                    .arg(tr.relativeTo1kDb, 0, 'f', 1);
            }
        }

        if (tr.dropoutPercent > 5.0) {
            fail = true;

            addIssue(
                QString("High dropout detected at tone %1 Hz: %2%.")
                    .arg(tr.frequencyHz, 0, 'f', 0)
                    .arg(tr.dropoutPercent, 0, 'f', 1),
                "Possible causes: audio buffer underrun, high CPU load, unstable USB microphone, PipeWire/ALSA routing issue, or recording/playback process interruption."
            );
        } else {
            report.notes << QString("Dropout at tone %1 Hz is acceptable: %2%.")
                                .arg(tr.frequencyHz, 0, 'f', 0)
                                .arg(tr.dropoutPercent, 0, 'f', 1);
        }
    }

    // ================================
    // THD rough check
    // ================================
    if (report.thd1kDb > -20.0) {
        warn = true;

        addIssue(
            QString("1 kHz distortion level may be high. THD ratio: %1 dB.")
                .arg(report.thd1kDb, 0, 'f', 1),
            "Possible causes: speaker volume is too high, microphone input is overloaded, speaker is distorted, or the acoustic path is too close and causing nonlinear capture."
        );
    } else {
        report.notes << QString("1 kHz distortion level is acceptable. THD ratio: %1 dB.")
                            .arg(report.thd1kDb, 0, 'f', 1);
    }

    // ================================
    // Final verdict
    // ================================
    if (fail) {
        report.ok = false;
        report.verdict = "FAIL";

        report.notes.prepend("Audio health check failed.");
        report.diagnose.prepend("The audio path is not reliable for normal operation. Check playback device, recording device, volume level, microphone gain, and room noise.");
    } else if (warn) {
        report.ok = true;
        report.verdict = "WARN";

        report.notes.prepend("Audio health check completed with warning.");
        report.diagnose.prepend("The audio path is working, but some parameters are not ideal. Review the warning details before using it for production.");
    } else {
        report.ok = true;
        report.verdict = "PASS";

        report.notes.prepend("Audio health check passed.");
        report.notes << "Audio path is working normally.";

        report.diagnose << "No critical audio issue detected. Playback, recording, level, SNR, clipping, and dropout are within acceptable limits.";
    }

    return report;
}

//---------------------------------------------------------------------------------------
void AudioHealthChecker::printAudioHealthReport(const AudioHealthChecker::Report &report)
{
    qDebug() << "========== AUDIO HEALTH REPORT ==========";
    qDebug() << "Verdict        :" << report.verdict;
    qDebug() << "OK             :" << report.ok;
    qDebug() << "Peak dBFS      :" << report.peakDbfs;
    qDebug() << "Clipping %     :" << report.clippingPercent;
    qDebug() << "Noise floor    :" << report.noiseFloorDbfs << "dBFS";
    qDebug() << "1k level       :" << report.level1kDbfs << "dBFS";
    qDebug() << "SNR 1k         :" << report.snr1kDb << "dB";
    qDebug() << "Latency        :" << report.latencyMs << "ms";
    qDebug() << "THD 1k         :" << report.thd1kDb << "dB";

    qDebug() << "------------- Tone Result -------------";

    for (const auto &tone : report.tones) {
        qDebug() << "Tone" << tone.frequencyHz << "Hz"
                 << "| Level:" << tone.levelDbfs << "dBFS"
                 << "| Relative to 1k:" << tone.relativeTo1kDb << "dB"
                 << "| Dropout:" << tone.dropoutPercent << "%";
    }

    qDebug() << "---------------- Notes ----------------";

    for (const QString &note : report.notes) {
        qDebug().noquote() << "-" << note;
    }

    qDebug() << "-------------- Diagnose ---------------";

    for (const QString &diag : report.diagnose) {
        qDebug().noquote() << "-" << diag;
    }

    qDebug() << "========================================";
}

//---------------------------------------------------------------------------------------
void AudioHealthChecker::printAudioHealthReportCompact(const AudioHealthChecker::Report &report)
{
    qDebug().noquote() << "========== AUDIO HEALTH REPORT ==========";

    QString statusLine;

    if (report.verdict == "PASS") {
        statusLine = "Status : PASS | Audio path normal";
    } else if (report.verdict == "WARN") {
        statusLine = "Status : WARN | Audio path works, but needs attention";
    } else {
        statusLine = "Status : FAIL | Audio path problem detected";
    }

    qDebug().noquote() << statusLine;
    qDebug().noquote() << "";

    qDebug().noquote()
        << QString("Level  : Peak %1 dBFS | 1k %2 dBFS | Clip %3%")
              .arg(report.peakDbfs, 0, 'f', 1)
              .arg(report.level1kDbfs, 0, 'f', 1)
              .arg(report.clippingPercent, 0, 'f', 2);

    qDebug().noquote()
        << QString("Noise  : Floor %1 dBFS | SNR %2 dB")
              .arg(report.noiseFloorDbfs, 0, 'f', 1)
              .arg(report.snr1kDb, 0, 'f', 1);

    qDebug().noquote()
        << QString("Timing : Latency %1 ms | THD %2 dB")
              .arg(report.latencyMs, 0, 'f', 0)
              .arg(report.thd1kDb, 0, 'f', 1);

    qDebug().noquote() << "";
    qDebug().noquote() << "Tone Response:";

    for (const auto &tone : report.tones) {
        QString freqText;

        if (tone.frequencyHz >= 1000.0) {
            freqText = QString("%1kHz").arg(tone.frequencyHz / 1000.0, 0, 'g', 2);
        } else {
            freqText = QString("%1Hz").arg(tone.frequencyHz, 0, 'f', 0);
        }

        qDebug().noquote()
            << QString("%1  %2 dBFS  (%3%4 dB)  Dropout %5%")
                  .arg(freqText.leftJustified(6, ' '))
                  .arg(tone.levelDbfs, 6, 'f', 1)
                  .arg(tone.relativeTo1kDb >= 0 ? "+" : "")
                  .arg(tone.relativeTo1kDb, 0, 'f', 1)
                  .arg(tone.dropoutPercent, 0, 'f', 1);
    }

    qDebug().noquote() << "";
    qDebug().noquote() << "Diagnosis:";

    if (!report.diagnose.isEmpty()) {
        // Print hanya diagnosis utama supaya tidak panjang.
        qDebug().noquote() << "-" << report.diagnose.first();
    } else if (!report.notes.isEmpty()) {
        qDebug().noquote() << "-" << report.notes.first();
    } else {
        qDebug().noquote() << "- No diagnosis available.";
    }

    qDebug().noquote() << "========================================";
}

//---------------------------------------------------------------------------------------
QString AudioHealthChecker::formatReportCompact(const AudioHealthChecker::Report &report)
{
    QString out;

    auto addLine = [&](const QString &line = QString()) {
        out += line + "\n";
    };

    addLine("========== AUDIO HEALTH REPORT ==========");

    if (report.verdict == "PASS") {
        addLine("Status : PASS | Audio path normal");
    } else if (report.verdict == "WARN") {
        addLine("Status : WARN | Audio path works, but needs attention");
    } else {
        addLine("Status : FAIL | Audio path problem detected");
    }

    addLine();

    addLine(QString("Level  : Peak %1 dBFS | 1k %2 dBFS | Clip %3%")
                .arg(report.peakDbfs, 0, 'f', 1)
                .arg(report.level1kDbfs, 0, 'f', 1)
                .arg(report.clippingPercent, 0, 'f', 2));

    addLine(QString("Noise  : Floor %1 dBFS | SNR %2 dB")
                .arg(report.noiseFloorDbfs, 0, 'f', 1)
                .arg(report.snr1kDb, 0, 'f', 1));

    addLine(QString("Timing : Latency %1 ms | THD %2 dB")
                .arg(report.latencyMs, 0, 'f', 0)
                .arg(report.thd1kDb, 0, 'f', 1));

    addLine();
    addLine("Tone Response:");

    for (const auto &tone : report.tones) {
        QString freqText;

        if (tone.frequencyHz >= 1000.0) {
            freqText = QString("%1kHz")
                           .arg(tone.frequencyHz / 1000.0, 0, 'g', 2);
        } else {
            freqText = QString("%1Hz")
                           .arg(tone.frequencyHz, 0, 'f', 0);
        }

        addLine(QString("%1 %2 dBFS  (%3%4 dB)  Dropout %5%")
                    .arg(freqText.leftJustified(6, ' '))
                    .arg(tone.levelDbfs, 6, 'f', 1)
                    .arg(tone.relativeTo1kDb >= 0 ? "+" : "")
                    .arg(tone.relativeTo1kDb, 0, 'f', 1)
                    .arg(tone.dropoutPercent, 0, 'f', 1));
    }

    addLine();
    addLine("Diagnosis:");

    if (!report.diagnose.isEmpty()) {
        addLine("- " + report.diagnose.first());
    } else if (!report.notes.isEmpty()) {
        addLine("- " + report.notes.first());
    } else {
        addLine("- No diagnosis available.");
    }

    addLine("========================================");

    return out;
}
