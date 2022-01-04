#ifndef ADJUST_TEXT_ELEMENT_H
#define ADJUST_TEXT_ELEMENT_H

#include "ui/board/board.h"
#include "ui/ui_global.h"

class BOARD_UI_EXPORT AdaptiveTextElement : public QCPTextElement
{
public:

    enum AdjustStrategy {
        asAdjustPointSize,
        asAdjustAndElide
    };

    AdaptiveTextElement(Board *dashboard);

    void update(QCPLayoutElement::UpdatePhase phase) Q_DECL_OVERRIDE;

    virtual QSizeF minimumOuterSizeHint() const Q_DECL_OVERRIDE;
    virtual QSizeF maximumOuterSizeHint() const Q_DECL_OVERRIDE;

    virtual void mousePressEvent(QMouseEvent *event, const QVariant &details) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent *event, const QPointF &startPos) Q_DECL_OVERRIDE;
    virtual void mouseDoubleClickEvent(QMouseEvent *event, const QVariant &details) Q_DECL_OVERRIDE;

    int maxPointSize() const;
    int minPointSize() const;
    int pointSize() const;
    QBrush backgroundBrush() const;
    bool needUpdate() const;
    AdjustStrategy adjustStrategy() const;

    void setDefaultColors();
    void setMaxPointSize(int maxPointSize);
    void setMinPointSize(int minPointSize);
    void setPointSize(int pointSize);
    void setBackgroundBrush(const QBrush &backgroundBrush);
    void setAdjustStrategy(const AdjustStrategy &adjustStrategy);
    void needUpdate(bool needUpdate);
    void setBoldText(bool bold);

    void setCachedPixmap(bool cachedPixmap);

    void setBorderPen(const QPen &borderPen);

protected:
    // property members:
    int mMaxPointSize;
    int mMinPointSize;
    QBrush mBackgroundBrush;
    QPen mBorderPen;

    QString mPreviousText;
    QString mTextDisplayed;
    QSizeF mPreviousSize;
    bool mNeedUpdate;
    AdjustStrategy mAdjustStrategy;

    virtual void draw(QCPPainter *painter) Q_DECL_OVERRIDE;

    bool mCachedPixmap;
    bool mCachedPixmapRendered;
    QPixmap mTextPixmap;
    Board *mBoard;
};

#endif // ADPATIVE_TEXT_ELEMENT_H
