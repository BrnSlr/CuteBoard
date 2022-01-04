QT       -= gui

TEMPLATE = lib
CONFIG += c++11

CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/../../build/debug
} else {
    DESTDIR = $$PWD/../../build/release
}

OBJECTS_DIR = $$DESTDIR/.obj/$$TARGET
MOC_DIR = $$DESTDIR/.moc/$$TARGET
RCC_DIR = $$DESTDIR/.qrc/$$TARGET
UI_DIR = $$DESTDIR/.ui/$$TARGET

QMAKE_RPATHDIR += $ORIGIN

DEFINES += QT_DEPRECATED_WARNINGS

TARGET = datasrc_live_demo

QMAKE_TARGET_COMPANY = "BrnSlr for Github"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) by BrnSlr for Github"
QMAKE_TARGET_PRODUCT = "datasrc_live_demo"
QMAKE_TARGET_DESCRIPTION = "Live demo library for CuteBoard"

win32:LIBS += -L$$DESTDIR -lboard_data -llog4qt
else:LIBS += -L$$DESTDIR -lboard_data -llog4qt

INCLUDEPATH += ../../data
INCLUDEPATH += ../../3rdparty/log4qt/src

SOURCES += \
        demodatasource.cpp \
        demoparameter.cpp

HEADERS += \
        demodatasource.h \
        demoparameter.h

DISTFILES += \
    demodatasource.json

