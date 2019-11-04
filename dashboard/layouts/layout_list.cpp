#include "layout_list.h"
#include "layout_reactive_element.h"

QTBLayoutList::QTBLayoutList() :
    QCPLayout(),
    mRowHeight(20),
    mRowSpacing(1)
{
    setMinimumMargins(QMargins(2,2,2,2));
    setMargins(QMargins(2,2,2,2));
}

QTBLayoutList::~QTBLayoutList()
{
    clearLayout();
}

void QTBLayoutList::clearLayout()
{
    int size = elementCount();
    for (int i=0; i< size; i++) {
        QCPLayoutElement * el = takeAt(0);
        if(el) {
            QTBLayoutReactiveElement * cuteEl = qobject_cast<QTBLayoutReactiveElement *>(el);
            if(cuteEl)
                cuteEl->clearElement();
            delete el;
        }
    }
    simplify();
}

void QTBLayoutList::simplify()
{
    int size = elementCount();
    for(int i=size - 1; i>= 0;i--) {
        QCPLayoutElement * el = takeAt(0);
        if(el) {
            QTBLayoutReactiveElement * cuteEl = qobject_cast<QTBLayoutReactiveElement *>(el);
            if(cuteEl)
                cuteEl->clearElement();
            delete el;
        }
    }
}

QCPLayoutElement *QTBLayoutList::takeAt(int index)
{
    if (QCPLayoutElement *el = elementAt(index))
    {
        releaseElement(el);
        mElements.removeAt(index);
        return el;
    } else
    {
        qDebug() << Q_FUNC_INFO << "Attempt to take invalid index:" << index;
        return nullptr;
    }
}

bool QTBLayoutList::take(QCPLayoutElement *element)
{
    if (element) {
        for (int i=0; i<elementCount(); ++i) {
            if (elementAt(i) == element) {
                takeAt(i);
                return true;
            }
        }
        qDebug() << Q_FUNC_INFO << "Element not in this layout, couldn't take";
    } else {
        qDebug() << Q_FUNC_INFO << "Can't take null element";
    }
    return false;
}

void QTBLayoutList::updateLayout()
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

int QTBLayoutList::elementCount() const
{
    return mElements.size();
}

QCPLayoutElement *QTBLayoutList::elementAt(int index) const
{
    if (index >= 0 && index < mElements.size())
        return mElements.at(index);
    else
        return nullptr;
}

void QTBLayoutList::addElement(QCPLayoutElement *element)
{
    if (element) {

        mElements.append(element);
        adoptElement(element);
    }else {
        qDebug() << Q_FUNC_INFO << "Can't add null element";
    }
}

void QTBLayoutList::takeAllElements()
{
    int size = elementCount();
    for (int i=0; i< size; i++) {
        takeAt(0);
    }
    simplify();
}

double QTBLayoutList::rowHeight() const
{
    return mRowHeight;
}

void QTBLayoutList::setRowHeight(double rowHeight)
{
    mRowHeight = rowHeight;
}

int QTBLayoutList::rowSpacing() const
{
    return mRowSpacing;
}

void QTBLayoutList::setRowSpacing(int rowSpacing)
{
    mRowSpacing = rowSpacing;
}
