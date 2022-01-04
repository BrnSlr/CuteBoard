#include "state_display.h"
#include "state_display_editor.h"
#include "ui/board/management/project.h"

StateDisplay::StateDisplay(Board *dashboard) : NumericalDisplay(dashboard)
{
    setConfigurationMode(ParameterConfiguration::cmState);
}

void StateDisplay::edit()
{
    StateDisplayEditor editor(this);
    editor.exec();
}

void StateDisplay::initialize(Board *dashboard)
{
    NumericalDisplay::initialize(dashboard);

    mTextBorderPen = QPen(QApplication::palette().midlight().color(), 0, Qt::SolidLine, Qt::SquareCap);
}

void StateDisplay::loadSettings(QSettings *settings)
{
    SingleDisplay::loadSettings(settings);

    settings->beginGroup("SpecDisplay");
    if(settings->contains("Orientation"))
        mOrientation = DisplayOrientation(settings->value("Orientation").toInt());
    settings->endGroup();
}

void StateDisplay::saveSettings(QSettings *settings)
{
    SingleDisplay::saveSettings(settings);
    settings->beginGroup("SpecDisplay");
    settings->setValue("Orientation", mOrientation);
    settings->endGroup();
}

void StateDisplay::displayData()
{
    QSharedPointer<BoardParameter> dashParam = boardParameter(0);
    if(dashParam && dashParam->getTimeSeries().parameterId() > 0) {

        dashParam->processStateData();

        auto text = dashParam->getStateString();
        auto brush = dashParam->getBackgroundBrush();

        if(text.isEmpty() && brush == Qt::NoBrush)
            mTextValue->setBorderPen(mTextBorderPen);
        else
            mTextValue->setBorderPen(Qt::NoPen);

        mTextValue->setText(text);
        mTextValue->setTextColor(dashParam->getForegroundColor());
        mTextValue->setBackgroundBrush(brush);
    }
}
