#ifndef PARAMETERPICKERWIDGET_H
#define PARAMETERPICKERWIDGET_H

#include <QWidget>
#include <QTreeWidgetItem>
#include "data/data_manager.h"
#include "ui/board/management/project.h"

namespace Ui {
class ParameterPickerWidget;
}

class ParameterPickerWidget : public QWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    explicit ParameterPickerWidget(QWidget *parent = nullptr);
    ~ParameterPickerWidget();

    void setDataManager(const QSharedPointer<DataManager> &dataManager);
    void setProject(const QSharedPointer<Project> &project);

public slots:
    void updateParameterList();
private slots:
    void changeListDisplay();
    void searchString(const QString& str);
    bool searchAndHide(QString str, QTreeWidgetItem *item);

private:
    Ui::ParameterPickerWidget *ui;
    QSharedPointer<DataManager> mDataManager;
    QSharedPointer<Project> mProject;
    bool mTreeMode;
};

#endif // PARAMETERPICKERWIDGET_H
