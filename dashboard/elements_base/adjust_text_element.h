#ifndef ADJUST_TEXT_ELEMENT_H
#define ADJUST_TEXT_ELEMENT_H

#include "dashboard/dashboard.h"

class QTBAdjustTextElement : public QCPTextElement
{
public:

    enum AdjustStrategy {
        asHide,
        asElide/*,
        asElideAndHide*/
    };

    QTBAdjustTextElement(QTBoard *dashboard);

    void update(QCPLayoutElement::UpdatePhase phase) Q_DECL_OVERRIDE;

    virtual QSizeF minimumOuterSizeHint() const Q_DECL_OVERRIDE;
    virtual QSizeF maximumOuterSizeHint() const Q_DECL_OVERRIDE;

    int maxPointSize() const;
    int minPointSize() const;
    int pointSize() const;
    QBrush backgroundBrush() const;
    bool needUpdate() const;
    AdjustStrategy adjustStrategy() const;

    void setMaxPointSize(int maxPointSize);
    void setMinPointSize(int minPointSize);
    void setPointSize(int pointSize);
    void setBackgroundBrush(const QBrush &backgroundBrush);
    void setAdjustStrategy(const AdjustStrategy &adjustStrategy);
    void needUpdate(bool needUpdate);
    void setBoldText(bool bold);

protected:
    // property members:
    int mMaxPointSize;
    int mMinPointSize;
    QBrush mBackgroundBrush;

    QString mPreviousText;
    QString mTextDisplayed;
    QSizeF mPreviousSize;
    bool mNeedUpdate;
    AdjustStrategy mAdjustStrategy;

    virtual void draw(QCPPainter *painter) Q_DECL_OVERRIDE;
    friend class QTBValueSingleDisplay;
};

#endif // ADPATIVE_TEXT_ELEMENT_H
