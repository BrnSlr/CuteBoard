QT       += widgets

TEMPLATE = lib
CONFIG += plugin
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
DEFINES += GAUGE_HORIZONTAL_LIBRARY
DEFINES += QCUSTOMPLOT_USE_LIBRARY

TARGET = board_element_gaugehorizontal

QMAKE_TARGET_COMPANY = "BrnSlr for Github"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) by BrnSlr for Github"
QMAKE_TARGET_PRODUCT = "board_element_gaugehorizontal"
QMAKE_TARGET_DESCRIPTION = "Gauge Horizontal element library for CuteBoard"

INCLUDEPATH += ../../board
INCLUDEPATH += ../..
INCLUDEPATH += ../../3rdparty/log4qt/src

win32:LIBS += -L$$DESTDIR  -llog4qt -lboard_element_gaugevertical -lboard_element_numericaldisplay -lboard_data -lboard_ui -lqcustomplot2
else:LIBS += -L$$DESTDIR  -llog4qt -lboard_element_gaugevertical -lboard_element_numericaldisplay -lboard_data -lboard_ui -lqcustomplot

SOURCES += \
    gauge_horizontal.cpp \
    gauge_horizontal_editor.cpp \
    gauge_horizontal_plugin.cpp

HEADERS += \
    gauge_horizontal.h \
    gauge_horizontal_editor.h \
    gauge_horizontal_plugin.h \
    gauge_horizontal_shared.h

RESOURCES += \
    iconGaugeHorizontal.qrc

DISTFILES += \
    gauge_horizontal.json

FORMS += \
    gauge_horizontal_editor.ui

