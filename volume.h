#ifndef VOLUME_H
#define VOLUME_H

#include <QObject>
#include <qdebug.h>
#include <QProcess>
#include <QRegularExpression>

class volume : public QObject
{
    Q_OBJECT
public:
    explicit volume(QObject *parent = nullptr);

signals:
public slots:
    //Volume control
    int getVolumePercent();                 // return 0–100, -1 jika gagal
    bool setVolumePercent(int percent);     // set 0–100%
    bool mute(bool enable);                 // true = mute, false = unmute
};

#endif // VOLUME_H
