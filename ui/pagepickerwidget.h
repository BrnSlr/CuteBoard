#ifndef PAGEPICKERWIDGET_H
#define PAGEPICKERWIDGET_H

#include <QWidget>
#include "project/project.h"

namespace Ui {
class PagePickerWidget;
}

class PagePickerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PagePickerWidget(QWidget *parent = nullptr);
    ~PagePickerWidget();

    void setProject(const QSharedPointer<QTBProject> &project);
    void loadPage(const QString& pageName);

public slots:
    void on_addPageButton_clicked();
    void on_reloadButton_clicked();
    void on_editButton_clicked();
    void updateList();
    void pageSelected();

signals:
    void saveCurrentPage();
    void openPage(QString pageName);
    void pageModified(QString pageName);

private:
    Ui::PagePickerWidget *ui;
    QSharedPointer<QTBProject> mProject;
};

#endif // PAGEPICKERWIDGET_H
