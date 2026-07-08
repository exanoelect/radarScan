QString err;
QString testFile = "/tmp/audio_health_test.wav";

if (!AudioTestWavGenerator::generate(testFile, &err)) {
    qWarning() << "Generate audio test failed:" << err;
    return;
}

qDebug() << "Audio test WAV generated:" << testFile;