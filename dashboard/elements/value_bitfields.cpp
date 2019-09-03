#include "value_bitfields.h"
#include "ui/element_editors/value_bitfields_editor.h"
#include <bitset>

QTBValueBitfields::QTBValueBitfields(QTBoard *dashboard) :
    QTBValueDisplay (dashboard),
    mBitsSize(32)
{
    mValueFormat = vsfHexa;
}

void QTBValueBitfields::clearElement()
{
    mAxisRect->clearPlottables();

    mBitsLayout->clear();
    delete mBitsLayout;

    QTBValueDisplay::clearElement();
}

void QTBValueBitfields::initializeElement(QTBoard *dashboard)
{
    if(dashboard) {
        QTBDashboardElement::initializeElement(dashboard);

        mBitsLayout = new QTBLayoutGrid();
        mBitsLayout->initializeParentPlot(dashboard);
        mBitsLayout->setRowSpacing(0);
        mBitsLayout->setColumnSpacing(1);
        //        mBitsLayout->setWrap(2);

        mBitsLayout->setWrap(1);
        mLayout = new QTBLayoutGrid();
        mLayout->initializeParentPlot(dashboard);
        mLayout->setRowSpacing(0);
        mLayout->setColumnSpacing(1);

        mAxisRect = new QTBAxisRect(dashboard);
        mAxisRect->setMinimumMargins(QMargins(1, 1, 1, 1));
        mAxisRect->setMargins(QMargins(1, 1, 1, 1));
        mAxisRect->setSizeConstraintRect(scrOuterRect);
        mAxisRect->setMinimumSize(1,1);
        mAxisRect->showAxis(QCPAxis::atLeft, false);
        mAxisRect->showAxis(QCPAxis::atBottom, false);
        mAxisRect->setAutoMargins(QCP::msNone);
        mAxisRect->axis(QCPAxis::atLeft)->setRange(0,32);
        mAxisRect->axis(QCPAxis::atLeft)->setTicks(false);
        mAxisRect->axis(QCPAxis::atLeft)->setTickLabels(false);
        mAxisRect->axis(QCPAxis::atLeft)->grid()->setVisible(false);
        mAxisRect->axis(QCPAxis::atBottom)->setTicks(false);
        mAxisRect->axis(QCPAxis::atBottom)->setTickLabels(false);
        mAxisRect->axis(QCPAxis::atBottom)->grid()->setVisible(false);

        for(int i=31; i>=0; i--) {

            QTBAdjustTextElement *bitLabel = new QTBAdjustTextElement(dashboard);
            bitLabel->setMaxPointSize(8);
            //            bitLabel->setAdjustStrategy(QTBAdjustTextElement::asElide);
            bitLabel->setTextColor(QColor(160, 170, 180));
            bitLabel->setMinimumSize(100,1);
            bitLabel->setTextFlags(Qt::AlignVCenter | Qt::AlignLeft);
            mBitLabels.append(bitLabel);

            QCPGraph *fillGraph = dashboard->addGraph(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
            fillGraph->setPen(Qt::NoPen);
            fillGraph->setBrush(Qt::NoBrush);
            fillGraph->addData(std::numeric_limits<double>::min(), double(i)+0.1);
            fillGraph->addData(std::numeric_limits<double>::max(), double(i)+0.1);

            QCPGraph *bitGraph = dashboard->addGraph(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atLeft));
            bitGraph->setAntialiased(false);
            bitGraph->setAntialiasedFill(false);
            bitGraph->setChannelFillGraph(fillGraph);
            bitGraph->setPen(Qt::NoPen);
            bitGraph->setBrush(QColor(0,131,179));
            bitGraph->setLineStyle(QCPGraph::lsStepLeft);
            mGraphs.append(bitGraph);
        }

        mBitsLayout->setSizeConstraintRect(scrOuterRect);

        QTBValueDisplay::initializeElement(dashboard);
    }
}

