#include "chrono.h"
#include "ui_chrono.h"
#include <QDebug>
#include <QAction>
#include "ui/widgets/utils/icon_factory.h"

Chrono::Chrono(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chrono)
{
    ui->setupUi(this);

     QIcon icoStart= create_icon(":/icons8_start_50px.png");
     QAction *actionStart = new QAction(icoStart, QString(""));
     ui->startButton->setDefaultAction(actionStart);
     QIcon icoReset= create_icon(":/icons8_stop_squared_50px.png");
     QAction *actionReset = new QAction(icoReset, QString(""));
     ui->resetButton->setDefaultAction(actionReset);
     QIcon icoPause= create_icon(":/icons8_pause_squared_50px.png");
     QAction *actionPause = new QAction(icoPause, QString(""));
     actionPause->setCheckable(true);
     actionPause->setChecked(false);
     ui->stopButton->setDefaultAction(actionPause);

    m_stopwatch = new Stopwatch {this};
    m_stopwatch->setGranularity(Stopwatch::Granularity::Tenths);

    connect(m_stopwatch, &Stopwatch::time, [=](int t) {
        ui->timeButton->setText(QTime::fromMSecsSinceStartOfDay(t).toString("HH:mm:ss.zzz"));
    });

    connect(m_stopwatch, &Stopwatch::running, this, &Chrono::slotRunning);
    connect(m_stopwatch, &Stopwatch::paused, this, &Chrono::slotPaused);
    connect(m_stopwatch, &Stopwatch::inactive, this, &Chrono::slotInactive);

    connect(ui->startButton, &QToolButton::clicked, m_stopwatch, &Stopwatch::start);
    connect(ui->startButton, &QToolButton::clicked, this, &Chrono::slotRunning);

    connect(ui->resetButton, &QToolButton::clicked, m_stopwatch, &Stopwatch::reset);
    connect(ui->resetButton, &QToolButton::clicked, this, &Chrono::slotInactive);

    connect(ui->stopButton, &QToolButton::toggled, [=](bool toggled) {
        if(toggled) {
            m_stopwatch->pause();
            slotPaused();
        } else {
            m_stopwatch->start();
            slotRunning();
        }
    });

    // triggers for MainWindow "gui" slots

    ui->startButton->setVisible(true);
    ui->stopButton->setVisible(false);
    ui->resetButton->setVisible(false);
}

Chrono::~Chrono()
{
    delete ui;
}

void Chrono::slotRunning()
{
    ui->startButton->setVisible(false);
    ui->stopButton->setVisible(true);
    ui->resetButton->setVisible(true);
}

void Chrono::slotPaused()
{
    ui->startButton->setVisible(false);
    ui->stopButton->setVisible(true);
    ui->resetButton->setVisible(true);
}

void Chrono::slotInactive()
{
    const QSignalBlocker blocker(ui->stopButton);
    ui->startButton->setVisible(true);
    ui->stopButton->setVisible(false);
    ui->stopButton->setChecked(false);
    ui->resetButton->setVisible(false);
}

void Chrono::slotResume()
{
    ui->startButton->setVisible(false);
    ui->stopButton->setVisible(true);
    ui->resetButton->setVisible(true);
}
