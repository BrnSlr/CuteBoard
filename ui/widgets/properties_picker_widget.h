#ifndef PROPERTIESPICKERWIDGET_H
#define PROPERTIESPICKERWIDGET_H

#include <QWidget>
#include "ui/board/management/project.h"

namespace Ui {
class PropertiesPickerWidget;
}

class PropertiesPickerWidget : public QWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    explicit PropertiesPickerWidget(QWidget *parent = nullptr);
    ~PropertiesPickerWidget();

    void setProject(const QSharedPointer<Project> &project);

public slots:
    void on_addButton_clicked();
    void on_reloadButton_clicked();
    void updateList();
    void itemDoubleClicked(QTreeWidgetItem *item, int column);
    void editProperties(QString label, QString descr);
    void duplicateProperties(QString label, QString descr);
    void distributeProperties(QString label, QString descr);
    void editAlarm(QString label);
    void searchString(const QString& str);

private:
    Ui::PropertiesPickerWidget *ui;
    QSharedPointer<Project> mProject;
};

#endif
