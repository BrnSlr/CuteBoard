#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QDebug>
#include <QDirIterator>
#include <QTableWidgetItem>
#include <QMessageBox>
#include "project/project.h"

namespace Ui {
class HomeWidget;
}

class HomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HomeWidget(QWidget *parent = nullptr);
    ~HomeWidget();

    void setProject(const QSharedPointer<QTBProject> &project);

signals:
    void showLivePage();
    void showDesignPage();
    void projectSelected(bool);

public slots:
    void loadWorkingDirectory();

private slots:
    void openWorkingDirectory();
    void createProject();
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

private:
    Ui::HomeWidget *ui;
    QSharedPointer<QTBProject> mProject;
};

#endif // HOMEWIDGET_H
