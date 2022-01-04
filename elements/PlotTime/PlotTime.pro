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
DEFINES += PLOT_TIME_LIBRARY
DEFINES += QCUSTOMPLOT_USE_LIBRARY

TARGET = board_element_plottime

QMAKE_TARGET_COMPANY = "BrnSlr for Github"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) by BrnSlr for Github"
QMAKE_TARGET_PRODUCT = "board_element_plottime"
QMAKE_TARGET_DESCRIPTION = "PlotTime element library for CuteBoard"

INCLUDEPATH += ../../board
INCLUDEPATH += ../..
INCLUDEPATH += ../../3rdparty/log4qt/src

win32:LIBS += -L$$DESTDIR  -llog4qt -lboard_element_numericaldisplay -lboard_data -lboard_ui -lqcustomplot2
else:LIBS += -L$$DESTDIR  -llog4qt -lboard_element_numericaldisplay -lboard_data -lboard_ui -lqcustomplot

SOURCES += \
    plot_time.cpp \
    plot_time_editor.cpp \
    plot_time_plugin.cpp

HEADERS += \
    plot_time.h \
    plot_time_editor.h \
    plot_time_plugin.h \
    plot_time_shared.h

RESOURCES += \
    iconPlotTime.qrc

DISTFILES += \
    plot_time.json

FORMS += \
    plot_time_editor.ui

