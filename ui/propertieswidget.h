#ifndef PROPERTIESWIDGET_H
#define PROPERTIESWIDGET_H

#include <QWidget>
#include "project/parameter_configuration.h"

namespace Ui {
class PropertiesWidget;
}

class PropertiesWidget : public QWidget
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

    PropertiesWidget(QWidget *parent = nullptr);
    ~PropertiesWidget();

    bool isConnected();
    void updateUi(QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings);
    void updateParameterSettings(QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings);

    void setEditionMode(const EditionMode &mode);
    void setPropertiesMode(const QTBParameterConfiguration::ConfigurationModule &propertiesMode);

private slots:
    void on_connectedCheckBox_stateChanged(int arg1);

    void on_colorModeComboBox_currentIndexChanged(int index);
    void on_defaultColorToolButton_clicked();
    void on_defaultColorToolButton_2_clicked();
    void on_listWidget_currentRowChanged(int currentRow);

    void on_validRangeCheckBox_stateChanged(int arg1);
    void on_outOfRangeColorModeComboBox_currentIndexChanged(int index);
    void on_outOfRangeCheckBox_stateChanged(int arg1);

    void on_highThresholdPushButton_clicked();
    void on_highTthresholdColorToolButton_clicked();
    void on_highThresholdColorModeComboBox_currentIndexChanged(int index);    
    void on_lowThresholdColorToolButton_clicked();
    void on_lowThresholdColorModeComboBox_currentIndexChanged(int index);
    void on_lowThresholdPushButton_clicked();
    void removeHighThreshold_clicked();
    void removeLowThreshold_clicked();

    void on_addStatePushButton_clicked();
    void on_stateColorToolButton_clicked();
    void on_stateColorModeComboBox_currentIndexChanged(int index);
    void removeState_clicked();
    void on_itemsColorComboBox_currentIndexChanged(int index);
    void on_itemsColorToolButton_clicked();

signals:
    void connectProperties(bool);

private:
    Ui::PropertiesWidget *ui;
    EditionMode mEditionMode;
    QTBParameterConfiguration::ConfigurationModule mPropertiesMode;
    bool mStatesModified;
    bool mThresholdsModified;
    bool mBitfieldsModified;
};

#endif
