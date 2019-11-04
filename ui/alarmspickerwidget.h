#ifndef ALARMSPICKERWIDGET_H
#define ALARMSPICKERWIDGET_H

#include <QWidget>
#include "project/project.h"

namespace Ui {
class AlarmsPickerWidget;
}

class AlarmsPickerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AlarmsPickerWidget(QWidget *parent = nullptr);
    ~AlarmsPickerWidget();

    void setProject(const QSharedPointer<QTBProject> &project);

public slots:
    void slot_addButton_clicked();
    void slot_reloadButton_clicked();
    void updateList();
    void itemDoubleClicked(QListWidgetItem *item);
    void editAlarm(QString label);


private:
    Ui::AlarmsPickerWidget *ui;
    QSharedPointer<QTBProject> mProject;
};

#endif // ALARMSPICKERWIDGET_H
