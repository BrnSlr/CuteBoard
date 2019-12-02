#ifndef QDBCIRCULARGAUGE_H
#define QDBCIRCULARGAUGE_H

#include "value_display.h"
#include "dashboard/elements_base/gaugerect.h"
#include "dashboard/elements_base/circularaxis.h"

#define CGAUGE_NAME "Value - Gauge Radial"

class QTBValueGaugeRadial: public QTBValueDisplay
{
    Q_OBJECT
public:
    enum AxisScale {
        asAuto,
        asParam,
        asCustom
    };
    QTBValueGaugeRadial(QTBoard *dashboard = nullptr);

    // reimplemented virtual methods:
    virtual int defaultWidth() Q_DECL_OVERRIDE;
    virtual int defaultHeight() Q_DECL_OVERRIDE;
    void initializeElement(QTBoard *dashboard) Q_DECL_OVERRIDE;
    void clearElement() Q_DECL_OVERRIDE;
    void edit() Q_DECL_OVERRIDE;
    QString name() Q_DECL_OVERRIDE { return QString(CGAUGE_NAME);}

    virtual void loadSettings(QSettings *settings)Q_DECL_OVERRIDE;
    virtual void saveSettings(QSettings *settings)Q_DECL_OVERRIDE;

    virtual void updateElement() Q_DECL_OVERRIDE;
    virtual void buildLayout() Q_DECL_OVERRIDE;
    virtual void processNewSamples() Q_DECL_OVERRIDE;
    virtual void updateSizeConstraints() Q_DECL_OVERRIDE;
    virtual void checkSizeAndVisibility() Q_DECL_OVERRIDE;

    AxisScale axisScale() const;
    double axisMaxCustom() const;
    double axisMinCustom() const;
    bool axisTicksVisible() const;
    bool axisLabelsVisible() const;
    bool thresholdsVisible() const;
    QTBGaugeRect::GaugeStyle radialStyle() const;

    void setAxisScale(const AxisScale &axisScale);
    void setAxisMaxCustom(double axisMaxCustom);
    void setAxisMinCustom(double axisMinCustom);
    void setAxisTicksVisible(bool axisTicksVisible);
    void setAxisLabelsVisible(bool axisLabelsVisible);
    void setThresholdsVisible(bool thresholdsVisible);
    void setRadialStyle(const QTBGaugeRect::GaugeStyle &radialStyle);

protected:
    QTBGaugeRect *mGauge;
    AxisScale mAxisScale;
    double mAxisMaxCustom;
    double mAxisMinCustom;

};

static ElementRegister<QTBValueGaugeRadial> cgaugeRegister(QString(CGAUGE_NAME), QTBDashboardElement::etSingleParam,":/elements/icons8_speed_50px.png");

#endif // QDBCIRCULARGAUGE_H
