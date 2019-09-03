#include "value_gauge_horizontal.h"
#include "ui/element_editors/value_gauge_horizontal_editor.h"
#include "project/project.h"

QTBValueGaugeHorizontal::QTBValueGaugeHorizontal(QTBoard *dashboard) :
    QTBValueGaugeVertical (dashboard)
{

}

void QTBValueGaugeHorizontal::initializeElement(QTBoard *dashboard)
{
    if(dashboard) {
        QTBDashboardElement::initializeElement(dashboard);

        mAxisRect = new QTBAxisRect(dashboard);

        mAxisRect->showAxis(QCPAxis::atLeft,false);
        mAxisRect->showAxis(QCPAxis::atBottom,true);

        mAxisRect->axis(QCPAxis::atBottom)->setBasePen(QPen(dashboard->frontColor()));
        mAxisRect->axis(QCPAxis::atLeft)->setBasePen(Qt::NoPen);

        mAxisRect->axis(QCPAxis::atBottom)->setTickPen(QPen(dashboard->frontColor()));
        mAxisRect->axis(QCPAxis::atLeft)->setTickPen(Qt::NoPen);

        mAxisRect->axis(QCPAxis::atBottom)->setSubTickPen(QPen(dashboard->frontColor()));
        mAxisRect->axis(QCPAxis::atLeft)->setSubTickPen(Qt::NoPen);

        mAxisRect->axis(QCPAxis::atBottom)->setTickLabelColor(dashboard->frontColor());

        mAxisRect->axis(QCPAxis::atBottom)->setTickLabels(mAxisTicksVisible);
        mAxisRect->axis(QCPAxis::atLeft)->setTickLabels(false);

        mAxisRect->axis(QCPAxis::atBottom)->grid()->setVisible(mAxisGridVisible);
        mAxisRect->axis(QCPAxis::atLeft)->grid()->setVisible(false);

        mAxisRect->axis(QCPAxis::atBottom)->setOffset(5);
        mAxisRect->axis(QCPAxis::atBottom)->setPadding(1);
        mAxisRect->axis(QCPAxis::atBottom)->setTickLabelPadding(3);

        mAxisRect->axis(QCPAxis::atLeft)->setRange(0.5, 1.5);
        mAxisRect->setMinimumMargins(QMargins(15, 15, 15, 15));

        mBar = new QCPBars(mAxisRect->axis(QCPAxis::atLeft), mAxisRect->axis(QCPAxis::atTop));
        mBar->setWidth(1);
        mBar->setAntialiased(false);
        mBar->setAntialiasedFill(false);
        mBar->setPen(Qt::NoPen);
        mBar->setBrush(QColor(0,131,179));

        QTBValueDisplay::initializeElement(dashboard);
    }
}

void QTBValueGaugeHorizontal::buildLayout()
{
    mSubLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);
    mMainLayout->setFillOrder(QCPLayoutGrid::foColumnsFirst);

    mTextLabel->setVisible(mHeaderVisible);
    mTextUnit->setVisible(mHeaderVisible);
    mTextValue->setVisible(mValueVisible);


    if(mHeaderVisible && !mValueVisible) {
        mSubLayout->addElement(mEmptyElementTop);
        mSubLayout->setRowStretchFactor(0,0.01);
    }

    if(mHeaderVisible) {
        mSubLayout->addElement(mTextLabel);
        mSubLayout->addElement(mTextUnit);
    }

    if(mValueVisible)
        mSubLayout->addElement(mTextValue);

    if(mHeaderVisible && !mValueVisible) {
        mSubLayout->addElement(mEmptyElementBottom);
        mSubLayout->setRowStretchFactor(3,0.01);
    }

    mSubLayout->setRowSpacing(0);

    if(mHeaderVisible || mValueVisible)
        mMainLayout->addElement(mSubLayout);
    mMainLayout->addElement(mAxisRect);

    mMainLayout->setColumnSpacing(0);

    mSubLayout->needUpdate(true);
    mMainLayout->needUpdate(true);
}

void QTBValueGaugeHorizontal::edit()
{
    QTBValueGaugeHorizontalEditor editor(this);
    editor.exec();
}

