/****************************************************************************
** Meta object code from reading C++ file 'volume.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../volume.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'volume.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_volume_t {
    uint offsetsAndSizes[14];
    char stringdata0[7];
    char stringdata1[17];
    char stringdata2[1];
    char stringdata3[17];
    char stringdata4[8];
    char stringdata5[5];
    char stringdata6[7];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_volume_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_volume_t qt_meta_stringdata_volume = {
    {
        QT_MOC_LITERAL(0, 6),  // "volume"
        QT_MOC_LITERAL(7, 16),  // "getVolumePercent"
        QT_MOC_LITERAL(24, 0),  // ""
        QT_MOC_LITERAL(25, 16),  // "setVolumePercent"
        QT_MOC_LITERAL(42, 7),  // "percent"
        QT_MOC_LITERAL(50, 4),  // "mute"
        QT_MOC_LITERAL(55, 6)   // "enable"
    },
    "volume",
    "getVolumePercent",
    "",
    "setVolumePercent",
    "percent",
    "mute",
    "enable"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_volume[] = {

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
       5,    1,   36,    2, 0x0a,    4 /* Public */,

 // slots: parameters
    QMetaType::Int,
    QMetaType::Bool, QMetaType::Int,    4,
    QMetaType::Bool, QMetaType::Bool,    6,

       0        // eod
};

Q_CONSTINIT const QMetaObject volume::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_volume.offsetsAndSizes,
    qt_meta_data_volume,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_volume_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<volume, std::true_type>,
        // method 'getVolumePercent'
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'setVolumePercent'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'mute'
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>
    >,
    nullptr
} };

void volume::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<volume *>(_o);
        (void)_t;
        switch (_id) {
        case 0: { int _r = _t->getVolumePercent();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 1: { bool _r = _t->setVolumePercent((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 2: { bool _r = _t->mute((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
}

const QMetaObject *volume::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *volume::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_volume.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int volume::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
