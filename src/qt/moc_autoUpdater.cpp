/****************************************************************************
** Meta object code from reading C++ file 'autoUpdater.h'
**
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qt/autoUpdater.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'autoUpdater.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AutoUpdater_t {
    QByteArrayData data[20];
    char stringdata0[251];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AutoUpdater_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AutoUpdater_t qt_meta_stringdata_AutoUpdater = {
    {
QT_MOC_LITERAL(0, 0, 11), // "AutoUpdater"
QT_MOC_LITERAL(1, 12, 7), // "message"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 5), // "title"
QT_MOC_LITERAL(4, 27, 5), // "style"
QT_MOC_LITERAL(5, 33, 15), // "updateAvailable"
QT_MOC_LITERAL(6, 49, 11), // "updateReady"
QT_MOC_LITERAL(7, 61, 11), // "updateError"
QT_MOC_LITERAL(8, 73, 5), // "error"
QT_MOC_LITERAL(9, 79, 12), // "startUpdater"
QT_MOC_LITERAL(10, 92, 23), // "checkForUpdatesPeriodic"
QT_MOC_LITERAL(11, 116, 20), // "onNetRequestFinished"
QT_MOC_LITERAL(12, 137, 14), // "QNetworkReply*"
QT_MOC_LITERAL(13, 152, 5), // "reply"
QT_MOC_LITERAL(14, 158, 20), // "checkVersionFinished"
QT_MOC_LITERAL(15, 179, 16), // "downloadFinished"
QT_MOC_LITERAL(16, 196, 13), // "updaterExited"
QT_MOC_LITERAL(17, 210, 8), // "exitCode"
QT_MOC_LITERAL(18, 219, 20), // "QProcess::ExitStatus"
QT_MOC_LITERAL(19, 240, 10) // "exitStatus"

    },
    "AutoUpdater\0message\0\0title\0style\0"
    "updateAvailable\0updateReady\0updateError\0"
    "error\0startUpdater\0checkForUpdatesPeriodic\0"
    "onNetRequestFinished\0QNetworkReply*\0"
    "reply\0checkVersionFinished\0downloadFinished\0"
    "updaterExited\0exitCode\0QProcess::ExitStatus\0"
    "exitStatus"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AutoUpdater[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   64,    2, 0x06 /* Public */,
       5,    0,   71,    2, 0x06 /* Public */,
       6,    0,   72,    2, 0x06 /* Public */,
       7,    1,   73,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   76,    2, 0x0a /* Public */,
      10,    0,   77,    2, 0x08 /* Private */,
      11,    1,   78,    2, 0x08 /* Private */,
      14,    1,   81,    2, 0x08 /* Private */,
      15,    1,   84,    2, 0x08 /* Private */,
      16,    2,   87,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::UInt,    3,    1,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 18,   17,   19,

       0        // eod
};

void AutoUpdater::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AutoUpdater *_t = static_cast<AutoUpdater *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->message((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< uint(*)>(_a[3]))); break;
        case 1: _t->updateAvailable(); break;
        case 2: _t->updateReady(); break;
        case 3: _t->updateError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->startUpdater(); break;
        case 5: _t->checkForUpdatesPeriodic(); break;
        case 6: _t->onNetRequestFinished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 7: _t->checkVersionFinished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 8: _t->downloadFinished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 9: _t->updaterExited((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AutoUpdater::*_t)(const QString & , const QString & , unsigned int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AutoUpdater::message)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (AutoUpdater::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AutoUpdater::updateAvailable)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (AutoUpdater::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AutoUpdater::updateReady)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (AutoUpdater::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AutoUpdater::updateError)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject AutoUpdater::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_AutoUpdater.data,
      qt_meta_data_AutoUpdater,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *AutoUpdater::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AutoUpdater::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_AutoUpdater.stringdata0))
        return static_cast<void*>(const_cast< AutoUpdater*>(this));
    return QObject::qt_metacast(_clname);
}

int AutoUpdater::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void AutoUpdater::message(const QString & _t1, const QString & _t2, unsigned int _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void AutoUpdater::updateAvailable()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void AutoUpdater::updateReady()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void AutoUpdater::updateError(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
