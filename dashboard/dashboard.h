#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "3rdparty/qcustomplot.h"
#include "data/data_manager.h"

class QTBProject;
class QTBLayoutReactive;
class QTBLayoutReactiveElement;
class QTBoard : public QCustomPlot
{
    Q_OBJECT
public:
    QTBoard(QWidget *parent = nullptr);
    ~QTBoard() Q_DECL_OVERRIDE;

    QFont fontLight() const;
    QFont fontRegular() const;
    QFont fontBlack() const;
    QFont fontLightItalic() const;

    QTBLayoutReactive *dashboardLayout() const;
    QSharedPointer<QTBDataManager> dataManager() const;
    QSharedPointer<QTBProject> project() const;

    void droppedAlarm(QDropEvent *event);
    void droppedParameterSettings(QDropEvent *event);
    void droppedDataParameter(QDropEvent *event);
    void checkModification();

    void loadHistoricalData();
    void initDataManager();

    static QString elementMimeType() { return QStringLiteral("qtb/element"); }
    static QString dataParameterMimeType() { return QStringLiteral("qtb/dataparam"); }
    static QString parameterConfigMimeType() { return QStringLiteral("qtb/paramconfig"); }
    static QString alarmConfigMimeType() { return QStringLiteral("qtb/alarmconfig"); }

    bool liveDataRefreshEnabled() const;
    void setLiveDataRefreshEnabled(bool liveDataRefreshEnabled);

    double currentTimestamp();

    QColor backColor() const;
    QColor frontColor() const;
    QColor randomColor();

signals:
    void timeUpdate(QDateTime);

public slots:
    void clearPage();
    void loadPage();
    void savePage();
    void checkParameters();
    void update(QDateTime time);

protected:
    QFont mFontLight;
    QFont mFontLightItalic;
    QFont mFontRegular;
    QFont mFontBlack;
    QColor mBoardColor;
    QColor mBackColor;
    QColor mFrontColor;
    int mColorIndex;

    QTBLayoutReactive *mDashboardLayout;
    QSharedPointer<QTBDataManager> mDataManager;
    QSharedPointer<QTBProject> mProject;

    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;
    void dragMoveEvent(QDragMoveEvent *event) Q_DECL_OVERRIDE;
    void dragLeaveEvent(QDragLeaveEvent *event) Q_DECL_OVERRIDE;

    void initStyle();

    QDateTime mReferenceTime;

    bool mLoadingPage{false};
    bool mPageModified{false};
    bool mLiveDataRefreshEnabled{true};
};

#endif // DASHBOARD_H
