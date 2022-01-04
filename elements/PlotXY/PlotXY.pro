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
DEFINES += PLOT_XY_LIBRARY
DEFINES += QCUSTOMPLOT_USE_LIBRARY

TARGET = board_element_plotxy

QMAKE_TARGET_COMPANY = "BrnSlr for Github"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) by BrnSlr for Github"
QMAKE_TARGET_PRODUCT = "board_element_plotxy"
QMAKE_TARGET_DESCRIPTION = "Plot XY element library for CuteBoard"

INCLUDEPATH += ../../board
INCLUDEPATH += ../..
INCLUDEPATH += ../../3rdparty/log4qt/src
INCLUDEPATH += ../../3rdparty/csv

win32:LIBS += -L$$DESTDIR  -llog4qt -lboard_element_numericaldisplay -lboard_data -lboard_ui -lqcustomplot2
else:LIBS += -L$$DESTDIR  -llog4qt -lboard_element_numericaldisplay -lboard_data -lboard_ui -lqcustomplot

SOURCES += \
    plot_xy.cpp \
    plot_xy_editor.cpp \
    plot_xy_patron_editor.cpp \
    plot_xy_plugin.cpp

HEADERS += \
    ../../3rdparty/csv/csv.h \
    plot_xy.h \
    plot_xy_editor.h \
    plot_xy_patron_editor.h \
    plot_xy_plugin.h \
    plot_xy_shared.h

RESOURCES += \
    ../../ui/resources/icons/icons.qrc \
    iconPlotXY.qrc

DISTFILES += \
    plot_xy.json

FORMS += \
    plot_xy_editor.ui \
    plot_xy_patron_editor.ui

