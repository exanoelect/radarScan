#ifndef AUDIOWORKER_H
#define AUDIOWORKER_H

#include <QObject>
#include <QQueue>
#include <QMediaPlayer>
#include <QAudioOutput>

#define SOUND_FALL_OCCUR 1
#define SOUND_HELP       2
#define SOUND_IAM_OK     3

class AudioWorker : public QObject
{
    Q_OBJECT
public:
    explicit AudioWorker(QObject *parent = nullptr);
    ~AudioWorker();

public slots:
    void init();               // <-- init di thread sendiri
    void enqueueSound(int requestId);

signals:
    void finishedPlaying(int requestId);

private slots:
    void playNext();

private:
    QString requestToFile(int requestId);

    QQueue<int> m_queue;
    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;
};

#endif // AUDIOWORKER_H
