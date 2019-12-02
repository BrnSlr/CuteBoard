#include "adjust_text_element.h"

bool andFunc(bool a, bool b) {return a && b;};
bool orFunc(bool a, bool b) {return a || b;};

QTBAdjustTextElement::QTBAdjustTextElement(QTBoard *dashboard) :
    QCPTextElement (dashboard),
    mMaxPointSize(11),
    mMinPointSize(5),
    mBackgroundBrush(Qt::NoBrush),
    mNeedUpdate(false),
    mAdjustStrategy(asHide)
{    
    setAntialiased(true);
    setTextColor(QColor(255, 255, 255));
    setTextFlags(Qt::AlignVCenter | Qt::AlignLeft);
    setMargins(QMargins(0,0,0,0));
    setLayer(QLatin1String("main"));
    setSizeConstraintRect(scrOuterRect);
}

int QTBAdjustTextElement::maxPointSize() const
{
    return mMaxPointSize;
}

void QTBAdjustTextElement::setMaxPointSize(int maxPointSize)
{
    mMaxPointSize = maxPointSize;
}

int QTBAdjustTextElement::minPointSize() const
{
    return mMinPointSize;
}

int QTBAdjustTextElement::pointSize() const
{
    return mFont.pointSize();
}

void QTBAdjustTextElement::setMinPointSize(int minPointSize)
{
    mMinPointSize = minPointSize;
}

void QTBAdjustTextElement::setPointSize(int pointSize)
{
    mFont.setPointSize(pointSize);
}

QBrush QTBAdjustTextElement::backgroundBrush() const
{
    return mBackgroundBrush;
}

void QTBAdjustTextElement::setBackgroundBrush(const QBrush &backgroundBrush)
{
    mBackgroundBrush = backgroundBrush;
}

void QTBAdjustTextElement::update(QCPLayoutElement::UpdatePhase phase)
{
    if (phase == upLayout) {

        if(mNeedUpdate || (mPreviousText != mText) || (mPreviousSize != mRect.size())) {
            mPreviousSize = mRect.size();
            mPreviousText = mText;
            mTextDisplayed = mText;
            if(!mText.isEmpty()) {
                QFont font = mFont;
                int pointSize = mMaxPointSize;

                setVisible(true);

                switch(mAdjustStrategy) {
                case asHide:
                {
                    pointSize = font.pointSize();
                    int adjustPoint = -1;
                    std::function<bool(int, int)> compareIntFunc = std::less_equal<int>();
                    std::function<bool(bool,bool)> compareRectFunc = andFunc;

                        QFontMetrics fontMetrics(font);
                        QRect boundingRect = fontMetrics.boundingRect(mRect.toRect(), textFlags(), mText);

                    if (compareRectFunc(compareIntFunc(boundingRect.height(),int(mRect.height())),
                                        compareIntFunc(boundingRect.width(),int(mRect.width())))) {
                        adjustPoint = 1;
                        compareIntFunc = std::greater_equal<int>();
                        compareRectFunc = orFunc;
                }

                    pointSize += adjustPoint;

                    for (;;) {
                        font.setPointSize(pointSize);
                        QFontMetrics fontMetrics(font);
                        boundingRect = fontMetrics.boundingRect(mRect.toRect(), textFlags(), mText);

                        if (compareRectFunc(compareIntFunc(boundingRect.height(),int(mRect.height())),
                                            compareIntFunc(boundingRect.width(),int(mRect.width())))) {
                            if(adjustPoint < 0) {
                            mFont.setPointSize(pointSize);
                            }
                            break;
                        }

                        if (pointSize < mMinPointSize) {
                            setVisible(false);
                            setMinimumSize(QSize(0,0));
                            break;
                        } else if (pointSize > mMaxPointSize) {
                            break;
                        }
                        mFont.setPointSize(pointSize);
                        pointSize += adjustPoint;
                    }
                    break;
                }
                case asElide:
                {
                    for (;;) {
                        font.setPointSize(pointSize);
                        QFontMetrics fontMetrics(font);
                        QRect boundingRect = fontMetrics.boundingRect(mRect.toRect(), textFlags(), mText);
                        if (boundingRect.height() <= mRect.height()) {
                            mFont.setPointSize(pointSize);
                            break;
                        }
                        pointSize -= 1;

                        if (pointSize <= 0) {
                            setVisible(false);
                            setMinimumSize(QSize(0,0));
                            break;
                        }
                    }

                    if(visible()) {
                        QFontMetrics fontMetrics(mFont);
                        QRect boundingRect = fontMetrics.boundingRect(mRect.toRect(), textFlags(), mText);
                        if (boundingRect.width() > mRect.width()) {
                            mTextDisplayed = fontMetrics.elidedText(mText, Qt::ElideMiddle, int(mRect.width()));
                        }
                    }
                    break;
                }
                }
            }
        }
    }
}

QSizeF QTBAdjustTextElement::minimumOuterSizeHint() const
{
    if(visible()) {
        QFont font = mFont;
        QFontMetrics metrics(font);
        QSizeF result(metrics.boundingRect(0, 0, 0, 0, mTextFlags, mText).size());
        result.rwidth() += mMargins.left()+mMargins.right();
        result.rheight() += mMargins.top()+mMargins.bottom();
        return result;
    } else {
        return QSize(0,0);
    }
}

QSizeF QTBAdjustTextElement::maximumOuterSizeHint() const
{
    return QSizeF(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}

bool QTBAdjustTextElement::needUpdate() const
{
    return mNeedUpdate;
}

void QTBAdjustTextElement::needUpdate(bool needUpdate)
{
    mNeedUpdate = needUpdate;
}

void QTBAdjustTextElement::setBoldText(bool bold)
{
    mFont.setBold(bold);
}

QTBAdjustTextElement::AdjustStrategy QTBAdjustTextElement::adjustStrategy() const
{
    return mAdjustStrategy;
}

void QTBAdjustTextElement::setAdjustStrategy(const AdjustStrategy &adjustStrategy)
{
    if(mAdjustStrategy != adjustStrategy) {
        mAdjustStrategy = adjustStrategy;
        mNeedUpdate = true;
    }
}

void QTBAdjustTextElement::draw(QCPPainter *painter)
{
    if(mBackgroundBrush != Qt::NoBrush) {
        painter->setBrush(mBackgroundBrush);
        painter->setPen(Qt::NoPen);
        painter->drawRect(mRect);
        painter->setBrush(Qt::NoBrush);
    }

    if(!mText.isEmpty()) {
        painter->setFont(mainFont());
        painter->setPen(QPen(mainTextColor()));
        painter->drawText(mRect, mTextFlags, mTextDisplayed, &mTextBoundingRect);
    }
}
