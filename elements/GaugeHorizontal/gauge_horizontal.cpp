#include "gauge_horizontal.h"
#include "gauge_horizontal_editor.h"
#include "ui/board/management/project.h"

GaugeHorizontal::GaugeHorizontal(Board *dashboard) :
    GaugeVertical (dashboard),
    mGaugeHPosition(gpRight)
{

}

void GaugeHorizontal::initialize(Board *dashboard)
{
    if(dashboard) {
        NumericalDisplay::initialize(dashboard);

        mAxisRectThresholds = new AxisRect(dashboard);
        mAxisRectThresholds->axis(QCPAxis::atRight)->setVisible(false);
        mAxisRectThresholds->axis(QCPAxis::atBottom)->setVisible(false);
        mAxisRectThresholds->setMinimumMargins(QMargins(15, 5, 15, 5));
        mAxisRectThresholds->setMaximumSize(QWIDGETSIZE_MAX,5);
        mAxisRectThresholds->axis(QCPAxis::atLeft)->setRange(0, 1);

        auto textColor = QApplication::palette().toolTipText().color();
        mAxisRect = new AxisRect(dashboard);
        mAxisRect->setAxisColor(textColor);

        mAxisRect->setRangeZoomAxes(QList<QCPAxis*>());
        mAxisRect->setRangeDragAxes(QList<QCPAxis*>());

        mAxisRect->axis(QCPAxis::atRight)->setVisible(false);

        mAxisRect->axis(QCPAxis::atBottom)->setBasePen(Qt::NoPen);
        mAxisRect->axis(QCPAxis::atRight)->setBasePen(Qt::NoPen);

        mAxisRect->axis(QCPAxis::atBottom)->setTickPen(QPen(textColor, 0));
        mAxisRect->axis(QCPAxis::atRight)->setTickPen(Qt::NoPen);

        mAxisRect->axis(QCPAxis::atBottom)->setSubTickPen(QPen(textColor, 0));
        mAxisRect->axis(QCPAxis::atRight)->setSubTickPen(Qt::NoPen);

        mAxisRect->axis(QCPAxis::atBottom)->setTickLabelColor(textColor);

        mAxisRect->axis(QCPAxis::atBottom)->setTickLabels(mAxisTicksVisible);
        mAxisRect->axis(QCPAxis::atRight)->setTickLabels(false);

        mAxisRect->axis(QCPAxis::atBottom)->grid()->setVisible(mAxisGridVisible);
        mAxisRect->axis(QCPAxis::atRight)->grid()->setVisible(false);

        mAxisRect->axis(QCPAxis::atBottom)->setOffset(5);

        mAxisRect->axis(QCPAxis::atBottom)->setTickLengthOut(5);
        mAxisRect->axis(QCPAxis::atBottom)->setTickLengthIn(0);
        mAxisRect->axis(QCPAxis::atBottom)->setSubTickLengthOut(2);
        mAxisRect->axis(QCPAxis::atBottom)->setSubTickLengthIn(0);

        mAxisRect->axis(QCPAxis::atRight)->setRange(0.5, 1.5);
        mAxisRect->setMinimumMargins(QMargins(20, 10, 20, 10));

        mAxisRect->setMaximumSize(QWIDGETSIZE_MAX, 200);

        mBar = new QCPBars(mAxisRect->axis(QCPAxis::atRight), mAxisRect->axis(QCPAxis::atTop));
        mBar->setWidth(1);
        mBar->setAntialiased(false);
        mBar->setAntialiasedFill(false);
        mBar->setPen(Qt::NoPen);
        mBar->setBrush(QColor(0,131,179));

        mValueAxisRange = QCPRange(0,0);

        mBarLayout = new LayoutGrid();
        mBarLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);
        mBarLayout->initializeParentPlot(dashboard);
        mBarLayout->setRowSpacing(5);

        mBarLayoutEmptyElementFirst = new QCPLayoutElement(dashboard);
        mBarLayoutEmptyElementFirst->setMinimumSize(0.1,0.1);
        mBarLayoutEmptyElementSecond = new QCPLayoutElement(dashboard);
        mBarLayoutEmptyElementSecond->setMinimumSize(0.1,0.1);

        mBarLayout->addElement(mBarLayoutEmptyElementFirst);
        mBarLayout->addElement(mAxisRectThresholds);
        mBarLayout->addElement(mAxisRect);
        mBarLayout->addElement(mBarLayoutEmptyElementSecond);

        mBarLayout->setRowStretchFactor(0,0.01);
        mBarLayout->setRowStretchFactor(3,0.01);
    }
}

void GaugeHorizontal::edit()
{
    GaugeHorizontalEditor editor(this);
    editor.exec();
}

