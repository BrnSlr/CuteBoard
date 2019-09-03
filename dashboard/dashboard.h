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

    void droppedParameterSettings(QDropEvent *event);
    void droppedDataParameter(QDropEvent *event);
    void checkModification();

    void loadHistoricalData();
    void initDataManager();

    static QString elementMimeType() { return QStringLiteral("qtb/element"); }
    static QString dataParameterMimeType() { return QStringLiteral("qtb/dataparam"); }
    static QString parameterSettingsMimeType() { return QStringLiteral("qtb/paramsettings"); }

    bool liveDataRefreshEnabled() const;
    void setLiveDataRefreshEnabled(bool liveDataRefreshEnabled);

    double currentTimestamp();

    QColor backColor() const;
    QColor frontColor() const;

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

    QTBLayoutReactive *mDashboardLayout;
    QSharedPointer<QTBDataManager> mDataManager;
    QSharedPointer<QTBProject> mProject;

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;

    void initStyle();

    QDateTime mReferenceTime;

    bool mLoadingPage{false};
    bool mPageModified{false};
    bool mLiveDataRefreshEnabled{true};
};

#endif // DASHBOARD_H
