#ifndef CHRONO_H
#define CHRONO_H

#include <QMainWindow>
#include "stopwatch.h"
#include "timedisplay.h"
#include "lapmodel.h"
#include <QTableView>
#include <QHeaderView>
#include <QSplitter>
#include <QSortFilterProxyModel>

namespace Ui {
class Chrono;
}

class Chrono : public QWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    explicit Chrono(QWidget *parent = nullptr);
    ~Chrono();

private slots:

    /**
     * Stopwatch running state triggers.
     */
    void slotRunning();

    /**
     * Stopwatch paused state triggers.
     */
    void slotPaused();

    /**
     * Stopwatch inactive state triggers.
     */
    void slotInactive();

    void slotResume();
private:
    Ui::Chrono *ui;
    Stopwatch *m_stopwatch;
};

#endif // MAINWINDOW_H
