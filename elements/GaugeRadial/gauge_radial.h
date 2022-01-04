#ifndef QDBCIRCULARGAUGE_H
#define QDBCIRCULARGAUGE_H

#include "../NumericalDisplay/numerical_display.h"
#include "gauge_radial_axisrect.h"
#include "gauge_radial_axis.h"
#include "gauge_radial_shared.h"

class GAUGE_RADIAL_EXPORT GaugeRadial: public NumericalDisplay
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    enum AxisScale {
        asAuto,
        asParam,
        asCustom
    };

    enum GaugeRPosition {
        gpTop,
        gpMiddle,
        gpBottom
    };

    GaugeRadial(Board *dashboard = nullptr);

    // reimplemented virtual methods:
    virtual int defaultWidth() Q_DECL_OVERRIDE;
    virtual int defaultHeight() Q_DECL_OVERRIDE;
    void initialize(Board *dashboard) Q_DECL_OVERRIDE;
    void clearElement() Q_DECL_OVERRIDE;
    void edit() Q_DECL_OVERRIDE;

    virtual void loadSettings(QSettings *settings)Q_DECL_OVERRIDE;
    virtual void saveSettings(QSettings *settings)Q_DECL_OVERRIDE;

    virtual void buildElement() Q_DECL_OVERRIDE;
    virtual void checkParameters() Q_DECL_OVERRIDE;

    virtual void displayData() Q_DECL_OVERRIDE;
    void loadHistoricalData() Q_DECL_OVERRIDE;

    AxisScale axisScale() const;
    double axisMaxCustom() const;
    double axisMinCustom() const;
    bool axisTicksVisible() const;
    bool axisLineVisible() const;
    bool axisLabelsVisible() const;
    bool thresholdsVisible() const;
    GaugeRPosition gaugePosition() const;
    int angleSpan() const;
    int angleStart() const;

    void setAxisScale(const AxisScale &axisScale);
    void setAxisMaxCustom(double axisMaxCustom);
    void setAxisMinCustom(double axisMinCustom);
    void setAxisTicksVisible(bool axisTicksVisible);
    void setAxisLineVisible(bool axisLineVisible);
    void setAxisLabelsVisible(bool axisLabelsVisible);
    void setThresholdsVisible(bool thresholdsVisible);
    void setGaugePosition(const GaugeRPosition &gaugePosition);
    void setAngleSpan(int angleSpan);
    void setAngleStart(int angleStart);

protected:
    GaugeRect *mGauge;
    AxisScale mAxisScale;
    double mAxisMaxCustom;
    double mAxisMinCustom;
    GaugeRPosition mGaugePosition;
    int mAngleSpan;
    int mAngleStart;

};

#endif // QDBCIRCULARGAUGE_H
