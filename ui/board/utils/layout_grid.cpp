#include "layout_grid.h"

LayoutGrid::LayoutGrid() :
    mNeedUpdate(false)
{

}

void LayoutGrid::updateLayout()
{
    if(mNeedUpdate || (mRect != mPreviousRect)) {
        QCPLayoutGrid::updateLayout();
        mPreviousRect = mRect;
        mNeedUpdate = false;
    }
}

bool LayoutGrid::needUpdate() const
{
    return mNeedUpdate;
}

void LayoutGrid::needUpdate(bool needUpdate)
{
    mNeedUpdate = needUpdate;
}

