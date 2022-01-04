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
DEFINES += GAUGE_RADIAL_LIBRARY
DEFINES += QCUSTOMPLOT_USE_LIBRARY

TARGET = board_element_gaugeradial

QMAKE_TARGET_COMPANY = "BrnSlr for Github"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) by BrnSlr for Github"
QMAKE_TARGET_PRODUCT = "board_element_gaugeradial"
QMAKE_TARGET_DESCRIPTION = "Gauge Radial element library for CuteBoard"

INCLUDEPATH += ../../board
INCLUDEPATH += ../..
INCLUDEPATH += ../../3rdparty/log4qt/src

win32:LIBS += -L$$DESTDIR  -llog4qt -lboard_element_numericaldisplay -lboard_data -lboard_ui -lqcustomplot2
else:LIBS += -L$$DESTDIR  -llog4qt -lboard_element_numericaldisplay -lboard_data -lboard_ui -lqcustomplot

SOURCES += \
    gauge_radial.cpp \
    gauge_radial_axis.cpp \
#    gauge_radial_axispainter.cpp \
    gauge_radial_axisrect.cpp \
    gauge_radial_editor.cpp \
    gauge_radial_plugin.cpp

HEADERS += \
    gauge_radial.h \
    gauge_radial_axis.h \
#    gauge_radial_axispainter.h \
    gauge_radial_axisrect.h \
    gauge_radial_editor.h \
    gauge_radial_plugin.h \
    gauge_radial_shared.h

RESOURCES += \
    iconGaugeRadial.qrc

DISTFILES += \
    gauge_radial.json

FORMS += \
    gauge_radial_editor.ui

