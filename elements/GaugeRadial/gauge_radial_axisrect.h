#ifndef QTBGAUGERECT_H
#define QTBGAUGERECT_H

#include "ui/board/board.h"
#include "gauge_radial_axis.h"

class GaugeRect : public QCPLayoutElement
{

public:
    GaugeRect(Board *dashboard);
    ~GaugeRect() Q_DECL_OVERRIDE;

    virtual void update(UpdatePhase phase) Q_DECL_OVERRIDE;

    void clearThresholdBands();
    void addLowThreshold(QColor color, double value);
    void addHighThreshold(QColor color, double value);

    bool thresholdsVisible() const;
    void setThresholdsVisible(bool thresholdsVisible);

    bool axisLabelsVisible() const;
    void setAxisLabelsVisible(bool axisLabelsVisible);

    bool axisTicksVisible() const;
    void setAxisTicksVisible(bool axisTicksVisible);

    void setCurrentColor(const QColor &currentColor);

    void setValue(double value);

    QCPRange& axisRange();

    CircularAxis *axis() const;

    bool axisLineVisible() const;
    void setAxisLineVisible(bool axisLineVisible);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &backgroundColor);

    bool enabled() const;
    void setEnabled(bool enabled);

    int angleSpan() const;
    void setAngleSpan(int angleSpan);

    int angleStart() const;
    void setAngleStart(int angleStart);

protected:
    Board *mBoard;
    CircularAxis *mAxis;
    QCPRange mAxisRange;
    QColor mCurrentColor;
    QColor mBackgroundColor;
    QColor mNeedleColor;
    double mValue;
    bool mAxisTicksVisible;
    bool mAxisLineVisible;
    bool mAxisLabelsVisible;
    bool mThresholdsVisible;
    bool mEnabled;

    int mAngleSpan;
    int mAngleStart;


    QList<QPair<QColor, double> > mLowThresholdsBandColors;
    QList<QPair<QColor, double> > mHighThresholdsBandColors;

    void drawColorBands(QCPPainter *painter);
    void drawNeedle(QCPPainter *painter);
    void drawCenter(QCPPainter *painter);
    void drawCenterBackground(QCPPainter *painter);
    void drawBackgroundBand(QCPPainter *painter);
    void drawBand(QCPPainter *painter);
    void draw(QCPPainter *painter) Q_DECL_OVERRIDE;
};

#endif // QTBGAUGENEEDLE_H