void QTBValueGaugeHorizontal::processNewSamples()
{
    QSharedPointer<QTBDashboardParameter> dashParam = dashParameter(0);
     if(dashParam && dashParam->getParameterId() > 0) {
        if(mValueVisible) {
            mTextValue->setText(dashParam->getValueString(mValueFormat));
            mTextValue->setTextColor(dashParam->getForegroundColor());
            mTextValue->setBackgroundBrush(dashParam->getBackgroundBrush());
        }

        mBar->setBrush(QBrush(dashParam->getColor()));

        double val = dashParam->getValueDouble();
        mBar->data()->clear();
        mBar->addData(1, val);

        if(val < mValueAxisRange.lower)
            mValueAxisRange.lower = val;
        if(val > mValueAxisRange.upper)
            mValueAxisRange.upper = val;

        switch(mAxisScale) {
        case asAuto:
            mAxisRect->axis(QCPAxis::atBottom)->setRange(mValueAxisRange);
            mAxisMinCustom = mValueAxisRange.lower;
            mAxisMaxCustom = mValueAxisRange.upper;
            break;
        case asParam:
        {
            if(dashParam->parameterConfiguration()->validRange()) {
                mAxisRect->axis(QCPAxis::atBottom)->setRange(QCPRange(dashParam->parameterConfiguration()->rangeMinimum(), dashParam->parameterConfiguration()->rangeMaximum()));
            } else {
                mAxisRect->axis(QCPAxis::atBottom)->setRange(mValueAxisRange);
                mAxisMinCustom = mValueAxisRange.lower;
                mAxisMaxCustom = mValueAxisRange.upper;
            }
            break;
        }
        case asCustom:
            mAxisRect->axis(QCPAxis::atBottom)->setRange(QCPRange(mAxisMinCustom, mAxisMaxCustom));
            break;
        }

        if(mTankGauge) {
            double val = dashParam->getValueDouble();
            QCPRange rangeTank(0, mAxisRect->axis(QCPAxis::atBottom)->range().size());
            mBar->data()->clear();
            mBar->addData(1, val - mAxisRect->axis(QCPAxis::atBottom)->range().lower);
            mAxisRect->axis(QCPAxis::atTop)->setRange(rangeTank);
        } else {
            mAxisRect->axis(QCPAxis::atTop)->setRange(mAxisRect->axis(QCPAxis::atBottom)->range());
        }
    } else {
        mTextValue->setText("X");
        mBar->data()->clear();
    }
}

void QTBValueGaugeHorizontal::updateSizeConstraints()
{

    QSizeF singleElementSize = mBoard->dashboardLayout()->singleElementSize();
    QSizeF labelSize = singleElementSize;
    if(mValueVisible) {
        if(mRect.height() < singleElementSize.height())
            labelSize.setHeight(mRect.height());
        if(mRect.width() < singleElementSize.width())
            labelSize.setWidth(mRect.width());

        mTextLabel->setMaximumSize(labelSize.width(), 0.3 * labelSize.height());
        mTextUnit->setMaximumSize(labelSize.width(), 0.2 * labelSize.height());
        mTextValue->setMaximumSize(labelSize.width(), mRect.height());
    } else {
        labelSize.setHeight(mRect.height());
        if(mRect.height() > singleElementSize.height() / 2.0)
            labelSize.setHeight(singleElementSize.height() / 2.0);

        mTextLabel->setMaximumSize(labelSize.width(), 0.6 * labelSize.height());
        mTextUnit->setMaximumSize(labelSize.width(), 0.4 * labelSize.height());
    }
}

void QTBValueGaugeHorizontal::checkSizeAndVisibility()
{
    //    if(mValueVisible) {
    //        if(!mTextValue->visible()) {
    //            mTextValue->setMaximumSize(mRect.width(), 0);
    //            mMainLayout->needUpdate(true);
    //        }
    //    }
    QTBValueDisplay::checkSizeAndVisibility();
}

void QTBValueGaugeHorizontal::setAxisTicksVisible(bool axisTicksVisible)
{
    mAxisTicksVisible = axisTicksVisible;
    if(mAxisTicksVisible) {
        mAxisRect->axis(QCPAxis::atBottom)->setBasePen(QPen(mBoard->frontColor()));
        mAxisRect->axis(QCPAxis::atBottom)->setTickPen(QPen(mBoard->frontColor()));
        mAxisRect->axis(QCPAxis::atBottom)->setSubTickPen(QPen(mBoard->frontColor()));
    } else {
        mAxisRect->axis(QCPAxis::atBottom)->setBasePen(Qt::NoPen);
        mAxisRect->axis(QCPAxis::atBottom)->setTickPen(Qt::NoPen);
        mAxisRect->axis(QCPAxis::atBottom)->setSubTickPen(Qt::NoPen);
    }
}

void QTBValueGaugeHorizontal::setAxisLabelsVisible(bool axisLabelsVisible)
{
    mAxisLabelsVisible = axisLabelsVisible;
    mAxisRect->axis(QCPAxis::atBottom)->setTickLabels(mAxisLabelsVisible);
}

void QTBValueGaugeHorizontal::setAxisGridVisible(bool axisGridVisible)
{
    mAxisGridVisible = axisGridVisible;
    mAxisRect->axis(QCPAxis::atBottom)->grid()->setVisible(mAxisGridVisible);
}

int QTBValueGaugeHorizontal::defaultWidth()
{
    return 4*mBoard->dashboardLayout()->singleElementColumnCount();
}

int QTBValueGaugeHorizontal::defaultHeight()
{
    return mBoard->dashboardLayout()->singleElementRowCount();
}
