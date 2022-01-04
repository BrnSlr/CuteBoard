#ifndef BOARD_WIDGET_H
#define BOARD_WIDGET_H

#include <QWidget>
#include "ui/board/board.h"

class BoardWidget : public QFrame
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    explicit BoardWidget(QWidget *parent = nullptr);
    ~BoardWidget() Q_DECL_OVERRIDE;

    Board *board() const;

signals:

public slots:

private:
    Board *mBoard;    
};

#endif // BOARD_WIDGET_H