void QTBValueBitfields::buildLayout()
{
    for(int i=0; i< mLayout->elementCount(); i++) {
        QCPLayoutElement *el = mLayout->takeAt(i);
        el->setVisible(false);
    }
    mLayout->simplify();

    for(int i=0; i< mBitsLayout->elementCount(); i++) {
        QCPLayoutElement *el = mBitsLayout->takeAt(i);
        el->setVisible(false);
    }
    mBitsLayout->simplify();

    mSubLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);
    mMainLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);
    mLayout->setFillOrder(QCPLayoutGrid::foColumnsFirst);

    mBitsLayout->setVisible(true);
    mAxisRect->setVisible(true);

    switch(mValueFormat) {
    case vsfDecimal:
    case vsfHexa:
    {
        mBitsSize = 32;
        mAxisRect->axis(QCPAxis::atLeft)->setRange(0,32);
        for(int i=0; i< mBitsSize; i++) {
            mBitLabels.at(i)->setVisible(true);
            mBitsLayout->addElement(mBitLabels.at(i));
        }
        break;
    }
    case vsfHexaLsb16:
    {
        mBitsSize = 16;
        mAxisRect->axis(QCPAxis::atLeft)->setRange(0,16);
        for(int i=16; i< 2*mBitsSize; i++) {
            mBitLabels.at(i)->setVisible(true);
            mBitsLayout->addElement(mBitLabels.at(i));
        }
        break;
    }
    case vsfHexaMsb16:
    {
        mBitsSize = 16;
        mAxisRect->axis(QCPAxis::atLeft)->setRange(16,32);
        for(int i=0; i< mBitsSize; i++) {
            mBitLabels.at(i)->setVisible(true);
            mBitsLayout->addElement(mBitLabels.at(i));
        }
        break;
    }
    }

    mLayout->addElement(mBitsLayout);
    mLayout->addElement(mAxisRect);
    mLayout->setColumnStretchFactor(0,1);
    mLayout->setColumnStretchFactor(1,3);

    mSubLayout->setRowSpacing(0);
    mMainLayout->setRowSpacing(0);

    if(mHeaderVisible) {
        mSubLayout->addElement(mTextLabel);
        mSubLayout->addElement(mTextUnit);
    }

    mTextLabel->setVisible(mHeaderVisible);
    mTextUnit->setVisible(mHeaderVisible);
    mTextValue->setVisible(mValueVisible);

    if(mHeaderVisible)
        mMainLayout->addElement(mSubLayout);
    if(mValueVisible)
        mMainLayout->addElement(mTextValue);

    mMainLayout->addElement(mLayout);
    mMainLayout->setRowStretchFactor(mMainLayout->rowCount()-1 , 0.01);

    mSubLayout->needUpdate(true);
    mMainLayout->needUpdate(true);
    mLayout->needUpdate(true);
    mBitsLayout->needUpdate(true);

}

void QTBValueBitfields::edit()
{
    QTBValueBitfieldsEditor editor(this);
    editor.exec();
}

void QTBValueBitfields::addDashboardParameter(QSharedPointer<QTBDashboardParameter> dashParameter)
{
    QTBDashboardElement::addDashboardParameter(dashParameter);
    processHistoricalSamples();
}

void QTBValueBitfields::saveParametersSettings(QSettings *settings, QTBParameterConfiguration::ConfigurationModule mode)
{
    Q_UNUSED(mode)
    QTBDashboardElement::saveParametersSettings(settings, QTBParameterConfiguration::cmBitFields);
}

void QTBValueBitfields::loadParametersSettings(QSettings *settings, QTBParameterConfiguration::ConfigurationModule mode)
{
    Q_UNUSED(mode)
    QTBDashboardElement::loadParametersSettings(settings, QTBParameterConfiguration::cmBitFields);

    updateElement();
}

void QTBValueBitfields::processNewSamples()
{
    QSharedPointer<QTBDashboardParameter> dashParam = dashParameter(0);
    if(dashParam && dashParam->getParameterId() > 0) {

        std::bitset<32> bits(dashParam->getValueBinaryWeight32());

        for (std::size_t i = 0; i < std::size_t(mBitsSize); ++i) {
            std::size_t bitIndex;
            int graphIndex;

            switch(mValueFormat) {
            case vsfDecimal:
            case vsfHexa:
            {
                bitIndex = i;
                graphIndex = mBitsSize - int(bitIndex) - 1;
                break;
            }
            case vsfHexaLsb16:
            {
                bitIndex = i;
                graphIndex = 2*mBitsSize - int(bitIndex) - 1;
                break;
            }
            case vsfHexaMsb16:
            {
                bitIndex = i + std::size_t(mBitsSize);
                graphIndex = 2*mBitsSize - int(bitIndex) - 1;
                break;
            }
            }

            if(dashParam->getBitLogic(int(bitIndex))) {
                if(!bits.test(bitIndex))
                    mGraphs.at(graphIndex)->addData(dashParam->getTimestamp(), qQNaN());
                else
                    mGraphs.at(graphIndex)->addData(dashParam->getTimestamp(), double(bitIndex) +0.9);
            } else {
                if(bits.test(bitIndex))
                    mGraphs.at(graphIndex)->addData(dashParam->getTimestamp(), qQNaN());
                else
                    mGraphs.at(graphIndex)->addData(dashParam->getTimestamp(), double(bitIndex) +0.9);
            }

            mGraphs.at(graphIndex)->data()->removeBefore(dashParam->getTimestamp()-5);
        }

        mAxisRect->axis(QCPAxis::atBottom)->setRange(mBoard->currentTimestamp() - 5, mBoard->currentTimestamp());

        mTextValue->setText(dashParam->getValueString(mValueFormat));
    } else {
        mTextValue->setText("X");
    }
}

