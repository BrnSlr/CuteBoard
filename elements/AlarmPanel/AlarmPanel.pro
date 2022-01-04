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
DEFINES += ALARMPANEL_LIBRARY
DEFINES += QCUSTOMPLOT_USE_LIBRARY

TARGET = board_element_alarmpanel

QMAKE_TARGET_COMPANY = "BrnSlr for Github"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) by BrnSlr for Github"
QMAKE_TARGET_PRODUCT = "board_element_alarmpanel"
QMAKE_TARGET_DESCRIPTION = "AlarmPanel element library for CuteBoard"

INCLUDEPATH += ../../board
INCLUDEPATH += ../..
INCLUDEPATH += ../../3rdparty/log4qt/src

win32:LIBS += -L$$DESTDIR -lboard_data -lboard_ui -lqcustomplot2 -llog4qt
else:LIBS += -L$$DESTDIR -lboard_data -lboard_ui -lqcustomplot -llog4qt

SOURCES += \
    alarm_panel.cpp \
    alarm_panel_plugin.cpp

HEADERS += \
    alarm_panel.h \
    alarm_panel_plugin.h \
    alarm_panel_shared.h

RESOURCES += \
    iconAlarmPanel.qrc

DISTFILES += \
    alarm_panel.json

FORMS +=

