#include "adaptive_text.h"

bool andFunc(bool a, bool b) {return a && b;};
bool orFunc(bool a, bool b) {return a || b;};

AdaptiveTextElement::AdaptiveTextElement(Board *dashboard) :
    QCPTextElement (dashboard),
    mMaxPointSize(11),
    mMinPointSize(5),
    mBackgroundBrush(Qt::NoBrush),
    mBorderPen(Qt::NoPen),
    mNeedUpdate(false),
    mAdjustStrategy(asAdjustPointSize),
    mCachedPixmap(false),
    mCachedPixmapRendered(false),
    mBoard(dashboard)
{    
    setAntialiased(true);
    setTextColor(QApplication::palette().brightText().color());
    setTextFlags(Qt::AlignVCenter | Qt::AlignLeft);
    setMinimumMargins(QMarginsF(1,0.5,1,0.5));
    setMargins(QMarginsF(1,0.5,1,0.5));
    setLayer(QLatin1String("main"));
    setSizeConstraintRect(scrOuterRect);
}

int AdaptiveTextElement::maxPointSize() const
{
    return mMaxPointSize;
}

void AdaptiveTextElement::setMaxPointSize(int maxPointSize)
{
    mMaxPointSize = maxPointSize;
}

int AdaptiveTextElement::minPointSize() const
{
    return mMinPointSize;
}

int AdaptiveTextElement::pointSize() const
{
    return mFont.pointSize();
}

void AdaptiveTextElement::setMinPointSize(int minPointSize)
{
    mMinPointSize = minPointSize;
}

void AdaptiveTextElement::setPointSize(int pointSize)
{
    if(pointSize != mFont.pointSize()) {
        mFont.setPointSize(pointSize);
        if(mCachedPixmap) {
            if(mRect.size().width() > 0 && mRect.size().height() > 0) {
                mTextPixmap = QPixmap(mRect.size().toSize());
                mTextPixmap.fill(Qt::transparent);
                QCPPainter cachePainter(&mTextPixmap);
                if (cachePainter.isActive()) {
                    cachePainter.setFont(mainFont());
                    cachePainter.setPen(QPen(mainTextColor()));
                    cachePainter.drawText(0, 0, int(mRect.width()), int(mRect.height()), textFlags(), mTextDisplayed);
                    mCachedPixmapRendered = true;
                }
            }
        }
    }
}

QBrush AdaptiveTextElement::backgroundBrush() const
{
    return mBackgroundBrush;
}

void AdaptiveTextElement::setBackgroundBrush(const QBrush &backgroundBrush)
{
    mBackgroundBrush = backgroundBrush;
}

