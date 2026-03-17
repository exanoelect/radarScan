#include "audioworker.h"
#include <QUrl>
#include <QDebug>

AudioWorker::AudioWorker(QObject *parent)
    : QObject(parent)
{
}

AudioWorker::~AudioWorker()
{
}

//---------------------------------------------------------------------------------------
void AudioWorker::init()
{
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout,
            this, &AudioWorker::onPlaybackTimeout);
}

//---------------------------------------------------------------------------------------
void AudioWorker::enqueueSound(int requestId)
{
    qDebug() << "enqueueSound received:" << requestId;
    m_queue.enqueue(requestId);

    if (m_player->playbackState() != QMediaPlayer::PlayingState) {
        playNext();
    }
}

//---------------------------------------------------------------------------------------
QString AudioWorker::requestToFile(int requestId)
{
    switch (requestId) {
    case SOUND_FALL_OCCUR:
#ifdef PLATFORM_LINUX
        return "/home/pi/wav/alarm.wav";
#else
        return "/Volumes/DATA/wav/alarm.wav";
#endif
    case SOUND_HELP:
#ifdef PLATFORM_LINUX
        return "/home/pi/wav/help.wav";
#else
        return "/Volumes/DATA/wav/help.wav";
#endif
    case SOUND_IAM_OK:
#ifdef PLATFORM_LINUX
        return "/home/pi/wav/i-am-ok.wav";
#else
        return "/Volumes/DATA/wav/i-am-ok.wav";
#endif
    default:
        return QString();
    }
}

//---------------------------------------------------------------------------------------
void AudioWorker::playNext()
{
    if (m_queue.isEmpty())
        return;

    m_currentRequestId = m_queue.dequeue();
    QString filePath = requestToFile(m_currentRequestId);
    if (filePath.isEmpty())
        return;

    qDebug() << "Playing:" << filePath;
    m_player->setSource(QUrl::fromLocalFile(filePath));
    m_player->play();

    // Mulai timer durasi playback
    m_timer->start(4000);
}

//---------------------------------------------------------------------------------------
void AudioWorker::onPlaybackTimeout()
{
    emit finishedPlaying(m_currentRequestId);  // <-- lebih logis di sini
    playNext();
}
