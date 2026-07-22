/****************************************************************************
** Meta object code from reading C++ file 'brightness.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../brightness.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'brightness.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_brightness_t {
    uint offsetsAndSizes[18];
    char stringdata0[11];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[21];
    char stringdata4[21];
    char stringdata5[8];
    char stringdata6[14];
    char stringdata7[6];
    char stringdata8[21];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_brightness_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_brightness_t qt_meta_stringdata_brightness = {
    {
        QT_MOC_LITERAL(0, 10),  // "brightness"
        QT_MOC_LITERAL(11, 13),  // "getBrightness"
        QT_MOC_LITERAL(25, 0),  // ""
        QT_MOC_LITERAL(26, 20),  // "getBrightnessPercent"
        QT_MOC_LITERAL(47, 20),  // "setBrightnessPercent"
        QT_MOC_LITERAL(68, 7),  // "percent"
        QT_MOC_LITERAL(76, 13),  // "setBrightness"
        QT_MOC_LITERAL(90, 5),  // "value"
        QT_MOC_LITERAL(96, 20)   // "getBacklightBasePath"
    },
    "brightness",
    "getBrightness",
    "",
    "getBrightnessPercent",
    "setBrightnessPercent",
    "percent",
    "setBrightness",
    "value",
    "getBacklightBasePath"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_brightness[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   44,    2, 0x0a,    1 /* Public */,
       3,    0,   45,    2, 0x0a,    2 /* Public */,
       4,    1,   46,    2, 0x0a,    3 /* Public */,
       6,    1,   49,    2, 0x0a,    5 /* Public */,
       8,    0,   52,    2, 0x0a,    7 /* Public */,

 // slots: parameters
    QMetaType::Int,
    QMetaType::Int,
    QMetaType::Bool, QMetaType::Int,    5,
    QMetaType::Bool, QMetaType::Int,    7,
    QMetaType::QString,

       0        // eod
};

Q_CONSTINIT const QMetaObject brightness::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_brightness.offsetsAndSizes,
    qt_meta_data_brightness,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_brightness_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<brightness, std::true_type>,
        // method 'getBrightness'
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'getBrightnessPercent'
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'setBrightnessPercent'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'setBrightness'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'getBacklightBasePath'
        QtPrivate::TypeAndForceComplete<QString, std::false_type>
    >,
    nullptr
} };

void brightness::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<brightness *>(_o);
        (void)_t;
        switch (_id) {
        case 0: { int _r = _t->getBrightness();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 1: { int _r = _t->getBrightnessPercent();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 2: { bool _r = _t->setBrightnessPercent((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 3: { bool _r = _t->setBrightness((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 4: { QString _r = _t->getBacklightBasePath();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
}

const QMetaObject *brightness::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *brightness::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_brightness.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int brightness::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
