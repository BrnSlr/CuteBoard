#ifndef PAGEEDITORDIALOG_H
#define PAGEEDITORDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QPixmap>

#define DEFAULT_NB_COLUMNS 12
#define DEFAULT_NB_ROWS 14

namespace Ui {
class PageEditorDialog;
}

class PageEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PageEditorDialog(bool editPage, QWidget *parent = nullptr);
    ~PageEditorDialog();

    QString getName();
    int getColumnCount();
    int getRowCount();
    int getSingleElementColumnCount();
    int getSingleElementRowCount();
    QString getDescription();
    bool getCustomLayout();
    QString getBackground();

    void setName(const QString& name);
    void setBackground(const QString& file);
    void setDescription(const QString& descr);
    void setColumnCount(int column);
    void setRowCount(int row);
    void setSingleElementColumnCount(int column);
    void setSingleElementRowCount(int row);
    void setCustomLayout(bool custom);

private slots:
    void on_densitySpinBox_valueChanged(int arg1);
    void on_checkBox_stateChanged(int arg1);

    void on_backgroundPushButton_clicked();

protected:
    void resizeEvent(QResizeEvent *ev) override;

private:
    Ui::PageEditorDialog *ui;
};

#endif // PAGEEDITORDIALOG_H
