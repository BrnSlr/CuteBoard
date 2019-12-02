#include "dashboard/layouts/layout_reactive.h"
#include "dashboard/elements_factory/elementfactory.h"
#include "dashboard/dashboard_element.h"

QTBLayoutReactive::QTBLayoutReactive(QTBoard *dashboard,
                                     int nbRows,
                                     int nbCols) :
    QCPLayout(),
    mRowCount(nbRows),
    mColumnCount(nbCols),
    mRowHeight(1),
    mColumnWidth(1),
    mRowSpacing(4),
    mColumnSpacing(4),
    mSingleElementRowCount(0),
    mSingleElementColumnCount(0),
    mLocked(false),
    mDragging(false),
    mDraggedElementIndex(-1),
    mHighlightRect(nullptr),
    mBoard(dashboard)
{
    setMinimumMargins(QMargins(2,2,2,2));
    setMargins(QMargins(2,2,2,2));
    mRightClickMenu = new QMenu();
    mEditAction = mRightClickMenu->addAction("Edit");
    mCloseAction = mRightClickMenu->addAction("Close");

    mHighlightRect = new QTBElementHighlightedRect(mBoard);
    mHighlightRect->setVisible(false);

    updateElementMap();
}

QTBLayoutReactive::~QTBLayoutReactive()
{
    delete mRightClickMenu;
    clearLayout();
}

void QTBLayoutReactive::clearLayout()
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

void QTBLayoutReactive::simplify()
{
    int size = elementCount();
    for(int i=size - 1; i>= 0;i--) {
        if((mElementsRow.at(i) >= mRowCount) ||
                (mElementsRow.at(i) + mElementsHeight.at(i) >= mRowCount) ||
                (mElementsColumn.at(i) >= mColumnCount) ||
                (mElementsColumn.at(i) + mElementsWidth.at(i) >= mRowCount)) {
            QCPLayoutElement * el = takeAt(0);
            if(el) {
                QTBLayoutReactiveElement * cuteEl = qobject_cast<QTBLayoutReactiveElement *>(el);
                if(cuteEl)
                    cuteEl->clearElement();
                delete el;
            }
        }
    }
    updateElementMap();
}

void QTBLayoutReactive::updateLayout()
{
    mRowHeight = double(rect().height()) / double(mRowCount);
    mColumnWidth = double(rect().width()) / double(mColumnCount);
    mSingleElementSize.setWidth(mSingleElementColumnCount * mColumnWidth - mColumnSpacing);
    mSingleElementSize.setHeight(mSingleElementRowCount * mRowHeight - mRowSpacing);

    for (int i=0; i<mElements.size(); ++i) {
        QRectF elementRect = QRectF(mMargins.left() + mElementsColumn.at(i) * mColumnWidth + double(mColumnSpacing / 2.0),
                                    mMargins.top() + mElementsRow.at(i) * mRowHeight + double(mRowSpacing / 2.0),
                                    mElementsWidth.at(i) * mColumnWidth - mColumnSpacing,
                                    mElementsHeight.at(i) * mRowHeight - mRowSpacing);

        mElements.at(i)->setOuterRect(elementRect);

        QRectF resizeRect(mElements.at(i)->outerRect().bottomRight().x() - mResizeHandles.at(i)->size(),
                          mElements.at(i)->outerRect().bottomRight().y() - mResizeHandles.at(i)->size(),
                          mResizeHandles.at(i)->size(),
                          mResizeHandles.at(i)->size());

        mResizeHandles.at(i)->setOuterRect(resizeRect);
    }
}

int QTBLayoutReactive::elementCount() const
{
    return mElements.size();
}

QCPLayoutElement *QTBLayoutReactive::elementAt(int index) const
{
    if (index >= 0 && index < mElements.size())
        return mElements.at(index);
    else
        return nullptr;
}

QCPLayoutElement *QTBLayoutReactive::elementAt(const QPointF &pos) const
{
    int indexCol = int((pos.x() - mMargins.left()) / mColumnWidth);
    int indexRow = int((pos.y() - mMargins.top()) / mRowHeight);
    if(indexRow < mRowCount &&
            indexCol < mColumnCount) {
        if(QCPLayoutElement *el = mElementsMap.at(indexCol).at(indexRow))
            if (el->realVisibility() && el->selectTest(pos, false) >= 0)
                return el;
    }

    return nullptr;
}

