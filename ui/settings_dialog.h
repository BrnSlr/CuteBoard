#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QDialog>
#include "dashboard/dashboard.h"

namespace Ui {
class QTBSettingsDialog;
}

class QTBSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QTBSettingsDialog(QTBoard *board, QWidget *parent = nullptr);
    ~QTBSettingsDialog();

public slots:
    void startDataSource();
    void stopDataSource();
    void updateStatus();
    void selectWorkingDir();

private:
    Ui::QTBSettingsDialog *ui;
    QTBoard *mBoard;
};

#endif // SETTINGS_DIALOG_H
