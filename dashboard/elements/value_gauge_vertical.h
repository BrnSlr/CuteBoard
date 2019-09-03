#ifndef VALUEVERTICALGAUGE_H
#define VALUEVERTICALGAUGE_H

#include "value_display.h"
#include "dashboard/elements_base/axisrect.h"

#define VGAUGE_NAME "Value - Gauge Vertical"

class QTBValueGaugeVertical : public QTBValueDisplay
{
    Q_OBJECT
public:
    enum AxisScale {
        asAuto,
        asParam,
        asCustom
    };

    QTBValueGaugeVertical(QTBoard *dashboard= nullptr);

    // reimplemented virtual methods:
    virtual int defaultWidth() Q_DECL_OVERRIDE;
    virtual int defaultHeight() Q_DECL_OVERRIDE;
    virtual void initializeElement(QTBoard *dashboard) Q_DECL_OVERRIDE;
    void clearElement() Q_DECL_OVERRIDE;
    virtual void edit() Q_DECL_OVERRIDE;
    virtual QString name() Q_DECL_OVERRIDE { return QString(VGAUGE_NAME);}

    virtual void loadSettings(QSettings *settings)Q_DECL_OVERRIDE;
    virtual void saveSettings(QSettings *settings)Q_DECL_OVERRIDE;

    virtual void addDashboardParameter(QSharedPointer<QTBDashboardParameter> dashParameter) Q_DECL_OVERRIDE;

    virtual void buildLayout() Q_DECL_OVERRIDE;
    virtual void processNewSamples() Q_DECL_OVERRIDE;
    virtual void updateSizeConstraints() Q_DECL_OVERRIDE;
    virtual void checkSizeAndVisibility() Q_DECL_OVERRIDE;

    bool tankGauge() const;
    AxisScale axisScale() const;
    double axisMaxCustom() const;
    double axisMinCustom() const;
    bool axisTicksVisible() const;
    bool axisLabelsVisible() const;
    bool axisGridVisible() const;

    void setTankGauge(bool tankGauge);
    void setAxisScale(const AxisScale &axisScale);
    void setAxisMaxCustom(double axisMaxCustom);
    void setAxisMinCustom(double axisMinCustom);
    virtual void setAxisTicksVisible(bool axisTicksVisible);
    virtual void setAxisLabelsVisible(bool axisLabelsVisible);
    virtual void setAxisGridVisible(bool axisGridVisible);

protected:
    QTBAxisRect *mAxisRect;
    QCPBars *mBar;
    QCPRange mValueAxisRange;
    bool mAxisTicksVisible;
    bool mAxisLabelsVisible;
    bool mAxisGridVisible;
    AxisScale mAxisScale;
    double mAxisMaxCustom;
    double mAxisMinCustom;
    bool mTankGauge;
};

static ElementRegister<QTBValueGaugeVertical> vgaugeRegister(QString(VGAUGE_NAME), ":/elements/icons8_vgauge_50px.png");

#endif // VERTICALGAUGE_H
