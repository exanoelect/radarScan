/****************************************************************************
** Meta object code from reading C++ file 'systemdmonitorqt.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../systemdmonitorqt.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'systemdmonitorqt.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN16systemdmonitorqtE_t {};
} // unnamed namespace

template <> constexpr inline auto systemdmonitorqt::qt_create_metaobjectdata<qt_meta_tag_ZN16systemdmonitorqtE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "systemdmonitorqt",
        "serviceStarted",
        "",
        "serviceStopped",
        "serviceFailed",
        "onPropertiesChanged",
        "interface",
        "QVariantMap",
        "changed",
        "invalidated"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'serviceStarted'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'serviceStopped'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'serviceFailed'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onPropertiesChanged'
        QtMocHelpers::SlotData<void(QString, QVariantMap, QStringList)>(5, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { 0x80000000 | 7, 8 }, { QMetaType::QStringList, 9 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<systemdmonitorqt, qt_meta_tag_ZN16systemdmonitorqtE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject systemdmonitorqt::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16systemdmonitorqtE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16systemdmonitorqtE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN16systemdmonitorqtE_t>.metaTypes,
    nullptr
} };

void systemdmonitorqt::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<systemdmonitorqt *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->serviceStarted(); break;
        case 1: _t->serviceStopped(); break;
        case 2: _t->serviceFailed(); break;
        case 3: _t->onPropertiesChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QVariantMap>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[3]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (systemdmonitorqt::*)()>(_a, &systemdmonitorqt::serviceStarted, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (systemdmonitorqt::*)()>(_a, &systemdmonitorqt::serviceStopped, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (systemdmonitorqt::*)()>(_a, &systemdmonitorqt::serviceFailed, 2))
            return;
    }
}

const QMetaObject *systemdmonitorqt::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *systemdmonitorqt::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16systemdmonitorqtE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int systemdmonitorqt::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void systemdmonitorqt::serviceStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void systemdmonitorqt::serviceStopped()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void systemdmonitorqt::serviceFailed()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
