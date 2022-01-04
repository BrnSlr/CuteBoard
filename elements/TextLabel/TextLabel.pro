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
DEFINES += TEXTLABEL_LIBRARY
DEFINES += QCUSTOMPLOT_USE_LIBRARY

TARGET = board_element_textlabel

QMAKE_TARGET_COMPANY = "BrnSlr for Github"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) by BrnSlr for Github"
QMAKE_TARGET_PRODUCT = "board_element_textlabel"
QMAKE_TARGET_DESCRIPTION = "TextLabel element library for CuteBoard"

INCLUDEPATH += ../../board
INCLUDEPATH += ../..
INCLUDEPATH += ../../3rdparty/log4qt/src

win32:LIBS += -L$$DESTDIR  -llog4qt -lboard_data -lboard_ui -lqcustomplot2
else:LIBS += -L$$DESTDIR  -llog4qt -lboard_data -lboard_ui -lqcustomplot

SOURCES += \
    text_label.cpp \
    text_label_editor.cpp \
    text_label_plugin.cpp

HEADERS += \
    text_label.h \
    text_label_editor.h \
    text_label_plugin.h \
    text_label_shared.h

RESOURCES += \
    iconTextLabel.qrc

DISTFILES += \
    text_label.json

FORMS += \
    text_label_editor.ui

