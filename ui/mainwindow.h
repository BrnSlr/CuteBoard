#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShortcut>
#include "ui/board/board.h"
#include "ui/board/management/project.h"
#include "ui/ui_global.h"

template<typename QEnum>
QString QtEnumToString (const QEnum value)
{
  return QMetaEnum::fromType<QEnum>().valueToKey(value);
}

namespace Ui {
class BOARD_UI_EXPORT MainWindow;
}

class QSimpleUpdater;
class BOARD_UI_EXPORT MainWindow : public QMainWindow
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() Q_DECL_OVERRIDE;

    bool maximized() const;
    void setMaximized(bool maximized);
    void init(QString profileName);
    bool loadProject(QString projectPath, QString page = QString());

public slots:
    void showHomePage();
    void showLivePage();
    void showDesignPage(bool checked);
    void showReplayPage();
    void showSettings();
    void fullScreen();
    void projectSelected();
    void resetWorkingDir();
    void checkUpdates();

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const override;

private:
    Ui::MainWindow *ui;

    bool mMaximized;
    QRect mNormalGeometry;
    QAction *mHomeAction;
    QAction *mDesignAction;
    QAction *mLiveAction;
    QAction *mReplayAction;
    QAction *mSettingsAction;

    QShortcut *mFullscreenShortcut;

    bool mStatMode{false};

    QString mUpdateUrl;
    QSimpleUpdater *mUpdater;
    QTimer mUpdateTimer;
};

#endif // MAINWINDOW_H
