#include "value_display.h"
#include "ui/element_editors/value_display_editor.h"
#include "project/project.h"

QTBValueDisplay::QTBValueDisplay(QTBoard *dashboard) :
    QTBSingleDisplay (dashboard),
    mOrientation(doVerticalAlignCenter),
    mValueFormat(vsfDecimal)
{
    setConfigurationMode(QTBParameterConfiguration::cmValue);
}

void QTBValueDisplay::buildLayout()
{
    mSubLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);

    switch(mOrientation) {
        case doVerticalAlignCenter:
            mMainLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);
            mTextLabel->setTextFlags(Qt::AlignCenter);
            mTextUnit->setTextFlags(Qt::AlignCenter);
            mTextValue->setTextFlags(Qt::AlignCenter);
            mSubLayout->addElement(mTextLabel);
            mSubLayout->addElement(mTextUnit);
            break;
        case doVerticalAlignLeftRight:
            mMainLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);
            mTextLabel->setTextFlags(Qt::AlignVCenter | Qt::AlignLeft);
            mTextUnit->setTextFlags(Qt::AlignVCenter | Qt::AlignLeft);
            mTextValue->setTextFlags(Qt::AlignVCenter | Qt::AlignRight);
            mSubLayout->addElement(mTextLabel);
            mSubLayout->addElement(mTextUnit);
            break;
        case doHorizontal:
            mMainLayout->setFillOrder(QCPLayoutGrid::foColumnsFirst);
            mTextLabel->setTextFlags(Qt::AlignCenter);
            mTextUnit->setTextFlags(Qt::AlignCenter);
            mTextValue->setTextFlags(Qt::AlignCenter);
            mSubLayout->addElement(mEmptyElementTop);
            mSubLayout->addElement(mTextLabel);
            mSubLayout->addElement(mTextUnit);
            mSubLayout->addElement(mEmptyElementBottom);
            mSubLayout->setRowStretchFactor(0,0.01);
            mSubLayout->setRowStretchFactor(3,0.01);
            break;
    }

    mSubLayout->setRowSpacing(0);

    mTextLabel->setVisible(mHeaderVisible);
    mTextUnit->setVisible(mHeaderVisible);

    if(mHeaderVisible)
        mMainLayout->addElement(mSubLayout);
    mMainLayout->addElement(mTextValue);
    mMainLayout->setRowSpacing(0);

    mSubLayout->needUpdate(true);
    mMainLayout->needUpdate(true);
}

void QTBValueDisplay::edit()
{
    QTBValueDisplayEditor editor(this);
    editor.exec();
}

void QTBValueDisplay::loadSettings(QSettings *settings)
{
    QTBSingleDisplay::loadSettings(settings);

    settings->beginGroup("SpecDisplay");
    if(settings->contains("Orientation"))
        mOrientation = DisplayOrientation(settings->value("Orientation").toInt());
    if(settings->contains("Format"))
        mValueFormat = ValueStringFormat(settings->value("Format").toInt());
    settings->endGroup();
}

void QTBValueDisplay::saveSettings(QSettings *settings)
{
    QTBSingleDisplay::saveSettings(settings);
    settings->beginGroup("SpecDisplay");
    settings->setValue("Orientation", mOrientation);
    settings->setValue("Format", mValueFormat);
    settings->endGroup();
}

void QTBValueDisplay::processNewSamples()
{
    QSharedPointer<QTBDashboardParameter> dashParam = dashParameter(0);
    if(dashParam && dashParam->getParameterId() > 0) {
        mTextValue->setText(dashParam->getValueString(mValueFormat));
        mTextValue->setTextColor(dashParam->getForegroundColor());
        mTextValue->setBackgroundBrush(dashParam->getBackgroundBrush());
    } else {
        mTextValue->setText("X");
    }
}

void QTBValueDisplay::updateSizeConstraints()
{
    if(mHeaderVisible) {
        QSizeF singleElementSize = mBoard->dashboardLayout()->singleElementSize();
        QSizeF labelSize = singleElementSize;

        if(mOrientation == doHorizontal) {
            labelSize.setHeight(mRect.height());
            if(mRect.height() > singleElementSize.height() / 2.0)
                labelSize.setHeight(singleElementSize.height() / 2.0);

            mTextLabel->setMinimumSize(labelSize.width(), 0.6 * labelSize.height());
            mTextUnit->setMinimumSize(labelSize.width(), 0.4 * labelSize.height());
            mTextLabel->setMaximumSize(labelSize.width(), 0.6 * labelSize.height());
            mTextUnit->setMaximumSize(labelSize.width(), 0.4 * labelSize.height());
        } else {

            if(mRect.height() < singleElementSize.height()) {
                mTextLabel->setMinimumSize(mRect.width(), 0.3 * mRect.height());
                mTextUnit->setMinimumSize(mRect.width(), 0.2 * mRect.height());
                mTextLabel->setMaximumSize(mRect.width(), 0.3 * mRect.height());
                mTextUnit->setMaximumSize(mRect.width(), 0.2 * mRect.height());
            } else {
                mTextLabel->setMinimumSize(mRect.width(), 0.3 * labelSize.height());
                mTextUnit->setMinimumSize(mRect.width(), 0.2 * labelSize.height());
                mTextLabel->setMaximumSize(mRect.width(), 0.3 * labelSize.height());
                mTextUnit->setMaximumSize(mRect.width(), 0.2 * labelSize.height());
            }
        }
    }
}

void QTBValueDisplay::checkSizeAndVisibility()
{
    if(mHeaderVisible) {
        if(mTextLabel->visible() && mTextUnit->visible()) {
            if(mTextUnit->pointSize() > mTextLabel->pointSize())
                mTextUnit->setPointSize(mTextLabel->pointSize());
        }
//        else {
//            if(!mTextLabel->visible()) {

//                mTextUnit->setVisible(false);

//                if(mOrientation == doHorizontal) {
//                    mTextLabel->setMinimumSize(0,0);
//                    mTextUnit->setMinimumSize(0,0);
//                    mTextLabel->setMaximumSize(0,0);
//                    mTextUnit->setMaximumSize(0,0);
//                } else {
//                    mTextLabel->setMinimumSize(mRect.width(), 0);
//                    mTextUnit->setMinimumSize(mRect.width(), 0);
//                    mTextLabel->setMaximumSize(mRect.width(), 0);
//                    mTextUnit->setMaximumSize(mRect.width(), 0);
//                }

//                mSubLayout->needUpdate(true);
//                mMainLayout->needUpdate(true);

//            }  else if(!mTextUnit->visible()) {
//                if(mOrientation == doHorizontal) {
//                    mTextUnit->setMinimumSize(0,0);
//                    mTextUnit->setMaximumSize(0,0);
//                } else {
//                    mTextUnit->setMinimumSize(mRect.width(),0);
//                    mTextUnit->setMaximumSize(mRect.width(), 0);
//                }

//                mSubLayout->needUpdate(true);
//                mMainLayout->needUpdate(true);
//            }
//        }
    }
}

ValueStringFormat QTBValueDisplay::valueFormat() const
{
    return mValueFormat;
}

void QTBValueDisplay::setValueFormat(const ValueStringFormat &valueFormat)
{
    if(mValueFormat != valueFormat) {
        mValueFormat = valueFormat;
        updateElement();
    }
}

QTBValueDisplay::DisplayOrientation QTBValueDisplay::orientation() const
{
    return mOrientation;
}

void QTBValueDisplay::setOrientation(const DisplayOrientation &orientation)
{
    if(mOrientation != orientation) {
        mOrientation = orientation;
        updateElement();
    }
}

