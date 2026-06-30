/****************************************************************************
** Meta object code from reading C++ file 'utilities.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../utilities.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'utilities.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN9utilitiesE_t {};
} // unnamed namespace

template <> constexpr inline auto utilities::qt_create_metaobjectdata<qt_meta_tag_ZN9utilitiesE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "utilities",
        "wifiListReady",
        "",
        "ssidList",
        "wifiListReadyComplete",
        "QList<WifiAP>",
        "wifiList",
        "ssidReady",
        "ssid",
        "wifiConnectResult",
        "success",
        "ipAddress",
        "gateway",
        "wifiRadioChanged",
        "wifiForgetResult",
        "message",
        "wifiCurrentInfoReady",
        "QJsonObject",
        "info",
        "wifiDisconnectResult",
        "wifiConnectProgress",
        "state",
        "stateText",
        "nmcliGetSSID",
        "nmcliGetWifiListSSid",
        "nmcliGetWifiListComplete",
        "nmcliGetCurrentWifiInfo",
        "nmcliWifiOn",
        "nmcliWifiOff",
        "nmcliConnectToWiFi",
        "password",
        "nmcliDisconnectCurrentWifi",
        "nmcliForgetConnection",
        "deviceStateToString",
        "rpiRestart",
        "rpiShutdown",
        "setTimezone",
        "tz",
        "getTimeZone",
        "onDeviceStateChanged",
        "newState",
        "oldState",
        "reason"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'wifiListReady'
        QtMocHelpers::SignalData<void(QStringList)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 3 },
        }}),
        // Signal 'wifiListReadyComplete'
        QtMocHelpers::SignalData<void(QList<WifiAP>)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 6 },
        }}),
        // Signal 'ssidReady'
        QtMocHelpers::SignalData<void(QString)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Signal 'wifiConnectResult'
        QtMocHelpers::SignalData<void(bool, QString, QString, QString)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 10 }, { QMetaType::QString, 8 }, { QMetaType::QString, 11 }, { QMetaType::QString, 12 },
        }}),
        // Signal 'wifiRadioChanged'
        QtMocHelpers::SignalData<void(bool)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 10 },
        }}),
        // Signal 'wifiForgetResult'
        QtMocHelpers::SignalData<void(bool, QString, QString)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 10 }, { QMetaType::QString, 8 }, { QMetaType::QString, 15 },
        }}),
        // Signal 'wifiCurrentInfoReady'
        QtMocHelpers::SignalData<void(QJsonObject)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 17, 18 },
        }}),
        // Signal 'wifiDisconnectResult'
        QtMocHelpers::SignalData<void(bool, QString, QString)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 10 }, { QMetaType::QString, 8 }, { QMetaType::QString, 15 },
        }}),
        // Signal 'wifiConnectProgress'
        QtMocHelpers::SignalData<void(int, QString)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 21 }, { QMetaType::QString, 22 },
        }}),
        // Slot 'nmcliGetSSID'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'nmcliGetWifiListSSid'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'nmcliGetWifiListComplete'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'nmcliGetCurrentWifiInfo'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'nmcliWifiOn'
        QtMocHelpers::SlotData<void()>(27, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'nmcliWifiOff'
        QtMocHelpers::SlotData<void()>(28, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'nmcliConnectToWiFi'
        QtMocHelpers::SlotData<void(const QString &, const QString &)>(29, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 }, { QMetaType::QString, 30 },
        }}),
        // Slot 'nmcliDisconnectCurrentWifi'
        QtMocHelpers::SlotData<void()>(31, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'nmcliForgetConnection'
        QtMocHelpers::SlotData<void(const QString &)>(32, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Slot 'deviceStateToString'
        QtMocHelpers::SlotData<QString(uint)>(33, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::UInt, 21 },
        }}),
        // Slot 'rpiRestart'
        QtMocHelpers::SlotData<bool()>(34, 2, QMC::AccessPublic, QMetaType::Bool),
        // Slot 'rpiShutdown'
        QtMocHelpers::SlotData<bool()>(35, 2, QMC::AccessPublic, QMetaType::Bool),
        // Slot 'setTimezone'
        QtMocHelpers::SlotData<bool(const QString &)>(36, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 37 },
        }}),
        // Slot 'getTimeZone'
        QtMocHelpers::SlotData<QString()>(38, 2, QMC::AccessPublic, QMetaType::QString),
        // Slot 'onDeviceStateChanged'
        QtMocHelpers::SlotData<void(uint, uint, uint)>(39, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::UInt, 40 }, { QMetaType::UInt, 41 }, { QMetaType::UInt, 42 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<utilities, qt_meta_tag_ZN9utilitiesE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject utilities::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9utilitiesE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9utilitiesE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9utilitiesE_t>.metaTypes,
    nullptr
} };

void utilities::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<utilities *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->wifiListReady((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 1: _t->wifiListReadyComplete((*reinterpret_cast<std::add_pointer_t<QList<WifiAP>>>(_a[1]))); break;
        case 2: _t->ssidReady((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->wifiConnectResult((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4]))); break;
        case 4: _t->wifiRadioChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 5: _t->wifiForgetResult((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 6: _t->wifiCurrentInfoReady((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 7: _t->wifiDisconnectResult((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 8: _t->wifiConnectProgress((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 9: _t->nmcliGetSSID(); break;
        case 10: _t->nmcliGetWifiListSSid(); break;
        case 11: _t->nmcliGetWifiListComplete(); break;
        case 12: _t->nmcliGetCurrentWifiInfo(); break;
        case 13: _t->nmcliWifiOn(); break;
        case 14: _t->nmcliWifiOff(); break;
        case 15: _t->nmcliConnectToWiFi((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 16: _t->nmcliDisconnectCurrentWifi(); break;
        case 17: _t->nmcliForgetConnection((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 18: { QString _r = _t->deviceStateToString((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 19: { bool _r = _t->rpiRestart();
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 20: { bool _r = _t->rpiShutdown();
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 21: { bool _r = _t->setTimezone((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 22: { QString _r = _t->getTimeZone();
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 23: _t->onDeviceStateChanged((*reinterpret_cast<std::add_pointer_t<uint>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<uint>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<uint>>(_a[3]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (utilities::*)(QStringList )>(_a, &utilities::wifiListReady, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (utilities::*)(QList<WifiAP> )>(_a, &utilities::wifiListReadyComplete, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (utilities::*)(QString )>(_a, &utilities::ssidReady, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (utilities::*)(bool , QString , QString , QString )>(_a, &utilities::wifiConnectResult, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (utilities::*)(bool )>(_a, &utilities::wifiRadioChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (utilities::*)(bool , QString , QString )>(_a, &utilities::wifiForgetResult, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (utilities::*)(QJsonObject )>(_a, &utilities::wifiCurrentInfoReady, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (utilities::*)(bool , QString , QString )>(_a, &utilities::wifiDisconnectResult, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (utilities::*)(int , QString )>(_a, &utilities::wifiConnectProgress, 8))
            return;
    }
}

const QMetaObject *utilities::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *utilities::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9utilitiesE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int utilities::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 24)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 24;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 24)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 24;
    }
    return _id;
}

// SIGNAL 0
void utilities::wifiListReady(QStringList _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void utilities::wifiListReadyComplete(QList<WifiAP> _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void utilities::ssidReady(QString _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void utilities::wifiConnectResult(bool _t1, QString _t2, QString _t3, QString _t4)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2, _t3, _t4);
}

// SIGNAL 4
void utilities::wifiRadioChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void utilities::wifiForgetResult(bool _t1, QString _t2, QString _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2, _t3);
}

// SIGNAL 6
void utilities::wifiCurrentInfoReady(QJsonObject _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void utilities::wifiDisconnectResult(bool _t1, QString _t2, QString _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2, _t3);
}

// SIGNAL 8
void utilities::wifiConnectProgress(int _t1, QString _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1, _t2);
}
QT_WARNING_POP
