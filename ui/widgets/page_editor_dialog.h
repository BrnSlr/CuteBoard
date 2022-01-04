#ifndef PAGEEDITORDIALOG_H
#define PAGEEDITORDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QPixmap>
#include "ui/board/management/project.h"
#include "log4qt/logger.h"

#define DEFAULT_NB_COLUMNS 12
#define DEFAULT_NB_ROWS 12

namespace Ui {
class PageEditorDialog;
}

class PageEditorDialog : public QDialog
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    explicit PageEditorDialog(bool editPage,
                              QSharedPointer<Project> project,
                              QWidget *parent = nullptr);
    ~PageEditorDialog() Q_DECL_OVERRIDE;

    QString getName();
    int getColumnCount();
    int getRowCount();
    int getSingleElementColumnCount();
    int getSingleElementRowCount();
    QString getDescription();
    QString getBackground();

    void setName(const QString& name);
    void setBackground(const QString& file);
    void setDescription(const QString& descr);
    void setColumnCount(int column);
    void setRowCount(int row);
    void setSingleElementColumnCount(int column);
    void setSingleElementRowCount(int row);

private slots:
    void loadImage();
    void on_backgroundPushButton_clicked();

protected:
    void resizeEvent(QResizeEvent *ev) override;

private:
    Ui::PageEditorDialog *ui;
    QSharedPointer<Project> mProject;
};

#endif // PAGEEDITORDIALOG_H
