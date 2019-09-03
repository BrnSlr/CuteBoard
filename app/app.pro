#-------------------------------------------------
#
# Project created by QtCreator 2019-04-04T10:07:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

DEFINES += QCUSTOMPLOT_USE_OPENGL

TARGET = CuteBoard
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

include(../CuteBoard.pri)

CONFIG(debug, debug|release) {
    DESTDIR = $$PROJECT_ROOT_DIRECTORY/build/debug
}
CONFIG(release, debug|release) {
    DESTDIR = $$PROJECT_ROOT_DIRECTORY/build/release
}

OBJECTS_DIR = $$PROJECT_ROOT_DIRECTORY/build/.obj
MOC_DIR = $$PROJECT_ROOT_DIRECTORY/build/.moc
RCC_DIR = $$PROJECT_ROOT_DIRECTORY/build/.qrc
UI_DIR = $$PROJECT_ROOT_DIRECTORY/build/.ui

RESOURCES += \
        ../resources/qdarkstyle/style.qrc \
        ../resources/fonts/fonts.qrc \
        ../resources/icons/icons.qrc \
        ../resources/logo/logo.qrc

RC_FILE += \
    ../resources/logo/cuteboard.rc

FORMS += \
    ../ui/dashboard_toolbar.ui \
    ../ui/element_editors/curves_time_editor.ui \
    ../ui/element_editors/curves_xy_editor.ui \
    ../ui/element_editors/state_display_editor.ui \
    ../ui/element_editors/value_bitfields_editor.ui \
    ../ui/element_editors/value_display_editor.ui \
    ../ui/element_editors/value_gauge_horizontal_editor.ui \
    ../ui/element_editors/value_gauge_radial_editor.ui \
    ../ui/element_editors/value_gauge_vertical_editor.ui \
    ../ui/dashboardwidget.ui \
    ../ui/elementpickerwidget.ui \
    ../ui/homewidget.ui \
    ../ui/mainwindow.ui \
    ../ui/pageeditordialog.ui \
    ../ui/pagepickerwidget.ui \
    ../ui/parameterpickerwidget.ui \
    ../ui/propertieseditordialog.ui \
    ../ui/propertiespickerwidget.ui \
    ../ui/propertieswidget.ui \
    ../ui/settings_dialog.ui \
    ../ui/statewidget.ui

HEADERS += \
    ../3rdparty/csv.h \
    ../3rdparty/qcustomplot.h \
    ../dashboard/elements/plot_time.h \
    ../dashboard/elements/plot_xy.h \
    ../dashboard/elements/state_display.h \
    ../dashboard/elements/value_bitfields.h \
    ../dashboard/elements/value_display.h \
    ../dashboard/elements/value_gauge_horizontal.h \
    ../dashboard/elements/value_gauge_radial.h \
    ../dashboard/elements/value_gauge_vertical.h \
    ../dashboard/elements_base/adjust_text_element.h \
    ../dashboard/elements_base/axisrect.h \
    ../dashboard/elements_base/circularaxis.h \
    ../dashboard/dashboard_element.h \
    ../dashboard/elements_base/elementhighlightedrect.h \
    ../dashboard/elements_base/elementresizehandle.h \
    ../dashboard/elements_base/labelpainter.h \
    ../dashboard/layouts/layout_list.h \
    ../dashboard/layouts/layout_reactive_element.h \
    ../dashboard/elements_base/single_display.h \
    ../dashboard/elements_factory/elementfactory.h \
    ../dashboard/dashboard.h \
    ../dashboard/layouts/layout_grid.h \
    ../dashboard/layouts/layout_reactive.h \
    ../dashboard/dashboard_parameter.h \
    ../data/data_buffer.h \
    ../data/data_common.h \
    ../project/bitfieldsmapping.h \
    ../project/colorsettings.h \
    ../project/page.h \
    ../project/parameter_configuration.h \
    ../project/project.h \
    ../project/statesmapping.h \
    ../project/thresholdsmapping.h \
    ../ui/dashboard_toolbar.h \
    ../ui/element_editors/curves_time_editor.h \
    ../ui/element_editors/curves_xy_editor.h \
    ../ui/element_editors/state_display_editor.h \
    ../ui/element_editors/value_bitfields_editor.h \
    ../ui/element_editors/value_display_editor.h \
    ../ui/element_editors/value_gauge_horizontal_editor.h \
    ../ui/element_editors/value_gauge_radial_editor.h \
    ../ui/element_editors/value_gauge_vertical_editor.h \
    ../ui/util/clickablelabel.h \
    ../ui/util/colorlineedit.h \
    ../ui/util/elementlistwidget.h \
    ../ui/util/parameterslistwidget.h \
    ../ui/dashboardwidget.h \
    ../ui/elementpickerwidget.h \
    ../ui/homewidget.h \
    ../ui/mainwindow.h \
    ../ui/pageeditordialog.h \
    ../ui/pagepickerwidget.h \
    ../ui/settings_dialog.h \
    ../ui/parameterpickerwidget.h \
    ../ui/propertieseditordialog.h \
    ../ui/propertiespickerwidget.h \
    ../ui/propertieswidget.h \
    ../ui/statewidget.h \
    ../data/data_source_interface.h \
    ../data/data_manager.h \
    ../data/data_sample.h \
    ../data/data_serie.h \
    ../data/data_parameter.h \
    ../data/data_value.h \
    ../ui/util/propertiestablewidget.h

