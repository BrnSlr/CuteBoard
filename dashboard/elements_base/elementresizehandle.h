#ifndef ELEMENTRESIZEHANDLE_H
#define ELEMENTRESIZEHANDLE_H

#include "dashboard/dashboard.h"

class QTBElementResizeHandle : public QCPLayoutElement
{
public:
    QTBElementResizeHandle(QCustomPlot *dashboard);

    void draw(QCPPainter *painter) Q_DECL_OVERRIDE;

    int size() const;
    void setSize(int size);

    void setColor(const QColor &color);

private:
    int mSize;
    QColor mColor;
};

#endif // ELEMENTRESIZEHANDLE_H
