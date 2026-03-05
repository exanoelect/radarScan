#include "volume.h"
#include <QtCore/qprocess.h>
#include <QMetaObject>

namespace {
struct SetVolumeData {
    volume *self;
    int percent;
};
}

volume::volume(QObject *parent)
    : QObject{parent}
{
    //initPulse();
}

volume::~volume(){

}

//---------------------------------------------------------------------------------------
int volume::getVolumePercent()
{
    QProcess proc;
    proc.start("bash", {"-c", "pactl get-sink-volume @DEFAULT_SINK@"});
    proc.waitForFinished();

    QString output = proc.readAllStandardOutput();
    QString error  = proc.readAllStandardError();
    QString all    = output + error;

    qDebug() << "pactl output:" << all;

    QRegularExpression re("(\\d+)%");
    QRegularExpressionMatch match = re.match(all);

    if (!match.hasMatch()) {
        qWarning() << "Failed to parse volume:" << all;
        return -1;
    }

    return match.captured(1).toInt();
}

//---------------------------------------------------------------------------------------
bool volume::setVolumePercent(int percent)
{
    QProcess proc;
    proc.start("pactl", {"set-sink-volume", "@DEFAULT_SINK@", QString::number(percent) + "%"});
    proc.waitForFinished();

    return proc.exitStatus() == QProcess::NormalExit &&
           proc.exitCode() == 0;
}

//---------------------------------------------------------------------------------------
bool volume::mute(bool enable)
{
    return false;
}


