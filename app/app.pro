QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TEMPLATE = app
CONFIG += c++11

CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/../build/debug
} else {
    DESTDIR = $$PWD/../build/release
}

OBJECTS_DIR = $$DESTDIR/.obj/$$TARGET
MOC_DIR = $$DESTDIR/.moc/$$TARGET
RCC_DIR = $$DESTDIR/.qrc/$$TARGET
UI_DIR = $$DESTDIR/.ui/$$TARGET

QMAKE_RPATHDIR += $ORIGIN
QMAKE_RPATHDIR += $ORIGIN/../lib

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QCUSTOMPLOT_USE_LIBRARY

TARGET = CuteBoard

QMAKE_TARGET_COMPANY = "BrnSlr for Github"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) by BrnSlr for Github"
QMAKE_TARGET_PRODUCT = "CuteBoard"
QMAKE_TARGET_DESCRIPTION = "Real time dashboard"

win32:LIBS += -L$$DESTDIR -lboard_data -lboard_ui -lqcustomplot2 -llog4qt
else:LIBS += -L$$DESTDIR -lboard_data -lboard_ui -lqcustomplot -llog4qt

INCLUDEPATH += ..
INCLUDEPATH += ../3rdparty/log4qt/src

RESOURCES += \
        $$PWD/resources/qdarkstyle/style.qrc \
        $$PWD/resources/logo/logo.qrc

RC_ICONS = icon.ico

SOURCES += \
    main.cpp

OTHER_FILES += \
    $$PWD/data_sources.profiles \
    $$PWD/CuteBoard.ini \
    $$PWD/palettes.ini

DISTFILES +=  \
    CuteBoard.desktop

defineTest(copyToDestDir) {
    files = $$1
    dir = $$2
    # replace slashes in destination path for Windows
    win32:dir ~= s,/,\\,g

    for(file, files) {
        # replace slashes in source path for Windows
        win32:file ~= s,/,\\,g

        QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$shell_quote($$file) $$shell_quote($$dir) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

copyToDestDir($$OTHER_FILES, $$DESTDIR)