QCPLayoutElement *QTBLayoutReactive::elementAt(int col, int row) const
{
    for (int i=0; i<mElements.size(); ++i) {
        if( (mElementsColumn.at(i) <= col) &&
                ((mElementsColumn.at(i) + mElementsWidth.at(i) - 1) >= col) &&
                (mElementsRow.at(i) <= row) &&
                ((mElementsRow.at(i) + mElementsHeight.at(i) - 1) >= row))
            return mElements.at(i);
    }

    return nullptr;
}

QCPLayoutElement *QTBLayoutReactive::elementAt(int col, int row, int width, int height) const
{
    QList<int> indexList;

    for(int i=0; i< mElementsColumn.count();i++) {
        if(mElementsColumn.at(i) == col)
            indexList.append(i);
    }

    for(int i=0; i< indexList.count();i++) {
        if(mElementsRow.at(indexList.at(i)) == row &&
                mElementsWidth.at(indexList.at(i)) == width &&
                mElementsHeight.at(indexList.at(i)) == height)
            return mElements.at(indexList.at(i));
    }

    return nullptr;
}

int QTBLayoutReactive::elementRowAt(int index) const
{
    if (index >= 0 && index < mElementsRow.size())
        return mElementsRow.at(index);
    else
        return -1;
}

int QTBLayoutReactive::elementColumnAt(int index) const
{
    if (index >= 0 && index < mElementsColumn.size())
        return mElementsColumn.at(index);
    else
        return -1;
}

int QTBLayoutReactive::elementWidthAt(int index) const
{
    if (index >= 0 && index < mElementsWidth.size())
        return mElementsWidth.at(index);
    else
        return -1;
}

int QTBLayoutReactive::elementHeightAt(int index) const
{
    if (index >= 0 && index < mElementsHeight.size())
        return mElementsHeight.at(index);
    else
        return -1;
}

QCPLayoutElement *QTBLayoutReactive::takeAt(int index)
{
    if (QCPLayoutElement *el = elementAt(index))
    {
        releaseElement(el);
        mElements.removeAt(index);
        mElementsColumn.removeAt(index);
        mElementsRow.removeAt(index);
        mElementsWidth.removeAt(index);
        mElementsHeight.removeAt(index);
        mElementsDraggable.removeAt(index);
        mElementsResizable.removeAt(index);
        mElementsEditable.removeAt(index);
        mElementsClosable.removeAt(index);
        QTBElementResizeHandle *h = mResizeHandles.takeAt(index);
        delete h;
        updateElementMap();
        return el;
    } else
    {
        qDebug() << Q_FUNC_INFO << "Attempt to take invalid index:" << index;
        return nullptr;
    }
}

bool QTBLayoutReactive::take(QCPLayoutElement *element)
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

QList<QCPLayoutElement *> QTBLayoutReactive::elements(bool recursive) const
{
    Q_UNUSED(recursive)
    return mElements;
}

double QTBLayoutReactive::selectTest(const QPointF &pos, bool onlySelectable, QVariant *details) const
{
    Q_UNUSED(details)
    if (onlySelectable)
        return -1;

    int indexCol = int((pos.x() - mMargins.left()) / mColumnWidth);
    int indexRow = int((pos.y() - mMargins.top()) / mRowHeight);
    if(indexRow < mRowCount &&
            indexCol < mColumnCount) {
        if(QCPLayoutElement *el = mElementsMap.at(indexCol).at(indexRow))
            if (el->realVisibility() && el->selectTest(pos, onlySelectable) >= 0)
                return  mParentPlot->selectionTolerance()*0.99;
    }

    return -1;
}

void QTBLayoutReactive::saveElementRectSettings(int index, QSettings *settings)
{
    settings->setValue("column", elementColumnAt(index));
    settings->setValue("row", elementRowAt(index));
    settings->setValue("width", elementWidthAt(index));
    settings->setValue("height", elementHeightAt(index));
}