void GaugeHorizontal::loadSettings(QSettings *settings)
{
    GaugeVertical::loadSettings(settings);

    settings->beginGroup("Gauge");
    if(settings->contains("Position"))
        setGaugeHPosition(GaugeHPosition(settings->value("Position").toInt()));
    settings->endGroup();
}

void GaugeHorizontal::saveSettings(QSettings *settings)
{
    GaugeVertical::saveSettings(settings);

    settings->beginGroup("Gauge");
    settings->setValue("Position", mGaugeHPosition);
    settings->endGroup();
}

void GaugeHorizontal::buildElement()
{
    mSubLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);
    mMainLayout->setFillOrder(QCPLayoutGrid::foColumnsFirst);

    mMainLayout->setColumnSpacing(0);
    mMainLayout->setRowSpacing(0);

    mTextLabel->setVisible(mHeaderVisible);
    mTextUnit->setVisible(mHeaderVisible);
    mTextValue->setVisible(mValueVisible);

    mSubLayout->addElement(mEmptyElementTop);
    mSubLayout->setRowStretchFactor(mSubLayout->elementCount()-1,1);

    if(mHeaderVisible) {
        mSubLayout->addElement(mTextLabel);
        mSubLayout->setRowStretchFactor(mSubLayout->elementCount()-1,0.001);
        mSubLayout->addElement(mTextUnit);
        mSubLayout->setRowStretchFactor(mSubLayout->elementCount()-1,0.001);
    }

    if(mValueVisible) {
        mSubLayout->addElement(mTextValue);
        mSubLayout->setRowStretchFactor(mSubLayout->elementCount()-1,0.001);
    }

    mSubLayout->addElement(mEmptyElementBottom);
    mSubLayout->setRowStretchFactor(mSubLayout->elementCount()-1,1);

    mSubLayout->setRowSpacing(0);
    mSubLayout->setMargins(QMarginsF(0,0,0,0));

    if(mGaugeHPosition == gpRight) {
        if(mHeaderVisible || mValueVisible)
            mMainLayout->addElement(mSubLayout);
        mMainLayout->addElement(mBarLayout);
    } else {
        mMainLayout->addElement(mBarLayout);
        if(mHeaderVisible || mValueVisible)
            mMainLayout->addElement(mSubLayout);
    }

    mSubLayout->needUpdate(true);
    mMainLayout->needUpdate(true);
    mBarLayout->needUpdate(true);

    switch(mBackgroundMode) {
    case bmBackground:
        mAxisRect->setBackground(QApplication::palette().light().color());
        mAxisRectThresholds->setBackground(Qt::NoBrush);
        break;
    case bmTransparent:
        mAxisRect->setBackground(QApplication::palette().dark().color());
        mAxisRectThresholds->setBackground(Qt::NoBrush);
        break;
    }

    if(mAxisTicksVisible || mAxisLabelsVisible || mAxisGridVisible)
        mAxisRect->axis(QCPAxis::atBottom)->setVisible(true);
    else
        mAxisRect->axis(QCPAxis::atBottom)->setVisible(false);

    mAxisRect->axis(QCPAxis::atBottom)->setTickLabels(mAxisLabelsVisible);
    mAxisRect->axis(QCPAxis::atBottom)->grid()->setVisible(mAxisGridVisible);


    auto textColor = QApplication::palette().toolTipText().color();
    textColor = QApplication::palette().toolTipText().color();
    if(mAxisLineVisible) {
        mAxisRect->axis(QCPAxis::atBottom)->setBasePen(QPen(textColor, 0));
    } else {
        mAxisRect->axis(QCPAxis::atBottom)->setBasePen(Qt::NoPen);
    }

    if(mAxisTicksVisible) {
        mAxisRect->axis(QCPAxis::atBottom)->setTickPen(QPen(textColor, 0));
        mAxisRect->axis(QCPAxis::atBottom)->setSubTickPen(QPen(textColor, 0));
        mAxisRect->axis(QCPAxis::atBottom)->setTickLengthOut(5);
        mAxisRect->axis(QCPAxis::atBottom)->setSubTickLengthOut(2);
        mAxisRect->axis(QCPAxis::atBottom)->setTickLabelPadding(3);
    } else {
        mAxisRect->axis(QCPAxis::atBottom)->setTickPen(Qt::NoPen);
        mAxisRect->axis(QCPAxis::atBottom)->setSubTickPen(Qt::NoPen);
        mAxisRect->axis(QCPAxis::atBottom)->setTickLengthOut(0);
        mAxisRect->axis(QCPAxis::atBottom)->setSubTickLengthOut(0);
        mAxisRect->axis(QCPAxis::atBottom)->setTickLabelPadding(0);
    }
}

