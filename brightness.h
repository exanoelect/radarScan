#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H

#include <QObject>
#include <QProcess>
#include <qdebug.h>
#include <QFile>
#include <QDir>

class brightness : public QObject
{
    Q_OBJECT
public:
    explicit brightness(QObject *parent = nullptr);
    ~brightness();

signals:
public slots:
    int getBrightness();
    int getBrightnessPercent();
    bool setBrightnessPercent(int percent);
    bool setBrightness(int value);

    QString getBacklightBasePath();

};

#endif // BRIGHTNESS_H
