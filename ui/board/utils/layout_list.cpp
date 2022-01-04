#include "layout_list.h"
#include "ui/board/board_element.h"

LayoutList::LayoutList() :
    QCPLayout(),
    mRowHeight(20),
    mRowSpacing(1)
{
    setMinimumMargins(QMargins(2,2,2,2));
    setMargins(QMargins(2,2,2,2));
}

LayoutList::~LayoutList()
{
    clearLayout();
}

void LayoutList::clearLayout()
{
    int size = elementCount();
    for (int i=0; i< size; i++) {
        QCPLayoutElement * el = takeAt(0);
        if(el) {
            auto boardEl = qobject_cast<BoardElement *>(el);
            if(boardEl)
                boardEl->clearElement();
            delete el;
        }
    }
    simplify();
}

void LayoutList::simplify()
{
    int size = elementCount();
    for(int i=size - 1; i>= 0;i--) {
        QCPLayoutElement * el = takeAt(0);
        if(el) {
            auto boardEl = qobject_cast<BoardElement *>(el);
            if(boardEl)
                boardEl->clearElement();
            delete el;
        }
    }
}

QCPLayoutElement *LayoutList::takeAt(int index)
{
    if (QCPLayoutElement *el = elementAt(index)) {
        releaseElement(el);
        mElements.removeAt(index);
        return el;
    } else {
        logger()->debug() << Q_FUNC_INFO << "Attempt to take invalid index:" << index;
        return nullptr;
    }
}

bool LayoutList::take(QCPLayoutElement *element)
{
    if (element) {
        for (int i=0; i<elementCount(); ++i) {
            if (elementAt(i) == element) {
                takeAt(i);
                return true;
            }
        }
        logger()->debug() << Q_FUNC_INFO << "Element not in this layout, couldn't take";
    } else {
        logger()->debug() << Q_FUNC_INFO << "Can't take null element";
    }
    return false;
}

void LayoutList::updateLayout()
{
    double yOffset = mRect.top();
    double yOffsetLimit = mRect.bottom() - mRowHeight;
    for (int row=0; row<mElements.size(); ++row) {
        if (row > 0)
            yOffset += mRowHeight+mRowSpacing;
        double xOffset = mRect.left();

        if (mElements.at(row)) {
            if(yOffset < yOffsetLimit)
                mElements.at(row)->setOuterRect(QRectF(xOffset, yOffset,double(rect().width()), mRowHeight));
            else
                mElements.at(row)->setOuterRect(QRectF(xOffset, yOffset,0,0));
        }
    }
}

int LayoutList::elementCount() const
{
    return mElements.size();
}

QCPLayoutElement *LayoutList::elementAt(int index) const
{
    if (index >= 0 && index < mElements.size())
        return mElements.at(index);
    else
        return nullptr;
}

void LayoutList::addElement(QCPLayoutElement *element)
{
    if (element) {
        mElements.append(element);
        adoptElement(element);
    }else {
        logger()->debug() << Q_FUNC_INFO << "Can't add null element";
    }
}

void LayoutList::takeAllElements()
{
    int size = elementCount();
    for (int i=0; i< size; i++)
        takeAt(0);
    simplify();
}

double LayoutList::rowHeight() const
{
    return mRowHeight;
}

void LayoutList::setRowHeight(double rowHeight)
{
    mRowHeight = rowHeight;
}

int LayoutList::rowSpacing() const
{
    return mRowSpacing;
}

void LayoutList::setRowSpacing(int rowSpacing)
{
    mRowSpacing = rowSpacing;
}
