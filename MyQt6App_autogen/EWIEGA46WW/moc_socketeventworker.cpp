/****************************************************************************
** Meta object code from reading C++ file 'socketeventworker.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
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
#error "This file was generated using the moc from 6.10.1. It"
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
        "sleepRequested",
        "pingDeviceUpRequested",
        "listenStateChanged",
        "state",
        "talkingStateChanged",
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
        // Signal 'sleepRequested'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'pingDeviceUpRequested'
        QtMocHelpers::SignalData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'listenStateChanged'
        QtMocHelpers::SignalData<void(const QString &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Signal 'talkingStateChanged'
        QtMocHelpers::SignalData<void(const QString &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Slot 'process'
        QtMocHelpers::SlotData<void()>(16, 2, QMC::AccessPublic, QMetaType::Void),
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
        case 8: _t->sleepRequested(); break;
        case 9: _t->pingDeviceUpRequested(); break;
        case 10: _t->listenStateChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->talkingStateChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 12: _t->process(); break;
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
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::sleepRequested, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)()>(_a, &SocketEventWorker::pingDeviceUpRequested, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)(const QString & )>(_a, &SocketEventWorker::listenStateChanged, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (SocketEventWorker::*)(const QString & )>(_a, &SocketEventWorker::talkingStateChanged, 11))
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
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
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
void SocketEventWorker::sleepRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void SocketEventWorker::pingDeviceUpRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}

// SIGNAL 10
void SocketEventWorker::listenStateChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1);
}

// SIGNAL 11
void SocketEventWorker::talkingStateChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 11, nullptr, _t1);
}
QT_WARNING_POP
