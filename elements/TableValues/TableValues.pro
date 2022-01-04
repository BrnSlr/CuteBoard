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
DEFINES += TABLEVALUES_LIBRARY
DEFINES += QCUSTOMPLOT_USE_LIBRARY

TARGET = board_element_tablevalues

QMAKE_TARGET_COMPANY = "BrnSlr for Github"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) by BrnSlr for Github"
QMAKE_TARGET_PRODUCT = "board_element_tablevalues"
QMAKE_TARGET_DESCRIPTION = "Table element library for CuteBoard"

INCLUDEPATH += ../../board
INCLUDEPATH += ../..
INCLUDEPATH += ../../3rdparty/log4qt/src

win32:LIBS += -L$$DESTDIR  -llog4qt -lboard_data -lboard_ui -lqcustomplot2
else:LIBS += -L$$DESTDIR  -llog4qt -lboard_data -lboard_ui -lqcustomplot

SOURCES += \
    table_values.cpp \
    table_values_editor.cpp \
    table_values_plugin.cpp

HEADERS += \
    table_values.h \
    table_values_editor.h \
    table_values_plugin.h \
    table_values_shared.h

RESOURCES += \
    iconTableValues.qrc

DISTFILES += \
    table_values.json

FORMS += \
    table_values_editor.ui