void GaugeHorizontal::resetElement()
{
    NumericalDisplay::resetElement();

    for(auto tracer : mThresholdTracers)
        mBoard->removeItem(tracer);
    mThresholdTracers.clear();

    QSharedPointer<BoardParameter> dashParam = boardParameter(0);
    if(dashParam) {
        QMap<double, ColorSettings> lowThr = dashParam->parameterConfiguration()->thresholdsSettingsRef().lowThresholds();

        QMap<double, ColorSettings>::iterator lowIt;
        double lower = -std::numeric_limits<double>::max();
        bool lowerFirst = true;
        for (lowIt = lowThr.begin();
             lowIt != lowThr.end(); ++lowIt) {

            auto rect = new QCPItemRect(mBoard);

            rect->bottomRight->setType(QCPItemPosition::ptPlotCoords);
            if(lowerFirst)
                rect->topLeft->setType(QCPItemPosition::ptAxisRectRatio);
            else
                rect->topLeft->setType(QCPItemPosition::ptPlotCoords);

            rect->topLeft->setAxisRect(mAxisRectThresholds);
            rect->bottomRight->setAxisRect(mAxisRectThresholds);
            rect->topLeft->setAxes(mAxisRectThresholds->axis(QCPAxis::atBottom), mAxisRectThresholds->axis(QCPAxis::atLeft));
            rect->bottomRight->setAxes(mAxisRectThresholds->axis(QCPAxis::atBottom), mAxisRectThresholds->axis(QCPAxis::atLeft));

            if(lowerFirst)
                rect->topLeft->setCoords(0, 0);
            else
                rect->topLeft->setCoords(lower, 1);

            rect->bottomRight->setCoords(lowIt.key(), 0);

            rect->setPen(Qt::NoPen);

            QColor color = lowIt.value().color();
            color.setAlpha(150);

            rect->setBrush(QBrush(color));
            rect->setClipAxisRect(mAxisRectThresholds);
            rect->setClipToAxisRect(true);

            mThresholdTracers.append(rect);

            lower = lowIt.key();
            lowerFirst = false;
        }

        QMap<double, ColorSettings> highThr = dashParam->parameterConfiguration()->thresholdsSettingsRef().highThresholds();

        QMapIterator<double, ColorSettings> highIt(highThr);
        highIt.toBack();
        double upper = std::numeric_limits<double>::max();
        bool upperFirst = true;
        while (highIt.hasPrevious()) {
            highIt.previous();

            auto rect = new QCPItemRect(mBoard);
            rect->topLeft->setType(QCPItemPosition::ptPlotCoords);
            if(upperFirst)
                rect->bottomRight->setType(QCPItemPosition::ptAxisRectRatio);
            else
                rect->bottomRight->setType(QCPItemPosition::ptPlotCoords);
            rect->topLeft->setAxisRect(mAxisRectThresholds);
            rect->bottomRight->setAxisRect(mAxisRectThresholds);
            rect->topLeft->setAxes(mAxisRectThresholds->axis(QCPAxis::atBottom), mAxisRectThresholds->axis(QCPAxis::atLeft));
            rect->bottomRight->setAxes(mAxisRectThresholds->axis(QCPAxis::atBottom), mAxisRectThresholds->axis(QCPAxis::atLeft));

            if(upperFirst)
                rect->bottomRight->setCoords(1, 1);
            else
                rect->bottomRight->setCoords(upper, 0);

            rect->topLeft->setCoords(highIt.key(), 1);

            rect->setPen(Qt::NoPen);

            QColor color = highIt.value().color();
            color.setAlpha(150);

            rect->setBrush(QBrush(color));
            rect->setClipAxisRect(mAxisRectThresholds);
            rect->setClipToAxisRect(true);

            mThresholdTracers.append(rect);

            upper = highIt.key();
            upperFirst = false;
        }
    }

    for(int i=0; i<mBarLayout->elementCount(); i++)
        mBarLayout->takeAt(i);
    mBarLayout->simplify();

    if(mThresholdTracers.count() > 0) {

        mBarLayout->addElement(mBarLayoutEmptyElementFirst);
        mBarLayout->addElement(mAxisRectThresholds);
        mBarLayout->addElement(mAxisRect);
        mBarLayout->addElement(mBarLayoutEmptyElementSecond);

        mBarLayout->setRowStretchFactor(0,0.01);
        mBarLayout->setRowStretchFactor(3,0.01);

        mAxisRectThresholds->setMinimumMargins(QMargins(20, 10, 20, 0));
        mAxisRect->setMinimumMargins(QMargins(20, 0, 20, 10));
        mAxisRectThresholds->setVisible(true);
    } else {

        mBarLayout->addElement(mBarLayoutEmptyElementFirst);
        mBarLayout->addElement(mAxisRect);
        mBarLayout->addElement(mBarLayoutEmptyElementSecond);

        mBarLayout->setRowStretchFactor(0,0.01);
        mBarLayout->setRowStretchFactor(2,0.01);

        mAxisRect->setMinimumMargins(QMargins(20, 10, 20, 10));
        mAxisRectThresholds->setVisible(false);
    }
}

