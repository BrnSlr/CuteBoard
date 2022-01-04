#ifndef DASHBOARDTOOLBAR_H
#define DASHBOARDTOOLBAR_H

#include <QWidget>
#include <QAction>
#include <QComboBox>
#include "ui/board/management/project.h"
#include "ui/board/chrono/chrono.h"
#include "ui/widgets/utils/calendar_widget.h"

namespace Ui {
class BoardToolbar;
}

class ComboboxEventFilter : public QObject
{
    Q_OBJECT
public:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if(mWatchedCombobox && mWatchedCombobox == watched) {
            if(event->type() == QEvent::KeyPress) {
                QKeyEvent *ke = static_cast<QKeyEvent*>(event);
                if(!ke->text().isEmpty()) {
                    event->ignore();
                    return true;
                }
            }
        }
        return QObject::eventFilter(watched, event);
    }

    void setWatchedCombobox(QComboBox *watchedCombobox)
    {
        mWatchedCombobox = watchedCombobox;
        mWatchedCombobox->installEventFilter(this);
    }

private:
    QComboBox *mWatchedCombobox;
};

class BoardToolbar : public QWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

    public:
        enum DashboardMode {
                 dmLive,
                 dmReplay,
                 dmLiveDesign,
                 dmReplayDesign
};

    explicit BoardToolbar(QWidget *parent = nullptr);
    ~BoardToolbar() override;

    void loadPage(const QString& pageName);
    void loadFirstPage();

    int startTimeMsecs() const;
    int endTimeMsecs() const;
    QDate date() const;
    DashboardMode mode() const;

    void setMode(DashboardMode mode);
    void setLiveDateTime(const QDateTime& time);
    void setStartTimeMsecs(int startTimeMsecs);
    void setEndTimeMsecs(int endTimeMsecs);
    void setDate(const QDate &date);
    void setProject(const QSharedPointer<Project> &project);
    void setDataManager(const QSharedPointer<DataManager> &dataManager);

    int currentSliderValue() const;

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void changeToolbarSize(double factor);

public slots:
    void updateList();
    void pageSelected();
    void chrono();
    void startReplay();
    void stopReplay();
    void play();

signals:
    void pageRequested(const QString pageName);
    void fullScreen();
    void pagePaused(bool pause);
    void enableInteraction(bool enabled);
    void enableStat(bool enabled);
    void timeUpdate(int timeMsecs, QDate date);
    void printScreen();
    void reload();

private slots:
    void on_replaySlider_valueChanged(int value);
    void on_timeEdit_timeChanged(const QTime &time);
    void on_replaySlider_sliderMoved(int position);
    void on_dateEdit_userDateChanged(const QDate &date);

private:
    Ui::BoardToolbar *ui;
    QSharedPointer<Project> mProject;
    DashboardMode mMode;

    QMenu *mRightClickMenu;
    QAction *mSmallAction;
    QAction *mMediumAction;
    QAction *mLargeAction;
    QAction *mCurrentAction;
    QAction *mPlayAction;
    QAction *mSpeedAction;

    Chrono mChronometer;
    CalendarWidget *mCalendar;

    int mStartTimeMsecs;
    int mEndTimeMsecs;
    int mCurrentSliderValue;

    QDate mDate;
    QSharedPointer<QCPAxisTickerTime> mTimeTicker;
    QSharedPointer<DataManager> mDataManager;
    QIcon mPageIcon;

    QTimer* mReplayTimer;
    int mReplayInterval;

    ComboboxEventFilter mComboboxEventFilter;
};

#endif // LIVETOOLBARWIDGET_H
