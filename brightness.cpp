#include "brightness.h"
#include <QtCore/qdebug.h>

brightness::brightness(QObject *parent)
    : QObject{parent}
{}

//------------------------------------------------------------------------
int brightness::getBrightness()
{
    QProcess proc;
    proc.start("brightnessctl", {"get"});
    proc.waitForFinished();

    QString output = proc.readAllStandardOutput().trimmed();
    bool ok = false;
    int value = output.toInt(&ok);

    if (!ok) {
        qDebug() << "Failed to parse brightness:" << output;
        return -1;
    }
    return value;
}

//------------------------------------------------------------------------
bool brightness::setBrightnessPercent(int percent)
{
    if((percent < 0 ) || (percent > 100)) return false;

    QProcess proc;
    proc.start("brightnessctl", {"set", QString::number(percent) + "%"});
    proc.waitForFinished();

    return proc.exitStatus() == QProcess::NormalExit &&
           proc.exitCode() == 0;
}

//------------------------------------------------------------------------
bool brightness::setBrightness(int value)
{
    if((value < 0 ) || (value > 255)) return false;

    QProcess proc;
    proc.start("brightnessctl", {"set", QString::number(value)});
    proc.waitForFinished();

    return proc.exitStatus() == QProcess::NormalExit &&
           proc.exitCode() == 0;
}
