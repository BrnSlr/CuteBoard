#ifndef PAGEPICKERWIDGET_H
#define PAGEPICKERWIDGET_H

#include <QWidget>
#include "ui/board/management/project.h"

namespace Ui {
class PagePickerWidget;
}

class PagePickerWidget : public QWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    explicit PagePickerWidget(QWidget *parent = nullptr);
    ~PagePickerWidget() override;

    void setBoard(Board *board);
    void loadPage(const QString& pageName);

protected:
    void showEvent(QShowEvent *event) override;

public slots:
    void on_addPageButton_clicked();
    void on_reloadButton_clicked();
    void on_editButton_clicked();
    void updateList();
    void pageSelected();
    void savePage();

signals:
    void openPage(QString pageName);
    void pageModified(QString pageName);

private slots:
    void on_findToolButton_clicked();

    void on_exportToolButton_clicked();

    void on_saveAsButton_clicked();

private:
    Ui::PagePickerWidget *ui;
    Board *mBoard;
    QSharedPointer<Project> mProject;
    QIcon mPageIcon;
};

#endif // PAGEPICKERWIDGET_H
