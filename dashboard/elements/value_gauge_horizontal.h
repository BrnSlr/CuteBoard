#ifndef VALUEHORIZONTALGAUGE_H
#define VALUEHORIZONTALGAUGE_H

#include "value_gauge_vertical.h"

#define HGAUGE_NAME "Value - Gauge Horizontal"

class QTBValueGaugeHorizontal: public QTBValueGaugeVertical
{
    Q_OBJECT
public:
    QTBValueGaugeHorizontal(QTBoard *dashboard = nullptr);

    // reimplemented virtual methods:
    int defaultWidth() Q_DECL_OVERRIDE;
    int defaultHeight() Q_DECL_OVERRIDE;
    void initializeElement(QTBoard *dashboard) Q_DECL_OVERRIDE;
    virtual void buildLayout() Q_DECL_OVERRIDE;
    void edit() Q_DECL_OVERRIDE;
    QString name() Q_DECL_OVERRIDE { return QString(HGAUGE_NAME);}

    void processNewSamples() Q_DECL_OVERRIDE;
    virtual void updateSizeConstraints() Q_DECL_OVERRIDE;
    virtual void checkSizeAndVisibility() Q_DECL_OVERRIDE;
    virtual void updateElement() Q_DECL_OVERRIDE;

    virtual void setAxisTicksVisible(bool axisTicksVisible) Q_DECL_OVERRIDE;
    virtual void setAxisLabelsVisible(bool axisLabelsVisible) Q_DECL_OVERRIDE;
    virtual void setAxisGridVisible(bool axisGridVisible) Q_DECL_OVERRIDE;
};

static ElementRegister<QTBValueGaugeHorizontal> hgaugeRegister(QString(HGAUGE_NAME),QTBDashboardElement::etSingleParam, ":/elements/icons8_hgauge_50px.png");

#endif // VALUEHORIZONTALGAUGE_H
