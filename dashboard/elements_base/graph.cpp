#include "graph.h"

QTBGraph::QTBGraph(QCPAxis *keyAxis, QCPAxis *valueAxis) :
    QCPGraph (keyAxis, valueAxis)
{

}

void QTBGraph::addColoredSegment(double lowerValue, double upperValue, QPen pen, QBrush brush)
{
    if(qFuzzyCompare(lowerValue, upperValue)) {
        qDebug() << "Invalid segment : identical values";
        return;
    }

    mLowerValues.append(lowerValue);
    mUpperValues.append(upperValue);
    mPens.append(pen);
    mBrushs.append(brush);
}

void QTBGraph::clearColoredSegments()
{
    mLowerValues.clear();
    mUpperValues.clear();
    mPens.clear();
    mBrushs.clear();
}

void QTBGraph::draw(QCPPainter *painter)
{
    if (!mKeyAxis || !mValueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return; }
    if (mKeyAxis.data()->range().size() <= 0 || mDataContainer->isEmpty()) return;
    if (mLineStyle == lsNone && mScatterStyle.isNone()) return;

    QVector<QPointF> lines, scatters; // line and (if necessary) scatter pixel coordinates will be stored here while iterating over segments

    QCPDataSelection thresholdsSelection;
    QList<QTBColoredSegment> allSegments;

    for(int index = 0; index < mLowerValues.count(); index++) {
        QCPGraphDataContainer::const_iterator begin = mDataContainer->constEnd();
        for (QCPGraphDataContainer::const_iterator it=mDataContainer->constBegin(); it!=mDataContainer->constEnd(); ++it)
        {
            if(it->value >= mLowerValues.at(index) && it->value < mUpperValues.at(index)) {
                if(begin == mDataContainer->constEnd())
                    begin = it;
            } else {
                if(begin != mDataContainer->constEnd()) {
                    thresholdsSelection.addDataRange(QCPDataRange(begin-mDataContainer->constBegin(), it-mDataContainer->constBegin()));
                    allSegments.append(QTBColoredSegment(QCPDataRange(begin-mDataContainer->constBegin(), it-mDataContainer->constBegin()), mPens.at(index), mBrushs.at(index)));
                    begin = mDataContainer->constEnd();
                }
            }
        }
        if(begin != mDataContainer->constEnd()) {
            thresholdsSelection.addDataRange(QCPDataRange(begin-mDataContainer->constBegin(), dataCount()));
            allSegments.append(QTBColoredSegment(QCPDataRange(begin-mDataContainer->constBegin(), dataCount()), mPens.at(index), mBrushs.at(index)));
        }
    }

    auto baseSegments = thresholdsSelection.inverse(QCPDataRange(0, dataCount())).dataRanges();

    for(int k=0; k< baseSegments.count(); k++) {
        allSegments.append(QTBColoredSegment(baseSegments.at(k).adjusted(-1,1), mPen, mBrush));
    }

    std::sort(allSegments.begin(), allSegments.end(), lessThanSegment);

    for (int i=0; i<allSegments.size(); ++i)
    {
        QCPDataRange lineDataRange = allSegments.at(i).dataRange();
        getLines(&lines, lineDataRange);

        painter->setBrush(allSegments.at(i).brush());
        painter->setPen(Qt::NoPen);
        drawFill(painter, &lines);

        // draw line:
        if (mLineStyle != lsNone)
        {
            painter->setPen(allSegments.at(i).pen());
            painter->setBrush(Qt::NoBrush);
            if (mLineStyle == lsImpulse)
                drawImpulsePlot(painter, lines);
            else
                drawLinePlot(painter, lines); // also step plots can be drawn as a line plot
        }

        // draw scatters:
        QCPScatterStyle finalScatterStyle = mScatterStyle;
        if (!finalScatterStyle.isNone())
        {
            finalScatterStyle.setPen(allSegments.at(i).pen());
            getScatters(&scatters, allSegments.at(i).dataRange());
            drawScatterPlot(painter, scatters, finalScatterStyle);
        }
    }
}

