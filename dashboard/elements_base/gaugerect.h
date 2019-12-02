#ifndef QTBGAUGERECT_H
#define QTBGAUGERECT_H

#include "dashboard/dashboard.h"
#include "dashboard/elements_base/circularaxis.h"

class QTBGaugeRect : public QCPLayoutElement
{

public:
    enum GaugeStyle {
        gsNeedle,
        gsBand
    };

    QTBGaugeRect(QTBoard *dashboard);
    ~QTBGaugeRect() Q_DECL_OVERRIDE;

    virtual void update(UpdatePhase phase) Q_DECL_OVERRIDE;

    void clearThresholdBands();
    void addLowThreshold(QColor color, double value);
    void addHighThreshold(QColor color, double value);

    GaugeStyle gaugeStyle() const;
    void setGaugeStyle(const GaugeStyle &gaugeStyle);

    bool thresholdsVisible() const;
    void setThresholdsVisible(bool thresholdsVisible);

    bool axisLabelsVisible() const;
    void setAxisLabelsVisible(bool axisLabelsVisible);

    bool axisTicksVisible() const;
    void setAxisTicksVisible(bool axisTicksVisible);

    void setCurrentColor(const QColor &currentColor);

    void setValue(double value);

    QCPRange& axisRange();

    QTBCircularAxis *axis() const;

protected:
    QTBCircularAxis *mAxis;
    QCPRange mAxisRange;
    QColor mCurrentColor;
    double mValue;
    GaugeStyle mGaugeStyle;
    bool mAxisTicksVisible;
    bool mAxisLabelsVisible;
    bool mThresholdsVisible;

    QList<QPair<QColor, double> > mLowThresholdsBandColors;
    QList<QPair<QColor, double> > mHighThresholdsBandColors;

    void drawColorBands(QCPPainter *painter);
    void drawNeedle(QCPPainter *painter);
    void drawBand(QCPPainter *painter);
    void draw(QCPPainter *painter) Q_DECL_OVERRIDE;
};

#endif // QTBGAUGENEEDLE_H
