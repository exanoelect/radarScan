#include "audioworker.h"

#include <QDebug>
#include <QFileInfo>

//---------------------------------------------------------------------------------------
AudioWorker::AudioWorker(QObject *parent)
    : QObject(parent)
{
}

//---------------------------------------------------------------------------------------
AudioWorker::~AudioWorker()
{
    if (m_playerProcess &&
        m_playerProcess->state() != QProcess::NotRunning) {

        m_playerProcess->kill();
    }
}

//---------------------------------------------------------------------------------------
void AudioWorker::init()
{
    if (m_playerProcess) {
        qWarning() << "AudioWorker has already been initialized";
        return;
    }

    m_playerProcess = new QProcess(this);

    connect(m_playerProcess,&QProcess::finished, this,&AudioWorker::onPlaybackFinished);
    connect(m_playerProcess,&QProcess::errorOccurred, this,&AudioWorker::onPlaybackError);

    qDebug() << "AudioWorker initialized";
}

//---------------------------------------------------------------------------------------
void AudioWorker::enqueueSound(int sentenceIndex,QString langIndex)
{
    qDebug() << "enqueueSound received:"
             << "sentenceIndex:" << sentenceIndex
             << "langIndex:" << langIndex;

    SoundQueueItem item;
    item.sentenceIndex = sentenceIndex;
    item.langIndex = langIndex;

    m_queue.enqueue(item);

    if (!m_isPlaying) {
        playNext();
    }
}

//---------------------------------------------------------------------------------------
QString AudioWorker::requestToFile(int sentenceIndex,const QString &langIndex) const
{
    QString langFolder;

    if (langIndex == "sv") {
        langFolder = "sv";
    } else if (langIndex == "id") {
        langFolder = "id";
    } else if (langIndex == "en") {
        langFolder = "en";
    } else {
        qWarning() << "Unsupported language:" << langIndex;
        return QString();
    }

#ifdef Q_OS_LINUX
    const QString basePath =
        QStringLiteral("/home/pi/wav/%1").arg(langFolder);

    switch (sentenceIndex) {
    case SOUND_FALL_OCCUR:
        return basePath + QStringLiteral("/fall.wav");

    case SOUND_HELP:
        return basePath + QStringLiteral("/help.wav");

    case SOUND_IAM_OK:
        return basePath + QStringLiteral("/iam_ok.wav");

    case SOUND_RECORD:
        return basePath + QStringLiteral("/record.wav");

    case SOUND_WAITING:
        return basePath + QStringLiteral("/wait.wav");

    case SOUND_HELPYOU:
        return basePath + QStringLiteral("/helpyou.wav");

    case SOUND_LOGIN:
        return basePath + QStringLiteral("/login.mp3");

    case SOUND_UPLOAD_FAILED:
        return basePath + QStringLiteral("/fail.wav");

    default:
        qWarning() << "Unknown sentence index:" << sentenceIndex;
        return QString();
    }
#else
    Q_UNUSED(sentenceIndex);
    return QString();
#endif

#ifdef Q_OS_MACOS
#endif
}

//---------------------------------------------------------------------------------------
void AudioWorker::playSoundFile(const QString &soundPath)
{
    if (!m_playerProcess) {
        qWarning() << "AudioWorker has not been initialized";

        emit playbackFailed(
            m_currentSound.sentenceIndex,
            m_currentSound.langIndex,
            QStringLiteral("AudioWorker belum diinisialisasi")
            );

        m_isPlaying = false;
        resetCurrentSound();
        playNext();
        return;
    }

    if (soundPath.isEmpty()) {
        qWarning() << "Sound path is empty";

        emit playbackFailed(
            m_currentSound.sentenceIndex,
            m_currentSound.langIndex,
            QStringLiteral("Path file audio kosong")
            );

        m_isPlaying = false;
        resetCurrentSound();
        playNext();
        return;
    }

    QFileInfo fileInfo(soundPath);

    if (!fileInfo.exists() || !fileInfo.isFile()) {
        const QString errorMessage =
            QStringLiteral("File audio tidak ditemukan: %1")
                .arg(soundPath);

        qWarning() << errorMessage;

        emit playbackFailed(
            m_currentSound.sentenceIndex,
            m_currentSound.langIndex,
            errorMessage
            );

        m_isPlaying = false;
        resetCurrentSound();
        playNext();
        return;
    }

    qDebug() << "Starting paplay:" << soundPath;

    // Jangan memakai startDetached karena event finished
    // tidak bisa dipantau oleh QProcess ini.
    m_playerProcess->setProgram(QStringLiteral("paplay"));
    m_playerProcess->setArguments(
        QStringList() << soundPath
        );

    m_playerProcess->start();
}

