QT += core xml network concurrent
include(../../build.pri)
include(../../g++.pri)
include(log4qt.pri)

CONFIG += c++14 \
          hide_symbols

contains(DEFINES, LOG4QT_STATIC) {
    message(Building static log4qt...)
    CONFIG += staticlib
}

TEMPLATE = lib
TARGET = log4qt
QT -= gui

# .. is needed for msvc since it is treating '.' as the directory of the current file
# and not the directory where the compiled source is found
INCLUDEPATH += .. .

CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/../../../../build/debug
} else {
    DESTDIR = $$PWD/../../../../build/release
}


OBJECTS_DIR = $$DESTDIR/.obj/$$TARGET
MOC_DIR = $$DESTDIR/.moc/$$TARGET
RCC_DIR = $$DESTDIR/.qrc/$$TARGET
UI_DIR = $$DESTDIR/.ui/$$TARGET

DEFINES += NOMINMAX QT_DEPRECATED_WARNINGS QT_NO_CAST_FROM_BYTEARRAY QT_USE_QSTRINGBUILDER
DEFINES += LOG4QT_LIBRARY