void AdaptiveTextElement::update(QCPLayoutElement::UpdatePhase phase)
{
    if (phase == upLayout) {

        if(mNeedUpdate || (mPreviousText != mText) || (mPreviousSize != mRect.size()) || !mCachedPixmapRendered) {

            mPreviousSize = mRect.size();
            mPreviousText = mText;
            mTextDisplayed = mText;
            if(!mText.isEmpty()) {
                QFont font = mFont;
                int pointSize = mMaxPointSize;

                setVisible(true);

                switch(mAdjustStrategy) {
                case asAdjustPointSize:
                {
                    pointSize = font.pointSize();
                    int adjustPoint = -1;
                    std::function<bool(int, int)> compareIntFunc = std::less_equal<int>();
                    std::function<bool(bool,bool)> compareRectFunc = andFunc;

                    QFontMetrics fontMetrics(font);
                    QRect boundingRect = fontMetrics.boundingRect(int(mRect.x()),
                                                                  int(mRect.y()),
                                                                  int(mRect.width()),
                                                                  int(mRect.height()),
                                                                  textFlags(),
                                                                  mText);

                    if (compareRectFunc(compareIntFunc(boundingRect.height(),int(mRect.height())),
                                        compareIntFunc(boundingRect.width(),int(mRect.width())))) {
                        adjustPoint = 1;
                        compareIntFunc = std::greater<int>();
                        compareRectFunc = orFunc;
                    }

                    for (;;) {

                        pointSize += adjustPoint;
                        if (pointSize < mMinPointSize) {
                            setVisible(false);
                            setMinimumSize(QSize(0,0));
                            break;
                        } else if (pointSize > mMaxPointSize) {
                            break;
                        }

                        font.setPointSize(pointSize);
                        QFontMetrics fm(font);
                        boundingRect = fm.boundingRect(int(mRect.x()),
                                                       int(mRect.y()),
                                                       int(mRect.width()),
                                                       int(mRect.height()),
                                                       textFlags(),
                                                       mText);

                        if (compareRectFunc(compareIntFunc(boundingRect.height(),int(mRect.height())),
                                            compareIntFunc(boundingRect.width(),int(mRect.width())))) {
                            if(adjustPoint < 0) {
                                mFont.setPointSize(pointSize);
                            }
                            break;
                        }

                        mFont.setPointSize(pointSize);
                    }
                    break;
                }
                case asAdjustAndElide:
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

            if(mCachedPixmap) {
                if(mRect.size().width() > 0 && mRect.size().height() > 0) {
                    mTextPixmap = QPixmap(mRect.size().toSize());
                    mTextPixmap.fill(Qt::transparent);
                    QCPPainter cachePainter(&mTextPixmap);
                    if (cachePainter.isActive()) {
                        cachePainter.setFont(mainFont());
                        cachePainter.setPen(QPen(mainTextColor()));
                        cachePainter.drawText(0, 0, int(mRect.width()), int(mRect.height()), textFlags(), mTextDisplayed);
                        mCachedPixmapRendered = true;
                    }
                }
            }
        }

    }
}

QSizeF AdaptiveTextElement::minimumOuterSizeHint() const
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

QSizeF AdaptiveTextElement::maximumOuterSizeHint() const
{
    return QSizeF(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}

void AdaptiveTextElement::mouseDoubleClickEvent(QMouseEvent *event, const QVariant &details)
{
    Q_UNUSED(details)
    event->ignore();
}

void AdaptiveTextElement::mousePressEvent(QMouseEvent *event, const QVariant &details)
{
    Q_UNUSED(details)
    event->ignore();
}

void AdaptiveTextElement::mouseMoveEvent(QMouseEvent *event, const QPointF &startPos)
{
    Q_UNUSED(event)
    Q_UNUSED(startPos)
    event->ignore();
}

bool AdaptiveTextElement::needUpdate() const
{
    return mNeedUpdate;
}

void AdaptiveTextElement::needUpdate(bool needUpdate)
{
    mNeedUpdate = needUpdate;
}

void AdaptiveTextElement::setBoldText(bool bold)
{
    mFont.setBold(bold);
}

AdaptiveTextElement::AdjustStrategy AdaptiveTextElement::adjustStrategy() const
{
    return mAdjustStrategy;
}

void AdaptiveTextElement::setDefaultColors()
{
    setTextColor(QApplication::palette().brightText().color());
    setBackgroundBrush(Qt::NoBrush);
}

void AdaptiveTextElement::setAdjustStrategy(const AdjustStrategy &adjustStrategy)
{
    if(mAdjustStrategy != adjustStrategy) {
        mAdjustStrategy = adjustStrategy;
        mNeedUpdate = true;
    }
}

void AdaptiveTextElement::draw(QCPPainter *painter)
{
    painter->setBrush(mBackgroundBrush);
    painter->setPen(mBorderPen);
    painter->drawRoundedRect(mRect, 4, 4);

    if(!mText.isEmpty()) {
        painter->setBrush(Qt::NoBrush);
        painter->setFont(mainFont());
        painter->setPen(QPen(mainTextColor()));
        if(mCachedPixmap) {
            painter->drawPixmap(mRect.topLeft(), mTextPixmap);
        } else {
            painter->drawText(mRect, mTextFlags, mTextDisplayed, &mTextBoundingRect);
        }
    }
}

void AdaptiveTextElement::setCachedPixmap(bool cachedPixmap)
{
    mCachedPixmap = cachedPixmap;
}

void AdaptiveTextElement::setBorderPen(const QPen &borderPen)
{
    mBorderPen = borderPen;
}
