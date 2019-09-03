QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport xml

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QCUSTOMPLOT_USE_OPENGL

HEADERS += \
    $$PWD/../3rdparty/csv.h \
    $$PWD/../3rdparty/qcustomplot.h \
    $$PWD/data_buffer.h \
    $$PWD/data_common.h \
    $$PWD/data_source_interface.h \
    $$PWD/data_manager.h \
    $$PWD/data_sample.h \
    $$PWD/data_serie.h \
    $$PWD/data_parameter.h \
    $$PWD/data_value.h

SOURCES += \
    $$PWD/../3rdparty/qcustomplot.cpp \
    $$PWD/data_buffer.cpp \
    $$PWD/data_parameter.cpp \
    $$PWD/data_manager.cpp

INCLUDEPATH += $$PWD/..
