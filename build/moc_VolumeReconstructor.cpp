/****************************************************************************
** Meta object code from reading C++ file 'VolumeReconstructor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../VolumeReconstructor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'VolumeReconstructor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_VolumeReconstructor_t {
    QByteArrayData data[11];
    char stringdata[129];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_VolumeReconstructor_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_VolumeReconstructor_t qt_meta_stringdata_VolumeReconstructor = {
    {
QT_MOC_LITERAL(0, 0, 19),
QT_MOC_LITERAL(1, 20, 14),
QT_MOC_LITERAL(2, 35, 0),
QT_MOC_LITERAL(3, 36, 8),
QT_MOC_LITERAL(4, 45, 13),
QT_MOC_LITERAL(5, 59, 9),
QT_MOC_LITERAL(6, 69, 15),
QT_MOC_LITERAL(7, 85, 11),
QT_MOC_LITERAL(8, 97, 13),
QT_MOC_LITERAL(9, 111, 5),
QT_MOC_LITERAL(10, 117, 11)
    },
    "VolumeReconstructor\0generateVolume\0\0"
    "inputTSV\0readPNGImages\0directory\0"
    "setOutputExtent\0insertSlice\0vtkImageData*\0"
    "image\0resetOutput"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_VolumeReconstructor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x0a /* Public */,
       4,    1,   42,    2, 0x0a /* Public */,
       6,    0,   45,    2, 0x0a /* Public */,
       7,    1,   46,    2, 0x0a /* Public */,
      10,    0,   49,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Int, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void,
    QMetaType::Int, 0x80000000 | 8,    9,
    QMetaType::Int,

       0        // eod
};

void VolumeReconstructor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        VolumeReconstructor *_t = static_cast<VolumeReconstructor *>(_o);
        switch (_id) {
        case 0: { int _r = _t->generateVolume((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 1: _t->readPNGImages((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->setOutputExtent(); break;
        case 3: { int _r = _t->insertSlice((*reinterpret_cast< vtkImageData*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 4: { int _r = _t->resetOutput();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObject VolumeReconstructor::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_VolumeReconstructor.data,
      qt_meta_data_VolumeReconstructor,  qt_static_metacall, 0, 0}
};


const QMetaObject *VolumeReconstructor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VolumeReconstructor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VolumeReconstructor.stringdata))
        return static_cast<void*>(const_cast< VolumeReconstructor*>(this));
    return QObject::qt_metacast(_clname);
}

int VolumeReconstructor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
