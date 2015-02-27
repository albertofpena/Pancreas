/****************************************************************************
** Meta object code from reading C++ file 'Parser.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Parser.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Parser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Parser_t {
    QByteArrayData data[7];
    char stringdata[62];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Parser_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Parser_t qt_meta_stringdata_Parser = {
    {
QT_MOC_LITERAL(0, 0, 6),
QT_MOC_LITERAL(1, 7, 12),
QT_MOC_LITERAL(2, 20, 0),
QT_MOC_LITERAL(3, 21, 8),
QT_MOC_LITERAL(4, 30, 10),
QT_MOC_LITERAL(5, 41, 9),
QT_MOC_LITERAL(6, 51, 10)
    },
    "Parser\0parseTSVFile\0\0inputTSV\0vtkPoints*\0"
    "positions\0directions"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Parser[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    3,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, 0x80000000 | 4, 0x80000000 | 4,    3,    5,    6,

       0        // eod
};

void Parser::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Parser *_t = static_cast<Parser *>(_o);
        switch (_id) {
        case 0: _t->parseTSVFile((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< vtkPoints*(*)>(_a[2])),(*reinterpret_cast< vtkPoints*(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObject Parser::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Parser.data,
      qt_meta_data_Parser,  qt_static_metacall, 0, 0}
};


const QMetaObject *Parser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Parser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Parser.stringdata))
        return static_cast<void*>(const_cast< Parser*>(this));
    return QObject::qt_metacast(_clname);
}

int Parser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
