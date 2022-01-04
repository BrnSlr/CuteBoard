#include "numerical_display.h"
#include "ui/board/management/project.h"
#include "numerical_display_editor.h"

NumericalDisplay::NumericalDisplay(Board *dashboard) :
    SingleDisplay (dashboard),
    mOrientation(doVerticalAlignCenter),
    mValueFormat(vsfDecimal),
    mStatMode(smNone)
{
    setConfigurationMode(ParameterConfiguration::cmValue);
    setName("_NumericalDisplay");
}

void NumericalDisplay::initialize(Board *dashboard)
{
    if(dashboard) {
        SingleDisplay::initialize(dashboard);

        mTextMode = new AdaptiveTextElement(dashboard);
        mTextMode->setTextFlags(Qt::AlignCenter);
        mTextMode->setMaxPointSize(8);
        mTextMode->setMinPointSize(1);
        mTextMode->setLayer(QLatin1String("main"));
        mTextMode->setCachedPixmap(true);

        mUnitLayout = new LayoutGrid();
        mUnitLayout->initializeParentPlot(dashboard);
        mUnitLayout->setParentLayerable(this);
        mUnitLayout->setLayer(QLatin1String("main"));
    }
}

void NumericalDisplay::resetElement()
{
    SingleDisplay::resetElement();

    mStatMaxValue = -1.*std::numeric_limits<double>::max();
    mStatMinValue = std::numeric_limits<double>::max();

    for(int i=0; i<mUnitLayout->elementCount(); i++)
        mUnitLayout->takeAt(i);
    mUnitLayout->simplify();
}

void NumericalDisplay::buildElement()
{
    SingleDisplay::buildElement();

    mUnitLayout->setFillOrder(QCPLayoutGrid::foColumnsFirst);
    mMainLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);

    switch(mOrientation) {
    case doVerticalAlignCenter:
        mTextLabel->setTextFlags(Qt::AlignCenter);
        mTextUnit->setTextFlags(Qt::AlignCenter);
        mTextMode->setTextFlags(Qt::AlignCenter);
        mTextValue->setTextFlags(Qt::AlignCenter);
        break;
    case doVerticalAlignLeftRight:
        mTextLabel->setTextFlags(Qt::AlignVCenter | Qt::AlignLeft);
        mTextUnit->setTextFlags(Qt::AlignVCenter | Qt::AlignLeft);
        mTextMode->setTextFlags(Qt::AlignVCenter | Qt::AlignRight);
        mTextValue->setTextFlags(Qt::AlignCenter);
        break;
    }

    mTextLabel->needUpdate(true);
    mTextUnit->needUpdate(true);
    mTextMode->needUpdate(true);

    ColorSettings colorSet;
    switch (mStatMode) {
    case smNone:
        mTextMode->setText("");
        colorSet.setMode(ColorSettings::cmNoBackground);
        mTextMode->setBackgroundBrush(colorSet.backgroundBrush());
        mTextMode->setTextColor(colorSet.foregroundColor());
        mUnitLayout->addElement(mTextUnit);
        break;
    case smMin:
        mTextMode->setText("Min");
        if(mStatModeEnabled)
            colorSet.setMode(ColorSettings::cmFilledBackground);
        else
            colorSet.setMode(ColorSettings::cmNoBackground);

        mTextMode->setBackgroundBrush(colorSet.backgroundBrush());
        mTextMode->setTextColor(colorSet.foregroundColor());

        mUnitLayout->addElement(mTextUnit);
        mUnitLayout->addElement(mTextMode);
        break;
    case smMax:
        mTextMode->setText("Max");
        if(mStatModeEnabled)
            colorSet.setMode(ColorSettings::cmFilledBackground);
        else
            colorSet.setMode(ColorSettings::cmNoBackground);

        mTextMode->setBackgroundBrush(colorSet.backgroundBrush());
        mTextMode->setTextColor(colorSet.foregroundColor());

        mUnitLayout->addElement(mTextUnit);
        mUnitLayout->addElement(mTextMode);
        break;
    }

    mTextMode->needUpdate(true);
    mUnitLayout->setColumnSpacing(0);

    mTextMode->setVisible(mHeaderVisible | mStatModeEnabled);
    mTextLabel->setVisible(mHeaderVisible);
    mTextUnit->setVisible(mHeaderVisible);

    if(mHeaderVisible) {
        mMainLayout->addElement(mTextLabel);
        mMainLayout->addElement(mUnitLayout);
        mMainLayout->setRowStretchFactor(0,0.001);
        mMainLayout->setRowStretchFactor(1,0.001);
    }
    mMainLayout->addElement(mTextValue);
    mMainLayout->setRowSpacing(0);
    mMainLayout->setRowStretchFactor(mMainLayout->elementCount()-1,1);

    mUnitLayout->needUpdate(true);
    mMainLayout->needUpdate(true);
}

void NumericalDisplay::edit()
{
    NumericalDisplayEditor editor(this);
    editor.exec();
}

void NumericalDisplay::loadSettings(QSettings *settings)
{
    SingleDisplay::loadSettings(settings);

    settings->beginGroup("SpecDisplay");
    if(settings->contains("Orientation"))
        mOrientation = DisplayOrientation(settings->value("Orientation").toInt());
    if(settings->contains("Format"))
        mValueFormat = ValueStringFormat(settings->value("Format").toInt());
    if(settings->contains("StatMode"))
        mStatMode = StatMode(settings->value("StatMode").toInt());
    settings->endGroup();
}

