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
DEFINES += NUMERICAL_DISPLAY_LIBRARY
DEFINES += QCUSTOMPLOT_USE_LIBRARY

TARGET = board_element_numericaldisplay

QMAKE_TARGET_COMPANY = "BrnSlr for Github"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) by BrnSlr for Github"
QMAKE_TARGET_PRODUCT = "board_element_numericaldisplay"
QMAKE_TARGET_DESCRIPTION = "Numerical display element library for CuteBoard"

INCLUDEPATH += ../../board
INCLUDEPATH += ../..
INCLUDEPATH += ../../3rdparty/log4qt/src

win32:LIBS += -L$$DESTDIR  -llog4qt -lboard_data -lboard_ui -lqcustomplot2
else:LIBS += -L$$DESTDIR  -llog4qt -lboard_data -lboard_ui -lqcustomplot

SOURCES += \
        numerical_display.cpp \
        numerical_display_editor.cpp \
        numerical_display_plugin.cpp

HEADERS += \
        numerical_display.h \
        numerical_display_editor.h \
        numerical_display_plugin.h \
        numerical_display_shared.h

RESOURCES += \
    iconNumericalDisplay.qrc

DISTFILES += \
    numerical_display.json

FORMS += \
    numerical_display_editor.ui

