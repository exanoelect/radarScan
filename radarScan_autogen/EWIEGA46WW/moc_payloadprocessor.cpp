/****************************************************************************
** Meta object code from reading C++ file 'payloadprocessor.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../payloadprocessor.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'payloadprocessor.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN16PayloadProcessorE_t {};
} // unnamed namespace

template <> constexpr inline auto PayloadProcessor::qt_create_metaobjectdata<qt_meta_tag_ZN16PayloadProcessorE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "PayloadProcessor",
        "uiUpdate",
        "",
        "source",
        "key",
        "value",
        "radarPoint",
        "x",
        "y",
        "velocityUpdate",
        "v",
        "motionUpdate",
        "motion",
        "debugMessage",
        "msg",
        "serialOpened",
        "ok",
        "serialError",
        "err",
        "fallDetected",
        "fallCancel",
        "initPort",
        "portName",
        "readData",
        "closePort",
        "enqueuePayload",
        "payload"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'uiUpdate'
        QtMocHelpers::SignalData<void(const QString &, const QString &, const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 4 }, { QMetaType::QString, 5 },
        }}),
        // Signal 'radarPoint'
        QtMocHelpers::SignalData<void(const QString &, double, double)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::Double, 7 }, { QMetaType::Double, 8 },
        }}),
        // Signal 'velocityUpdate'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 10 },
        }}),
        // Signal 'motionUpdate'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 12 },
        }}),
        // Signal 'debugMessage'
        QtMocHelpers::SignalData<void(const QString &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 14 },
        }}),
        // Signal 'serialOpened'
        QtMocHelpers::SignalData<void(bool)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 16 },
        }}),
        // Signal 'serialError'
        QtMocHelpers::SignalData<void(const QString &)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 18 },
        }}),
        // Signal 'fallDetected'
        QtMocHelpers::SignalData<void(const QString &)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'fallCancel'
        QtMocHelpers::SignalData<void(const QString &)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Slot 'initPort'
        QtMocHelpers::SlotData<void(const QString &)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 22 },
        }}),
        // Slot 'readData'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'closePort'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'enqueuePayload'
        QtMocHelpers::SlotData<void(const QByteArray &)>(25, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QByteArray, 26 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<PayloadProcessor, qt_meta_tag_ZN16PayloadProcessorE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject PayloadProcessor::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16PayloadProcessorE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16PayloadProcessorE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN16PayloadProcessorE_t>.metaTypes,
    nullptr
} };

void PayloadProcessor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<PayloadProcessor *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->uiUpdate((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 1: _t->radarPoint((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[3]))); break;
        case 2: _t->velocityUpdate((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->motionUpdate((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 4: _t->debugMessage((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->serialOpened((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 6: _t->serialError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->fallDetected((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->fallCancel((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->initPort((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 10: _t->readData(); break;
        case 11: _t->closePort(); break;
        case 12: _t->enqueuePayload((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (PayloadProcessor::*)(const QString & , const QString & , const QString & )>(_a, &PayloadProcessor::uiUpdate, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (PayloadProcessor::*)(const QString & , double , double )>(_a, &PayloadProcessor::radarPoint, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (PayloadProcessor::*)(const QString & , const QString & )>(_a, &PayloadProcessor::velocityUpdate, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (PayloadProcessor::*)(const QString & , const QString & )>(_a, &PayloadProcessor::motionUpdate, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (PayloadProcessor::*)(const QString & )>(_a, &PayloadProcessor::debugMessage, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (PayloadProcessor::*)(bool )>(_a, &PayloadProcessor::serialOpened, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (PayloadProcessor::*)(const QString & )>(_a, &PayloadProcessor::serialError, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (PayloadProcessor::*)(const QString & )>(_a, &PayloadProcessor::fallDetected, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (PayloadProcessor::*)(const QString & )>(_a, &PayloadProcessor::fallCancel, 8))
            return;
    }
}

const QMetaObject *PayloadProcessor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PayloadProcessor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16PayloadProcessorE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int PayloadProcessor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void PayloadProcessor::uiUpdate(const QString & _t1, const QString & _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2, _t3);
}

// SIGNAL 1
void PayloadProcessor::radarPoint(const QString & _t1, double _t2, double _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2, _t3);
}

// SIGNAL 2
void PayloadProcessor::velocityUpdate(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void PayloadProcessor::motionUpdate(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}

// SIGNAL 4
void PayloadProcessor::debugMessage(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void PayloadProcessor::serialOpened(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void PayloadProcessor::serialError(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void PayloadProcessor::fallDetected(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void PayloadProcessor::fallCancel(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1);
}
QT_WARNING_POP
