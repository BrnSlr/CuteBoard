#include "graph_tracer.h"

PlotTimeTracer::PlotTimeTracer(AxisRect *axisrect) : QObject(axisrect),
    mAxisRect(axisrect),
    mVerticalLine(nullptr),
    mTimeText(nullptr),
    mVisible(false),
    mIsInitialized(false)
{

}

PlotTimeTracer::~PlotTimeTracer()
{
    if(mAxisRect) {
        mAxisRect->parentPlot()->removeItem(mVerticalLine);

        for(auto tracer : mTracers)
            mAxisRect->parentPlot()->removeItem(tracer);
        for(auto arrow : mArrows)
            mAxisRect->parentPlot()->removeItem(arrow);
        for(auto text : mTexts)
            mAxisRect->parentPlot()->removeItem(text);

        mAxisRect->parentPlot()->removeItem(mTimeText);
    }
}

void PlotTimeTracer::initialize()
{
    if(!mIsInitialized && mAxisRect) {
        mVerticalLine = new QCPItemStraightLine(mAxisRect->parentPlot());
        mVerticalLine->setLayer(QLatin1String("tracer"));
        mVerticalLine->setPen(QPen(mLineColor, 0, Qt::DashLine));
        mVerticalLine->setClipAxisRect(mAxisRect);
        mVerticalLine->setClipToAxisRect(true);
        mVerticalLine->point1->setAxisRect(mAxisRect);
        mVerticalLine->point1->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atRight));
        mVerticalLine->point2->setAxisRect(mAxisRect);
        mVerticalLine->point2->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atRight));
        mVerticalLine->point1->setCoords(0, 0);
        mVerticalLine->point2->setCoords(0, 0);
        mVerticalLine->setVisible(mVisible);

        for(auto graph : mAxisRect->graphs()) {
            auto tracer = new QCPItemTracer(mAxisRect->parentPlot());
            tracer->position->setAxisRect(mAxisRect);
            tracer->position->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atRight));
            tracer->setLayer(QLatin1String("tracer"));
            tracer->position->setType(QCPItemPosition::ptPlotCoords);
            tracer->setClipAxisRect(mAxisRect);
            tracer->setClipToAxisRect(true);
            tracer->setStyle(QCPItemTracer::tsSquare);
            tracer->setPen(graph->pen());
            tracer->setBrush(Qt::NoBrush);
            tracer->setGraph(graph);
            tracer->setSize(7);
            tracer->setVisible(mVisible);
            tracer->setInterpolating(false);

            mTracers.append(tracer);

            auto arrow = new QCPItemLine(mAxisRect->parentPlot());
            arrow->setLayer(QLatin1String("tracer"));
            arrow->end->setParentAnchor(tracer->position);
            arrow->start->setParentAnchor(arrow->end);
            arrow->start->setCoords(15, 0);
            arrow->start->setAxisRect(mAxisRect);
            arrow->start->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atRight));
            arrow->end->setAxisRect(mAxisRect);
            arrow->end->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atRight));
            arrow->setClipAxisRect(mAxisRect);
            arrow->setClipToAxisRect(true);
            arrow->setPen(graph->pen());
            arrow->setVisible(mVisible);

            mArrows.append(arrow);

            auto text = new QCPItemText(mAxisRect->parentPlot());
            text->setLayer(QLatin1String("tracer"));
            text->setClipAxisRect(mAxisRect);
            text->setClipToAxisRect(true);
            text->setPadding(QMargins(8, 0, 8, 0));
            text->setPen(graph->pen());
            text->setColor(graph->pen().color());
            text->setBrush(mTextBrush);
            text->setPositionAlignment(Qt::AlignLeft|Qt::AlignVCenter);
            text->position->setParentAnchor(arrow->start);
            text->position->setAxisRect(mAxisRect);
            text->position->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atRight));
            text->setVisible(mVisible);

            mTexts.append(text);
        }

        mTimeText = new QCPItemText(mAxisRect->parentPlot());
        mTimeText->setLayer(QLatin1String("tracer"));
        mTimeText->setClipAxisRect(mAxisRect);
        mTimeText->setClipToAxisRect(false);
        mTimeText->setPadding(QMargins(8, 0, 8, 0));
        mTimeText->setPen(QPen(mLineColor, 0));
        mTimeText->setColor(mTextColor);
        mTimeText->setBrush(mTextBrush);
        mTimeText->setPositionAlignment(Qt::AlignHCenter);
        mTimeText->position->setAxisRect(mAxisRect);
        mTimeText->position->setAxes(mAxisRect->axis(QCPAxis::atBottom), mAxisRect->axis(QCPAxis::atRight));
        mTimeText->position->setTypeX(QCPItemPosition::ptPlotCoords);
        mTimeText->position->setTypeY(QCPItemPosition::ptAxisRectRatio);
        mTimeText->setVisible(mVisible);

        mIsInitialized = true;
    } else {
        qDebug() << Q_FUNC_INFO << "Already initialized or axisRect null";
    }
}

