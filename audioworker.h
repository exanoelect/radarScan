#ifndef AUDIOWORKER_H
#define AUDIOWORKER_H

#include <QObject>
#include <QQueue>
#include <QProcess>

#define SOUND_FALL_OCCUR 1
#define SOUND_HELP       2
#define SOUND_IAM_OK     3
#define SOUND_RECORD     4
#define SOUND_WAITING    5
#define SOUND_HELPYOU    6
#define SOUND_LOGIN      7
#define SOUND_UPLOAD_FAILED 8

struct SoundQueueItem
{
    int sentenceIndex = -1;
    QString langIndex = "sv";
};

class AudioWorker : public QObject
{
    Q_OBJECT

public:
    explicit AudioWorker(QObject *parent = nullptr);
    ~AudioWorker();

public slots:
    void init();
    void enqueueSound(int sentenceIndex, QString langIndex);

signals:
    // Hanya di-emit jika paplay selesai dengan normal.
    void finishedPlaying(int sentenceIndex, QString langIndex);

    // Opsional, agar kegagalan playback juga dapat diketahui.
    void playbackFailed(
        int sentenceIndex,
        QString langIndex,
        QString errorMessage
        );

private slots:
    void playNext();

    void onPlaybackFinished(
        int exitCode,
        QProcess::ExitStatus exitStatus
        );

    void onPlaybackError(QProcess::ProcessError error);

private:
    QString requestToFile(
        int sentenceIndex,
        const QString &langIndex
        ) const;

    void playSoundFile(const QString &soundPath);
    void resetCurrentSound();

    QProcess *m_playerProcess = nullptr;

    QQueue<SoundQueueItem> m_queue;
    SoundQueueItem m_currentSound;

    bool m_isPlaying = false;
};

#endif // AUDIOWORKER_H