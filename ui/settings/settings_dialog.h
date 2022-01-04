#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QDialog>
#include "ui/board/board.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    explicit SettingsDialog(Board *board, QWidget *parent = nullptr);
    ~SettingsDialog();

public slots:
    void startLiveDataSource();
    void stopLiveDataSource();
    void selectDataSourceForReferenceTime();
    void clockReferenceChanged();
    void refreshPeriodChanged();
    void colorThemeChanged();
    void updateLiveDataSources();
    void updatePlaybackDataSources();
    void selectWorkingDir();

private:
    Ui::SettingsDialog *ui;
    Board *mBoard;
};

#endif // SETTINGS_DIALOG_H
