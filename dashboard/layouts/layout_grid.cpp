#include "layout_grid.h"

QTBLayoutGrid::QTBLayoutGrid() :
    mNeedUpdate(false)
{

}

void QTBLayoutGrid::updateLayout()
{
    if(mNeedUpdate || (mRect != mPreviousRect)) {
        QCPLayoutGrid::updateLayout();
        mPreviousRect = mRect;
        mNeedUpdate = false;
    }
}

bool QTBLayoutGrid::needUpdate() const
{
    return mNeedUpdate;
}

void QTBLayoutGrid::needUpdate(bool needUpdate)
{
    mNeedUpdate = needUpdate;
}