QRect QTBLayoutReactive::loadElementRectSettings(QSettings *settings)
{
    QRect rect;

    rect.setX(settings->value("column").toInt());
    rect.setY(settings->value("row").toInt());
    rect.setWidth(settings->value("width").toInt());
    rect.setHeight(settings->value("height").toInt());

    return rect;
}

void QTBLayoutReactive::draggedElement(QDragMoveEvent *event)
{
    if(rect().contains(event->pos())) {
        if(selectTest(event->pos(), false) >= 0) {
            event->ignore();
            mHighlightRect->setVisible(false);
        } else {
            event->accept();
            mHighlightRect->setVisible(true);

            int indexCol = int((event->pos().x() - mMargins.left()) / mColumnWidth);
            int indexRow = int((event->pos().y() - mMargins.top()) / mRowHeight);

            QRectF elementRect = QRectF(mMargins.left() + indexCol * mColumnWidth + double(mColumnSpacing / 2.0),
                                        mMargins.top() + indexRow * mRowHeight + double(mRowSpacing / 2.0),
                                        mColumnWidth - mColumnSpacing,
                                        mRowHeight - mRowSpacing);

            mHighlightRect->setOuterRect(elementRect);
        }
    } else {
        event->ignore();
        mHighlightRect->setVisible(false);
    }

    mBoard->fullReplot();
    mParentPlot->layer(QLatin1String("overlay"))->replot();
}

void QTBLayoutReactive::droppedElement(QDropEvent *event)
{
    if(rect().contains(event->pos())) {
        if(selectTest(event->pos(), false) < 0) {
            const QMimeData *mimeData = event->mimeData();
            QByteArray encoded = mimeData->data( QTBoard::elementMimeType() );
            QString elementName = QString::fromUtf8(encoded);

            int indexCol = int(mColumnCount *  event->pos().x() / mOuterRect.width()) ;
            int indexRow = int(mRowCount * event->pos().y() / mOuterRect.height()) ;

            QTBLayoutReactiveElement* el = ElementFactory::Instance()->Create(elementName);
            if(el) {
                el->initializeElement(qobject_cast<QTBoard*>(mParentPlot));
                addElement(el, indexCol, indexRow);
                emit pageModified();
            }
        }
    }
    event->accept();
    mHighlightRect->setVisible(false);
    mBoard->fullReplot();
    mParentPlot->layer(QLatin1String("overlay"))->replot();

}

void QTBLayoutReactive::addElement(QCPLayoutElement *element, QPoint pos)
{
    int indexCol = int(mColumnCount *  pos.x() / mOuterRect.width()) ;
    int indexRow = int(mRowCount * pos.y() / mOuterRect.height()) ;

    addElement(element, indexCol, indexRow);
}

