#ifndef PLOTXY_H
#define PLOTXY_H

#include "ui/board/utils/layout_grid.h"
#include "ui/board/board_element.h"
#include "3rdparty/qcustomplot/axis_rect.h"
#include "ui/board/factory/element_factory_helper.h"
#include "ui/board/management/curve_patron.h"
#include "../NumericalDisplay/numerical_display.h"
#include "plot_xy_shared.h"

class PLOT_XY_EXPORT PlotXY : public BoardElement
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
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

    PlotXY(Board *dashboard = nullptr);

    // reimplemented virtual methods:
    int defaultWidth() Q_DECL_OVERRIDE;
    int defaultHeight() Q_DECL_OVERRIDE;
    void initialize(Board *dashboard) Q_DECL_OVERRIDE;
    void clearElement() Q_DECL_OVERRIDE;
    void edit() Q_DECL_OVERRIDE;

    virtual void loadSettings(QSettings *settings)Q_DECL_OVERRIDE;
    virtual void saveSettings(QSettings *settings)Q_DECL_OVERRIDE;
    void saveConfigurations(QSettings *settings) Q_DECL_OVERRIDE;
    void loadConfigurations(QSettings *settings) Q_DECL_OVERRIDE;

    void checkParameters() Q_DECL_OVERRIDE;

    void droppedParameter(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings) Q_DECL_OVERRIDE;
    void droppedParameter(QSharedPointer<TimeSeries> dataParameter, bool replace) Q_DECL_OVERRIDE;
    QSharedPointer<BoardParameter> addYParameter(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings);
    QSharedPointer<BoardParameter> addYParameter(QString paramLabel);
    QSharedPointer<BoardParameter> addYParameter(QSharedPointer<TimeSeries> dataParameter);
    QSharedPointer<BoardParameter> addXParameter(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings);
    QSharedPointer<BoardParameter> addXParameter(QString paramLabel);
    QSharedPointer<BoardParameter> addXParameter(const QSharedPointer<TimeSeries>& dataParameter);
    void removeYParameter(int index, bool update = true);
    void removeXParameter();
    virtual void removeBoardParameters() Q_DECL_OVERRIDE;

    void update(UpdatePhase phase) Q_DECL_OVERRIDE;
    void resetElement() Q_DECL_OVERRIDE;
    void buildElement() Q_DECL_OVERRIDE;
    void loadData() Q_DECL_OVERRIDE;
    void loadHistoricalData() Q_DECL_OVERRIDE;
    void displayData() Q_DECL_OVERRIDE;

    void applySizeConstraints();
    void updateAxes();
    void updateItems();
    void updateGraphsStyle();

    void addPatron(QSharedPointer<CurvePatron> patron);
    void removePatron(int index);

    QSharedPointer<BoardParameter> xParameter() const;

    YLegendPosition yLegendPosition() const;
    XLegendPosition xLegendPosition() const;
    AxisScale yAxisScale() const;
    AxisScale xAxisScale() const;
    bool yLegendVisible() const;
    bool xLegendVisible() const;
    bool yAxisTicksVisible() const;
    int yAxisTicksCount() const;
    bool yAxisLabelsVisible() const;
    bool yAxisGridVisible() const;
    bool xAxisTicksVisible() const;
    int xAxisTicksCount() const;
    bool xAxisLabelsVisible() const;
    bool xAxisGridVisible() const;
    int xAxisHistory() const;
    double yAxisMaxCustom() const;
    double yAxisMinCustom() const;
    double xAxisMaxCustom() const;
    double xAxisMinCustom() const;
    bool yThresholdsVisible() const;
    bool xThresholdsVisible() const;
    bool yAxisLineVisible() const;
    bool xAxisLineVisible() const;
    bool titleVisible() const;
    QString title() const;
    bool plotInRangePoints() const;

    void setYLegendPosition(const YLegendPosition &yLegendPosition);
    void setXLegendPosition(const XLegendPosition &xLegendPosition);
    void setYAxisScale(const AxisScale &yAxisScale);
    void setXAxisScale(const AxisScale &xAxisScale);
    void setYLegendVisible(bool yLegendVisible);
    void setXLegendVisible(bool xLegendVisible);
    void setYAxisTicksVisible(bool yAxisTicksVisible);
    void setYAxisTicksCount(int yAxisTicksCount);
    void setYAxisLineVisible(bool yAxisLineVisible);
    void setYAxisLabelsVisible(bool yAxisLabelsVisible);
    void setYAxisGridVisible(bool yAxisGridVisible);
    void setXAxisTicksVisible(bool xAxisTicksVisible);
    void setXAxisTicksCount(int xAxisTicksCount);
    void setXAxisLineVisible(bool xAxisLineVisible);
    void setXAxisLabelsVisible(bool xAxisLabelsVisible);
    void setXAxisGridVisible(bool xAxisGridVisible);
    void setXAxisHistory(int xAxisHistory);
    void setYAxisMaxCustom(double yAxisMaxCustom);
    void setYAxisMinCustom(double yAxisMinCustom);
    void setXAxisMaxCustom(double xAxisMaxCustom);
    void setXAxisMinCustom(double xAxisMinCustom);
    void setYThresholdsVisible(bool yThresholdsVisible);
    void setXThresholdsVisible(bool xThresholdsVisible);
    void setTitleVisible(bool titleVisible);
    void setTitle(const QString &title);    
    void setPlotInRangePoints(bool plotInRangePoints);

    QList<QSharedPointer<CurvePatron> > patrons() const;

    QList<QSharedPointer<BoardParameter> > yParameters() const;

protected:

    LayoutGrid *mMainLayout;
    LayoutGrid *mLayout;
    LayoutGrid *mYLegendLayout;
    LayoutGrid *mXLegendLayout;
    AxisRect *mAxisRect;
    AdaptiveTextElement *mTitleLabel;

    bool mTitleVisible;
    QString mTitle;

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
    int mYAxisTicksCount;
    bool mYAxisLineVisible;
    bool mYAxisLabelsVisible;
    bool mYAxisGridVisible;
    bool mXAxisTicksVisible;
    int mXAxisTicksCount;
    bool mXAxisLineVisible;
    bool mXAxisLabelsVisible;
    bool mXAxisGridVisible;
    int mXAxisHistory;

    bool mYThresholdsVisible;
    bool mXThresholdsVisible;

    bool mPlotInRangePoints;

    QList<QCPCurve*> mCurves;
    QList<QCPItemTracer*> mTracers;
    QList<QCPItemStraightLine*> mHLines;
    QList<QCPItemStraightLine*> mVLines;

    QList<QSharedPointer<CurvePatron>> mPatrons;

    QList<QSharedPointer<BoardParameter>> mYParameters;
    QList<NumericalDisplay *> mYDisplays;
    QSharedPointer<BoardParameter> mXParameter;
    NumericalDisplay *mXDisplay;
};

#endif // PLOTXY_H
