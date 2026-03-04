#include "brightness.h"
#include <QtCore/qdebug.h>

brightness::brightness(QObject *parent)
    : QObject{parent}
{}

brightness::~brightness(){

}
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
int brightness::getBrightnessPercent()
{
    QString base = getBacklightBasePath();
     if (base.isEmpty()) return -1;

     QFile maxFile(base + "/max_brightness");
     QFile valFile(base + "/brightness");

     if (!maxFile.open(QIODevice::ReadOnly) ||
         !valFile.open(QIODevice::ReadOnly))
         return -1;

     int maxVal = QString(maxFile.readAll()).trimmed().toInt();
     int curVal = QString(valFile.readAll()).trimmed().toInt();

     if (maxVal <= 0) return -1;

     return (curVal * 100) / maxVal;

}

//------------------------------------------------------------------------
bool brightness::setBrightnessPercent(int percent)
{
    if ((percent < 0) || (percent > 100)){
        return false;
}
    QString base = getBacklightBasePath();
    if (base.isEmpty())
        return false;

    QFile maxFile(base + "/max_brightness");
    if (!maxFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed open max_brightness:" << maxFile.errorString();
        return false;
    }

    bool ok = false;
    int maxVal = QString(maxFile.readAll()).trimmed().toInt(&ok);
    maxFile.close();

    if (!ok || maxVal <= 0)
        return false;

    int newVal = (percent * maxVal) / 100;

    QFile valFile(base + "/brightness");
    if (!valFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed open brightness:" << valFile.errorString();
        return false;
    }

    valFile.write(QString::number(newVal).toUtf8());
    valFile.close();

    return true;
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

//------------------------------------------------------------------------
QString brightness::getBacklightBasePath()
{
    QDir dir("/sys/class/backlight");
    QStringList list = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    if (list.isEmpty())
        return "";

    return "/sys/class/backlight/" + list.first();

}
