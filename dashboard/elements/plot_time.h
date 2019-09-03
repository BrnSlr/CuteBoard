#ifndef PLOTTIME_H
#define PLOTTIME_H

#include "dashboard/layouts/layout_grid.h"
#include "dashboard/dashboard_element.h"
#include "dashboard/elements_base/axisrect.h"
#include "dashboard/elements_factory/elementfactory.h"

#define GRAPHPLOT_NAME "Plot - Graphs Y=f(t)"

class QTBPlotTime : public QTBDashboardElement
{
    Q_OBJECT
public:
    enum YAxisScale {
        asAuto,
        asParam,
        asCustom
    };

    enum XAxisDirection {
        adRightToLeft,
        adLeftToRight
    };

    enum LegendPosition {
        lpTop,
        lpRight,
        lpBottom,
        lpLeft
    };

    QTBPlotTime(QTBoard *dashboard = nullptr);

    // reimplemented virtual methods:
    int defaultWidth() Q_DECL_OVERRIDE;
    int defaultHeight() Q_DECL_OVERRIDE;
    void initializeElement(QTBoard *dashboard) Q_DECL_OVERRIDE;
    void clearElement() Q_DECL_OVERRIDE;
    void edit() Q_DECL_OVERRIDE;
    QString name() Q_DECL_OVERRIDE { return QString(GRAPHPLOT_NAME);}

    virtual void loadSettings(QSettings *settings)Q_DECL_OVERRIDE;
    virtual void saveSettings(QSettings *settings)Q_DECL_OVERRIDE;
    void saveParametersSettings(QSettings *settings, QTBParameterConfiguration::ConfigurationModule mode = QTBParameterConfiguration::cmFull) Q_DECL_OVERRIDE;
    void loadParametersSettings(QSettings *settings, QTBParameterConfiguration::ConfigurationModule mode = QTBParameterConfiguration::cmFull) Q_DECL_OVERRIDE;

    void addDashboardParameter(QSharedPointer<QTBDashboardParameter> dashParameter) Q_DECL_OVERRIDE;
    void removeDashParameter(int index) Q_DECL_OVERRIDE;

    void update(UpdatePhase phase) Q_DECL_OVERRIDE;
    void updateElement() Q_DECL_OVERRIDE;
    void processNewSamples() Q_DECL_OVERRIDE;
    void processHistoricalSamples() Q_DECL_OVERRIDE;

    void updateLegendSize();
    void updateAxes();
    void updateThresholdsItems();
    void updateGraphsStyle();
    void updateLayout();

    LegendPosition legendPosition() const;
    bool legendVisible() const;
    bool yAxisTicksVisible() const;
    bool yAxisLabelsVisible() const;
    bool yAxisGridVisible() const;
    YAxisScale yAxisScale() const;
    double yAxisMaxCustom() const;
    double yAxisMinCustom() const;
    bool xAxisTicksVisible() const;
    bool xAxisLabelsVisible() const;
    bool xAxisGridVisible() const;
    int xAxisHistory() const;
    XAxisDirection xAxisDirection() const;
    bool thresholdsVisible() const;

    void setLegendPosition(const LegendPosition &legendPosition);
    void setLegendVisible(bool legendVisible);
    void setYAxisTicksVisible(bool yAxisTicksVisible);
    void setYAxisLabelsVisible(bool yAxisLabelsVisible);
    void setYAxisGridVisible(bool yAxisGridVisible);
    void setYAxisScale(const YAxisScale &yAxisScale);
    void setYAxisMaxCustom(double yAxisMaxCustom);
    void setYAxisMinCustom(double yAxisMinCustom);
    void setXAxisTicksVisible(bool xAxisTicksVisible);
    void setXAxisLabelsVisible(bool xAxisLabelsVisible);
    void setXAxisGridVisible(bool xAxisGridVisible);
    void setXAxisHistory(int xAxisHistory);
    void setXAxisDirection(const XAxisDirection &xAxisDirection);
    void setThresholdsVisible(bool thresholdsVisible);

protected:
    QCPLayoutGrid *mLayout;
    QTBLayoutGrid *mLegendLayout{};
    QCPLayoutElement *mLegendEmptyElementFirst{};
    QCPLayoutElement *mLegendEmptyElementLast{};

    QTBAxisRect *mAxisRect{};
    LegendPosition mLegendPosition;
    bool mLegendVisible;
    bool mYAxisTicksVisible;
    bool mYAxisLabelsVisible;
    bool mYAxisGridVisible;
    bool mXAxisTicksVisible;
    bool mXAxisLabelsVisible;
    bool mXAxisGridVisible;
    YAxisScale mYAxisScale;
    double mYAxisMaxCustom{};
    double mYAxisMinCustom{};
    int mXAxisHistory;
    XAxisDirection mXAxisDirection;
    bool mThresholdsVisible;
    QCPRange mAutoRange;
};

static ElementRegister<QTBPlotTime> graphRegister(QString(GRAPHPLOT_NAME), ":/elements/icons8_cosine_50px.png");


#endif // PLOTTIME_H
