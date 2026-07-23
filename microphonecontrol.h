#ifndef MICROPHONECONTROL_H
#define MICROPHONECONTROL_H

#include <QObject>
#include <QProcess>
#include <QQueue>
#include <QString>
#include <QStringList>

class MicrophoneControl : public QObject
{
    Q_OBJECT

public:
    explicit MicrophoneControl(QObject *parent = nullptr);
    ~MicrophoneControl() override;

    bool isBusy() const;

public slots:
    void mute();
    void unmute();
    void toggleMute();
    void getStatus();

signals:
    void muteChanged(bool muted);
    void statusReceived(double volume, bool muted);
    void commandCompleted(const QString &command, const QString &output);
    void commandFailed(const QString &command, const QString &error);
    void busyChanged(bool busy);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    enum class CommandType
    {
        Mute,
        Unmute,
        ToggleMute,
        GetStatus
    };

    struct PendingCommand
    {
        QStringList arguments;
        CommandType type;
    };

    void enqueueCommand(const QStringList &arguments, CommandType type);
    void startNextCommand();
    void parseStatusOutput(const QString &output);
    void setBusy(bool busy);
    void resetCurrentCommand();

    QProcess *m_process = nullptr;
    QQueue<PendingCommand> m_queue;

    CommandType m_currentType = CommandType::GetStatus;
    QString m_currentCommandText;
    bool m_hasCurrentCommand = false;
    bool m_busy = false;
};

#endif // MICROPHONECONTROL_H