void PlotTimeTracer::setVisible(bool visible)
{
    if(!mIsInitialized)
        return;

    if(visible != mVisible) {
        mVisible = visible;
        if(mVerticalLine)
            mVerticalLine->setVisible(mVisible);

        for(auto tracer : mTracers)
            tracer->setVisible(mVisible);
        for(auto arrow : mArrows)
            arrow->setVisible(mVisible);
        for(auto text : mTexts)
            text->setVisible(mVisible);
        if(mTimeText)
            mTimeText->setVisible(mVisible);
    }
}

bool PlotTimeTracer::visible() const
{
    return mVisible;
}

QCPAxis::AxisType PlotTimeTracer::yAxisType() const
{
    return mYAxisType;
}

void PlotTimeTracer::setYAxisType(const QCPAxis::AxisType &yAxisType)
{
    mYAxisType = yAxisType;
}

QPointF PlotTimeTracer::mousePos() const
{
    return mMousePos;
}

void PlotTimeTracer::setMousePos(const QPointF &mousePos)
{
    mMousePos = mousePos;
    updateItems();
}

void PlotTimeTracer::updateItems()
{
    if(mIsInitialized) {
        double keyValue = mAxisRect->axis(QCPAxis::atBottom)->pixelToCoord(mMousePos.x());

        mVerticalLine->point1->setCoords(keyValue, mAxisRect->axis(mYAxisType)->range().lower);
        mVerticalLine->point2->setCoords(keyValue, mAxisRect->axis(mYAxisType)->range().upper);

        if(mAxisRect->rect().right() - mMousePos.x() < 200) {
            for(int i=0; i<mTracers.count();i++) {
                mTracers.at(i)->setGraphKey(keyValue);
                mTracers.at(i)->updatePosition();
                mArrows.at(i)->start->setCoords(-15, 0);
                mTexts.at(i)->setPositionAlignment(Qt::AlignRight|Qt::AlignVCenter);
                mTexts.at(i)->setText(QString::number(mTracers.at(i)->position->value()));
            }
        } else {
            for(int i=0; i<mTracers.count();i++) {
                mTracers.at(i)->setGraphKey(keyValue);
                mTracers.at(i)->updatePosition();
                mArrows.at(i)->start->setCoords(15, 0);
                mTexts.at(i)->setPositionAlignment(Qt::AlignLeft|Qt::AlignVCenter);
                mTexts.at(i)->setText(QString::number(mTracers.at(i)->position->value()));
            }
        }
        mTimeText->position->setCoords(keyValue, 0);
        mTimeText->setText(QDateTime::fromMSecsSinceEpoch(qint64(keyValue*1000)).toUTC().toString("HH:mm:ss.zzz"));
    } else {
        qDebug() << Q_FUNC_INFO << "Not initialized";
    }

}

QColor PlotTimeTracer::lineColor() const
{
    return mLineColor;
}

void PlotTimeTracer::setLineColor(const QColor &lineColor)
{
    mLineColor = lineColor;
}

QColor PlotTimeTracer::textColor() const
{
    return mTextColor;
}

void PlotTimeTracer::setTextColor(const QColor &textColor)
{
    mTextColor = textColor;
}

QBrush PlotTimeTracer::textBrush() const
{
    return mTextBrush;
}

void PlotTimeTracer::setTextBrush(const QBrush &textBrush)
{
    mTextBrush = textBrush;
}