SOURCES += \
    ../3rdparty/qcustomplot.cpp \
    ../app/main.cpp \
    ../dashboard/elements/plot_time.cpp \
    ../dashboard/elements/plot_xy.cpp \
    ../dashboard/elements/state_display.cpp \
    ../dashboard/elements/value_bitfields.cpp \
    ../dashboard/elements/value_display.cpp \
    ../dashboard/elements/value_gauge_horizontal.cpp \
    ../dashboard/elements/value_gauge_radial.cpp \
    ../dashboard/elements/value_gauge_vertical.cpp \
    ../dashboard/elements_base/adjust_text_element.cpp \
    ../dashboard/elements_base/axisrect.cpp \
    ../dashboard/elements_base/circularaxis.cpp \
    ../dashboard/dashboard_element.cpp \
    ../dashboard/elements_base/elementhighlightedrect.cpp \
    ../dashboard/elements_base/elementresizehandle.cpp \
    ../dashboard/elements_base/labelpainter.cpp \
    ../dashboard/layouts/layout_list.cpp \
    ../dashboard/layouts/layout_reactive_element.cpp \
    ../dashboard/elements_base/single_display.cpp \
    ../dashboard/dashboard_parameter.cpp \
    ../dashboard/dashboard.cpp \
    ../dashboard/layouts/layout_grid.cpp \
    ../dashboard/layouts/layout_reactive.cpp \
    ../data/data_buffer.cpp \
    ../data/data_parameter.cpp \
    ../project/bitfieldsmapping.cpp \
    ../project/colorsettings.cpp \
    ../project/page.cpp \
    ../project/parameter_configuration.cpp \
    ../project/project.cpp \
    ../project/statesmapping.cpp \
    ../project/thresholdsmapping.cpp \
    ../ui/dashboard_toolbar.cpp \
    ../ui/element_editors/curves_time_editor.cpp \
    ../ui/element_editors/curves_xy_editor.cpp \
    ../ui/element_editors/state_display_editor.cpp \
    ../ui/element_editors/value_bitfields_editor.cpp \
    ../ui/element_editors/value_display_editor.cpp \
    ../ui/element_editors/value_gauge_horizontal_editor.cpp \
    ../ui/element_editors/value_gauge_radial_editor.cpp \
    ../ui/element_editors/value_gauge_vertical_editor.cpp \
    ../ui/util/clickablelabel.cpp \
    ../ui/util/colorlineedit.cpp \
    ../ui/util/elementlistwidget.cpp \
    ../ui/util/parameterslistwidget.cpp \
    ../ui/dashboardwidget.cpp \
    ../ui/elementpickerwidget.cpp \
    ../ui/homewidget.cpp \
    ../ui/settings_dialog.cpp \
    ../ui/mainwindow.cpp \
    ../ui/pageeditordialog.cpp \
    ../ui/pagepickerwidget.cpp \
    ../ui/parameterpickerwidget.cpp \
    ../ui/propertieseditordialog.cpp \
    ../ui/propertiespickerwidget.cpp \
    ../ui/propertieswidget.cpp \
    ../ui/statewidget.cpp \
    ../data/data_manager.cpp \
    ../ui/util/propertiestablewidget.cpp

INCLUDEPATH += ../
