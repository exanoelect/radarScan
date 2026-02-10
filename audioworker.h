#ifndef AUDIOWORKER_H
#define AUDIOWORKER_H

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
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
    void enqueueSound(int requestId);

signals:
    void finishedPlaying(int requestId);

private slots:
    void playNext();

private:
    QString requestToFile(int requestId);

    QQueue<int> m_queue;
    QMediaPlayer *m_player;
    QAudioOutput *m_audioOutput;
};


#endif // AUDIOWORKER_H
