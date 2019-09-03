#include "axisrect.h"

QTBAxisRect::QTBAxisRect(QTBoard *dashboard) :
    QCPAxisRect(dashboard),
    mBottomAxisVisible(true),
    mLeftAxisVisible(true)
{
    setMinimumMargins(QMargins(22,10,22,10));
    setMinimumSize(1, 1);
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

    axis(QCPAxis::atBottom)->setBasePen(Qt::NoPen);
    axis(QCPAxis::atLeft)->setBasePen(Qt::NoPen);
    axis(QCPAxis::atBottom)->setTickPen(Qt::NoPen);
    axis(QCPAxis::atLeft)->setTickPen(Qt::NoPen);
    axis(QCPAxis::atBottom)->setSubTickPen(Qt::NoPen);
    axis(QCPAxis::atLeft)->setSubTickPen(Qt::NoPen);
    axis(QCPAxis::atBottom)->setTickLabelColor(dashboard->frontColor());
    axis(QCPAxis::atLeft)->setTickLabelColor(dashboard->frontColor());
    axis(QCPAxis::atBottom)->grid()->setPen(QPen(dashboard->frontColor(), 1, Qt::DotLine));
    axis(QCPAxis::atLeft)->grid()->setPen(QPen(dashboard->frontColor(), 1, Qt::DotLine));
    axis(QCPAxis::atBottom)->grid()->setZeroLinePen(Qt::NoPen);
    axis(QCPAxis::atLeft)->grid()->setZeroLinePen(Qt::NoPen);

    axis(QCPAxis::atLeft)->ticker()->setTickCount(3);
}

void QTBAxisRect::showAxis(QCPAxis::AxisType at, bool visible)
{
    axis(at)->setVisible(visible);
    if(at == QCPAxis::atBottom) {
        mBottomAxisVisible = visible;
    }

    if(at == QCPAxis::atLeft) {
        mLeftAxisVisible = visible;
    }
}

void QTBAxisRect::clearPlottables()
{
    QList<QCPAbstractPlottable*> plotabblesList = plottables();
    int c = plotabblesList.size();

    for (int i=c-1; i >= 0; --i)
        mParentPlot->removePlottable(plotabblesList[i]);
}

void QTBAxisRect::update(QCPLayoutElement::UpdatePhase phase)
{
    QCPAxisRect::update(phase);

//    switch (phase)
//    {
//    case upPreparation:
//    {
//        break;
//    }
//    case upLayout:
//    {
//        if(mBottomAxisVisible) {
//            QVector<QString> bottomTickVectorLabels = axis(QCPAxis::atBottom)->tickVectorLabels();
//            QFont bottomTickLabelsFont = axis(QCPAxis::atBottom)->tickLabelFont();
//            QFontMetrics bottomFontMetrics(bottomTickLabelsFont);

//            int bottomTickLabelsWidth = 0;
//            int bottomTickLabelsHeight = 0;
//            for(int i=0;i<bottomTickVectorLabels.size();i++) {
//                bottomTickLabelsWidth += bottomFontMetrics.boundingRect(bottomTickVectorLabels.at(i)).width();
//                if(bottomFontMetrics.height() > bottomTickLabelsHeight)
//                    bottomTickLabelsHeight = bottomFontMetrics.height();
//            }

//            bottomTickLabelsWidth += (bottomTickVectorLabels.size() - 1);

//            if((bottomTickLabelsWidth < mOuterRect.width()) &&
//                    (3*bottomTickLabelsHeight < mOuterRect.height())) {
//                axis(QCPAxis::atBottom)->setVisible(true);
//            } else {
//                axis(QCPAxis::atBottom)->setVisible(false);
//            }
//        }

//        if(mLeftAxisVisible) {
//            QVector<QString> leftTickVectorLabels = axis(QCPAxis::atLeft)->tickVectorLabels();
//            QFont leftTickLabelsFont = axis(QCPAxis::atLeft)->tickLabelFont();
//            QFontMetrics leftFontMetrics(leftTickLabelsFont);

//            int leftTickLabelsWidth = 0;
//            int leftTickLabelsHeight = 0;
//            for(int i=0;i<leftTickVectorLabels.size();i++) {
//                leftTickLabelsHeight += leftFontMetrics.height();

//                int tickLabelWidth = leftFontMetrics.boundingRect(leftTickVectorLabels.at(i)).width();
//                if(tickLabelWidth > leftTickLabelsWidth)
//                    leftTickLabelsWidth = tickLabelWidth;
//            }

//            leftTickLabelsHeight += (leftTickVectorLabels.size() - 1);

//            if((3*leftTickLabelsWidth < mOuterRect.width()) &&
//                    (leftTickLabelsHeight < mInsetLayout->rect().height())) {
//                axis(QCPAxis::atLeft)->setVisible(true);
//            } else {
//                axis(QCPAxis::atLeft)->setVisible(false);
//            }
//        }

//        break;
//    }
//    default: break;
//    }

}