void QTBLayoutReactive::addElement(QCPLayoutElement *element, int col, int row, int width, int height)
{
    if (element) {
        QTBLayoutReactiveElement * cuteEl = qobject_cast<QTBLayoutReactiveElement *>(element);
        if(width < 0) {
            if(cuteEl)
                width = cuteEl->defaultWidth();
            else
                width = 1;
        }

        if(height < 0){
            if(cuteEl)
                height = cuteEl->defaultHeight();
            else
                height = 1;
        }

        QPointF adjustedPoint;
        // width ajustement
        int adjustedWidth = 1;
        while((adjustedWidth < width) && (col + adjustedWidth < mColumnCount)) {
            adjustedPoint.setX(mMargins.left() + (adjustedWidth +col + 0.5) * mColumnWidth );
            adjustedPoint.setY(mMargins.top() + ( row + 0.5) * mRowHeight);
            if(selectTest(adjustedPoint, false) >= 0)
                break;
            else
                adjustedWidth ++;
        }
        width = adjustedWidth;

        // height ajustement
        int adjustedHeight = 1;
        while((adjustedHeight < height) && (row + adjustedHeight < mRowCount)) {
            bool fit = true;
            for(int i=0; i< width;i++) {
                adjustedPoint.setX(mMargins.left() + ( col + i + 0.5) * mColumnWidth);
                adjustedPoint.setY(mMargins.top() + ( adjustedHeight + row + 0.5) * mRowHeight);
                if(selectTest(adjustedPoint, false) >= 0) {
                    fit = false;
                    break;
                }
            }
            if(fit)
                adjustedHeight ++;
            else
                break;
        }
        height = adjustedHeight;

        if((col < 0) || (col + width > mColumnCount)) {
            qDebug() << QString("Requested column/width (%1/%2) exceeds layout dimension (%3/%4)").arg(col).arg(width).arg(mColumnCount).arg(mRowCount);
            delete element;
            element = nullptr;
            return;
        }

        if((row < 0) || (row + height > mRowCount)) {
            qDebug() << QString("Requested row/height (%1/%2) exceeds layout dimension (%3/%4)").arg(row).arg(height).arg(mColumnCount).arg(mRowCount);
            delete element;
            element = nullptr;
            return;
        }

        if (element->layout()) // remove from old layout first
            element->layout()->take(element);

        mElements.append(element);
        mElementsColumn.append(col);
        mElementsRow.append(row);
        mElementsWidth.append(width);
        mElementsHeight.append(height);
        adoptElement(element);
        updateElementMap();

        bool editable = false;
        bool closable = false;
        if(cuteEl) {
            editable = true;
            closable = true;
        }
        mElementsDraggable.append(true);
        mElementsResizable.append(true);
        mElementsEditable.append(editable);
        mElementsClosable.append(closable);

        QTBElementResizeHandle *resizeHandle = new QTBElementResizeHandle(mParentPlot);
        resizeHandle->setVisible(!mLocked);
        mResizeHandles.append(resizeHandle);

    } else {
        qDebug() << Q_FUNC_INFO << "Can't add null element";
    }
}

void QTBLayoutReactive::addElement(QCPLayoutElement *element, QRect rect)
{
    addElement(element, rect.x(), rect.y(), rect.width(), rect.height());
}

void QTBLayoutReactive::updateElementMap()
{
    mElementsMap.clear();

    for(int i=0;i<mColumnCount;i++) {
        QList<QCPLayoutElement*> colList;
        for(int j=0;j<mRowCount;j++) {
            colList.append(nullptr);
        }
        mElementsMap.append(colList);
    }

    for(int i=0; i<mElements.size();i++) {
        for(int j=mElementsColumn.at(i); j<mElementsColumn.at(i)+mElementsWidth.at(i);j++)
            for(int k=mElementsRow.at(i); k<mElementsRow.at(i)+mElementsHeight.at(i);k++)
                mElementsMap[j][k] = mElements.at(i);
    }
}

int QTBLayoutReactive::rowCount() const
{
    return mRowCount;
}

void QTBLayoutReactive::setRowCount(int rowCount)
{
    if(rowCount > 0) {
        if(rowCount != mRowCount) {
            mRowCount = rowCount;
            simplify();
        }
    }
}

int QTBLayoutReactive::columnCount() const
{
    return mColumnCount;
}

void QTBLayoutReactive::setColumnCount(int columnCount)
{
    if(columnCount > 0) {
        if(columnCount != mColumnCount) {
            mColumnCount = columnCount;
            simplify();
        }
    }
}

int QTBLayoutReactive::columnSpacing() const
{
    return mColumnSpacing;
}

void QTBLayoutReactive::setColumnSpacing(int columnSpacing)
{
    if(columnSpacing >= 0)
        mColumnSpacing = columnSpacing;
}

int QTBLayoutReactive::rowSpacing() const
{
    return mRowSpacing;
}

void QTBLayoutReactive::setRowSpacing(int rowSpacing)
{
    if(rowSpacing >= 0)
        mRowSpacing = rowSpacing;
}

bool QTBLayoutReactive::locked() const
{
    return mLocked;
}

void QTBLayoutReactive::setLocked(bool locked)
{
    if(locked != mLocked) {
        mLocked = locked;
        for (int i=0; i<mElements.size(); ++i) {
            mResizeHandles.at(i)->setVisible(!mLocked && mElementsResizable.at(i));
        }
        mParentPlot->replot();
    }
}

