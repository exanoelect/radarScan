/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../mainwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto MainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN10MainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MainWindow",
        "on_btnLoad_clicked",
        "",
        "on_btnOpenSerialPort_clicked",
        "on_btnLoad2_clicked",
        "on_btnOpenSerialPort2_clicked",
        "on_btnGetProductID_clicked",
        "on_btnGetProductModel_clicked",
        "on_btnGetFirmwareVersion_clicked",
        "on_btnGetProductID2_clicked",
        "on_btnGetProductModel2_clicked",
        "on_btnGetFirmwareVersion2_clicked",
        "realtimeDataSlot",
        "value",
        "realtimeDataVelocity",
        "realtimeDataSlot2",
        "realtimeDataVelocity2",
        "on_btnHWModel_clicked",
        "on_btnCmdInitCompleteCek_clicked",
        "on_btnHWModel2_clicked",
        "on_btnCmdInitCompleteCek2_clicked",
        "on_btnGetAngleInst_clicked",
        "on_btnGetHeightInst_clicked",
        "on_btnGetAngleInst2_clicked",
        "on_btnGetHeightInst2_clicked",
        "on_cbPresence_toggled",
        "checked",
        "on_cbFallDetection_toggled",
        "on_btnGetFallDuration_clicked",
        "on_cbStandStill_toggled",
        "on_cbTraceTracking_toggled",
        "on_cbPresence2_toggled",
        "on_cbFallDetection2_toggled",
        "on_btnGetFallDuration2_clicked",
        "on_cbStandStill2_toggled",
        "on_cbTraceTracking2_toggled",
        "on_btnSetHeight_clicked",
        "on_btnSetHeight2_clicked",
        "on_btnSetFallDuration_clicked",
        "on_btnSetAngle_clicked",
        "on_btnSetFallDuration2_clicked",
        "on_btnSetAngle2_clicked",
        "on_btnPlaySound_clicked",
        "onSocketEventReceived",
        "eventName",
        "QJsonValue",
        "data",
        "onDeviceReadyConnected",
        "vol",
        "bright",
        "on_btnColor1_clicked",
        "on_btnColor2_clicked",
        "on_btnColor3_clicked",
        "on_btnColor4_clicked",
        "on_hsBrightness_valueChanged",
        "on_btnGetBrightness_clicked",
        "on_btnsetBrightness_clicked",
        "on_btnGetVol_clicked",
        "on_hsVol_valueChanged",
        "on_btnsetVol_clicked",
        "on_btnConnect_clicked",
        "on_btnFallSimulation_clicked",
        "onListenStateChanged",
        "state",
        "onTalkingStateChanged",
        "onVolumeGetRequested",
        "onVolumeSetRequested",
        "onPingDeviceUpRequested",
        "onSleepRequested",
        "onBrightnessSetRequested",
        "onBrightnessGetRequested",
        "onVolumeIncreaseReq",
        "onVolumeDecreaseReq",
        "onBrihtnessIncreaseReq",
        "onBrightnessDecreaseReq",
        "onIncidentFallOccur",
        "onIncidentFallCancel",
        "onIncidentIamnotOK",
        "onIncidentIamOK",
        "on_btnPlayFall_clicked",
        "on_btnPlayHelp_clicked",
        "on_btnPlayIamOK_clicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'on_btnLoad_clicked'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnOpenSerialPort_clicked'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnLoad2_clicked'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnOpenSerialPort2_clicked'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnGetProductID_clicked'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnGetProductModel_clicked'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnGetFirmwareVersion_clicked'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnGetProductID2_clicked'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnGetProductModel2_clicked'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnGetFirmwareVersion2_clicked'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'realtimeDataSlot'
        QtMocHelpers::SlotData<void(QString)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 13 },
        }}),
        // Slot 'realtimeDataVelocity'
        QtMocHelpers::SlotData<void(QString)>(14, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 13 },
        }}),
        // Slot 'realtimeDataSlot2'
        QtMocHelpers::SlotData<void(QString)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 13 },
        }}),
        // Slot 'realtimeDataVelocity2'
        QtMocHelpers::SlotData<void(QString)>(16, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 13 },
        }}),
        // Slot 'on_btnHWModel_clicked'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnCmdInitCompleteCek_clicked'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnHWModel2_clicked'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnCmdInitCompleteCek2_clicked'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnGetAngleInst_clicked'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnGetHeightInst_clicked'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnGetAngleInst2_clicked'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnGetHeightInst2_clicked'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_cbPresence_toggled'
        QtMocHelpers::SlotData<void(bool)>(25, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 26 },
        }}),
        // Slot 'on_cbFallDetection_toggled'
        QtMocHelpers::SlotData<void(bool)>(27, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 26 },
        }}),
        // Slot 'on_btnGetFallDuration_clicked'
        QtMocHelpers::SlotData<void()>(28, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_cbStandStill_toggled'
        QtMocHelpers::SlotData<void(bool)>(29, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 26 },
        }}),
        // Slot 'on_cbTraceTracking_toggled'
        QtMocHelpers::SlotData<void(bool)>(30, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 26 },
        }}),
        // Slot 'on_cbPresence2_toggled'
        QtMocHelpers::SlotData<void(bool)>(31, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 26 },
        }}),
        // Slot 'on_cbFallDetection2_toggled'
        QtMocHelpers::SlotData<void(bool)>(32, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 26 },
        }}),
        // Slot 'on_btnGetFallDuration2_clicked'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_cbStandStill2_toggled'
        QtMocHelpers::SlotData<void(bool)>(34, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 26 },
        }}),
        // Slot 'on_cbTraceTracking2_toggled'
        QtMocHelpers::SlotData<void(bool)>(35, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 26 },
        }}),
        // Slot 'on_btnSetHeight_clicked'
        QtMocHelpers::SlotData<void()>(36, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnSetHeight2_clicked'
        QtMocHelpers::SlotData<void()>(37, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnSetFallDuration_clicked'
        QtMocHelpers::SlotData<void()>(38, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnSetAngle_clicked'
        QtMocHelpers::SlotData<void()>(39, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnSetFallDuration2_clicked'
        QtMocHelpers::SlotData<void()>(40, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnSetAngle2_clicked'
        QtMocHelpers::SlotData<void()>(41, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnPlaySound_clicked'
        QtMocHelpers::SlotData<void()>(42, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSocketEventReceived'
        QtMocHelpers::SlotData<void(const QString &, const QJsonValue &)>(43, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 44 }, { 0x80000000 | 45, 46 },
        }}),
        // Slot 'onDeviceReadyConnected'
        QtMocHelpers::SlotData<void(int, int)>(47, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 48 }, { QMetaType::Int, 49 },
        }}),
        // Slot 'on_btnColor1_clicked'
        QtMocHelpers::SlotData<void()>(50, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnColor2_clicked'
        QtMocHelpers::SlotData<void()>(51, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnColor3_clicked'
        QtMocHelpers::SlotData<void()>(52, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnColor4_clicked'
        QtMocHelpers::SlotData<void()>(53, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_hsBrightness_valueChanged'
        QtMocHelpers::SlotData<void(int)>(54, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 13 },
        }}),
        // Slot 'on_btnGetBrightness_clicked'
        QtMocHelpers::SlotData<void()>(55, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnsetBrightness_clicked'
        QtMocHelpers::SlotData<void()>(56, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnGetVol_clicked'
        QtMocHelpers::SlotData<void()>(57, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_hsVol_valueChanged'
        QtMocHelpers::SlotData<void(int)>(58, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 13 },
        }}),
        // Slot 'on_btnsetVol_clicked'
        QtMocHelpers::SlotData<void()>(59, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnConnect_clicked'
        QtMocHelpers::SlotData<void()>(60, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnFallSimulation_clicked'
        QtMocHelpers::SlotData<void()>(61, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onListenStateChanged'
        QtMocHelpers::SlotData<void(const QString &)>(62, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 63 },
        }}),
        // Slot 'onTalkingStateChanged'
        QtMocHelpers::SlotData<void(const QString &)>(64, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 63 },
        }}),
        // Slot 'onVolumeGetRequested'
        QtMocHelpers::SlotData<void()>(65, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onVolumeSetRequested'
        QtMocHelpers::SlotData<void(int)>(66, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 13 },
        }}),
        // Slot 'onPingDeviceUpRequested'
        QtMocHelpers::SlotData<void()>(67, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSleepRequested'
        QtMocHelpers::SlotData<void()>(68, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBrightnessSetRequested'
        QtMocHelpers::SlotData<void(int)>(69, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 13 },
        }}),
        // Slot 'onBrightnessGetRequested'
        QtMocHelpers::SlotData<void()>(70, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onVolumeIncreaseReq'
        QtMocHelpers::SlotData<void()>(71, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onVolumeDecreaseReq'
        QtMocHelpers::SlotData<void()>(72, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBrihtnessIncreaseReq'
        QtMocHelpers::SlotData<void()>(73, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBrightnessDecreaseReq'
        QtMocHelpers::SlotData<void()>(74, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onIncidentFallOccur'
        QtMocHelpers::SlotData<void()>(75, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onIncidentFallCancel'
        QtMocHelpers::SlotData<void()>(76, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onIncidentIamnotOK'
        QtMocHelpers::SlotData<void()>(77, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onIncidentIamOK'
        QtMocHelpers::SlotData<void()>(78, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnPlayFall_clicked'
        QtMocHelpers::SlotData<void()>(79, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnPlayHelp_clicked'
        QtMocHelpers::SlotData<void()>(80, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'on_btnPlayIamOK_clicked'
        QtMocHelpers::SlotData<void()>(81, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MainWindow, qt_meta_tag_ZN10MainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MainWindowE_t>.metaTypes,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->on_btnLoad_clicked(); break;
        case 1: _t->on_btnOpenSerialPort_clicked(); break;
        case 2: _t->on_btnLoad2_clicked(); break;
        case 3: _t->on_btnOpenSerialPort2_clicked(); break;
        case 4: _t->on_btnGetProductID_clicked(); break;
        case 5: _t->on_btnGetProductModel_clicked(); break;
        case 6: _t->on_btnGetFirmwareVersion_clicked(); break;
        case 7: _t->on_btnGetProductID2_clicked(); break;
        case 8: _t->on_btnGetProductModel2_clicked(); break;
        case 9: _t->on_btnGetFirmwareVersion2_clicked(); break;
        case 10: _t->realtimeDataSlot((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->realtimeDataVelocity((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 12: _t->realtimeDataSlot2((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->realtimeDataVelocity2((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 14: _t->on_btnHWModel_clicked(); break;
        case 15: _t->on_btnCmdInitCompleteCek_clicked(); break;
        case 16: _t->on_btnHWModel2_clicked(); break;
        case 17: _t->on_btnCmdInitCompleteCek2_clicked(); break;
        case 18: _t->on_btnGetAngleInst_clicked(); break;
        case 19: _t->on_btnGetHeightInst_clicked(); break;
        case 20: _t->on_btnGetAngleInst2_clicked(); break;
        case 21: _t->on_btnGetHeightInst2_clicked(); break;
        case 22: _t->on_cbPresence_toggled((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 23: _t->on_cbFallDetection_toggled((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 24: _t->on_btnGetFallDuration_clicked(); break;
        case 25: _t->on_cbStandStill_toggled((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 26: _t->on_cbTraceTracking_toggled((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 27: _t->on_cbPresence2_toggled((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 28: _t->on_cbFallDetection2_toggled((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 29: _t->on_btnGetFallDuration2_clicked(); break;
        case 30: _t->on_cbStandStill2_toggled((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 31: _t->on_cbTraceTracking2_toggled((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 32: _t->on_btnSetHeight_clicked(); break;
        case 33: _t->on_btnSetHeight2_clicked(); break;
        case 34: _t->on_btnSetFallDuration_clicked(); break;
        case 35: _t->on_btnSetAngle_clicked(); break;
        case 36: _t->on_btnSetFallDuration2_clicked(); break;
        case 37: _t->on_btnSetAngle2_clicked(); break;
        case 38: _t->on_btnPlaySound_clicked(); break;
        case 39: _t->onSocketEventReceived((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QJsonValue>>(_a[2]))); break;
        case 40: _t->onDeviceReadyConnected((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 41: _t->on_btnColor1_clicked(); break;
        case 42: _t->on_btnColor2_clicked(); break;
        case 43: _t->on_btnColor3_clicked(); break;
        case 44: _t->on_btnColor4_clicked(); break;
        case 45: _t->on_hsBrightness_valueChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 46: _t->on_btnGetBrightness_clicked(); break;
        case 47: _t->on_btnsetBrightness_clicked(); break;
        case 48: _t->on_btnGetVol_clicked(); break;
        case 49: _t->on_hsVol_valueChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 50: _t->on_btnsetVol_clicked(); break;
        case 51: _t->on_btnConnect_clicked(); break;
        case 52: _t->on_btnFallSimulation_clicked(); break;
        case 53: _t->onListenStateChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 54: _t->onTalkingStateChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 55: _t->onVolumeGetRequested(); break;
        case 56: _t->onVolumeSetRequested((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 57: _t->onPingDeviceUpRequested(); break;
        case 58: _t->onSleepRequested(); break;
        case 59: _t->onBrightnessSetRequested((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 60: _t->onBrightnessGetRequested(); break;
        case 61: _t->onVolumeIncreaseReq(); break;
        case 62: _t->onVolumeDecreaseReq(); break;
        case 63: _t->onBrihtnessIncreaseReq(); break;
        case 64: _t->onBrightnessDecreaseReq(); break;
        case 65: _t->onIncidentFallOccur(); break;
        case 66: _t->onIncidentFallCancel(); break;
        case 67: _t->onIncidentIamnotOK(); break;
        case 68: _t->onIncidentIamOK(); break;
        case 69: _t->on_btnPlayFall_clicked(); break;
        case 70: _t->on_btnPlayHelp_clicked(); break;
        case 71: _t->on_btnPlayIamOK_clicked(); break;
        default: ;
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 72)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 72;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 72)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 72;
    }
    return _id;
}
QT_WARNING_POP
