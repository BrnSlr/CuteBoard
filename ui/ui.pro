QT       += widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += printsupport

TEMPLATE = lib
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

DEFINES += BOARD_UI_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QCUSTOMPLOT_USE_LIBRARY

TARGET = board_ui

QMAKE_TARGET_COMPANY = "BrnSlr for Github"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) by BrnSlr for Github"
QMAKE_TARGET_PRODUCT = "board_ui"
QMAKE_TARGET_DESCRIPTION = "Ui library for CuteBoard"

include ($$PWD/../3rdparty/QSimpleUpdater/QSimpleUpdater.pri)

INCLUDEPATH += ..
INCLUDEPATH += ../3rdparty/log4qt/src

win32:LIBS += -L$$DESTDIR  -llog4qt -lboard_data -lqcustomplot2
else:LIBS += -L$$DESTDIR  -llog4qt -lboard_data -lqcustomplot

SOURCES += \
    ../3rdparty/qpluginfactory.cpp \
    board/board.cpp \
    board/board_element.cpp \
    board/board_layout.cpp \
    board/board_parameter.cpp \
    board/factory/element_factory_helper.cpp \
    board/utils/adaptive_text.cpp \
    board/utils/layout_grid.cpp \
    board/utils/layout_list.cpp \
    board/utils/single_display.cpp \
    board/management/alarms.cpp \
    board/management/bitfield.cpp \
    board/management/color_settings.cpp \
    board/management/curve_patron.cpp \
    board/management/page.cpp \
    board/management/parameter_configuration.cpp \
    board/management/project.cpp \
    board/management/states.cpp \
    board/management/thresholds.cpp \
    board/management/utils/string_util.cpp \
    board/utils/resize_handle.cpp \
    board/utils/selection_rect.cpp \
    board/board_page.cpp \
    board/board_toolbar.cpp \
    board/board_widget.cpp \
    home/home_page.cpp \
    widgets/add_parameter_widget.cpp \
    widgets/alarm_configuration_widget.cpp \
    mainwindow.cpp \
    board/chrono/chrono.cpp \
    board/chrono/digitdisplay.cpp \
    board/chrono/lap.cpp \
    board/chrono/lapmodel.cpp \
    board/chrono/session.cpp \
    board/chrono/stopwatch.cpp \
    board/chrono/timedisplay.cpp \
    board/chrono/timeformat.cpp \
    settings/settings_dialog.cpp \
    widgets/element_picker_widget.cpp \
    widgets/find_param_widget.cpp \
    widgets/page_editor_dialog.cpp \
    widgets/page_picker_widget.cpp \
    widgets/parameter_picker_widget.cpp \
    widgets/properties_picker_widget.cpp \
    widgets/properties_widget.cpp \
    widgets/utils/alarms_list_widget.cpp \
    widgets/utils/calendar_widget.cpp \
    widgets/utils/clickable_label.cpp \
    ../common/color_line_edit.cpp \
    widgets/utils/element_list_widget.cpp \
    widgets/utils/parameters_list_widget.cpp \
    widgets/utils/properties_table_widget.cpp \
    widgets/utils/time_button.cpp

HEADERS += \
    ../3rdparty/qpluginfactory.h \
    board/board.h \
    board/board_element.h \
    board/board_layout.h \
    board/board_parameter.h \
    board/factory/element_factory_helper.h \
    board/factory/element_interface.h \
    board/utils/adaptive_text.h \
    board/utils/layout_grid.h \
    board/utils/layout_list.h \
    board/utils/single_display.h \
    board/management/alarms.h \
    board/management/bitfield.h \
    board/management/color_settings.h \
    board/management/curve_patron.h \
    board/management/page.h \
    board/management/parameter_configuration.h \
    board/management/project.h \
    board/management/states.h \
    board/management/thresholds.h \
    board/management/utils/string_util.h \
    board/utils/resize_handle.h \
    board/utils/selection_rect.h \
    board/board_page.h \
    board/board_toolbar.h \
    board/board_widget.h \
    home/home_page.h \
    widgets/add_parameter_widget.h \
    widgets/alarm_configuration_widget.h \
    mainwindow.h \
    board/chrono/chrono.h \
    board/chrono/digitdisplay.h \
    board/chrono/lap.h \
    board/chrono/lapmodel.h \
    board/chrono/session.h \
    board/chrono/stopwatch.h \
    board/chrono/timedisplay.h \
    board/chrono/timeformat.h \
    settings/settings_dialog.h \
    widgets/element_picker_widget.h \
    widgets/find_param_widget.h \
    widgets/page_editor_dialog.h \
    widgets/page_picker_widget.h \
    widgets/parameter_picker_widget.h \
    widgets/properties_picker_widget.h \
    widgets/properties_widget.h \
    widgets/utils/alarms_list_widget.h \
    widgets/utils/calendar_widget.h \
    widgets/utils/clickable_label.h \
    ../common/color_line_edit.h \
    widgets/utils/element_list_widget.h \
    widgets/utils/icon_factory.h \
    widgets/utils/parameters_list_widget.h \
    widgets/utils/properties_table_widget.h \
    ui_global.h \
    widgets/utils/time_button.h

FORMS += \
    board/board_page.ui \
    board/board_toolbar.ui \
    home/home_page.ui \
    widgets/add_parameter_widget.ui \
    widgets/alarm_configuration_widget.ui \
    mainwindow.ui \
    settings/settings_dialog.ui \
    widgets/element_picker_widget.ui \
    widgets/find_param_widget.ui \
    widgets/page_editor_dialog.ui \
    widgets/page_picker_widget.ui \
    widgets/parameter_picker_widget.ui \
    board/chrono/chrono.ui \
    board/chrono/timedisplay.ui \
    widgets/properties_picker_widget.ui \
    widgets/properties_widget.ui

RESOURCES += \
        $$PWD/resources/icons/icons.qrc \
        $$PWD/resources/images/images.qrc
