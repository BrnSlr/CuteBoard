#ifndef ELEMENTHIGHLIGHTEDRECT_H
#define ELEMENTHIGHLIGHTEDRECT_H

#include "dashboard/dashboard.h"

class QTBElementHighlightedRect : public QCPLayoutElement
{
public:
    QTBElementHighlightedRect(QTBoard *dashboard);

    void draw(QCPPainter *painter) Q_DECL_OVERRIDE;

private:
    QColor mColor;
};

#endif // ELEMENTHIGHLIGHTEDRECT_H
