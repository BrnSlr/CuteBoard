#ifndef PLOTXY_H
#define PLOTXY_H

#include "dashboard/layouts/layout_grid.h"
#include "dashboard/dashboard_element.h"
#include "dashboard/elements_base/axisrect.h"
#include "dashboard/elements_factory/elementfactory.h"

#define CURVEPLOT_NAME "Plot - Curves Y=f(X)"

class QTBPlotXY : public QTBDashboardElement
{
    Q_OBJECT
public:
    enum AxisScale {
        asAuto,
        asParam,
        asCustom
    };

    enum YLegendPosition {
        lpRight,
        lpLeft
    };
    enum XLegendPosition {
        lpTop,
        lpBottom
    };
    QTBPlotXY(QTBoard *dashboard = nullptr);

    // reimplemented virtual methods:
    int defaultWidth() Q_DECL_OVERRIDE;
    int defaultHeight() Q_DECL_OVERRIDE;
    void initializeElement(QTBoard *dashboard) Q_DECL_OVERRIDE;
    void clearElement() Q_DECL_OVERRIDE;
    void edit() Q_DECL_OVERRIDE;
    QString name() Q_DECL_OVERRIDE { return QString(CURVEPLOT_NAME);}

    virtual void loadSettings(QSettings *settings)Q_DECL_OVERRIDE;
    virtual void saveSettings(QSettings *settings)Q_DECL_OVERRIDE;
    void saveParametersSettings(QSettings *settings, QTBParameterConfiguration::ConfigurationModule mode = QTBParameterConfiguration::cmFull) Q_DECL_OVERRIDE;
    void loadParametersSettings(QSettings *settings, QTBParameterConfiguration::ConfigurationModule mode = QTBParameterConfiguration::cmFull) Q_DECL_OVERRIDE;

    void checkParameters() Q_DECL_OVERRIDE;

    void droppedParameter(QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings) Q_DECL_OVERRIDE;
    void droppedParameter(QSharedPointer<QTBParameter> dataParameter) Q_DECL_OVERRIDE;
    QSharedPointer<QTBDashboardParameter> addYParameter(QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings);
    QSharedPointer<QTBDashboardParameter> addYParameter(QString paramLabel);
    QSharedPointer<QTBDashboardParameter> addYParameter(QSharedPointer<QTBParameter> dataParameter);
    QSharedPointer<QTBDashboardParameter> addXParameter(QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings);
    QSharedPointer<QTBDashboardParameter> addXParameter(QString paramLabel);
    QSharedPointer<QTBDashboardParameter> addXParameter(const QSharedPointer<QTBParameter>& dataParameter);
    void removeYParameter(int index);
    void removeXParameter();

    void update(UpdatePhase phase) Q_DECL_OVERRIDE;
    void updateDashboardParameters(QTBDashboardParameter::UpdateMode mode = QTBDashboardParameter::umValue) Q_DECL_OVERRIDE;
    void updateElement() Q_DECL_OVERRIDE;
    void processNewSamples() Q_DECL_OVERRIDE;

    void updateLegendSize();
    void updateAxes();
    void updateItems();
    void updateGraphsStyle();
    void updateLayout();

    QSharedPointer<QTBDashboardParameter> xParameter() const;

    YLegendPosition yLegendPosition() const;
    XLegendPosition xLegendPosition() const;
    AxisScale yAxisScale() const;
    AxisScale xAxisScale() const;
    bool yLegendVisible() const;
    bool xLegendVisible() const;
    bool yAxisTicksVisible() const;
    bool yAxisLabelsVisible() const;
    bool yAxisGridVisible() const;
    bool xAxisTicksVisible() const;
    bool xAxisLabelsVisible() const;
    bool xAxisGridVisible() const;
    int xAxisHistory() const;
    double yAxisMaxCustom() const;
    double yAxisMinCustom() const;
    double xAxisMaxCustom() const;
    double xAxisMinCustom() const;
    bool yThresholdsVisible() const;
    bool xThresholdsVisible() const;

    void setYLegendPosition(const YLegendPosition &yLegendPosition);
    void setXLegendPosition(const XLegendPosition &xLegendPosition);
    void setYAxisScale(const AxisScale &yAxisScale);
    void setXAxisScale(const AxisScale &xAxisScale);
    void setYLegendVisible(bool yLegendVisible);
    void setXLegendVisible(bool xLegendVisible);
    void setYAxisTicksVisible(bool yAxisTicksVisible);
    void setYAxisLabelsVisible(bool yAxisLabelsVisible);
    void setYAxisGridVisible(bool yAxisGridVisible);
    void setXAxisTicksVisible(bool xAxisTicksVisible);
    void setXAxisLabelsVisible(bool xAxisLabelsVisible);
    void setXAxisGridVisible(bool xAxisGridVisible);
    void setXAxisHistory(int xAxisHistory);
    void setYAxisMaxCustom(double yAxisMaxCustom);
    void setYAxisMinCustom(double yAxisMinCustom);
    void setXAxisMaxCustom(double xAxisMaxCustom);
    void setXAxisMinCustom(double xAxisMinCustom);
    void setYThresholdsVisible(bool yThresholdsVisible);
    void setXThresholdsVisible(bool xThresholdsVisible);

protected:
    QCPLayoutGrid *mLayout;
    QTBLayoutGrid *mYLegendLayout;
    QTBLayoutGrid *mXLegendLayout;
    QTBAxisRect *mAxisRect;

    YLegendPosition mYLegendPosition;
    AxisScale mYAxisScale;
    bool mYLegendVisible;
    double mYAxisMaxCustom;
    double mYAxisMinCustom;
    QCPLayoutElement *mYLegendEmptyElementBottom;
    QCPLayoutElement *mYLegendEmptyElementTop;

    XLegendPosition mXLegendPosition;
    AxisScale mXAxisScale;
    bool mXLegendVisible;
    double mXAxisMaxCustom;
    double mXAxisMinCustom;
    QCPLayoutElement *mXLegendEmptyElementRight;
    QCPLayoutElement *mXLegendEmptyElementLeft;

    bool mYAxisTicksVisible;
    bool mYAxisLabelsVisible;
    bool mYAxisGridVisible;
    bool mXAxisTicksVisible;
    bool mXAxisLabelsVisible;
    bool mXAxisGridVisible;
    int mXAxisHistory;

    bool mYThresholdsVisible;
    bool mXThresholdsVisible;

    QList<QCPCurve*> mCurves;
    QList<QCPItemStraightLine*> mHLines;
    QList<QCPItemStraightLine*> mVLines;

    QSharedPointer<QTBDashboardParameter> mXParameter;
};

static ElementRegister<QTBPlotXY> curveRegister(QString(CURVEPLOT_NAME), ":/elements/icons8_curve_50px.png");

#endif // PLOTXY_H
