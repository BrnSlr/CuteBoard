#ifndef DASHBOARD_ELEMENT_H
#define DASHBOARD_ELEMENT_H

#include "board.h"
#include "board_layout.h"
#include "board_parameter.h"
#include "ui/board/management/alarms.h"

class BOARD_UI_EXPORT BoardElement : public QCPLayoutElement
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    enum ElementType {
        etUnknown = 0,
        etAlarm = 1,
        etTimeSeries = 2,
        etOther = 10
    };

    enum TimeSeriesType {
        tstUnknown = 1,
        tstScalar = 2,
        tstVector = 4,
        tstScalarAndVector = 8
    };

    enum TimeSeriesSize {
        tssUnknown = 1,
        tssSingle = 2,
        tssMulti = 4
    };

    friend inline TimeSeriesType operator| (TimeSeriesType a, TimeSeriesType b) {
        return static_cast<TimeSeriesType>( static_cast<int>(a) | static_cast<int>(b));
    }
    friend inline TimeSeriesType operator& (TimeSeriesType a, TimeSeriesType b) {
        return static_cast<TimeSeriesType>( static_cast<int>(a) & static_cast<int>(b));
    }

    friend inline TimeSeriesSize operator| (TimeSeriesSize a, TimeSeriesSize b) {
        return static_cast<TimeSeriesSize>( static_cast<int>(a) | static_cast<int>(b));
    }
    friend inline TimeSeriesSize operator& (TimeSeriesSize a, TimeSeriesSize b) {
        return static_cast<TimeSeriesSize>( static_cast<int>(a) & static_cast<int>(b));
    }

    enum BackgroundMode {
        bmBackground,
        bmTransparent
    };
    Q_ENUM(BackgroundMode)

    BoardElement(Board *dashboard = nullptr);
    ~BoardElement() Q_DECL_OVERRIDE;

    virtual void initialize(Board *dashboard);
    virtual void edit() {}

    void rebuildElement(bool replot = false);
    virtual void clearElement() {}
    virtual void resetElement() {}
    virtual void buildElement() {}

    virtual void beforeDataUpdate(bool refresh);
    virtual void resetModifications();

    virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=nullptr) const Q_DECL_OVERRIDE;

    virtual void droppedParameter(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings);
    virtual void droppedParameter(QSharedPointer<TimeSeries> dataParameter, bool replace);

    QSharedPointer<BoardParameter> boardParameter(int index);
    QSharedPointer<BoardParameter> boardParameter(QString label);
    virtual void removeBoardParameter(int index, bool update = true);
    virtual void removeBoardParameter(QString label, bool update = true);
    virtual void removeBoardParameters();
    virtual QSharedPointer<BoardParameter> replaceParameter(int index, QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings);
    virtual QSharedPointer<BoardParameter> replaceParameter(int index, QString paramLabel);
    virtual QSharedPointer<BoardParameter> replaceParameter(int index, QSharedPointer<TimeSeries> dataParameter);
    virtual void replaceParameter(int index, QSharedPointer<BoardParameter> boardParameter);
    virtual QSharedPointer<BoardParameter> addParameter(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings);
    virtual QSharedPointer<BoardParameter> addParameter(QString paramLabel);
    virtual QSharedPointer<BoardParameter> addParameter(QSharedPointer<TimeSeries> dataParameter);
    virtual void addParameter(QSharedPointer<BoardParameter> boardParameter);
    virtual void addAlarm(QExplicitlySharedDataPointer<Alarms> alarmConfig) {Q_UNUSED(alarmConfig)}

    virtual void loadData();
    virtual void loadHistoricalData();
    virtual void displayData();
    virtual void update(UpdatePhase phase) Q_DECL_OVERRIDE;
    virtual void checkParameters();

    int parametersCount() {return mBoardParameters.count();}

    virtual void loadSettings(QSettings *settings) {Q_UNUSED(settings)}
    virtual void saveSettings(QSettings *settings){Q_UNUSED(settings)}

    virtual void saveConfigurations(QSettings *settings);
    virtual void loadConfigurations(QSettings *settings);


    virtual void setBoardLiveState(bool isLive);

    void setMainElement(bool mainElement);

    virtual int defaultWidth() = 0;
    virtual int defaultHeight() = 0;

    QString name() const;
    ElementType type() const;
    TimeSeriesType timeSeriesType() const;
    TimeSeriesSize timeSeriesSize() const;
    BackgroundMode backgroundMode() const;
    int parametersMaxCount() const;
    bool statModeEnabled() const;
    bool mouseInteractionsEnabled() const;
    ParameterConfiguration::ConfigurationMode configurationMode() const;

    void setName(const QString &name);
    void setType(const ElementType &type);
    void setTimeSeriesType(const TimeSeriesType &timeSeriesType);
    void setTimeSeriesSize(const TimeSeriesSize &timeSeriesSize);
    void setBackgroundMode(const BackgroundMode &backgroundMode);
    void setParametersMaxCount(int parametersMaxCount);
    void setStatModeEnabled(bool statModeEnabled);
    void setMouseInteractionsEnabled(bool mouseInteractionsEnabled);
    void setConfigurationMode(const ParameterConfiguration::ConfigurationMode &configurationMode);

    Board *board() const;

protected:
    void draw(QCPPainter *painter) Q_DECL_OVERRIDE;

public slots:
    virtual void mouseMoveSlot(QMouseEvent *event) {Q_UNUSED(event)}

protected:
    BackgroundMode mBackgroundMode;
    QBrush mDefaultBrush;
    QBrush mHGradientBrush;
    QBrush mVGradientBrush;
    QPen mBorderPen;
    Board *mBoard;

    int mParametersMaxCount;
    QString mName;
    ElementType mType;
    TimeSeriesType mTimeSeriesType;
    TimeSeriesSize mTimeSeriesSize;
    bool mStatModeEnabled;
    bool mMouseInteractionsEnabled;
    bool mMainElement;

    QList<QSharedPointer<BoardParameter>> mBoardParameters;
    QList<QString> mParametersLabel;
    ParameterConfiguration::ConfigurationMode mConfigurationMode;

    int mParameterColorIndex;

};


#endif // DASHBOARD_ELEMENT_H