//---------------------------------------------------------------------------------------
void AudioWorker::playNext()
{
    if (m_isPlaying) {
        return;
    }

    if (m_queue.isEmpty()) {
        resetCurrentSound();
        return;
    }

    m_currentSound = m_queue.dequeue();
    m_isPlaying = true;

    const QString filePath = requestToFile(
        m_currentSound.sentenceIndex,
        m_currentSound.langIndex
        );

    if (filePath.isEmpty()) {
        qWarning() << "Sound file path is empty for"
                   << "sentenceIndex:"
                   << m_currentSound.sentenceIndex
                   << "langIndex:"
                   << m_currentSound.langIndex;

        emit playbackFailed(
            m_currentSound.sentenceIndex,
            m_currentSound.langIndex,
            QStringLiteral("File audio tidak terdaftar")
            );

        m_isPlaying = false;
        resetCurrentSound();
        playNext();
        return;
    }

    qDebug() << "Playing:"
             << filePath
             << "sentenceIndex:"
             << m_currentSound.sentenceIndex
             << "langIndex:"
             << m_currentSound.langIndex;

    playSoundFile(filePath);
}

//---------------------------------------------------------------------------------------
void AudioWorker::onPlaybackFinished(int exitCode,QProcess::ExitStatus exitStatus)
{
    if (!m_isPlaying) {
        return;
    }

    const SoundQueueItem finishedSound = m_currentSound;

    const bool success = exitStatus == QProcess::NormalExit && exitCode == 0;

    if (success) {
        qDebug() << "Sound playback finished:"
                 << "sentenceIndex:"
                 << finishedSound.sentenceIndex
                 << "langIndex:"
                 << finishedSound.langIndex;

        emit finishedPlaying(
            finishedSound.sentenceIndex,
            finishedSound.langIndex
            );
    } else {
        const QString errorMessage =
            QStringLiteral(
                "paplay berhenti dengan exitCode %1, exitStatus %2"
                )
                .arg(exitCode)
                .arg(static_cast<int>(exitStatus));

        qWarning() << errorMessage;

        emit playbackFailed(
            finishedSound.sentenceIndex,
            finishedSound.langIndex,
            errorMessage
            );
    }

    m_isPlaying = false;
    resetCurrentSound();

    // Setelah audio benar-benar selesai,
    // lanjutkan file berikutnya di dalam queue.
    playNext();
}

//---------------------------------------------------------------------------------------
void AudioWorker::onPlaybackError(QProcess::ProcessError error)
{
    qWarning() << "paplay process error:"
               << error
               << m_playerProcess->errorString();

    /*
     * CrashError biasanya tetap diikuti signal finished().
     * FailedToStart tidak selalu menghasilkan finished(),
     * sehingga harus diselesaikan di sini.
     */
    if (error != QProcess::FailedToStart || !m_isPlaying) {
        return;
    }

    const SoundQueueItem failedSound = m_currentSound;
    const QString errorMessage = m_playerProcess->errorString();

    emit playbackFailed(
        failedSound.sentenceIndex,
        failedSound.langIndex,
        errorMessage
        );

    m_isPlaying = false;
    resetCurrentSound();
    playNext();
}

//---------------------------------------------------------------------------------------
void AudioWorker::resetCurrentSound()
{
    m_currentSound.sentenceIndex = -1;
    m_currentSound.langIndex.clear();
}