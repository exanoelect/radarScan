#include "VolumeMonitor.h"
#include <QMetaObject>

struct SetVolumeData {
    VolumeMonitor *self;
    int percent;
};

VolumeMonitor::VolumeMonitor(QObject *parent)
    : QObject(parent)
{
    initPulse();
}

//------------------------------------------------------------------------
VolumeMonitor::~VolumeMonitor()
{
    cleanup();
}

//------------------------------------------------------------------------
void VolumeMonitor::initPulse()
{
    m_mainloop = pa_threaded_mainloop_new();
    pa_mainloop_api *api = pa_threaded_mainloop_get_api(m_mainloop);

    m_context = pa_context_new(api, "QtVolumeMonitor");

    pa_context_set_state_callback(m_context,
                                  contextStateCallback,
                                  this);

    pa_threaded_mainloop_start(m_mainloop);

    pa_context_connect(m_context,
                       nullptr,
                       PA_CONTEXT_NOFLAGS,
                       nullptr);
}

//------------------------------------------------------------------------
void VolumeMonitor::cleanup()
{
    if (!m_mainloop)
        return;

    pa_threaded_mainloop_lock(m_mainloop);

    if (m_context) {
        pa_context_disconnect(m_context);
        pa_context_unref(m_context);
        m_context = nullptr;
    }

    pa_threaded_mainloop_unlock(m_mainloop);

    pa_threaded_mainloop_stop(m_mainloop);
    pa_threaded_mainloop_free(m_mainloop);
    m_mainloop = nullptr;
}

//------------------------------------------------------------------------
////////////////////////////////////////////////////////////
///////////////////// CALLBACK SECTION /////////////////////
////////////////////////////////////////////////////////////

void VolumeMonitor::contextStateCallback(pa_context *c,
                                         void *userdata)
{
    auto *self = static_cast<VolumeMonitor *>(userdata);

    switch (pa_context_get_state(c)) {

    case PA_CONTEXT_READY:

        pa_context_set_subscribe_callback(c,
                                          subscribeCallback,
                                          self);

        pa_context_subscribe(c,
                             PA_SUBSCRIPTION_MASK_SINK,
                             nullptr,
                             nullptr);

        self->requestSinkInfoInternal();

        QMetaObject::invokeMethod(self,
                                  &VolumeMonitor::connected,
                                  Qt::QueuedConnection);
        break;

    case PA_CONTEXT_FAILED:
    case PA_CONTEXT_TERMINATED:

        QMetaObject::invokeMethod(self,
                                  &VolumeMonitor::disconnected,
                                  Qt::QueuedConnection);
        break;

    default:
        break;
    }
}

//------------------------------------------------------------------------
void VolumeMonitor::subscribeCallback(pa_context *,
                                       pa_subscription_event_type_t type,
                                       uint32_t,
                                       void *userdata)
{
    auto *self = static_cast<VolumeMonitor *>(userdata);

    if ((type & PA_SUBSCRIPTION_EVENT_FACILITY_MASK)
        == PA_SUBSCRIPTION_EVENT_SINK)
    {
        self->requestSinkInfoInternal();  // NO LOCK
    }
}

//------------------------------------------------------------------------
void VolumeMonitor::sinkInfoCallback(pa_context *,
                                     const pa_sink_info *info,
                                     int eol,
                                     void *userdata)
{
    if (eol > 0 || !info)
        return;

    auto *self = static_cast<VolumeMonitor *>(userdata);

    qDebug() << "Raw volume:"
             << info->volume.values[0];

    int percent =
        (info->volume.values[0] * 100) / PA_VOLUME_NORM;

    QMetaObject::invokeMethod(
        self,
        [self, percent]() {
            emit self->volumeChanged(percent);
        },
        Qt::QueuedConnection);
}

////////////////////////////////////////////////////////////
///////////////////// INTERNAL LOGIC ///////////////////////
////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
void VolumeMonitor::requestSinkInfoInternal()
{
    pa_operation *op =
        pa_context_get_sink_info_by_name(
            m_context,
            "@DEFAULT_SINK@",
            sinkInfoCallback,
            this);

    if (op)
        pa_operation_unref(op);
}

//------------------------------------------------------------------------
void VolumeMonitor::refresh()
{
    if (!m_context ||
        pa_context_get_state(m_context) != PA_CONTEXT_READY)
        return;

    pa_threaded_mainloop_lock(m_mainloop);

    requestSinkInfoInternal();

    pa_threaded_mainloop_unlock(m_mainloop);
}

////////////////////////////////////////////////////////////
///////////////////// SET VOLUME ///////////////////////////
////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
void VolumeMonitor::setVolumePercent(int percent)
{
    if (!m_context)
        return;

    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    SetVolumeData *data = new SetVolumeData;
    data->self = this;
    data->percent = percent;

    pa_threaded_mainloop_lock(m_mainloop);

    pa_operation *op = pa_context_get_sink_info_by_name(
        m_context,
        "@DEFAULT_SINK@",
        [](pa_context *, const pa_sink_info *info, int eol, void *userdata)
        {
            if (eol || !info)
                return;

            SetVolumeData *data = static_cast<SetVolumeData*>(userdata);

            pa_cvolume volume;

            // WAJIB init dulu
            pa_cvolume_init(&volume);

            // convert percent → raw PulseAudio
            pa_volume_t raw =
                (data->percent * PA_VOLUME_NORM) / 100;

            // set semua channel
            pa_cvolume_set(&volume,
                           info->volume.channels,
                           raw);

            pa_operation *setOp =
                pa_context_set_sink_volume_by_index(
                    data->self->m_context,
                    info->index,
                    &volume,
                    nullptr,
                    nullptr);

            if (setOp)
                pa_operation_unref(setOp);
        },
        data);

    if (op)
        pa_operation_unref(op);

    pa_threaded_mainloop_unlock(m_mainloop);
}

