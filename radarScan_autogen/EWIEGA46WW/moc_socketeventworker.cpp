/****************************************************************************
** Meta object code from reading C++ file 'socketeventworker.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../socketeventworker.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'socketeventworker.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN17SocketEventWorkerE_t {};
} // unnamed namespace

template <> constexpr inline auto SocketEventWorker::qt_create_metaobjectdata<qt_meta_tag_ZN17SocketEventWorkerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SocketEventWorker",
        "volumeGetRequested",
        "",
        "volumeSetRequested",
        "value",
        "volumeIncreaseReq",
        "volumeDecreaseReq",
        "brightnessSetRequested",
        "brightnessGetRequested",
        "brightnessIncreaseReq",
        "brightnessDecreaseReq",
        "modeSleep",
        "modeWakeUp",
        "pingDeviceUp",
        "modeListen",
        "modeRecording",
        "modeWaiting",
        "modeTalking",
        "speechModuleReady",
        "langCurrent",
        "lang",
        "langGet",
        "langSet",
        "langAckSet",
        "incidentFallEventDetected",
        "incidentFallWakeUpByFallDetection",
        "incidentFallAckFallEventDetected",
        "incidentFallNoResponse",
        "incidentHelp",
        "incidentFallIamOK",
        "incidentFallHelpEventDetected",
        "incidentFallOKEventDetected",
        "incidentFallCompleted",
        "incidentFallIamnotOK",
        "alarmRing",
        "alarmStop",
        "alarmSnooze",
        "alarmWakeUp",
        "alarmStopButton",
        "alarmSnoozeButton",
        "wifiOn",
        "wifiOff",
        "wifiGetSsid",
        "wifiScanSsidReqReceived",
        "wifiDisconnectCurrentSsid",
        "wifiSsidList",
        "wifiSsidListComplete",
        "wifiConnect",
        "ssid",
        "pwd",
        "wifiForget",
        "rpiRestart",
        "rpiShutdown",
        "tzSetReq",
        "tz",
        "tzGetReq",
        "process"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'volumeGetRequested'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'volumeSetRequested'
        QtMocHelpers::SignalData<void(int)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 4 },
        }}),
        // Signal 'volumeIncreaseReq'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'volumeDecreaseReq'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'brightnessSetRequested'
        QtMocHelpers::SignalData<void(int)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 4 },
        }}),
        // Signal 'brightnessGetRequested'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'brightnessIncreaseReq'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'brightnessDecreaseReq'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'modeSleep'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'modeWakeUp'
        QtMocHelpers::SignalData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'pingDeviceUp'
        QtMocHelpers::SignalData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'modeListen'
        QtMocHelpers::SignalData<void()>(14, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'modeRecording'
        QtMocHelpers::SignalData<void()>(15, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'modeWaiting'
        QtMocHelpers::SignalData<void()>(16, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'modeTalking'
        QtMocHelpers::SignalData<void()>(17, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'speechModuleReady'
        QtMocHelpers::SignalData<void()>(18, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'langCurrent'
        QtMocHelpers::SignalData<void(QString)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 20 },
        }}),
        // Signal 'langGet'
        QtMocHelpers::SignalData<void()>(21, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'langSet'
        QtMocHelpers::SignalData<void(QString)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 20 },
        }}),
        // Signal 'langAckSet'
        QtMocHelpers::SignalData<void()>(23, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'incidentFallEventDetected'
        QtMocHelpers::SignalData<void()>(24, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'incidentFallWakeUpByFallDetection'
        QtMocHelpers::SignalData<void()>(25, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'incidentFallAckFallEventDetected'
        QtMocHelpers::SignalData<void()>(26, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'incidentFallNoResponse'
        QtMocHelpers::SignalData<void()>(27, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'incidentHelp'
        QtMocHelpers::SignalData<void()>(28, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'incidentFallIamOK'
        QtMocHelpers::SignalData<void()>(29, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'incidentFallHelpEventDetected'
        QtMocHelpers::SignalData<void()>(30, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'incidentFallOKEventDetected'
        QtMocHelpers::SignalData<void()>(31, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'incidentFallCompleted'
        QtMocHelpers::SignalData<void()>(32, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'incidentFallIamnotOK'
        QtMocHelpers::SignalData<void()>(33, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'alarmRing'
        QtMocHelpers::SignalData<void()>(34, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'alarmStop'
        QtMocHelpers::SignalData<void()>(35, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'alarmSnooze'
        QtMocHelpers::SignalData<void()>(36, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'alarmWakeUp'
        QtMocHelpers::SignalData<void()>(37, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'alarmStopButton'
        QtMocHelpers::SignalData<void()>(38, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'alarmSnoozeButton'
        QtMocHelpers::SignalData<void()>(39, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'wifiOn'
        QtMocHelpers::SignalData<void()>(40, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'wifiOff'
        QtMocHelpers::SignalData<void()>(41, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'wifiGetSsid'
        QtMocHelpers::SignalData<void()>(42, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'wifiScanSsidReqReceived'
        QtMocHelpers::SignalData<void()>(43, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'wifiDisconnectCurrentSsid'
        QtMocHelpers::SignalData<void()>(44, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'wifiSsidList'
        QtMocHelpers::SignalData<void()>(45, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'wifiSsidListComplete'
        QtMocHelpers::SignalData<void()>(46, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'wifiConnect'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(47, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 48 }, { QMetaType::QString, 49 },
        }}),
        // Signal 'wifiForget'
        QtMocHelpers::SignalData<void(const QString &)>(50, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 48 },
        }}),
        // Signal 'rpiRestart'
        QtMocHelpers::SignalData<void()>(51, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'rpiShutdown'
        QtMocHelpers::SignalData<void()>(52, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'tzSetReq'
        QtMocHelpers::SignalData<void(QString)>(53, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 54 },
        }}),
        // Signal 'tzGetReq'
        QtMocHelpers::SignalData<void()>(55, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'process'
        QtMocHelpers::SlotData<void()>(56, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SocketEventWorker, qt_meta_tag_ZN17SocketEventWorkerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SocketEventWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17SocketEventWorkerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17SocketEventWorkerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17SocketEventWorkerE_t>.metaTypes,
    nullptr
} };

void SocketEventWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SocketEventWorker *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->volumeGetRequested(); break;
        case 1: _t->volumeSetRequested((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->volumeIncreaseReq(); break;
        case 3: _t->volumeDecreaseReq(); break;
        case 4: _t->brightnessSetRequested((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->brightnessGetRequested(); break;
        case 6: _t->brightnessIncreaseReq(); break;
        case 7: _t->brightnessDecreaseReq(); break;
        case 8: _t->modeSleep(); break;
        case 9: _t->modeWakeUp(); break;
        case 10: _t->pingDeviceUp(); break;
        case 11: _t->modeListen(); break;
        case 12: _t->modeRecording(); break;
        case 13: _t->modeWaiting(); break;
        case 14: _t->modeTalking(); break;
        case 15: _t->speechModuleReady(); break;
        case 16: _t->langCurrent((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 17: _t->langGet(); break;
        case 18: _t->langSet((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 19: _t->langAckSet(); break;
        case 20: _t->incidentFallEventDetected(); break;
        case 21: _t->incidentFallWakeUpByFallDetection(); break;
        case 22: _t->incidentFallAckFallEventDetected(); break;
        case 23: _t->incidentFallNoResponse(); break;
        case 24: _t->incidentHelp(); break;
        case 25: _t->incidentFallIamOK(); break;
        case 26: _t->incidentFallHelpEventDetected(); break;
        case 27: _t->incidentFallOKEventDetected(); break;
        case 28: _t->incidentFallCompleted(); break;
        case 29: _t->incidentFallIamnotOK(); break;
        case 30: _t->alarmRing(); break;
        case 31: _t->alarmStop(); break;
        case 32: _t->alarmSnooze(); break;
        case 33: _t->alarmWakeUp(); break;
        case 34: _t->alarmStopButton(); break;
        case 35: _t->alarmSnoozeButton(); break;
        case 36: _t->wifiOn(); break;
        case 37: _t->wifiOff(); break;
        case 38: _t->wifiGetSsid(); break;
        case 39: _t->wifiScanSsidReqReceived(); break;
        case 40: _t->wifiDisconnectCurrentSsid(); break;
        case 41: _t->wifiSsidList(); break;
        case 42: _t->wifiSsidListComplete(); break;
        case 43: _t->wifiConnect((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 44: _t->wifiForget((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 45: _t->rpiRestart(); break;
        case 46: _t->rpiShutdown(); break;
        case 47: _t->tzSetReq((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 48: _t->tzGetReq(); break;
        case 49: _t->process(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::volumeGetRequested, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)(int )>(_a, &SocketEventWorker::volumeSetRequested, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::volumeIncreaseReq, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::volumeDecreaseReq, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)(int )>(_a, &SocketEventWorker::brightnessSetRequested, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::brightnessGetRequested, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::brightnessIncreaseReq, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::brightnessDecreaseReq, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::modeSleep, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::modeWakeUp, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::pingDeviceUp, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::modeListen, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::modeRecording, 12))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::modeWaiting, 13))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::modeTalking, 14))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::speechModuleReady, 15))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)(QString )>(_a, &SocketEventWorker::langCurrent, 16))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::langGet, 17))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)(QString )>(_a, &SocketEventWorker::langSet, 18))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::langAckSet, 19))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::incidentFallEventDetected, 20))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::incidentFallWakeUpByFallDetection, 21))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::incidentFallAckFallEventDetected, 22))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::incidentFallNoResponse, 23))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::incidentHelp, 24))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::incidentFallIamOK, 25))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::incidentFallHelpEventDetected, 26))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::incidentFallOKEventDetected, 27))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::incidentFallCompleted, 28))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::incidentFallIamnotOK, 29))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::alarmRing, 30))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::alarmStop, 31))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::alarmSnooze, 32))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::alarmWakeUp, 33))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::alarmStopButton, 34))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::alarmSnoozeButton, 35))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::wifiOn, 36))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::wifiOff, 37))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::wifiGetSsid, 38))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::wifiScanSsidReqReceived, 39))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::wifiDisconnectCurrentSsid, 40))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::wifiSsidList, 41))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::wifiSsidListComplete, 42))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)(const QString & , const QString & )>(_a, &SocketEventWorker::wifiConnect, 43))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)(const QString & )>(_a, &SocketEventWorker::wifiForget, 44))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::rpiRestart, 45))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::rpiShutdown, 46))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)(QString )>(_a, &SocketEventWorker::tzSetReq, 47))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::tzGetReq, 48))
            return;
    }
}

const QMetaObject *SocketEventWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SocketEventWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17SocketEventWorkerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SocketEventWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 50)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 50;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 50)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 50;
    }
    return _id;
}

// SIGNAL 0
void SocketEventWorker::volumeGetRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SocketEventWorker::volumeSetRequested(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void SocketEventWorker::volumeIncreaseReq()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void SocketEventWorker::volumeDecreaseReq()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void SocketEventWorker::brightnessSetRequested(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void SocketEventWorker::brightnessGetRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void SocketEventWorker::brightnessIncreaseReq()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void SocketEventWorker::brightnessDecreaseReq()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void SocketEventWorker::modeSleep()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void SocketEventWorker::modeWakeUp()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void SocketEventWorker::pingDeviceUp()
{
    QMetaObject::activate(this, &staticMetaObject, 10, nullptr);
}

// SIGNAL 11
void SocketEventWorker::modeListen()
{
    QMetaObject::activate(this, &staticMetaObject, 11, nullptr);
}

// SIGNAL 12
void SocketEventWorker::modeRecording()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}

// SIGNAL 13
void SocketEventWorker::modeWaiting()
{
    QMetaObject::activate(this, &staticMetaObject, 13, nullptr);
}

// SIGNAL 14
void SocketEventWorker::modeTalking()
{
    QMetaObject::activate(this, &staticMetaObject, 14, nullptr);
}

// SIGNAL 15
void SocketEventWorker::speechModuleReady()
{
    QMetaObject::activate(this, &staticMetaObject, 15, nullptr);
}

// SIGNAL 16
void SocketEventWorker::langCurrent(QString _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 16, nullptr, _t1);
}

// SIGNAL 17
void SocketEventWorker::langGet()
{
    QMetaObject::activate(this, &staticMetaObject, 17, nullptr);
}

// SIGNAL 18
void SocketEventWorker::langSet(QString _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 18, nullptr, _t1);
}

// SIGNAL 19
void SocketEventWorker::langAckSet()
{
    QMetaObject::activate(this, &staticMetaObject, 19, nullptr);
}

// SIGNAL 20
void SocketEventWorker::incidentFallEventDetected()
{
    QMetaObject::activate(this, &staticMetaObject, 20, nullptr);
}

// SIGNAL 21
void SocketEventWorker::incidentFallWakeUpByFallDetection()
{
    QMetaObject::activate(this, &staticMetaObject, 21, nullptr);
}

// SIGNAL 22
void SocketEventWorker::incidentFallAckFallEventDetected()
{
    QMetaObject::activate(this, &staticMetaObject, 22, nullptr);
}

// SIGNAL 23
void SocketEventWorker::incidentFallNoResponse()
{
    QMetaObject::activate(this, &staticMetaObject, 23, nullptr);
}

// SIGNAL 24
void SocketEventWorker::incidentHelp()
{
    QMetaObject::activate(this, &staticMetaObject, 24, nullptr);
}

// SIGNAL 25
void SocketEventWorker::incidentFallIamOK()
{
    QMetaObject::activate(this, &staticMetaObject, 25, nullptr);
}

// SIGNAL 26
void SocketEventWorker::incidentFallHelpEventDetected()
{
    QMetaObject::activate(this, &staticMetaObject, 26, nullptr);
}

// SIGNAL 27
void SocketEventWorker::incidentFallOKEventDetected()
{
    QMetaObject::activate(this, &staticMetaObject, 27, nullptr);
}

// SIGNAL 28
void SocketEventWorker::incidentFallCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 28, nullptr);
}

// SIGNAL 29
void SocketEventWorker::incidentFallIamnotOK()
{
    QMetaObject::activate(this, &staticMetaObject, 29, nullptr);
}

// SIGNAL 30
void SocketEventWorker::alarmRing()
{
    QMetaObject::activate(this, &staticMetaObject, 30, nullptr);
}

// SIGNAL 31
void SocketEventWorker::alarmStop()
{
    QMetaObject::activate(this, &staticMetaObject, 31, nullptr);
}

// SIGNAL 32
void SocketEventWorker::alarmSnooze()
{
    QMetaObject::activate(this, &staticMetaObject, 32, nullptr);
}

// SIGNAL 33
void SocketEventWorker::alarmWakeUp()
{
    QMetaObject::activate(this, &staticMetaObject, 33, nullptr);
}

// SIGNAL 34
void SocketEventWorker::alarmStopButton()
{
    QMetaObject::activate(this, &staticMetaObject, 34, nullptr);
}

// SIGNAL 35
void SocketEventWorker::alarmSnoozeButton()
{
    QMetaObject::activate(this, &staticMetaObject, 35, nullptr);
}

// SIGNAL 36
void SocketEventWorker::wifiOn()
{
    QMetaObject::activate(this, &staticMetaObject, 36, nullptr);
}

// SIGNAL 37
void SocketEventWorker::wifiOff()
{
    QMetaObject::activate(this, &staticMetaObject, 37, nullptr);
}

// SIGNAL 38
void SocketEventWorker::wifiGetSsid()
{
    QMetaObject::activate(this, &staticMetaObject, 38, nullptr);
}

// SIGNAL 39
void SocketEventWorker::wifiScanSsidReqReceived()
{
    QMetaObject::activate(this, &staticMetaObject, 39, nullptr);
}

// SIGNAL 40
void SocketEventWorker::wifiDisconnectCurrentSsid()
{
    QMetaObject::activate(this, &staticMetaObject, 40, nullptr);
}

// SIGNAL 41
void SocketEventWorker::wifiSsidList()
{
    QMetaObject::activate(this, &staticMetaObject, 41, nullptr);
}

// SIGNAL 42
void SocketEventWorker::wifiSsidListComplete()
{
    QMetaObject::activate(this, &staticMetaObject, 42, nullptr);
}

// SIGNAL 43
void SocketEventWorker::wifiConnect(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 43, nullptr, _t1, _t2);
}

// SIGNAL 44
void SocketEventWorker::wifiForget(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 44, nullptr, _t1);
}

// SIGNAL 45
void SocketEventWorker::rpiRestart()
{
    QMetaObject::activate(this, &staticMetaObject, 45, nullptr);
}

// SIGNAL 46
void SocketEventWorker::rpiShutdown()
{
    QMetaObject::activate(this, &staticMetaObject, 46, nullptr);
}

// SIGNAL 47
void SocketEventWorker::tzSetReq(QString _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 47, nullptr, _t1);
}

// SIGNAL 48
void SocketEventWorker::tzGetReq()
{
    QMetaObject::activate(this, &staticMetaObject, 48, nullptr);
}
QT_WARNING_POP
