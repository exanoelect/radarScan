/****************************************************************************
** Meta object code from reading C++ file 'payloadprocessor.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../payloadprocessor.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'payloadprocessor.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_PayloadProcessor_t {
    uint offsetsAndSizes[58];
    char stringdata0[17];
    char stringdata1[9];
    char stringdata2[1];
    char stringdata3[7];
    char stringdata4[4];
    char stringdata5[6];
    char stringdata6[11];
    char stringdata7[2];
    char stringdata8[2];
    char stringdata9[15];
    char stringdata10[2];
    char stringdata11[13];
    char stringdata12[7];
    char stringdata13[13];
    char stringdata14[4];
    char stringdata15[13];
    char stringdata16[3];
    char stringdata17[12];
    char stringdata18[4];
    char stringdata19[13];
    char stringdata20[11];
    char stringdata21[10];
    char stringdata22[9];
    char stringdata23[9];
    char stringdata24[9];
    char stringdata25[10];
    char stringdata26[15];
    char stringdata27[8];
    char stringdata28[13];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_PayloadProcessor_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_PayloadProcessor_t qt_meta_stringdata_PayloadProcessor = {
    {
        QT_MOC_LITERAL(0, 16),  // "PayloadProcessor"
        QT_MOC_LITERAL(17, 8),  // "uiUpdate"
        QT_MOC_LITERAL(26, 0),  // ""
        QT_MOC_LITERAL(27, 6),  // "source"
        QT_MOC_LITERAL(34, 3),  // "key"
        QT_MOC_LITERAL(38, 5),  // "value"
        QT_MOC_LITERAL(44, 10),  // "radarPoint"
        QT_MOC_LITERAL(55, 1),  // "x"
        QT_MOC_LITERAL(57, 1),  // "y"
        QT_MOC_LITERAL(59, 14),  // "velocityUpdate"
        QT_MOC_LITERAL(74, 1),  // "v"
        QT_MOC_LITERAL(76, 12),  // "motionUpdate"
        QT_MOC_LITERAL(89, 6),  // "motion"
        QT_MOC_LITERAL(96, 12),  // "debugMessage"
        QT_MOC_LITERAL(109, 3),  // "msg"
        QT_MOC_LITERAL(113, 12),  // "serialOpened"
        QT_MOC_LITERAL(126, 2),  // "ok"
        QT_MOC_LITERAL(129, 11),  // "serialError"
        QT_MOC_LITERAL(141, 3),  // "err"
        QT_MOC_LITERAL(145, 12),  // "fallDetected"
        QT_MOC_LITERAL(158, 10),  // "fallCancel"
        QT_MOC_LITERAL(169, 9),  // "heartBeat"
        QT_MOC_LITERAL(179, 8),  // "initPort"
        QT_MOC_LITERAL(188, 8),  // "portName"
        QT_MOC_LITERAL(197, 8),  // "readData"
        QT_MOC_LITERAL(206, 9),  // "closePort"
        QT_MOC_LITERAL(216, 14),  // "enqueuePayload"
        QT_MOC_LITERAL(231, 7),  // "payload"
        QT_MOC_LITERAL(239, 12)   // "prepareRadar"
    },
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
    "heartBeat",
    "initPort",
    "portName",
    "readData",
    "closePort",
    "enqueuePayload",
    "payload",
    "prepareRadar"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_PayloadProcessor[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    3,  104,    2, 0x06,    1 /* Public */,
       6,    3,  111,    2, 0x06,    5 /* Public */,
       9,    2,  118,    2, 0x06,    9 /* Public */,
      11,    2,  123,    2, 0x06,   12 /* Public */,
      13,    1,  128,    2, 0x06,   15 /* Public */,
      15,    1,  131,    2, 0x06,   17 /* Public */,
      17,    1,  134,    2, 0x06,   19 /* Public */,
      19,    1,  137,    2, 0x06,   21 /* Public */,
      20,    1,  140,    2, 0x06,   23 /* Public */,
      21,    1,  143,    2, 0x06,   25 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      22,    1,  146,    2, 0x0a,   27 /* Public */,
      24,    0,  149,    2, 0x0a,   29 /* Public */,
      25,    0,  150,    2, 0x0a,   30 /* Public */,
      26,    1,  151,    2, 0x0a,   31 /* Public */,
      28,    1,  154,    2, 0x0a,   33 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString,    3,    4,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::Double, QMetaType::Double,    3,    7,    8,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    3,   10,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    3,   12,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void, QMetaType::QString,   18,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,   23,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,   27,
    QMetaType::Void, QMetaType::QString,   23,

       0        // eod
};

Q_CONSTINIT const QMetaObject PayloadProcessor::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_PayloadProcessor.offsetsAndSizes,
    qt_meta_data_PayloadProcessor,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_PayloadProcessor_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PayloadProcessor, std::true_type>,
        // method 'uiUpdate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'radarPoint'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'velocityUpdate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'motionUpdate'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'debugMessage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'serialOpened'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'serialError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'fallDetected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'fallCancel'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'heartBeat'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'initPort'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'readData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'closePort'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'enqueuePayload'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QByteArray &, std::false_type>,
        // method 'prepareRadar'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString, std::false_type>
    >,
    nullptr
} };

void PayloadProcessor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PayloadProcessor *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->uiUpdate((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[3]))); break;
        case 1: _t->radarPoint((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[3]))); break;
        case 2: _t->velocityUpdate((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->motionUpdate((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 4: _t->debugMessage((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->serialOpened((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 6: _t->serialError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->fallDetected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->fallCancel((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->heartBeat((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 10: _t->initPort((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->readData(); break;
        case 12: _t->closePort(); break;
        case 13: _t->enqueuePayload((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 14: _t->prepareRadar((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PayloadProcessor::*)(const QString & , const QString & , const QString & );
            if (_t _q_method = &PayloadProcessor::uiUpdate; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PayloadProcessor::*)(const QString & , double , double );
            if (_t _q_method = &PayloadProcessor::radarPoint; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (PayloadProcessor::*)(const QString & , const QString & );
            if (_t _q_method = &PayloadProcessor::velocityUpdate; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (PayloadProcessor::*)(const QString & , const QString & );
            if (_t _q_method = &PayloadProcessor::motionUpdate; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (PayloadProcessor::*)(const QString & );
            if (_t _q_method = &PayloadProcessor::debugMessage; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (PayloadProcessor::*)(bool );
            if (_t _q_method = &PayloadProcessor::serialOpened; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (PayloadProcessor::*)(const QString & );
            if (_t _q_method = &PayloadProcessor::serialError; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (PayloadProcessor::*)(const QString & );
            if (_t _q_method = &PayloadProcessor::fallDetected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (PayloadProcessor::*)(const QString & );
            if (_t _q_method = &PayloadProcessor::fallCancel; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (PayloadProcessor::*)(const QString & );
            if (_t _q_method = &PayloadProcessor::heartBeat; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
    }
}

const QMetaObject *PayloadProcessor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PayloadProcessor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PayloadProcessor.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int PayloadProcessor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void PayloadProcessor::uiUpdate(const QString & _t1, const QString & _t2, const QString & _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PayloadProcessor::radarPoint(const QString & _t1, double _t2, double _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void PayloadProcessor::velocityUpdate(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void PayloadProcessor::motionUpdate(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void PayloadProcessor::debugMessage(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void PayloadProcessor::serialOpened(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void PayloadProcessor::serialError(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void PayloadProcessor::fallDetected(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void PayloadProcessor::fallCancel(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void PayloadProcessor::heartBeat(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