void QTBValueBitfields::processHistoricalSamples()
{
    QSharedPointer<QTBDashboardParameter> dashParam = dashParameter(0);
    if(dashParam) {

        if(mBoard->dataManager()) {
            QTBDataSerie serie = mBoard->dataManager()->dataSerie(dashParam->getParameterId());
            QCPDataContainer<QTBDataSample>::const_iterator it;
            for (it = serie.constBegin(); it != serie.constEnd(); ++it) {
                std::bitset<32> bits(it->value().uint32_value());

                for (std::size_t i = 0; i < std::size_t(mBitsSize); ++i) {
                    std::size_t bitIndex;
                    int graphIndex;

                    switch(mValueFormat) {
                    case vsfDecimal:
                    case vsfHexa:
                    {
                        bitIndex = i;
                        graphIndex = mBitsSize - int(bitIndex) - 1;
                        break;
                    }
                    case vsfHexaLsb16:
                    {
                        bitIndex = i;
                        graphIndex = 2*mBitsSize - int(bitIndex) - 1;
                        break;
                    }
                    case vsfHexaMsb16:
                    {
                        bitIndex = i + std::size_t(mBitsSize);
                        graphIndex = 2*mBitsSize - int(bitIndex) - 1;
                        break;
                    }
                    }

                    if(dashParam->getBitLogic(int(bitIndex))) {
                        if(!bits.test(bitIndex))
                            mGraphs.at(graphIndex)->addData(it->datationSecFromMidnight(), qQNaN());
                        else
                            mGraphs.at(graphIndex)->addData(it->datationSecFromMidnight(), double(bitIndex) +0.9);
                    } else {
                        if(bits.test(bitIndex))
                            mGraphs.at(graphIndex)->addData(it->datationSecFromMidnight(), qQNaN());
                        else
                            mGraphs.at(graphIndex)->addData(it->datationSecFromMidnight(), double(bitIndex) +0.9);
                    }
                }
            }
        }
    }
}

void QTBValueBitfields::updateElement()
{
    if(mValueFormat == vsfHexa) {
        mBitsSize = 32;
    } else {
        mBitsSize = 16;
    }
    QSharedPointer<QTBDashboardParameter> dashParam = dashParameter(0);
    if(dashParam) {
        QColor color = dashParam->parameterConfiguration()->defaultColorSettingsRef().color();
        for(int i=31; i>=0; i--) {
            mGraphs.at(i)->setBrush(color);
            mBitLabels.at(i)->setText(QString("%1/%2 %3").arg(31-i, 2, 10, QChar('0'))
                                      .arg(dashParam->getBitLogic(31-i))
                                      .arg(dashParam->getBitDescription(31-i)));
        }

        mTextValue->setTextColor(dashParam->parameterConfiguration()->defaultColorSettingsRef().foregroundColor());
        mTextValue->setBackgroundBrush(dashParam->parameterConfiguration()->defaultColorSettingsRef().backgroundBrush());
    } else {
        if(mGraphs.count() >= mBitsSize) {
            for (int i = 0; i < mBitsSize; ++i) {
                int bitIndex = i;
                if(mValueFormat == vsfHexaMsb16)
                    bitIndex += mBitsSize;
                mGraphs.at(mBitsSize - bitIndex - 1)->data()->clear();
                mBitLabels.at(mBitsSize - bitIndex - 1)->setText(QString("%1").arg(bitIndex, 2, 10, QChar('0')));
            }
        }
    }

    QTBValueDisplay::updateElement();

}

void QTBValueBitfields::updateSizeConstraints()
{
    if(mValueVisible) {
        QSizeF singleElementSize = mBoard->dashboardLayout()->singleElementSize();
        QSizeF labelSize = singleElementSize;
        if(mRect.height() < singleElementSize.height())
            labelSize.setHeight(mRect.height());

        mTextValue->setMaximumSize(mRect.width(), 0.5 * labelSize.height());
    }

    QTBValueDisplay::updateSizeConstraints();
}

void QTBValueBitfields::checkSizeAndVisibility()
{
    if(mValueVisible) {
        if(!mTextValue->visible()) {
            mTextValue->setMaximumSize(mRect.width(), 0);
            mMainLayout->needUpdate(true);
        }
    }
    QTBValueDisplay::checkSizeAndVisibility();
}

int QTBValueBitfields::defaultWidth()
{
    return 2*mBoard->dashboardLayout()->singleElementColumnCount();
}

int QTBValueBitfields::defaultHeight()
{
    return 6*mBoard->dashboardLayout()->singleElementRowCount();
}