void NumericalDisplay::saveSettings(QSettings *settings)
{
    SingleDisplay::saveSettings(settings);
    settings->beginGroup("SpecDisplay");
    settings->setValue("Orientation", mOrientation);
    settings->setValue("Format", mValueFormat);
    settings->setValue("StatMode", mStatMode);
    settings->endGroup();
}

void NumericalDisplay::loadData()
{
    QSharedPointer<BoardParameter> dashParam = boardParameter(0);
    if(dashParam) {
        dashParam->updateData();
    }
}

void NumericalDisplay::displayData()
{
    QSharedPointer<BoardParameter> dashParam = boardParameter(0);
    if(dashParam && dashParam->getTimeSeries().parameterId() > 0) {
        if(!mStatModeEnabled) {
            dashParam->processValueData();
            mTextValue->setText(dashParam->getValueString(mValueFormat));
            mTextValue->setTextColor(dashParam->getForegroundColor());
            mTextValue->setBackgroundBrush(dashParam->getBackgroundBrush());

        } else {
            switch(mStatMode) {
            case smNone:
                dashParam->processValueData();
                mTextValue->setText(dashParam->getValueString(mValueFormat));
                mTextValue->setTextColor(dashParam->getForegroundColor());
                mTextValue->setBackgroundBrush(dashParam->getBackgroundBrush());
                break;
            case smMin:
            {
                bool found = false;
                QVector<DataValue> values;
                QVector<double> timestamps;
                if(mBoard->dataManager()->lastBoardData(dashParam->getTimeSeries().parameterId(), values, timestamps)) {
                    QVector<DataValue>::const_iterator itValues;
                    QVector<double>::const_iterator itTimestamps;

                    for (itValues = values.constBegin(), itTimestamps = timestamps.constBegin();
                         (itValues != values.constEnd()) && (itTimestamps != timestamps.constEnd());
                         ++itValues, ++itTimestamps) {
                        if(itValues->toDouble() < mStatMinValue) {
                            mStatMinValue = itValues->toDouble();
                            found =true;
                        }
                    }
                }

                if(found) {
                    dashParam->processValueData(mStatMinValue);
                    mTextValue->setText(QString::number(mStatMinValue,'f',dashParam->parameterConfiguration()->precision()));

                    mTextValue->setTextColor(dashParam->getForegroundColor());
                    mTextValue->setBackgroundBrush(dashParam->getBackgroundBrush());
                }
            }
                break;
            case smMax:
            {
                bool found = false;
                QVector<DataValue> values;
                QVector<double> timestamps;
                if(mBoard->dataManager()->lastBoardData(dashParam->getTimeSeries().parameterId(), values, timestamps)) {
                    QVector<DataValue>::const_iterator itValues;
                    QVector<double>::const_iterator itTimestamps;

                    for (itValues = values.constBegin(), itTimestamps = timestamps.constBegin();
                         (itValues != values.constEnd()) && (itTimestamps != timestamps.constEnd());
                         ++itValues, ++itTimestamps) {
                        if(itValues->toDouble() > mStatMaxValue) {
                            mStatMaxValue = itValues->toDouble();
                            found =true;
                        }
                    }
                }
                if(found) {
                    dashParam->processValueData(mStatMaxValue);
                    mTextValue->setText(QString::number(mStatMaxValue, 'f', dashParam->parameterConfiguration()->precision()));

                    mTextValue->setTextColor(dashParam->getForegroundColor());
                    mTextValue->setBackgroundBrush(dashParam->getBackgroundBrush());
                }
            }
                break;
            }
        }
    }
}

void NumericalDisplay::applySizeConstraints()
{
    QSizeF defaultSize = mBoard->dashboardLayout()->singleElementSize();
    defaultSize.rwidth() -= mMargins.left() + mMargins.right() + mMainLayout->margins().left() + mMainLayout->margins().right();
    defaultSize.rheight() -= mMargins.top() + mMargins.bottom() + mMainLayout->margins().top() + mMainLayout->margins().bottom();

    QSizeF currentSize = mRect.size();
    currentSize.rwidth() -= mMainLayout->margins().left() + mMainLayout->margins().right();
    currentSize.rheight() -= mMainLayout->margins().top() + mMainLayout->margins().bottom();

    QSizeF labelSize = defaultSize;

    if(currentSize.height() < defaultSize.height())
        labelSize = currentSize;

    mTextLabel->setMinimumSize(labelSize.width(), LABEL_HEIGHT_RATIO * labelSize.height());
    mTextUnit->setMinimumSize(labelSize.width(), UNIT_HEIGHT_RATIO * labelSize.height());
    mTextMode->setMinimumSize(labelSize.width(), UNIT_HEIGHT_RATIO * labelSize.height());
    mTextValue->setMinimumSize(labelSize.width(), VALUE_HEIGHT_RATIO * labelSize.height());
}

ValueStringFormat NumericalDisplay::valueFormat() const
{
    return mValueFormat;
}

void NumericalDisplay::setValueFormat(const ValueStringFormat &valueFormat)
{
    mValueFormat = valueFormat;
}

NumericalDisplay::DisplayOrientation NumericalDisplay::orientation() const
{
    return mOrientation;
}

void NumericalDisplay::setOrientation(const DisplayOrientation &orientation)
{
    mOrientation = orientation;
}

NumericalDisplay::StatMode NumericalDisplay::statMode() const
{
    return mStatMode;
}

void NumericalDisplay::setStatMode(const StatMode &statMode)
{
    mStatMode = statMode;
}

