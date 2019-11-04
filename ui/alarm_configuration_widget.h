#ifndef ALARM_CONFIGURATION_WIDGET_H
#define ALARM_CONFIGURATION_WIDGET_H

#include <QWidget>
#include "project/alarm_configuration.h"

namespace Ui {
class QTBAlarmConfigurationWidget;
}

class QTBAlarmConfigurationWidget : public QWidget
{
    Q_OBJECT

public:
    enum EditionMode {
        emCreation,
        emEdition,
        emElementConnected,
        emElementDisconnected,
        emElementStandAlone
    };

    explicit QTBAlarmConfigurationWidget(QWidget *parent = nullptr);
    ~QTBAlarmConfigurationWidget() override;

    void setEditionMode(const EditionMode &mode);

    void updateConfig();
    void updateUi();

    bool isConnected();
    QString getUiName();
    QExplicitlySharedDataPointer<QTBAlarmConfiguration> alarmConfig() const;
    void setAlarmConfig(const QExplicitlySharedDataPointer<QTBAlarmConfiguration> &alarmConfig);

public slots:
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
    Ui::QTBAlarmConfigurationWidget *ui;
    EditionMode mEditionMode;
    QExplicitlySharedDataPointer<QTBAlarmConfiguration> mAlarmConfig;
    bool mModified;

};

#endif // ALARM_CONFIGURATION_WIDGET_H
