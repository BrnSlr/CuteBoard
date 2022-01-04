#ifndef FIND_PARAM_WIDGET_H
#define FIND_PARAM_WIDGET_H

#include <QDialog>
#include "ui/board/management/project.h"

namespace Ui {
class find_param_widget;
}

class find_param_widget : public QDialog
{
    Q_OBJECT

public:
    explicit find_param_widget(QWidget *parent = nullptr);
    ~find_param_widget();    

    void setProject(const QSharedPointer<Project> &project);

signals:
    void pageRequested(QString pageName);
public slots:
    void find();

private:
    Ui::find_param_widget *ui;
    QSharedPointer<Project> mProject;
};

#endif // FIND_PARAM_WIDGET_H