void QTBLayoutReactive::mousePress(QMouseEvent *event)
{
    if(!mLocked) {

        int indexCol = int((event->pos().x() - mMargins.left()) / mColumnWidth);
        int indexRow = int((event->pos().y() - mMargins.top()) / mRowHeight);

        int indexElement = -1;
        if(QCPLayoutElement *el = mElementsMap.at(indexCol).at(indexRow)) {
            indexElement = mElements.indexOf(el);
        }

        if (event->buttons() & Qt::LeftButton)
        {
            if(indexElement >= 0) {
                if ((mElements.at(indexElement)->realVisibility() && mElements.at(indexElement)->selectTest(event->pos(), false) >= 0) ||
                        (mResizeHandles.at(indexElement)->realVisibility() && mResizeHandles.at(indexElement)->selectTest(event->pos(), false) >= 0) ) {

                    if(mElementsDraggable.at(indexElement)) {
                        mDragAction = daMoveElement;
                        mDraggedElementIndex = indexElement;
                        mDragStartPoint = event->pos();

                        mDraggedElementInitialColumn = mElementsColumn.at(indexElement);
                        mDraggedElementInitialRow = mElementsRow.at(indexElement);
                        mDraggedElementInitialWidth = mElementsWidth.at(indexElement);
                        mDraggedElementInitialHeight = mElementsHeight.at(indexElement);

                        mDraggedElementNewColumn = mDraggedElementInitialColumn;
                        mDraggedElementNewRow = mDraggedElementInitialRow;
                        mDraggedElementNewWidth = mDraggedElementInitialWidth;
                        mDraggedElementNewHeight = mDraggedElementInitialHeight;

                        if (mResizeHandles.at(indexElement)->realVisibility() && mResizeHandles.at(indexElement)->selectTest(event->pos(), false) >= 0)
                            mDragAction = daResizeElement;

                        mHighlightRect->setVisible(true);
                        mDraggedElementInitalRect = mElements.at(indexElement)->outerRect();
                        mHighlightRect->setOuterRect(mDraggedElementInitalRect);

                        mDragging = true;
                        mParentPlot->replot(QCustomPlot::rpQueuedReplot);
                    }
                    return;
                }
            }

        } else if (event->buttons() & Qt::RightButton) {
            if(indexElement >= 0) {
                mEditAction->setEnabled(mElementsEditable.at(indexElement));
                mCloseAction->setEnabled(mElementsClosable.at(indexElement));
                if(mElementsEditable.at(indexElement) ||
                        mElementsClosable.at(indexElement)) {
                    QAction *action = mRightClickMenu->exec(event->globalPos());
                    if(action) {
                        if(action->text() == "Edit") {
                            if (QTBDashboardElement *el = qobject_cast<QTBDashboardElement*>(mElements.at(indexElement)))
                                el->edit();
                        } else {
                            QCPLayoutElement *element = takeAt(indexElement);
                            if (QTBDashboardElement *el = qobject_cast<QTBDashboardElement*>(element))
                                el->clearElement();
                            element->deleteLater();
                        }
                    }
                }
                return;
            }
        }
    }
}

