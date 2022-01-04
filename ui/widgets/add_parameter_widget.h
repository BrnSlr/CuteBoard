#ifndef ADD_PARAMETER_WIDGET_H
#define ADD_PARAMETER_WIDGET_H

#include <QDialog>
#include "ui/ui_global.h"
#include "log4qt/logger.h"
#include "log4qt/logmanager.h"
#include "data/data_manager.h"
#include "ui/board/management/project.h"

namespace Ui {
class AddParameterWidget;
}

class BOARD_UI_EXPORT AddParameterWidget : public QDialog
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    explicit AddParameterWidget(QWidget *parent,
                                const QSharedPointer<DataManager> &dataManager,
                                const QSharedPointer<Project> &project);
    ~AddParameterWidget();

private slots:
    void on_addToolButton_clicked();

    void on_removeToolButton_clicked();

private:
    Ui::AddParameterWidget *ui;
};

#endif // ADD_PARAMETER_WIDGET_H
