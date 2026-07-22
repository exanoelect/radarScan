/****************************************************************************
** Meta object code from reading C++ file 'gpio.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../gpio.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gpio.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_gpio_t {
    uint offsetsAndSizes[16];
    char stringdata0[5];
    char stringdata1[10];
    char stringdata2[1];
    char stringdata3[9];
    char stringdata4[6];
    char stringdata5[7];
    char stringdata6[4];
    char stringdata7[6];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_gpio_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_gpio_t qt_meta_stringdata_gpio = {
    {
        QT_MOC_LITERAL(0, 4),  // "gpio"
        QT_MOC_LITERAL(5, 9),  // "setupGPIO"
        QT_MOC_LITERAL(15, 0),  // ""
        QT_MOC_LITERAL(16, 8),  // "setColor"
        QT_MOC_LITERAL(25, 5),  // "color"
        QT_MOC_LITERAL(31, 6),  // "setPin"
        QT_MOC_LITERAL(38, 3),  // "pin"
        QT_MOC_LITERAL(42, 5)   // "level"
    },
    "gpio",
    "setupGPIO",
    "",
    "setColor",
    "color",
    "setPin",
    "pin",
    "level"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_gpio[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   32,    2, 0x0a,    1 /* Public */,
       3,    1,   33,    2, 0x0a,    2 /* Public */,
       5,    2,   36,    2, 0x0a,    4 /* Public */,

 // slots: parameters
    QMetaType::Int,
    QMetaType::Void, QMetaType::SChar,    4,
    QMetaType::Void, QMetaType::UChar, QMetaType::UChar,    6,    7,

       0        // eod
};

Q_CONSTINIT const QMetaObject gpio::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_gpio.offsetsAndSizes,
    qt_meta_data_gpio,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_gpio_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<gpio, std::true_type>,
        // method 'setupGPIO'
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'setColor'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint8, std::false_type>,
        // method 'setPin'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint8, std::false_type>,
        QtPrivate::TypeAndForceComplete<quint8, std::false_type>
    >,
    nullptr
} };

void gpio::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<gpio *>(_o);
        (void)_t;
        switch (_id) {
        case 0: { int _r = _t->setupGPIO();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 1: _t->setColor((*reinterpret_cast< std::add_pointer_t<qint8>>(_a[1]))); break;
        case 2: _t->setPin((*reinterpret_cast< std::add_pointer_t<quint8>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<quint8>>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject *gpio::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *gpio::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_gpio.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int gpio::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 3;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
