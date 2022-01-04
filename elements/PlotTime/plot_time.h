#ifndef PLOTTIME_H
#define PLOTTIME_H

#include "ui/board/utils/layout_grid.h"
#include "ui/board/board_element.h"
#include "3rdparty/qcustomplot/graph_tracer.h"
#include "3rdparty/qcustomplot/graph.h"
#include "3rdparty/qcustomplot/axis_rect.h"
#include "plot_time_shared.h"
#include "ui/board/factory/element_factory_helper.h"
#include "ui/board/utils/adaptive_text.h"

class PLOT_TIME_EXPORT PlotTime : public BoardElement
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    enum YAxisScale {
        asAuto,
        asAutoZero,
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

    enum StatMode {
        smNone,
        smMin,
        smMax,
        smMinMax
    };

    PlotTime(Board *dashboard = nullptr);

    // reimplemented virtual methods:
    int defaultWidth() Q_DECL_OVERRIDE;
    int defaultHeight() Q_DECL_OVERRIDE;
    void initialize(Board *dashboard) Q_DECL_OVERRIDE;
    void clearElement() Q_DECL_OVERRIDE;
    void edit() Q_DECL_OVERRIDE;
    virtual QList<QCPLayoutElement*> elements(bool recursive) const Q_DECL_OVERRIDE;

    virtual void loadSettings(QSettings *settings)Q_DECL_OVERRIDE;
    virtual void saveSettings(QSettings *settings)Q_DECL_OVERRIDE;

    void loadConfigurations(QSettings *settings) Q_DECL_OVERRIDE;

    QSharedPointer<BoardParameter> addParameter(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings) Q_DECL_OVERRIDE;
    QSharedPointer<BoardParameter> addParameter(QString paramLabel) Q_DECL_OVERRIDE;
    void addParameter(QSharedPointer<BoardParameter> boardParameter) Q_DECL_OVERRIDE;

    virtual QSharedPointer<BoardParameter> replaceParameter(int index, QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings);
    virtual QSharedPointer<BoardParameter> replaceParameter(int index, QString paramLabel);
    void replaceParameter(int index, QSharedPointer<BoardParameter> boardParameter) Q_DECL_OVERRIDE;
    void removeBoardParameter(int index, bool update = true) Q_DECL_OVERRIDE;
    virtual void checkParameters() Q_DECL_OVERRIDE;

    void update(UpdatePhase phase) Q_DECL_OVERRIDE;
    void resetElement() Q_DECL_OVERRIDE;
    void buildElement() Q_DECL_OVERRIDE;
    void loadData() Q_DECL_OVERRIDE;
    void displayData() Q_DECL_OVERRIDE;
    void loadHistoricalData() Q_DECL_OVERRIDE;

    void applySizeConstraints();
    void updateAxes();
    void resetItems();
    void updateItems();
    void updateGraphsStyle();
    void resetGraphsStyle();

    LegendPosition legendPosition() const;
    bool legendVisible() const;
    bool yAxisTicksVisible() const;
    int yAxisTicksCount() const;
    bool yAxisLineVisible() const;
    bool yAxisLabelsVisible() const;
    bool yAxisGridVisible() const;
    YAxisScale yAxisScale() const;
    double yAxisMaxCustom() const;
    double yAxisMinCustom() const;
    bool xAxisTicksVisible() const;
    int xAxisTicksCount() const;
    bool xAxisLineVisible() const;
    bool xAxisLabelsVisible() const;
    bool xAxisGridVisible() const;
    double xAxisHistory() const;
    XAxisDirection xAxisDirection() const;
    bool xAxisFixedTicker() const;
    bool thresholdsVisible() const;    
    bool boardReferenceTimeActive() const;
    StatMode statMode() const;
    double tickStepMinValue() const;
    bool tickStepMinValueEnabled() const;
    bool titleVisible() const;
    QString title() const;

    void setLegendPosition(const LegendPosition &legendPosition);
    void setLegendVisible(bool legendVisible);
    void setYAxisTicksVisible(bool yAxisTicksVisible);
    void setYAxisTicksCount(int yAxisTicksCount);
    void setYAxisLineVisible(bool yAxisLineVisible);
    void setYAxisLabelsVisible(bool yAxisLabelsVisible);
    void setYAxisGridVisible(bool yAxisGridVisible);
    void setYAxisScale(const YAxisScale &yAxisScale);
    void setYAxisMaxCustom(double yAxisMaxCustom);
    void setYAxisMinCustom(double yAxisMinCustom);
    void setXAxisTicksVisible(bool xAxisTicksVisible);
    void setXAxisTicksCount(int xAxisTicksCount);
    void setXAxisLineVisible(bool xAxisLineVisible);
    void setXAxisLabelsVisible(bool xAxisLabelsVisible);
    void setXAxisGridVisible(bool xAxisGridVisible);
    void setXAxisHistory(double xAxisHistory);
    void setXAxisDirection(const XAxisDirection &xAxisDirection);
    void setXAxisFixedTicker(bool xAxisFixedTicker);
    void setThresholdsVisible(bool thresholdsVisible);
    void setBoardReferenceTimeActive(bool boardReferenceTimeActive);
    void setStatMode(const StatMode &statMode);
    void setTickStepMinValue(double tickStepMinValue);
    void setTickStepMinValueEnabled(bool tickStepMinValueEnabled);
    void setTitleVisible(bool titleVisible);
    void setTitle(const QString &title);

public slots:
    void mouseMoveSlot(QMouseEvent *event) Q_DECL_OVERRIDE;

protected:

    LayoutGrid *mMainLayout;
    LayoutGrid *mLayout;
    LayoutGrid *mLegendLayout;
    QCPLayoutElement *mLegendEmptyElementFirst{};
    QCPLayoutElement *mLegendEmptyElementLast{};
    AdaptiveTextElement *mTitleLabel;

    AxisRect *mAxisRect{};

    QCPAxis *mYRightAxis;
    QCPAxis *mXBottomMovingAxis;
    QCPAxis *mXBottomStaticAxis;
    LegendPosition mLegendPosition;
    bool mLegendVisible;
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
    bool mXAxisFixedTicker;
    bool mBoardReferenceTimeActive;
    YAxisScale mYAxisScale;
    double mYAxisMaxCustom{};
    double mYAxisMinCustom{};
    double mXAxisHistory;
    XAxisDirection mXAxisDirection;
    bool mThresholdsVisible;
    QCPRange mAutoRange;
    double mTickStepMinValue;
    bool mTickStepMinValueEnabled;
    bool mTitleVisible;
    QString mTitle;

    PlotTimeTracer *mTracer;

    StatMode mStatMode;
    QVector<QCPItemTracer*> mStatMaxTracers;
    QVector<QCPItemText*> mStatMaxTexts;
    QVector<QCPItemLine*> mStatMaxArrows;
    QVector<QCPItemTracer*> mStatMinTracers;
    QVector<QCPItemText*> mStatMinTexts;
    QVector<QCPItemLine*> mStatMinArrows;

    QSharedPointer<QCPAxisTickerFixed> mValueTicker;
    QSharedPointer<QCPAxisTickerDateTime> mTimeTicker;
};

#endif // PLOTTIME_H
