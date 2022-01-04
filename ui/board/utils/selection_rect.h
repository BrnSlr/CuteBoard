#ifndef HIGHLIGHTWIDGET_H
#define HIGHLIGHTWIDGET_H

#include <QWidget>
#include <QPainterPath>
#include "ui/ui_global.h"
#include "log4qt/logger.h"
#include "ui/board/utils/resize_handle.h"

class BOARD_UI_EXPORT SelectionRect : public QWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
    public:

    enum HoverHandle {
                 hhNone,
                 hhTopLeft,
                 hhTopRight,
                 hhBottomLeft,
                 hhBottomRight };

    SelectionRect(QWidget *parent = nullptr);
    ~SelectionRect() Q_DECL_OVERRIDE;

    void setColor(const QColor &color);    
    void setPenActive(bool penActive);
    void setBrushActive(bool brushActive);
    void setResizeHandlesActive(bool resizeHandlesActive);

    HoverHandle hoverHandle(QPoint point);
    void setResizeHandleSize(int resizeHandleSize);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void moveEvent(QMoveEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

    void updateHandles();

signals:

public slots:

protected:
    QColor mColor;
    QColor mBrushColor;
    QBrush mBrush;

    bool mPenActive;
    bool mBrushActive;
    bool mResizeHandlesActive;
    int mResizeHandleSize;

    QPainterPath mTopLeftPath;
    QPainterPath mTopRightPath;
    QPainterPath mBottomLeftPath;
    QPainterPath mBottomRightPath;
};

#endif // HIGHLIGHTWIDGET_H