void QTBGraph::drawFill(QCPPainter *painter, QVector<QPointF> *lines) const
{
    if (mLineStyle == lsImpulse) return; // fill doesn't make sense for impulse plot
    if (painter->brush().style() == Qt::NoBrush || painter->brush().color().alpha() == 0) return;

    applyFillAntialiasingHint(painter);
    QVector<QCPDataRange> segments = getNonNanSegments(lines, keyAxis()->orientation());
    if (!mChannelFillGraph)
    {
        // draw base fill under graph, fill goes all the way to the zero-value-line:
        for (int i=0; i<segments.size(); ++i)
            painter->drawPolygon(getCustomFillPolygon(lines, segments.at(i)));
    } else
    {
        // draw fill between this graph and mChannelFillGraph:
        QVector<QPointF> otherLines;
        mChannelFillGraph->getLines(&otherLines, QCPDataRange(0, mChannelFillGraph->dataCount()));
        if (!otherLines.isEmpty())
        {
            QVector<QCPDataRange> otherSegments = getNonNanSegments(&otherLines, mChannelFillGraph->keyAxis()->orientation());
            QVector<QPair<QCPDataRange, QCPDataRange> > segmentPairs = getOverlappingSegments(segments, lines, otherSegments, &otherLines);
            for (int i=0; i<segmentPairs.size(); ++i)
                painter->drawPolygon(getChannelFillPolygon(lines, segmentPairs.at(i).first, &otherLines, segmentPairs.at(i).second));
        }
    }
}


const QPolygonF QTBGraph::getCustomFillPolygon(const QVector<QPointF> *lineData, QCPDataRange segment) const
{
    if (segment.size() < 2)
        return QPolygonF();
    QPolygonF result(segment.size()+2);

    result[0] = getCustomFillBasePoint(lineData->at(segment.begin()));
    std::copy(lineData->constBegin()+segment.begin(), lineData->constBegin()+segment.end(), result.begin()+1);
    result[result.size()-1] = getCustomFillBasePoint(lineData->at(segment.end()-1));

    return result;
}

QPointF QTBGraph::getCustomFillBasePoint(QPointF matchingDataPoint) const
{
    QCPAxis *keyAxis = mKeyAxis.data();
    QCPAxis *valueAxis = mValueAxis.data();
    if (!keyAxis || !valueAxis) { qDebug() << Q_FUNC_INFO << "invalid key or value axis"; return QPointF(); }

    double fillRefValue;
    if(mFillStyle == fsZero)
        fillRefValue = valueAxis->coordToPixel(0);
    else
        fillRefValue = valueAxis->axisRect()->bottom();

    QPointF result;
    if (valueAxis->scaleType() == QCPAxis::stLinear)
    {
        if (keyAxis->orientation() == Qt::Horizontal)
        {
            result.setX(matchingDataPoint.x());
            result.setY(fillRefValue);
        } else // keyAxis->orientation() == Qt::Vertical
        {
            result.setX(fillRefValue);
            result.setY(matchingDataPoint.y());
        }
    } else // valueAxis->mScaleType == QCPAxis::stLogarithmic
    {
        // In logarithmic scaling we can't just draw to value 0 so we just fill all the way
        // to the axis which is in the direction towards 0
        if (keyAxis->orientation() == Qt::Vertical)
        {
            if ((valueAxis->range().upper < 0 && !valueAxis->rangeReversed()) ||
                    (valueAxis->range().upper > 0 && valueAxis->rangeReversed())) // if range is negative, zero is on opposite side of key axis
                result.setX(keyAxis->axisRect()->right());
            else
                result.setX(keyAxis->axisRect()->left());
            result.setY(matchingDataPoint.y());
        } else if (keyAxis->axisType() == QCPAxis::atTop || keyAxis->axisType() == QCPAxis::atBottom)
        {
            result.setX(matchingDataPoint.x());
            if ((valueAxis->range().upper < 0 && !valueAxis->rangeReversed()) ||
                    (valueAxis->range().upper > 0 && valueAxis->rangeReversed())) // if range is negative, zero is on opposite side of key axis
                result.setY(keyAxis->axisRect()->top());
            else
                result.setY(keyAxis->axisRect()->bottom());
        }
    }
    return result;
}

QTBGraph::FillStyle QTBGraph::getFillStyle() const
{
    return mFillStyle;
}

void QTBGraph::setFillStyle(const FillStyle &fillStyle)
{
    mFillStyle = fillStyle;
}
