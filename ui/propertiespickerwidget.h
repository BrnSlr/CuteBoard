#ifndef PROPERTIESPICKERWIDGET_H
#define PROPERTIESPICKERWIDGET_H

#include <QWidget>
#include "project/project.h"

namespace Ui {
class PropertiesPickerWidget;
}

class PropertiesPickerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PropertiesPickerWidget(QWidget *parent = nullptr);
    ~PropertiesPickerWidget();

    void setProject(const QSharedPointer<QTBProject> &project);

public slots:
    void on_addButton_clicked();
    void on_reloadButton_clicked();
    void updateList();
    void itemDoubleClicked(QTreeWidgetItem *item, int column);
    void editProperties(QString label, QString descr);

private:
    Ui::PropertiesPickerWidget *ui;
    QSharedPointer<QTBProject> mProject;
};

#endif
