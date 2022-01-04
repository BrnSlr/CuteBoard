#ifndef ALARM_PANEL_H
#define ALARM_PANEL_H

#include "ui/board/board_element.h"
#include "ui/board/utils/adaptive_text.h"
#include "ui/board/utils/layout_list.h"
#include "ui/board/management/alarms.h"
#include "ui/widgets/alarm_configuration_widget.h"
#include "ui/board/utils/layout_grid.h"
#include "alarm_panel_shared.h"


class ALARMPANEL_EXPORT AlarmPanel : public BoardElement
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    AlarmPanel(Board *dashboard = nullptr);

    // reimplemented virtual methods:
    int defaultWidth() Q_DECL_OVERRIDE;
    int defaultHeight() Q_DECL_OVERRIDE;
    void initialize(Board *dashboard) Q_DECL_OVERRIDE;
    void clearElement() Q_DECL_OVERRIDE;
    void resetElement() Q_DECL_OVERRIDE;
    void buildElement() Q_DECL_OVERRIDE;
    void edit() Q_DECL_OVERRIDE;

    void update(UpdatePhase phase) Q_DECL_OVERRIDE;
    void loadData() Q_DECL_OVERRIDE;
    void displayData() Q_DECL_OVERRIDE;
    void checkParameters() Q_DECL_OVERRIDE;

    void saveSettings(QSettings *settings) Q_DECL_OVERRIDE;
    void loadSettings(QSettings *settings) Q_DECL_OVERRIDE;

    void saveConfigurations(QSettings *settings) Q_DECL_OVERRIDE;
    void loadConfigurations(QSettings *settings) Q_DECL_OVERRIDE;

    QSharedPointer<BoardParameter> addParameter(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings) Q_DECL_OVERRIDE;
    QSharedPointer<BoardParameter> addParameter(QString paramLabel) Q_DECL_OVERRIDE;
    QSharedPointer<BoardParameter> addParameter(QSharedPointer<TimeSeries> dataParameter) Q_DECL_OVERRIDE;  
    virtual void removeBoardParameters() Q_DECL_OVERRIDE;

    void addAlarm(QExplicitlySharedDataPointer<Alarms> alarmConfig) Q_DECL_OVERRIDE;
    void updateAlarmConfiguration();

    bool connected() const;
    void disconnectAlarmConfig();
    void resetModifications() Q_DECL_OVERRIDE;
    void beforeDataUpdate(bool refresh) Q_DECL_OVERRIDE;

    bool autoRowHeight() const;
    int rowHeight() const;
    bool headerVisible() const;
    bool autoHeaderText() const;
    QString headerText() const;

    void setAutoRowHeight(bool autoRowHeight);
    void setRowHeight(int rowHeight);
    void setHeaderVisible(bool headerVisible);
    void setAutoHeaderText(bool autoHeaderText);
    void setHeaderText(const QString &headerText);

protected:
    void draw(QCPPainter *painter) Q_DECL_OVERRIDE;

    AdaptiveTextElement *mHeader;
    QList<AdaptiveTextElement *> mTextElements;
    LayoutGrid *mMainLayout{};
    LayoutList *mListLayout;

    QList<QSharedPointer<BoardParameter>> mDashParametersPrimary;
    QList<QSharedPointer<BoardParameter>> mDashParametersSecondary;

    bool mAlarmConfigurationIsShared;
    QExplicitlySharedDataPointer<Alarms> mSharedAlarmConfiguration;
    QExplicitlySharedDataPointer<Alarms> mExclusiveAlarmConfiguration;
    QExplicitlySharedDataPointer<Alarms> mAlarmConfiguration;

    bool mUpdatingConfig;
    bool mAutoRowHeight;
    int mRowHeight;
    bool mHeaderVisible;
    bool mAutoHeaderText;
    QString mHeaderText;

    QPen mListBorderPen;

};

#endif // ALARM_PANEL_H
