#ifndef RESIZE_HANDLE_H
#define RESIZE_HANDLE_H

#include <QWidget>
#include <QApplication>
#include "log4qt/logger.h"

class ResizeHandle : public QWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
    public:

        enum Position {
                 pTopLeft = 0,
                 pTopRight = 1,
                 pBottomLeft = 2,
                 pBottomRight = 3 };

    explicit ResizeHandle(QWidget *parent, Position position = pBottomRight);
    ~ResizeHandle() override;

    void setBottomRight(const QPointF &bottomRight);

protected:
    void paintEvent(QPaintEvent *event) override;

    void moveEvent(QMoveEvent *event) override;
    void enterEvent(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void leaveEvent(QEvent *event) override;
signals:

public slots:

private:
    QPointF mBottomRight;
    Position mPosition;
};

#endif // RESIZE_HANDLE_H
