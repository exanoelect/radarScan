/****************************************************************************
** Meta object code from reading C++ file 'NetworkMonitorQt.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../NetworkMonitorQt.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NetworkMonitorQt.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_NetworkMonitorQt_t {
    uint offsetsAndSizes[32];
    char stringdata0[17];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[17];
    char stringdata4[15];
    char stringdata5[21];
    char stringdata6[17];
    char stringdata7[6];
    char stringdata8[18];
    char stringdata9[5];
    char stringdata10[8];
    char stringdata11[11];
    char stringdata12[16];
    char stringdata13[19];
    char stringdata14[10];
    char stringdata15[3];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_NetworkMonitorQt_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_NetworkMonitorQt_t qt_meta_stringdata_NetworkMonitorQt = {
    {
        QT_MOC_LITERAL(0, 16),  // "NetworkMonitorQt"
        QT_MOC_LITERAL(17, 13),  // "wifiConnected"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 16),  // "wifiDisconnected"
        QT_MOC_LITERAL(49, 14),  // "wifiSignalLost"
        QT_MOC_LITERAL(64, 20),  // "networkInterfaceDown"
        QT_MOC_LITERAL(85, 16),  // "ipAddressChanged"
        QT_MOC_LITERAL(102, 5),  // "newIp"
        QT_MOC_LITERAL(108, 17),  // "wifiHealthUpdated"
        QT_MOC_LITERAL(126, 4),  // "rssi"
        QT_MOC_LITERAL(131, 7),  // "latency"
        QT_MOC_LITERAL(139, 10),  // "packetLoss"
        QT_MOC_LITERAL(150, 15),  // "handleLinkEvent"
        QT_MOC_LITERAL(166, 18),  // "handleLinkMessages"
        QT_MOC_LITERAL(185, 9),  // "nlmsghdr*"
        QT_MOC_LITERAL(195, 2)   // "nh"
    },
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
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_NetworkMonitorQt[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   62,    2, 0x06,    1 /* Public */,
       3,    0,   63,    2, 0x06,    2 /* Public */,
       4,    0,   64,    2, 0x06,    3 /* Public */,
       5,    0,   65,    2, 0x06,    4 /* Public */,
       6,    1,   66,    2, 0x06,    5 /* Public */,
       8,    3,   69,    2, 0x06,    7 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      12,    0,   76,    2, 0x08,   11 /* Private */,
      13,    1,   77,    2, 0x08,   12 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::Int, QMetaType::Double, QMetaType::Double,    9,   10,   11,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 14,   15,

       0        // eod
};

Q_CONSTINIT const QMetaObject NetworkMonitorQt::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_NetworkMonitorQt.offsetsAndSizes,
    qt_meta_data_NetworkMonitorQt,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_NetworkMonitorQt_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<NetworkMonitorQt, std::true_type>,
        // method 'wifiConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'wifiDisconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'wifiSignalLost'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'networkInterfaceDown'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ipAddressChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'wifiHealthUpdated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'handleLinkEvent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleLinkMessages'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<nlmsghdr *, std::false_type>
    >,
    nullptr
} };

void NetworkMonitorQt::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<NetworkMonitorQt *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->wifiConnected(); break;
        case 1: _t->wifiDisconnected(); break;
        case 2: _t->wifiSignalLost(); break;
        case 3: _t->networkInterfaceDown(); break;
        case 4: _t->ipAddressChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->wifiHealthUpdated((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[3]))); break;
        case 6: _t->handleLinkEvent(); break;
        case 7: _t->handleLinkMessages((*reinterpret_cast< std::add_pointer_t<nlmsghdr*>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (NetworkMonitorQt::*)();
            if (_t _q_method = &NetworkMonitorQt::wifiConnected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (NetworkMonitorQt::*)();
            if (_t _q_method = &NetworkMonitorQt::wifiDisconnected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (NetworkMonitorQt::*)();
            if (_t _q_method = &NetworkMonitorQt::wifiSignalLost; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (NetworkMonitorQt::*)();
            if (_t _q_method = &NetworkMonitorQt::networkInterfaceDown; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (NetworkMonitorQt::*)(QString );
            if (_t _q_method = &NetworkMonitorQt::ipAddressChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (NetworkMonitorQt::*)(int , double , double );
            if (_t _q_method = &NetworkMonitorQt::wifiHealthUpdated; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
    }
}

const QMetaObject *NetworkMonitorQt::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NetworkMonitorQt::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_NetworkMonitorQt.stringdata0))
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
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
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
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void NetworkMonitorQt::wifiHealthUpdated(int _t1, double _t2, double _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
