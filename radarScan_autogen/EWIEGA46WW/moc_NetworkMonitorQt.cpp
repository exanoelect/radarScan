/****************************************************************************
** Meta object code from reading C++ file 'NetworkMonitorQt.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../NetworkMonitorQt.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NetworkMonitorQt.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN16NetworkMonitorQtE_t {};
} // unnamed namespace

template <> constexpr inline auto NetworkMonitorQt::qt_create_metaobjectdata<qt_meta_tag_ZN16NetworkMonitorQtE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "NetworkMonitorQt",
        "wifiConnected",
        "",
        "wifiDisconnected",
        "wifiSignalLost",
        "networkInterfaceDown",
        "ipAddressChanged",
        "newIp",
        "wifiHealthUpdated",
        "rssi",
        "latency",
        "packetLoss",
        "handleLinkEvent",
        "handleLinkMessages",
        "nlmsghdr*",
        "nh"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'wifiConnected'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'wifiDisconnected'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'wifiSignalLost'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'networkInterfaceDown'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'ipAddressChanged'
        QtMocHelpers::SignalData<void(QString)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 },
        }}),
        // Signal 'wifiHealthUpdated'
        QtMocHelpers::SignalData<void(int, double, double)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 9 }, { QMetaType::Double, 10 }, { QMetaType::Double, 11 },
        }}),
        // Slot 'handleLinkEvent'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'handleLinkMessages'
        QtMocHelpers::SlotData<void(nlmsghdr *)>(13, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 14, 15 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<NetworkMonitorQt, qt_meta_tag_ZN16NetworkMonitorQtE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject NetworkMonitorQt::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16NetworkMonitorQtE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16NetworkMonitorQtE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN16NetworkMonitorQtE_t>.metaTypes,
    nullptr
} };

void NetworkMonitorQt::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<NetworkMonitorQt *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->wifiConnected(); break;
        case 1: _t->wifiDisconnected(); break;
        case 2: _t->wifiSignalLost(); break;
        case 3: _t->networkInterfaceDown(); break;
        case 4: _t->ipAddressChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->wifiHealthUpdated((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[3]))); break;
        case 6: _t->handleLinkEvent(); break;
        case 7: _t->handleLinkMessages((*reinterpret_cast<std::add_pointer_t<nlmsghdr*>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (NetworkMonitorQt::*)()>(_a, &NetworkMonitorQt::wifiConnected, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkMonitorQt::*)()>(_a, &NetworkMonitorQt::wifiDisconnected, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkMonitorQt::*)()>(_a, &NetworkMonitorQt::wifiSignalLost, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkMonitorQt::*)()>(_a, &NetworkMonitorQt::networkInterfaceDown, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkMonitorQt::*)(QString )>(_a, &NetworkMonitorQt::ipAddressChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (NetworkMonitorQt::*)(int , double , double )>(_a, &NetworkMonitorQt::wifiHealthUpdated, 5))
            return;
    }
}

const QMetaObject *NetworkMonitorQt::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NetworkMonitorQt::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16NetworkMonitorQtE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int NetworkMonitorQt::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void NetworkMonitorQt::wifiConnected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void NetworkMonitorQt::wifiDisconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void NetworkMonitorQt::wifiSignalLost()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void NetworkMonitorQt::networkInterfaceDown()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void NetworkMonitorQt::ipAddressChanged(QString _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void NetworkMonitorQt::wifiHealthUpdated(int _t1, double _t2, double _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2, _t3);
}
QT_WARNING_POP
