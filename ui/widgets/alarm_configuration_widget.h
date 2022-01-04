#ifndef ALARM_CONFIGURATION_WIDGET_H
#define ALARM_CONFIGURATION_WIDGET_H

#include <QWidget>
#include "ui/board/management/alarms.h"
#include "ui/ui_global.h"

namespace Ui {
class AlarmConfigurationWidget;
}

class BOARD_UI_EXPORT AlarmConfigurationWidget : public QWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    enum EditionMode {
        emCreation,
        emEdition,
        emElementConnected,
        emElementDisconnected,
        emElementStandAlone
    };

    explicit AlarmConfigurationWidget(QWidget *parent = nullptr);
    ~AlarmConfigurationWidget() override;

    void setEditionMode(const EditionMode &mode);

    void updateConfig();
    void updateUi();

    bool isConnected();
    QString getUiName();
    QExplicitlySharedDataPointer<Alarms> alarmConfig() const;
    void setAlarmConfig(const QExplicitlySharedDataPointer<Alarms> &alarmConfig);

public slots:
    void filter();
    void clearFilter();
    void newAlarm();
    void moveDown();
    void moveUp();
    void removeAlarm();
    void doubleClickedItem(QTableWidgetItem *item);

signals:
    void stateChanged(int state);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void resizeColumns();

private:
    Ui::AlarmConfigurationWidget *ui;
    EditionMode mEditionMode;
    QExplicitlySharedDataPointer<Alarms> mAlarmConfig;
    bool mModified;

};

#endif // ALARM_CONFIGURATION_WIDGET_H
