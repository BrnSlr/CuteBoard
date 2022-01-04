#ifndef HORIZONTALGAUGE_H
#define HORIZONTALGAUGE_H

#include "../GaugeVertical/gauge_vertical.h"
#include "gauge_horizontal_shared.h"

class GAUGE_HORIZONTAL_EXPORT GaugeHorizontal: public GaugeVertical
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    enum GaugeHPosition {
        gpRight,
        gpLeft
    };

    GaugeHorizontal(Board *dashboard = nullptr);

    // reimplemented virtual methods:
    int defaultWidth()  Q_DECL_OVERRIDE;
    int defaultHeight() Q_DECL_OVERRIDE;
    void initialize(Board *dashboard) Q_DECL_OVERRIDE;
    void edit() Q_DECL_OVERRIDE;

    virtual void loadSettings(QSettings *settings)Q_DECL_OVERRIDE;
    virtual void saveSettings(QSettings *settings)Q_DECL_OVERRIDE;

    virtual void buildElement() Q_DECL_OVERRIDE;
    virtual void resetElement() Q_DECL_OVERRIDE;

    void displayData() Q_DECL_OVERRIDE;
    virtual void applySizeConstraints() Q_DECL_OVERRIDE;

    virtual void setAxisTicksVisible(bool axisTicksVisible) Q_DECL_OVERRIDE;
    virtual void setAxisLabelsVisible(bool axisLabelsVisible) Q_DECL_OVERRIDE;
    virtual void setAxisGridVisible(bool axisGridVisible) Q_DECL_OVERRIDE;
    virtual void setAxisLineVisible(bool axisLineVisible) Q_DECL_OVERRIDE;

    GaugeHPosition gaugeHPosition() const;
    void setGaugeHPosition(const GaugeHPosition &gaugePosition);

protected:
    GaugeHPosition mGaugeHPosition;
};

#endif // HORIZONTALGAUGE_H