void QTBLayoutReactive::mouseMove(QMouseEvent *event)
{
    if(!mLocked) {

        if(mDragging && mDraggedElementIndex >= 0) {

            int xShift = event->pos().x() - mDragStartPoint.x();
            int yShift = event->pos().y() - mDragStartPoint.y();

            if(mDragAction == daMoveElement) {

                // the selected element is resized according to user event
                QRectF requestedRect = mDraggedElementInitalRect;
                requestedRect.translate(xShift, yShift);
                mHighlightRect->setOuterRect(requestedRect);

                // the highlighted zone is updated to display the target
                int draggedElementNewColumn = qRound((requestedRect.x() - mMargins.left()) / mColumnWidth);
                int draggedElementNewRow = qRound((requestedRect.y() - mMargins.top()) / mRowHeight);
                mDraggedElementNewWidth = mElementsWidth.at(mDraggedElementIndex);
                mDraggedElementNewHeight = mElementsHeight.at(mDraggedElementIndex);

                if(draggedElementNewColumn < 0)
                    draggedElementNewColumn = 0;
                if(draggedElementNewColumn + mElementsWidth.at(mDraggedElementIndex) >= mColumnCount)
                    draggedElementNewColumn = mColumnCount - mElementsWidth.at(mDraggedElementIndex);

                if(draggedElementNewRow < 0)
                    draggedElementNewRow = 0;
                if(draggedElementNewRow + mElementsHeight.at(mDraggedElementIndex) >= mRowCount)
                    draggedElementNewRow = mRowCount - mElementsHeight.at(mDraggedElementIndex);

                QRectF elementRect = QRectF(mMargins.left() + draggedElementNewColumn * mColumnWidth + double(mColumnSpacing / 2.0),
                                            mMargins.top() + draggedElementNewRow * mRowHeight + double(mRowSpacing / 2.0),
                                            mElementsWidth.at(mDraggedElementIndex) * mColumnWidth - mColumnSpacing,
                                            mElementsHeight.at(mDraggedElementIndex) * mRowHeight - mRowSpacing);

                QRectF adjRect = elementRect.adjusted(mColumnSpacing / 2.0, mRowSpacing / 2.0, -mColumnSpacing / 2.0, -mRowSpacing / 2.0);
                bool validPosition = true;
                for (int i=0; i<mElements.size(); ++i) {
                    if(i != mDraggedElementIndex) {
                        if ( adjRect.intersects(mElements.at(i)->outerRect())) {
                            validPosition = false;
                            break;
                        }
                    }
                }

                if(validPosition) {
                    mElementsColumn[mDraggedElementIndex] = draggedElementNewColumn;
                    mElementsRow[mDraggedElementIndex] = draggedElementNewRow;
                    updateElementMap();
                }

            } else {

                // the selected element is resized according to user event
                QRectF newDragRect = mDraggedElementInitalRect;
                double newWidth = newDragRect.width() + xShift;
                double newHeight = newDragRect.height() + yShift;

                if(newWidth < mColumnWidth)
                    newWidth = mColumnWidth;

                if(newHeight < mRowHeight)
                    newHeight = mRowHeight;

                newDragRect.setWidth(newWidth);
                newDragRect.setHeight(newHeight);

                mHighlightRect->setOuterRect(newDragRect);

                // the highlighted zone is updated to display the target
                int newDragWidth = qRound(newWidth / mColumnWidth);
                int newDragHeight = qRound(newHeight / mRowHeight);

                mDraggedElementNewColumn = mElementsColumn.at(mDraggedElementIndex);
                mDraggedElementNewRow = mElementsRow.at(mDraggedElementIndex);

                if(newDragWidth < 1)
                    newDragWidth = 1;
                if(newDragWidth + mDraggedElementNewColumn > mColumnCount)
                    newDragWidth = mColumnCount - mDraggedElementNewColumn;

                if(newDragHeight < 1)
                    newDragHeight = 1;
                if(newDragHeight + mDraggedElementNewRow > mRowCount)
                    newDragHeight = mRowCount - mDraggedElementNewRow;

                QRectF elementRect = QRectF(mMargins.left() + mElementsColumn.at(mDraggedElementIndex) * mColumnWidth + double(mColumnSpacing / 2.0),
                                            mMargins.top() + mElementsRow.at(mDraggedElementIndex) * mRowHeight + double(mRowSpacing / 2.0),
                                            newDragWidth * mColumnWidth - mColumnSpacing,
                                            newDragHeight * mRowHeight - mRowSpacing);

                QRectF adjRect = elementRect.adjusted(mColumnSpacing / 2.0, mRowSpacing / 2.0, -mColumnSpacing / 2.0, -mRowSpacing / 2.0);
                bool validPosition = true;
                for (int i=0; i<mElements.size(); ++i) {
                    if(i != mDraggedElementIndex) {
                        if ( adjRect.intersects(mElements.at(i)->outerRect())) {
                            validPosition = false;
                            break;
                        }
                    }
                }

                if(validPosition) {
                    mElementsWidth[mDraggedElementIndex] = newDragWidth;
                    mElementsHeight[mDraggedElementIndex] = newDragHeight;
                    updateElementMap();
                }
            }
            mBoard->fullReplot();
            mParentPlot->layer(QLatin1String("overlay"))->replot();
        } else {

            int indexCol = int((event->pos().x() - mMargins.left()) / mColumnWidth);
            int indexRow = int((event->pos().y() - mMargins.top()) / mRowHeight);

            int indexElement = -1;
            if(indexRow < mRowCount &&
                    indexCol < mColumnCount) {
                if(QCPLayoutElement *el = mElementsMap.at(indexCol).at(indexRow)) {
                    indexElement = mElements.indexOf(el);
                }
                if(indexElement > -1) {
                    if (mResizeHandles.at(indexElement)->realVisibility() && mResizeHandles.at(indexElement)->selectTest(event->pos(), false) >= 0) {
                        mParentPlot->setCursor(QCursor(Qt::SizeFDiagCursor));
                        return;
                    }
                }
            }
            mParentPlot->setCursor(QCursor(Qt::ArrowCursor));
        }
    }
}

