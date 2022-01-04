#ifndef BOARDPAGE_H
#define BOARDPAGE_H

#include <QWidget>
#include "board_widget.h"
//#include "ui/board/board.h"
#include "ui/board/management/project.h"
#include "board_toolbar.h"

namespace Ui {
class BoardPage;
}

class BoardPage : public QWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    explicit BoardPage(QWidget *parent = nullptr);
    ~BoardPage() Q_DECL_OVERRIDE;

    QSharedPointer<Project> project() const;
    Board *board() const;
    BoardToolbar::DashboardMode mode() {return mToolBar->mode();}
    void setMode(BoardToolbar::DashboardMode mode);
    BoardToolbar * toolBar();

signals:
    void fullScreenMode();

public slots:
    void savePage();
    void updateTime(const QDateTime& time);
    void saveJpg();
    void keyboardText();

protected:
    QSize minimumSizeHint() const override;
    void keyPressEvent(QKeyEvent *event) override;


private:
    Ui::BoardPage *ui;
    Board *mBoard;
    BoardWidget *mBoardWidget;
    BoardToolbar *mToolBar;
    QVBoxLayout *mMainLayout;
    QTimer *mKeyboardTimer;
    QString mKeyboardText;

};

#endif // DASHBOARDWIDGET_H
