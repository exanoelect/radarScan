#include "microphonecontrol.h"

#include <QRegularExpression>

namespace {
constexpr auto WPCTL_PROGRAM = "/usr/bin/wpctl";
constexpr auto DEFAULT_SOURCE = "@DEFAULT_AUDIO_SOURCE@";
}

MicrophoneControl::MicrophoneControl(QObject *parent)
    : QObject(parent),
      m_process(new QProcess(this))
{
    connect(m_process,
            &QProcess::finished,
            this,
            &MicrophoneControl::onProcessFinished);

    connect(m_process,
            &QProcess::errorOccurred,
            this,
            &MicrophoneControl::onProcessError);
}

MicrophoneControl::~MicrophoneControl()
{
    if (m_process->state() != QProcess::NotRunning) {
        m_process->kill();
    }
}

bool MicrophoneControl::isBusy() const
{
    return m_busy;
}

void MicrophoneControl::mute()
{
    enqueueCommand({
        QStringLiteral("set-mute"),
        QString::fromLatin1(DEFAULT_SOURCE),
        QStringLiteral("1")
    }, CommandType::Mute);
}

void MicrophoneControl::unmute()
{
    enqueueCommand({
        QStringLiteral("set-mute"),
        QString::fromLatin1(DEFAULT_SOURCE),
        QStringLiteral("0")
    }, CommandType::Unmute);
}

void MicrophoneControl::toggleMute()
{
    enqueueCommand({
        QStringLiteral("set-mute"),
        QString::fromLatin1(DEFAULT_SOURCE),
        QStringLiteral("toggle")
    }, CommandType::ToggleMute);
}

void MicrophoneControl::getStatus()
{
    enqueueCommand({
        QStringLiteral("get-volume"),
        QString::fromLatin1(DEFAULT_SOURCE)
    }, CommandType::GetStatus);
}

void MicrophoneControl::enqueueCommand(const QStringList &arguments,
                                       CommandType type)
{
    m_queue.enqueue({arguments, type});
    startNextCommand();
}

void MicrophoneControl::startNextCommand()
{
    if (m_process->state() != QProcess::NotRunning || m_queue.isEmpty()) {
        return;
    }

    const PendingCommand command = m_queue.dequeue();

    m_currentType = command.type;
    m_currentCommandText = QStringLiteral("%1 %2")
                               .arg(QString::fromLatin1(WPCTL_PROGRAM),
                                    command.arguments.join(QLatin1Char(' ')));
    m_hasCurrentCommand = true;

    m_process->setProgram(QString::fromLatin1(WPCTL_PROGRAM));
    m_process->setArguments(command.arguments);

    setBusy(true);
    m_process->start();
}

void MicrophoneControl::onProcessFinished(int exitCode,
                                          QProcess::ExitStatus exitStatus)
{
    const QString output =
        QString::fromUtf8(m_process->readAllStandardOutput()).trimmed();

    const QString errorOutput =
        QString::fromUtf8(m_process->readAllStandardError()).trimmed();

    const CommandType completedType = m_currentType;
    const QString completedCommand = m_currentCommandText;

    resetCurrentCommand();

    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        const QString errorMessage = errorOutput.isEmpty()
            ? QStringLiteral("wpctl gagal dengan exit code %1").arg(exitCode)
            : errorOutput;

        emit commandFailed(completedCommand, errorMessage);
        startNextCommand();
        return;
    }

    emit commandCompleted(completedCommand, output);

    switch (completedType) {
    case CommandType::Mute:
        emit muteChanged(true);
        break;

    case CommandType::Unmute:
        emit muteChanged(false);
        break;

    case CommandType::ToggleMute:
        // Ambil status aktual setelah toggle selesai.
        m_queue.prepend({
            {
                QStringLiteral("get-volume"),
                QString::fromLatin1(DEFAULT_SOURCE)
            },
            CommandType::GetStatus
        });
        break;

    case CommandType::GetStatus:
        parseStatusOutput(output);
        break;
    }

    startNextCommand();
}

void MicrophoneControl::onProcessError(QProcess::ProcessError error)
{
    Q_UNUSED(error)

    const QString failedCommand = m_currentCommandText;
    const QString errorMessage = m_process->errorString();

    resetCurrentCommand();

    emit commandFailed(failedCommand, errorMessage);
    startNextCommand();
}

void MicrophoneControl::parseStatusOutput(const QString &output)
{
    // Contoh:
    // Volume: 1.00
    // Volume: 1.00 [MUTED]
    static const QRegularExpression volumeRegex(
        QStringLiteral(R"(Volume:\s*([0-9]+(?:\.[0-9]+)?))"),
        QRegularExpression::CaseInsensitiveOption);

    const QRegularExpressionMatch match = volumeRegex.match(output);

    if (!match.hasMatch()) {
        emit commandFailed(
            QStringLiteral("wpctl get-volume @DEFAULT_AUDIO_SOURCE@"),
            QStringLiteral("Format output wpctl tidak dikenali: %1").arg(output));
        return;
    }

    bool ok = false;
    const double volume = match.captured(1).toDouble(&ok);

    if (!ok) {
        emit commandFailed(
            QStringLiteral("wpctl get-volume @DEFAULT_AUDIO_SOURCE@"),
            QStringLiteral("Nilai volume tidak valid: %1")
                .arg(match.captured(1)));
        return;
    }

    const bool muted = output.contains(QStringLiteral("[MUTED]"),
                                       Qt::CaseInsensitive);

    emit statusReceived(volume, muted);
    emit muteChanged(muted);
}

void MicrophoneControl::setBusy(bool busy)
{
    if (m_busy == busy) {
        return;
    }

    m_busy = busy;
    emit busyChanged(m_busy);
}

void MicrophoneControl::resetCurrentCommand()
{
    m_hasCurrentCommand = false;
    m_currentCommandText.clear();

    if (m_queue.isEmpty()) {
        setBusy(false);
    }
}
