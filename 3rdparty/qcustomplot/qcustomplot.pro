QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TEMPLATE = lib
CONFIG += c++11
static {
  CONFIG += static
} else {
  CONFIG += shared
}

CONFIG(debug, release|debug) {
    DESTDIR = $$PWD/../../build/debug
} else {
    DESTDIR = $$PWD/../../build/release
}

OBJECTS_DIR = $$DESTDIR/.obj/$$TARGET
MOC_DIR = $$DESTDIR/.moc/$$TARGET
RCC_DIR = $$DESTDIR/.qrc/$$TARGET
UI_DIR = $$DESTDIR/.ui/$$TARGET

QMAKE_RPATHDIR += $ORIGIN

DEFINES += QCUSTOMPLOT_COMPILE_LIBRARY

TARGET = qcustomplot
VERSION = 2.0.1

QMAKE_TARGET_COMPANY = "www.qcustomplot.com"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) by Emanuel Eichhammer"
QMAKE_TARGET_PRODUCT = "QCustomPlot"
QMAKE_TARGET_DESCRIPTION = "Plotting library for Qt"

INCLUDEPATH += ../log4qt/src
LIBS += -L$$DESTDIR -llog4qt

SOURCES += qcustomplot.cpp \
        axis_rect.cpp \
        graph.cpp \
        graph_tracer.cpp
HEADERS += qcustomplot.h \
        axis_rect.h \
        graph.h \
        graph_tracer.h
