#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "3rdparty/qcustomplot/qcustomplot.h"
#include "data/data_manager.h"
#include "ui/ui_global.h"
#include "utils/selection_rect.h"
#include <QRubberBand>
#include <QSharedPointer>

typedef std::tuple<int,int,int> ElementIdRect;

class Project;
class BoardLayout;
class BoardElement;
class BOARD_UI_EXPORT Board : public QCustomPlot
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    Board(QWidget *parent = nullptr);
    ~Board() Q_DECL_OVERRIDE;

    BoardLayout *dashboardLayout() const;
    QSharedPointer<DataManager> dataManager() const;
    QSharedPointer<Project> project() const;

    bool toggleFullscreen();

    BoardElement *element(QPointF pos);
    void addElement(BoardElement *el, QPointF pos);

    void checkModification();

    void loadHistoricalData();
    void initDataManager();

    static QString elementNameMimeType() { return QStringLiteral("board/element_name"); }
    static QString elementCopyMimeType() { return QStringLiteral("board/elements_copy"); }
    static QString parameterNameMimeType() { return QStringLiteral("board/parameter_name"); }
    static QString propertyNameMimeType() { return QStringLiteral("board/property_name"); }
    static QString alarmConfigMimeType() { return QStringLiteral("board/alarm_config"); }

    bool liveDataRefreshEnabled() const;
    void setLiveDataRefreshEnabled(bool liveDataRefreshEnabled);
    void setUserInteractions(bool userInteractions);
    void setMouseInteractions(bool mouseInteractions);
    void setStatMode(bool statMode);

    double currentTimestamp();
    bool statMode() const;

    QColor randomColor(int index = 0);

    void setReferencePlaybackTime(int referencePlaybackTime);
    void setReferencePlaybackDate(const QDate &referencePlaybackDate);
    void setLastPlaybackTime();

    bool pageModified() const;
    void setPageModified(bool pageModified);

    void setRefreshTimerPeriod(int refreshTimerPeriod);

    bool mouseInteractions() const;

    bool layoutLocked() const;
    void setLayoutLocked(bool layoutLocked);

signals:
    void timeUpdate(QDateTime);
    void mouseLeave(QEvent *e);
    void widgetClosed();
    void dataSourcesLoaded();

public slots:
    void reloadProject();
    void clearPage();
    void loadPage();
    void savePage();
    void checkParameters();
    void startLiveTimer();
    void stopLiveTimer();
    void replotLayer(QString layerName);

    void updateElements(bool forceUpdate = false);
    void updateFinished();
    void partialReplot();

    void updateLive();
    void updateLiveData();

    void updatePlayback(int msecSinceStartOfTheDay,
                        QDate date);
    void updatePlaybackData();
    void resetPlayback();

protected slots:
    void editElement();
    void clearElements();
    void deleteElements();
    void cutElements();
    void copyElements();
    void pasteElements();

protected:
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dragMoveEvent(QDragMoveEvent *event) Q_DECL_OVERRIDE;
    void dragLeaveEvent(QDragLeaveEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

    void buildPalette();
    int mColorIndex;

    QTimer *mLiveRefreshTimer;
    int mRefreshTimerPeriod;

    BoardLayout *mDashboardLayout;
    QSharedPointer<DataManager> mDataManager;
    QSharedPointer<Project> mProject;

    QDateTime mReferenceLiveTime;
    int mReferencePlaybackTime;
    QDate mReferencePlaybackDate;

    bool mLayoutLocked;
    bool mPageLoading{false};
    bool mBoardReplotting{false};
    bool mPageModified{false};
    bool mLiveDataRefreshEnabled{true};
    bool mUserInteractions{false};
    bool mStatMode{false};
    bool mMouseInteractions{false};

    bool mFirstReplot;

    QLabel *mEmptyBoardLabel;

    QWidget *mFullScreenParent;
    QWidget *mFullScreenEmbeddedParent;
    int mFullScreenParentLayoutIndex;
    int mFullScreenParentGridRow;
    int mFullScreenParentGridCol;

    QShortcut *mFullscreenLockedShortcut;    

    SelectionRect * mSaveRect;

    QPoint mMousePos;
    SelectionRect * mHoverRect;
    SelectionRect::HoverHandle mResizeOrientation;    

    QParallelAnimationGroup *mAnimationGroup;

    QRubberBand *mRubberBand;
    QPoint mRubberBandOrigin;

    QVector<QSharedPointer<SelectionRect>> mSelectedRects;
    QVector<int> mSelectedIndexes;

    QMenu *mContextMenu;
    QAction *mEditElementAction;
    QAction *mClearElementsAction;
    QAction *mDeleteElementsAction;
    QAction *mCutElementsAction;
    QAction *mCopyElementsAction;
    QAction *mPasteElementsAction;
};

#endif // DASHBOARD_H
