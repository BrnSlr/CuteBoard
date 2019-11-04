#-------------------------------------------------
#
# Project created by QtCreator 2019-08-30T10:13:56
#
#-------------------------------------------------

QT       -= gui

TARGET = DemoDataSource
TEMPLATE = lib

DEFINES += DEMODATASOURCE_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../../CuteBoard.pri)

CONFIG(debug, debug|release) {
    DESTDIR = $$PROJECT_ROOT_DIRECTORY/build/debug/DataSources/$$TARGET
}
CONFIG(release, debug|release) {
    DESTDIR = $$PROJECT_ROOT_DIRECTORY/build/release/DataSources/$$TARGET
}

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

include($$PROJECT_ROOT_DIRECTORY/data/data.pri)

SOURCES += \
        demodatasource.cpp \
        demoparameter.cpp

HEADERS += \
        demodatasource.h \
        demoparameter.h

