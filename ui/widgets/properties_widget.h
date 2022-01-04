#ifndef PROPERTIESWIDGET_H
#define PROPERTIESWIDGET_H

#include <QWidget>
#include "ui/board/management/parameter_configuration.h"
#include "ui/board/management/project.h"
#include "ui/ui_global.h"

namespace Ui {
class PropertiesWidget;
}

class BOARD_UI_EXPORT PropertiesWidget : public QWidget
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

    PropertiesWidget(QWidget *parent = nullptr);
    ~PropertiesWidget();

    bool isConnected();
    void updateUi(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings);
    void updateParameterSettings(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings);
    void setEditionMode(const EditionMode &mode);
    void setPropertiesMode(const ParameterConfiguration::ConfigurationMode &propertiesMode);
    void setProject(QSharedPointer<Project> project);
    QExplicitlySharedDataPointer<ParameterConfiguration> currentSettings() const;

private slots:
    void createSharedConfiguration();
    void on_connectedCheckBox_stateChanged(int arg1);

    void on_colorModeComboBox_currentIndexChanged(int index);
    void on_defaultColorToolButton_clicked();
    void on_defaultColorToolButton_2_clicked();
    void on_listWidget_currentRowChanged(int currentRow);

    void on_validRangeCheckBox_stateChanged(int arg1);
    void on_outOfRangeColorModeComboBox_currentIndexChanged(int index);
    void on_outOfRangeCheckBox_stateChanged(int arg1);

    void removeHighThreshold_clicked();
    void removeLowThreshold_clicked();

    void removeState_clicked();
    void on_itemsColorComboBox_currentIndexChanged(int index);
    void on_itemsColorToolButton_clicked();

    void on_statesTableWidget_cellDoubleClicked(int row, int column);

    void on_editPropPushButton_clicked();
    void on_addStateToolButton_clicked();

    void on_addHighThresholdToolButton_clicked();
    void on_addLowThresholdToolButton_clicked();
    void on_highThresholdsTableWidget_cellDoubleClicked(int row, int column);

    void on_lowThresholdsTableWidget_cellDoubleClicked(int row, int column);

signals:
    void connectProperties(bool);

private:
    Ui::PropertiesWidget *ui;
    QSharedPointer<Project> mProject;
    EditionMode mEditionMode;
    ParameterConfiguration::ConfigurationMode mPropertiesMode;
    bool mStatesModified;
    bool mThresholdsModified;
    bool mBitfieldsModified;
    QExplicitlySharedDataPointer<ParameterConfiguration> mCurrentSettings;
};

#endif
