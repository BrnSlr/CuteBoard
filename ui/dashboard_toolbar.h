#ifndef DASHBOARDTOOLBAR_H
#define DASHBOARDTOOLBAR_H

#include <QWidget>
#include <QAction>
#include <QComboBox>
#include "project/project.h"

namespace Ui {
class DashboardToolbar;
}

class DashboardToolbar : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardToolbar(QWidget *parent = nullptr);
    ~DashboardToolbar();

    void setProject(const QSharedPointer<QTBProject> &project);
    void loadPage(const QString& pageName);
    void setTime(const QString& time);
    void loadFirstPage();
    void setEdition(bool edition);

public slots:
    void updateList();
    void pageSelected();

signals:
    void pageIndexChanged(const QString pageName);
    void fullScreen(bool fullScreen);
    void pagePaused(bool pause);

private:
    Ui::DashboardToolbar *ui;
    QSharedPointer<QTBProject> mProject;
};

#endif // LIVETOOLBARWIDGET_H
