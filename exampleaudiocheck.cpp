AudioHealthChecker checker;

QString err;

const QString testWav = "/tmp/audio_health_test.wav";
const QString recWav  = "/tmp/audio_health_recorded.wav";

if (!checker.createTestWav(testWav, &err)) {
    qWarning() << "Create test WAV failed:" << err;
    return;
}

// Isi target sesuai device PipeWire kamu.
// Bisa juga dikosongkan kalau mau default device.
QString inputTarget  = "alsa_input.usb-SEEED_ReSpeaker_4_Mic_Array__UAC1.0_-00.pro-input-0";
QString outputTarget = ""; // contoh: default speaker

if (!checker.runPipeWireTest(testWav, recWav, inputTarget, outputTarget, &err)) {
    qWarning() << "Audio test failed:" << err;
    return;
}

AudioHealthChecker::Report report = checker.analyzeRecording(recWav, &err);

qDebug() << "Verdict:" << report.verdict;
qDebug() << "Peak dBFS:" << report.peakDbfs;
qDebug() << "Clipping %:" << report.clippingPercent;
qDebug() << "Noise floor dBFS:" << report.noiseFloorDbfs;
qDebug() << "1k level dBFS:" << report.level1kDbfs;
qDebug() << "SNR 1k dB:" << report.snr1kDb;
qDebug() << "Latency ms:" << report.latencyMs;
qDebug() << "THD 1k dB:" << report.thd1kDb;

for (const auto &tone : report.tones) {
    qDebug() << "Tone"
             << tone.frequencyHz << "Hz"
             << "level:" << tone.levelDbfs
             << "relative:" << tone.relativeTo1kDb
             << "dropout:" << tone.dropoutPercent << "%";
}

for (const QString &note : report.notes) {
    qDebug() << "Note:" << note;
}