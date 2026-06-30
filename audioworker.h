#ifndef AUDIOWORKER_H
#define AUDIOWORKER_H

#include <QObject>
#include <QQueue>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QTimer>
#include <QProcess>
#include <QRegularExpression>

#define SOUND_FALL_OCCUR 1
#define SOUND_HELP       2
#define SOUND_IAM_OK     3
#define SOUND_RECORD     4

struct SoundQueueItem
{
    int sentenceIndex = -1;
    int langIndex = 0;
};

class AudioWorker : public QObject
{
    Q_OBJECT
public:
    explicit AudioWorker(QObject *parent = nullptr);
    ~AudioWorker();

public slots:
    void init();
    void enqueueSound(int sentenceIndex, int langIndex);

signals:
    void finishedPlaying(int sentenceIndex, int langIndex);

private slots:
    void playNext();
    void onPlaybackTimeout();

private:
    QString requestToFile(int sentenceIndex, int langIndex);
    QString getKtUsbAudioDevice();
    void playWavFile(const QString &wavPath);

    //QQueue<int> m_queue;
    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
    QTimer *m_timer = nullptr;

    //int m_currentRequestId = -1;   // <-- TARUH DI SINI

    QString soundCardIdx;

    QQueue<SoundQueueItem> m_queue;

    SoundQueueItem m_currentSound;   // pengganti m_currentRequestId
    bool m_isPlaying = false;
};

#endif // AUDIOWORKER_H
