#ifndef VOLUMEMONITOR_H
#define VOLUMEMONITOR_H


#pragma once

#include <QObject>
#include <pulse/pulseaudio.h>
#include <qdebug.h>


class VolumeMonitor : public QObject
{
    Q_OBJECT

public:
    explicit VolumeMonitor(QObject *parent = nullptr);
    ~VolumeMonitor();

    void refresh();                 // manual refresh
    void setVolumePercent(int percent);

signals:
    void volumeChanged(int percent);
    void connected();
    void disconnected();

private:
    void initPulse();
    void cleanup();

    // internal (NO LOCK) – hanya dipanggil dari worker thread
    void requestSinkInfoInternal();

    // libpulse callbacks
    static void contextStateCallback(pa_context *c, void *userdata);
    static void subscribeCallback(pa_context *c,
                                   pa_subscription_event_type_t type,
                                   uint32_t index,
                                   void *userdata);
    static void sinkInfoCallback(pa_context *c,
                                 const pa_sink_info *info,
                                 int eol,
                                 void *userdata);

private:
    pa_threaded_mainloop *m_mainloop = nullptr;
    pa_context *m_context = nullptr;
};

#endif // VOLUMEMONITOR_H
