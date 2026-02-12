#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H

#include <QObject>
#include <QProcess>
#include <qdebug.h>

class brightness : public QObject
{
    Q_OBJECT
public:
    explicit brightness(QObject *parent = nullptr);

signals:
public slots:
    int getBrightness();
    bool setBrightnessPercent(int percent);
    bool setBrightness(int value);
};

#endif // BRIGHTNESS_H
