#ifndef LAYOUTELEMENT_H
#define LAYOUTELEMENT_H

#include "dashboard/dashboard.h"
#include "project/parameter_configuration.h"
#include "dashboard/layouts/layout_reactive.h"

class QTBLayoutReactiveElement : public QCPLayoutElement
{
    Q_OBJECT
public:
    QTBLayoutReactiveElement(QTBoard *dashboard = nullptr);

    // reimplemented virtual methods:
    virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=nullptr) const Q_DECL_OVERRIDE;

    virtual int defaultWidth() {return 1;}
    virtual int defaultHeight() {return 1;}    

    virtual void initializeElement(QTBoard *dashboard);
    virtual void clearElement() {}

    bool transparentBackground() const;
    void setTransparentBackground(bool transparentBackground);

    void setBrush(const QBrush &brush);
    virtual void update(UpdatePhase phase) Q_DECL_OVERRIDE;

protected:
    void draw(QCPPainter *painter) Q_DECL_OVERRIDE;
    void drawBackground(QCPPainter *painter);

protected:
    bool mTransparentBackground;
    QColor mBackgroundColor;
    QBrush mBrush;
    QTBoard *mBoard;
};

#endif // LAYOUTELEMENT_H
