QT       -= gui

TEMPLATE = lib
CONFIG += c++11

static {
  CONFIG += static
} else {
  CONFIG += shared
}

CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/../build/debug
} else {
    DESTDIR = $$PWD/../build/release
}

OBJECTS_DIR = $$DESTDIR/.obj/$$TARGET
MOC_DIR = $$DESTDIR/.moc/$$TARGET
RCC_DIR = $$DESTDIR/.qrc/$$TARGET
UI_DIR = $$DESTDIR/.ui/$$TARGET

QMAKE_RPATHDIR += $ORIGIN

DEFINES += DATA_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

TARGET = board_data

QMAKE_TARGET_COMPANY = "BrnSlr for Github"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) by BrnSlr for Github"
QMAKE_TARGET_PRODUCT = "board_data"
QMAKE_TARGET_DESCRIPTION = "Data management library for CuteBoard"

LIBS += -L$$DESTDIR -llog4qt
INCLUDEPATH += ../3rdparty/log4qt/src

SOURCES += \
    data_manager.cpp \
    time_series.cpp \
    time_series_parameter.cpp

HEADERS += \
    data.h \
    data_manager.h \
    data_global.h  \
    data_source.h \
    live_data_source_interface.h \
    playback_data_source_interface.h \
    time_series.h \
    time_series_parameter.h
