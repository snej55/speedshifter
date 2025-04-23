/****************************************************************************
** Meta object code from reading C++ file 'player.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/player.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'player.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.2. It"
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
struct qt_meta_tag_ZN6PlayerE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN6PlayerE = QtMocHelpers::stringData(
    "Player",
    "QML.Element",
    "auto",
    "filePathChanged",
    "",
    "playbackSpeedChanged",
    "durationChanged",
    "timeElapsedChanged",
    "playingChanged",
    "shouldSeekChanged",
    "onMediaStatusChanged",
    "QMediaPlayer::MediaStatus",
    "status",
    "play",
    "pause",
    "stop",
    "softStop",
    "seek",
    "pos",
    "force",
    "update_rate",
    "filePath",
    "playbackSpeed",
    "duration",
    "timeElapsed",
    "playing",
    "shouldSeek"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN6PlayerE[] = {

 // content:
      12,       // revision
       0,       // classname
       1,   14, // classinfo
      14,   16, // methods
       6,  128, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // classinfo: key, value
       1,    2,

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       3,    0,  100,    4, 0x06,    7 /* Public */,
       5,    0,  101,    4, 0x06,    8 /* Public */,
       6,    0,  102,    4, 0x06,    9 /* Public */,
       7,    0,  103,    4, 0x06,   10 /* Public */,
       8,    0,  104,    4, 0x06,   11 /* Public */,
       9,    0,  105,    4, 0x06,   12 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      10,    1,  106,    4, 0x0a,   13 /* Public */,

 // methods: name, argc, parameters, tag, flags, initial metatype offsets
      13,    0,  109,    4, 0x02,   15 /* Public */,
      14,    0,  110,    4, 0x02,   16 /* Public */,
      15,    0,  111,    4, 0x02,   17 /* Public */,
      16,    0,  112,    4, 0x02,   18 /* Public */,
      17,    3,  113,    4, 0x02,   19 /* Public */,
      17,    2,  120,    4, 0x22,   23 /* Public | MethodCloned */,
      17,    1,  125,    4, 0x22,   26 /* Public | MethodCloned */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 11,   12,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool, QMetaType::Bool,   18,   19,   20,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,   18,   19,
    QMetaType::Void, QMetaType::Int,   18,

 // properties: name, type, flags, notifyId, revision
      21, QMetaType::QString, 0x00015103, uint(0), 0,
      22, QMetaType::Float, 0x00015103, uint(1), 0,
      23, QMetaType::Int, 0x00015103, uint(2), 0,
      24, QMetaType::Int, 0x00015103, uint(3), 0,
      25, QMetaType::Bool, 0x00015001, uint(4), 0,
      26, QMetaType::Bool, 0x00015103, uint(5), 0,

       0        // eod
};

Q_CONSTINIT const QMetaObject Player::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_ZN6PlayerE.offsetsAndSizes,
    qt_meta_data_ZN6PlayerE,
    qt_static_metacall,
    nullptr,
    qt_metaTypeArray<
        // property 'filePath'
        QString,
        // property 'playbackSpeed'
        float,
        // property 'duration'
        int,
        // property 'timeElapsed'
        int,
        // property 'playing'
        bool,
        // property 'shouldSeek'
        bool,
        // Q_OBJECT / Q_GADGET
        Player,
        // method 'filePathChanged'
        void,
        // method 'playbackSpeedChanged'
        void,
        // method 'durationChanged'
        void,
        // method 'timeElapsedChanged'
        void,
        // method 'playingChanged'
        void,
        // method 'shouldSeekChanged'
        void,
        // method 'onMediaStatusChanged'
        void,
        QMediaPlayer::MediaStatus,
        // method 'play'
        void,
        // method 'pause'
        void,
        // method 'stop'
        void,
        // method 'softStop'
        void,
        // method 'seek'
        void,
        const int &,
        bool,
        bool,
        // method 'seek'
        void,
        const int &,
        bool,
        // method 'seek'
        void,
        const int &
    >,
    nullptr
} };

void Player::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Player *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->filePathChanged(); break;
        case 1: _t->playbackSpeedChanged(); break;
        case 2: _t->durationChanged(); break;
        case 3: _t->timeElapsedChanged(); break;
        case 4: _t->playingChanged(); break;
        case 5: _t->shouldSeekChanged(); break;
        case 6: _t->onMediaStatusChanged((*reinterpret_cast< std::add_pointer_t<QMediaPlayer::MediaStatus>>(_a[1]))); break;
        case 7: _t->play(); break;
        case 8: _t->pause(); break;
        case 9: _t->stop(); break;
        case 10: _t->softStop(); break;
        case 11: _t->seek((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 12: _t->seek((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[2]))); break;
        case 13: _t->seek((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (Player::*)();
            if (_q_method_type _q_method = &Player::filePathChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _q_method_type = void (Player::*)();
            if (_q_method_type _q_method = &Player::playbackSpeedChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _q_method_type = void (Player::*)();
            if (_q_method_type _q_method = &Player::durationChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _q_method_type = void (Player::*)();
            if (_q_method_type _q_method = &Player::timeElapsedChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _q_method_type = void (Player::*)();
            if (_q_method_type _q_method = &Player::playingChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _q_method_type = void (Player::*)();
            if (_q_method_type _q_method = &Player::shouldSeekChanged; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->filePath(); break;
        case 1: *reinterpret_cast< float*>(_v) = _t->playbackSpeed(); break;
        case 2: *reinterpret_cast< int*>(_v) = _t->getDuration(); break;
        case 3: *reinterpret_cast< int*>(_v) = _t->getTimeElapsed(); break;
        case 4: *reinterpret_cast< bool*>(_v) = _t->getPlaying(); break;
        case 5: *reinterpret_cast< bool*>(_v) = _t->getShouldSeek(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setFilePath(*reinterpret_cast< QString*>(_v)); break;
        case 1: _t->setPlaybackSpeed(*reinterpret_cast< float*>(_v)); break;
        case 2: _t->setDuration(*reinterpret_cast< int*>(_v)); break;
        case 3: _t->setTimeElapsed(*reinterpret_cast< int*>(_v)); break;
        case 5: _t->setShouldSeek(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *Player::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Player::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN6PlayerE.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Player::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 14;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void Player::filePathChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Player::playbackSpeedChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Player::durationChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Player::timeElapsedChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void Player::playingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void Player::shouldSeekChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
QT_WARNING_POP