void QTBLayoutReactive::mouseRelease(QMouseEvent *event)
{
    Q_UNUSED(event);
    if(!mLocked) {
        if(mDragging && mDraggedElementIndex >= 0) {

            QPropertyAnimation *animation = new QPropertyAnimation(this);
            animation->setTargetObject(mHighlightRect); //text
            animation->setPropertyName("outerRect");
            animation->setDuration(100);
            animation->setStartValue(mHighlightRect->outerRect());
            animation->setEndValue(mElements.at(mDraggedElementIndex)->outerRect());


            connect(animation, &QPropertyAnimation::valueChanged, [=](){
                mBoard->fullReplot();
                mParentPlot->layer(QLatin1String("overlay"))->replot();
            });

            connect(animation, &QPropertyAnimation::finished, [=](){
                mHighlightRect->setVisible(false);
                mDragging = false;
                mDraggedElementIndex = -1;

                mBoard->fullReplot();
                mParentPlot->layer(QLatin1String("overlay"))->replot();
            });

            animation->start(QAbstractAnimation::DeleteWhenStopped);

            emit pageModified();
        }
    }
}

void QTBLayoutReactive::mouseDoubleClick(QMouseEvent *event)
{
    if(!mLocked) {
        int indexCol = int((event->pos().x() - mMargins.left()) / mColumnWidth);
        int indexRow = int((event->pos().y() - mMargins.top()) / mRowHeight);
        if(indexRow < mRowCount &&
                indexCol < mColumnCount) {
            if(QCPLayoutElement *el = mElementsMap.at(indexCol).at(indexRow))
                if (el->realVisibility() && el->selectTest(event->pos(), false) >= 0)
                    if (QTBDashboardElement *cuteEl = qobject_cast<QTBDashboardElement*>(el))
                        cuteEl->edit();
        }
    }
}

QSizeF QTBLayoutReactive::singleElementSize() const
{
    return mSingleElementSize;
}

double QTBLayoutReactive::columnWidth() const
{
    return mColumnWidth;
}

double QTBLayoutReactive::rowHeight() const
{
    return mRowHeight;
}

int QTBLayoutReactive::singleElementColumnCount() const
{
    return mSingleElementColumnCount;
}

void QTBLayoutReactive::setSingleElementColumnCount(int singleElementColumnCount)
{
    mSingleElementColumnCount = singleElementColumnCount;
}

void QTBLayoutReactive::setElementResizable(int index, bool resizable)
{
    if (index >= 0 && index < mElementsResizable.size())
        mElementsResizable[index] = resizable;
}

void QTBLayoutReactive::setElementDraggable(int index, bool draggable)
{
    if (index >= 0 && index < mElementsDraggable.size())
        mElementsDraggable[index] = draggable;
}

void QTBLayoutReactive::setElementEditable(int index, bool editable)
{
    if (index >= 0 && index < mElementsEditable.size())
        mElementsEditable[index] = editable;
}

void QTBLayoutReactive::setElementClosable(int index, bool closable)
{
    if (index >= 0 && index < mElementsClosable.size())
        mElementsClosable[index] = closable;
}

int QTBLayoutReactive::singleElementRowCount() const
{
    return mSingleElementRowCount;
}

void QTBLayoutReactive::setSingleElementRowCount(int singleElementRowCount)
{
    mSingleElementRowCount = singleElementRowCount;
}
