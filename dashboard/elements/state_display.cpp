#include "state_display.h"
#include "ui/element_editors/state_display_editor.h"
#include "project/project.h"

QTBStateDisplay::QTBStateDisplay(QTBoard *dashboard) : QTBValueDisplay(dashboard)
{

}

void QTBStateDisplay::edit()
{
    QTBStateDisplayEditor editor(this);
    editor.exec();
}

void QTBStateDisplay::updateDashboardParameters(QTBDashboardParameter::UpdateMode mode)
{
    Q_UNUSED(mode)
    QTBSingleDisplay::updateDashboardParameters(QTBDashboardParameter::umState);
}

void QTBStateDisplay::loadSettings(QSettings *settings)
{
    QTBSingleDisplay::loadSettings(settings);

    settings->beginGroup("SpecDisplay");
    if(settings->contains("Orientation"))
        mOrientation = DisplayOrientation(settings->value("Orientation").toInt());
    settings->endGroup();
}

void QTBStateDisplay::saveSettings(QSettings *settings)
{
    QTBSingleDisplay::saveSettings(settings);
    settings->beginGroup("SpecDisplay");
    settings->setValue("Orientation", mOrientation);
    settings->endGroup();
}

void QTBStateDisplay::saveParametersSettings(QSettings *settings, QTBParameterConfiguration::ConfigurationModule mode)
{
    Q_UNUSED(mode)
    QTBDashboardElement::saveParametersSettings(settings, QTBParameterConfiguration::cmState);
}

void QTBStateDisplay::loadParametersSettings(QSettings *settings, QTBParameterConfiguration::ConfigurationModule mode)
{
    Q_UNUSED(mode)
    QTBDashboardElement::loadParametersSettings(settings, QTBParameterConfiguration::cmState);
}

void QTBStateDisplay::processNewSamples()
{
    QSharedPointer<QTBDashboardParameter> dashParam = dashParameter(0);
    if(dashParam && dashParam->getParameterId() > 0) {
        mTextValue->setText(dashParam->getStateString());
        mTextValue->setTextColor(dashParam->getForegroundColor());
        mTextValue->setBackgroundBrush(dashParam->getBackgroundBrush());
    } else {
        mTextValue->setText("X");
    }
}
