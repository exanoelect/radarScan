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

    connect(m_player, &QMediaPlayer::mediaStatusChanged,
            this, [this](QMediaPlayer::MediaStatus status) {
                if (status == QMediaPlayer::EndOfMedia) {
                    playNext();
                }
            });
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

    int requestId = m_queue.dequeue();
    QString filePath = requestToFile(requestId);
    if (filePath.isEmpty())
        return;

    qDebug() << "Playing:" << filePath;
    m_player->setSource(QUrl::fromLocalFile(filePath));
    m_player->play();

    emit finishedPlaying(requestId);
}
