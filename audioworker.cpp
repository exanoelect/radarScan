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
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);

    soundCardIdx = getKtUsbAudioDevice();
    connect(m_timer, &QTimer::timeout, this, [this]() {
        qDebug() << "Sound playback timer finished";

        m_isPlaying = false;
        playNext();
    });
}

//---------------------------------------------------------------------------------------
void AudioWorker::enqueueSound(int sentenceIndex, QString langIndex)
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
QString AudioWorker::requestToFile(int sentenceIndex, QString langIndex)
{
    QString langFolder;

    if(langIndex == "sv") langFolder = "sv";
    if(langIndex == "id") langFolder = "id";
    if(langIndex == "en") langFolder = "en";

    switch (sentenceIndex) {
    case SOUND_FALL_OCCUR:
#ifdef Q_OS_LINUX
        return QString("/home/pi/wav/%1/fall.wav").arg(langFolder);
#else
#endif
    case SOUND_HELP:
#ifdef Q_OS_LINUX
        return QString("/home/pi/wav/%1/help.wav").arg(langFolder);
#else
#endif
    case SOUND_IAM_OK:
#ifdef Q_OS_LINUX

        return QString("/home/pi/wav/%1/iam_ok.wav").arg(langFolder);
#else
#endif
    case SOUND_RECORD:
#ifdef Q_OS_LINUX
        return QString("/home/pi/wav/%1/record.wav").arg(langFolder);
#else
#endif
    case SOUND_WAITING:
#ifdef Q_OS_LINUX
        return QString("/home/pi/wav/%1/wait.wav").arg(langFolder);
#else
#endif

    case SOUND_HELPYOU:
#ifdef Q_OS_LINUX
        return QString("/home/pi/wav/%1/helpyou.wav").arg(langFolder);
#else
#endif
    case SOUND_LOGIN:
#ifdef Q_OS_LINUX
        return QString("/home/pi/wav/%1/login.mp3").arg(langFolder);
#else
#endif
    default:
        return QString();
    }
}

//---------------------------------------------------------------------------------------
QString AudioWorker::getKtUsbAudioDevice()
{
    QProcess process;
    process.start("aplay", QStringList() << "-l");

    if (!process.waitForFinished(3000)) {
        qDebug() << "aplay -l timeout";
        return "";
    }

    QString output = process.readAllStandardOutput();
    QString error  = process.readAllStandardError();

    if (!error.isEmpty()) {
        qDebug() << "aplay error:" << error;
    }

    qDebug() << "aplay -l output:" << output;

    /*
     * Contoh line:
     * card 2: Device [KT USB Audio], device 0: USB Audio [USB Audio]
     *
     * Ambil:
     * card = 2
     * device = 0
     */

    QRegularExpression re(
        R"(card\s+(\d+):.*\[KT USB Audio\].*device\s+(\d+):)",
        QRegularExpression::CaseInsensitiveOption
        );

    QRegularExpressionMatch match = re.match(output);

    if (match.hasMatch()) {
        QString cardIndex   = match.captured(1);
        QString deviceIndex = match.captured(2);

        QString hwDevice = QString("hw:%1,%2").arg(cardIndex, deviceIndex);

        qDebug() << "KT USB Audio found:" << hwDevice;

        return hwDevice;
    }

    qDebug() << "KT USB Audio not found";
    return "";
}

//---------------------------------------------------------------------------------------
void AudioWorker::playWavFile(const QString &wavPath)
{
    QTimer::singleShot(1000, this, [this, wavPath]() {

        if (wavPath.isEmpty()) {
            qDebug() << "WAV path is empty";
            return;
        }

        /*if (soundCardIdx.isEmpty()) {
            qDebug() << "KT USB Audio not found, using default audio";

            QProcess::startDetached(
                "paplay" //"aplay",
                QStringList()
                    << wavPath
                );

            return;
        }*/

        QProcess::startDetached(
            "paplay",
            QStringList()
                //<< "-D"
                //<< soundCardIdx
                << wavPath
            );
    });
}
//---------------------------------------------------------------------------------------
void AudioWorker::playNext()
{
    if (m_queue.isEmpty()) {
        m_isPlaying = false;

        m_currentSound.sentenceIndex = -1;
        m_currentSound.langIndex = 0;

        return;
    }

    m_isPlaying = true;

    m_currentSound = m_queue.dequeue();

    QString filePath = requestToFile(
        m_currentSound.sentenceIndex,
        m_currentSound.langIndex
        );

    if (filePath.isEmpty()) {
        qDebug() << "Sound file path is empty for"
                 << "sentenceIndex:" << m_currentSound.sentenceIndex
                 << "langIndex:" << m_currentSound.langIndex;

        m_isPlaying = false;
        playNext();
        return;
    }

    qDebug() << "Playing:"
             << filePath
             << "sentenceIndex:" << m_currentSound.sentenceIndex
             << "langIndex:" << m_currentSound.langIndex;

    playWavFile(filePath);

    // Timer durasi playback manual
    m_timer->start(4000);
}

//---------------------------------------------------------------------------------------
void AudioWorker::onPlaybackTimeout()
{
    SoundQueueItem finishedSound = m_currentSound;

    qDebug() << "Finished playing:"
             << "sentenceIndex:" << finishedSound.sentenceIndex
             << "langIndex:" << finishedSound.langIndex;

    m_isPlaying = false;

    emit finishedPlaying(
        finishedSound.sentenceIndex,
        finishedSound.langIndex
        );

    playNext();
}