void GaugeHorizontal::displayData()
{
    QSharedPointer<BoardParameter> dashParam = boardParameter(0);
    if(dashParam && dashParam->getTimeSeries().parameterId() > 0) {
        dashParam->processValueData();
        if(mValueVisible) {
            mTextValue->setText(dashParam->getValueString(mValueFormat));
            mTextValue->setTextColor(dashParam->getForegroundColor());
            mTextValue->setBackgroundBrush(dashParam->getBackgroundBrush());
        }

        QColor colorBar;

        switch(dashParam->parameterConfiguration()->itemColorMode()) {
        case ParameterConfiguration::icBase:
            colorBar = (dashParam->parameterConfiguration()->defaultColorSettingsRef().color());
            break;
        case ParameterConfiguration::icDynamic:
        case ParameterConfiguration::icDynamicSegmented:
            colorBar = (dashParam->getColor());
            break;
        case ParameterConfiguration::icCustom:
            colorBar = (dashParam->parameterConfiguration()->itemStaticColor());
            break;
        }

        mBar->setBrush(colorBar);

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

        mAxisRectThresholds->axis(QCPAxis::atBottom)->setRange(mAxisRect->axis(QCPAxis::atBottom)->range());

        if(mTankGauge) {
            QCPRange rangeTank(0, mAxisRect->axis(QCPAxis::atBottom)->range().size());
            mBar->data()->clear();
            mBar->addData(1, val - mAxisRect->axis(QCPAxis::atBottom)->range().lower);
            mAxisRect->axis(QCPAxis::atTop)->setRange(rangeTank);
        } else {
            mAxisRect->axis(QCPAxis::atTop)->setRange(mAxisRect->axis(QCPAxis::atBottom)->range());
        }
    }
}

void GaugeHorizontal::applySizeConstraints()
{
    QSizeF defaultSize = mBoard->dashboardLayout()->singleElementSize();
    defaultSize.rwidth() -= mMargins.left() + mMargins.right() + mMainLayout->margins().left() + mMainLayout->margins().right();
    defaultSize.rheight() -= mMargins.top() + mMargins.bottom() + mMainLayout->margins().top() + mMainLayout->margins().bottom();

    QSizeF currentSize = mRect.size();
    currentSize.rwidth() -= mMainLayout->margins().left() + mMainLayout->margins().right();
    currentSize.rheight() -= mMainLayout->margins().top() + mMainLayout->margins().bottom();

    QSizeF labelSize = defaultSize;

    if(currentSize.height() < defaultSize.height())
        labelSize.setHeight(currentSize.height());

    mTextLabel->setMinimumSize(labelSize.width(), LABEL_HEIGHT_RATIO * labelSize.height());
    mTextUnit->setMinimumSize(labelSize.width(), UNIT_HEIGHT_RATIO * labelSize.height());
    mTextMode->setMinimumSize(labelSize.width(), UNIT_HEIGHT_RATIO * labelSize.height());
    mTextValue->setMinimumSize(labelSize.width(), VALUE_HEIGHT_RATIO * labelSize.height());
    mSubLayout->setMaximumSize(labelSize.width(), currentSize.height());
}

void GaugeHorizontal::setAxisTicksVisible(bool axisTicksVisible)
{
    mAxisTicksVisible = axisTicksVisible;
}

void GaugeHorizontal::setAxisLabelsVisible(bool axisLabelsVisible)
{
    mAxisLabelsVisible = axisLabelsVisible;
}

void GaugeHorizontal::setAxisGridVisible(bool axisGridVisible)
{
    mAxisGridVisible = axisGridVisible;
}

void GaugeHorizontal::setAxisLineVisible(bool axisLineVisible)
{
    mAxisLineVisible = axisLineVisible;
}

GaugeHorizontal::GaugeHPosition GaugeHorizontal::gaugeHPosition() const
{
    return mGaugeHPosition;
}

void GaugeHorizontal::setGaugeHPosition(const GaugeHPosition &gaugePosition)
{
    mGaugeHPosition = gaugePosition;
}

int GaugeHorizontal::defaultWidth()
{
    return 4*mBoard->dashboardLayout()->singleElementColumnCount();
}

int GaugeHorizontal::defaultHeight()
{
    return mBoard->dashboardLayout()->singleElementRowCount();
}
