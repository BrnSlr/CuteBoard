#ifndef VERTICALGAUGE_H
#define VERTICALGAUGE_H

#include "../NumericalDisplay/numerical_display.h"
#include "3rdparty/qcustomplot/axis_rect.h"
#include "gauge_vertical_shared.h"

class GAUGE_VERTICAL_EXPORT GaugeVertical : public NumericalDisplay
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    enum AxisScale {
        asAuto,
        asParam,
        asCustom
    };

    enum GaugeVPosition {
        gpTop,
        gpMiddle,
        gpBottom
    };

    GaugeVertical(Board *dashboard= nullptr);

    // reimplemented virtual methods:
    virtual int defaultWidth() Q_DECL_OVERRIDE;
    virtual int defaultHeight() Q_DECL_OVERRIDE;
    virtual void initialize(Board *dashboard) Q_DECL_OVERRIDE;
    void clearElement() Q_DECL_OVERRIDE;
    virtual void edit() Q_DECL_OVERRIDE;

    virtual void loadSettings(QSettings *settings)Q_DECL_OVERRIDE;
    virtual void saveSettings(QSettings *settings)Q_DECL_OVERRIDE;

    virtual void buildElement() Q_DECL_OVERRIDE;
    virtual void checkParameters() Q_DECL_OVERRIDE;
    virtual void resetElement() Q_DECL_OVERRIDE;

    virtual void displayData() Q_DECL_OVERRIDE;
    void loadHistoricalData() Q_DECL_OVERRIDE;
    virtual void applySizeConstraints() Q_DECL_OVERRIDE;    

    void resetThresholds();

    bool tankGauge() const;
    AxisScale axisScale() const;
    double axisMaxCustom() const;
    double axisMinCustom() const;
    bool axisTicksVisible() const;
    bool axisLabelsVisible() const;
    bool axisGridVisible() const;
    bool axisLineVisible() const;

    void setTankGauge(bool tankGauge);
    void setAxisScale(const AxisScale &axisScale);
    void setAxisMaxCustom(double axisMaxCustom);
    void setAxisMinCustom(double axisMinCustom);
    virtual void setAxisTicksVisible(bool axisTicksVisible);
    virtual void setAxisLabelsVisible(bool axisLabelsVisible);
    virtual void setAxisGridVisible(bool axisGridVisible);
    virtual void setAxisLineVisible(bool axisLineVisible);

    GaugeVPosition gaugeVPosition() const;
    void setGaugeVPosition(const GaugeVPosition &gaugePosition);

protected:
    LayoutGrid *mBarLayout;
    QCPLayoutElement *mBarLayoutEmptyElementFirst;
    QCPLayoutElement *mBarLayoutEmptyElementSecond;
    AxisRect *mAxisRect;
    AxisRect *mAxisRectThresholds;
    QCPBars *mBar;
    QVector<QCPItemRect*> mThresholdTracers;
    QCPRange mValueAxisRange;
    bool mAxisTicksVisible;
    bool mAxisLabelsVisible;
    bool mAxisLineVisible;
    bool mAxisGridVisible;
    AxisScale mAxisScale;
    double mAxisMaxCustom;
    double mAxisMinCustom;
    bool mTankGauge;
    GaugeVPosition mGaugeVPosition;
};

#endif // VERTICALGAUGE_H
