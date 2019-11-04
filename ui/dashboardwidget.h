#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include <QWidget>
#include "dashboard/dashboard.h"
#include "dashboard/elements/plot_time.h"
#include "dashboard/elements/value_display.h"
#include "dashboard/elements/value_gauge_vertical.h"
#include "dashboard/elements/value_gauge_horizontal.h"
#include "dashboard/elements/plot_xy.h"
#include "dashboard/elements/value_bitfields.h"
#include "dashboard/layouts/layout_reactive.h"
#include "dashboard/elements/value_gauge_radial.h"
#include "project/project.h"
#include "data/data_source_interface.h"
#include "dashboard_toolbar.h"

namespace Ui {
class DashboardWidget;
}

class DashboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardWidget(QWidget *parent = nullptr);
    ~DashboardWidget() Q_DECL_OVERRIDE;

    QSharedPointer<QTBProject> project() const;
    QTBoard *board() const;
    void setMode(DashboardToolbar::DashboardMode mode);

signals:
    void fullScreenMode(bool fullscreen);

public slots:
    void savePage();
    void setEdition(bool edit);
    void updateTime(const QDateTime& time);

protected:

private:
    Ui::DashboardWidget *ui;
    QTBoard *mBoard;
    QHBoxLayout *mLiveToolBarLayout;
    QVBoxLayout *mMainLayout;

};

#endif // DASHBOARDWIDGET_H
