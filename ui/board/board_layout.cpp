#include "board_layout.h"
#include "factory/element_factory_helper.h"
#include "board_element.h"
#include <QSize>

BoardLayout::BoardLayout(Board *dashboard,
                         int nbRows,
                         int nbCols) :
    QCPLayout(),
    mRowCount(nbRows),
    mColumnCount(nbCols),
    mRowHeight(1),
    mColumnWidth(1),
    mRowSpacing(0),
    mColumnSpacing(0),
    mSingleElementRowCount(0),
    mSingleElementColumnCount(0),
    mBackgroundBrush(Qt::NoBrush),
    mBackgroundPen(Qt::NoPen),
    mBackgroundScaled(true),
    mBackgroundScaledMode(Qt::KeepAspectRatioByExpanding),
    mBoard(dashboard)
{
    initializeParentPlot(dashboard);
    setLayer("layout_background");

    setMargins(QMargins(0,0,0,0));

    QSettings settings(QApplication::applicationDirPath() + QDir::separator() + QApplication::applicationName() + QString(".ini"),
                       QSettings::IniFormat);
    mRowSpacing = settings.value(QString("RowSpacing"), 0).toInt();
    mColumnSpacing = settings.value(QString("ColumnSpacing"), 0).toInt();

    updateElementMap();
}

BoardLayout::~BoardLayout()
{
    clearLayout();
}

void BoardLayout::clearLayout()
{
    int size = elementCount();
    for (int i=0; i< size; i++) {
        QCPLayoutElement * el = takeAt(0);
        if(el) {
            auto boardElement = qobject_cast<BoardElement *>(el);
            if(boardElement)
                boardElement->clearElement();
            delete el;
        }
    }
    simplify();
}

void BoardLayout::update(QCPLayoutElement::UpdatePhase phase)
{
    QCPLayout::update(phase);
}

void BoardLayout::simplify()
{
    int size = elementCount();
    for(int i=size - 1; i>= 0;i--) {
        auto rect = mElementsGridRect.at(i);
        if((rect.y() >= mRowCount) ||
                (rect.y() + rect.height() >= mRowCount) ||
                (rect.x() >= mColumnCount) ||
                (rect.x() + rect.width() >= mColumnCount)) {
            QCPLayoutElement * el = takeAt(0);
            if(el) {
                auto boardElement = qobject_cast<BoardElement *>(el);
                if(boardElement)
                    boardElement->clearElement();
                delete el;
            }
        }
    }
    updateElementMap();
}

void BoardLayout::updateLayout()
{
    updateSingleElementSize();

    for (int i=0; i<mElements.size(); ++i) {
        auto rect = mElementsGridRect.at(i);
        QRectF elementRect = QRectF(mMargins.left() + rect.x() * mColumnWidth + double(mColumnSpacing / 2.0),
                                    mMargins.top() + rect.y() * mRowHeight + double(mRowSpacing / 2.0),
                                    rect.width() * mColumnWidth - mColumnSpacing,
                                    rect.height() * mRowHeight - mRowSpacing);

        mElements.at(i)->setOuterRect(elementRect);
    }
}

int BoardLayout::elementCount() const
{
    return mElements.size();
}

int BoardLayout::elementIndex(int col, int row) const
{
    int indexElement = -1;
    if(col >= 0 && col < columnCount()
            && row >= 0 && row < rowCount()) {
        if(QCPLayoutElement *el = mElementsMap.at(col).at(row)) {
            indexElement = mElements.indexOf(el);
        }
    }
    return indexElement;
}

QCPLayoutElement *BoardLayout::elementAt(int index) const
{
    if (index >= 0 && index < mElements.size())
        return mElements.at(index);
    else
        return nullptr;
}

QCPLayoutElement *BoardLayout::elementAt(const QPointF &pos) const
{
    int indexCol = int((pos.x() - mMargins.left()) / mColumnWidth);
    int indexRow = int((pos.y() - mMargins.top()) / mRowHeight);
    if(indexRow >= 0 &&
            indexCol >= 0 &&
            indexRow < mRowCount &&
            indexCol < mColumnCount) {
        if(QCPLayoutElement *el = mElementsMap.at(indexCol).at(indexRow))
            if (el->realVisibility() && el->selectTest(pos, false) >= 0)
                return el;
    }

    return nullptr;
}

QCPLayoutElement *BoardLayout::elementAt(int col, int row) const
{
    for (int i=0; i<mElements.size(); ++i) {
        auto rect = mElementsGridRect.at(i);
        if( (rect.x() <= col) &&
                ((rect.x() + rect.width() - 1) >= col) &&
                (rect.y() <= row) &&
                ((rect.y() + rect.height() - 1) >= row))
            return mElements.at(i);
    }

    return nullptr;
}

QCPLayoutElement *BoardLayout::elementAt(int col, int row, int width, int height) const
{
    QRect rect(col, row, width, height);
    auto index = mElementsGridRect.indexOf(rect);
    if(index >= 0)
        return mElements.at(index);

    return nullptr;
}

int BoardLayout::colIndex(qreal x)
{
    return int(mColumnCount * x / mOuterRect.width());
}

int BoardLayout::rowIndex(qreal y)
{
    return int(mRowCount * y / mOuterRect.height());
}


QRect BoardLayout::elementGridRectAt(int index) const
{
    if (index >= 0 && index < mElementsGridRect.size())
        return mElementsGridRect.at(index);

    return QRect();
}

int BoardLayout::elementRowAt(int index) const
{
    if (index >= 0 && index < mElementsGridRect.size())
        return mElementsGridRect.at(index).y();
    else
        return -1;
}

int BoardLayout::elementColumnAt(int index) const
{
    if (index >= 0 && index < mElementsGridRect.size())
        return mElementsGridRect.at(index).x();
    else
        return -1;
}

int BoardLayout::elementWidthAt(int index) const
{
    if (index >= 0 && index < mElementsGridRect.size())
        return mElementsGridRect.at(index).width();
    else
        return -1;
}

int BoardLayout::elementHeightAt(int index) const
{
    if (index >= 0 && index < mElementsGridRect.size())
        return mElementsGridRect.at(index).height();
    else
        return -1;
}

int BoardLayout::elementIndex(QCPLayoutElement *element) const
{
    if (element) {
        for (int i=0; i<elementCount(); ++i) {
            if (elementAt(i) == element) {
                return i;
            }
        }
        logger()->debug() << Q_FUNC_INFO << "Element not in this layout";
    } else {
        logger()->debug() << Q_FUNC_INFO << "null element";
    }
    return -1;
}

QCPLayoutElement *BoardLayout::takeAt(int index)
{
    if (QCPLayoutElement *el = elementAt(index)) {
        releaseElement(el);
        mElements.removeAt(index);
        mElementsGridRect.removeAt(index);
        updateElementMap();
        return el;
    } else {
        logger()->debug() << Q_FUNC_INFO << "Attempt to take invalid index:" << index;
        return nullptr;
    }
}

bool BoardLayout::take(QCPLayoutElement *element)
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

QList<QCPLayoutElement *> BoardLayout::elements(bool recursive) const
{
    Q_UNUSED(recursive)
    return mElements;
}

double BoardLayout::selectTest(const QPointF &pos, bool onlySelectable, QVariant *details) const
{
    Q_UNUSED(details)
    if (onlySelectable)
        return -1;

    int indexCol = int((pos.x() - mMargins.left()) / mColumnWidth);
    int indexRow = int((pos.y() - mMargins.top()) / mRowHeight);
    if(indexRow >= 0 &&
            indexCol >= 0 &&
            indexRow < mRowCount &&
            indexCol < mColumnCount) {
        if(QCPLayoutElement *el = mElementsMap.at(indexCol).at(indexRow))
            if (el->realVisibility() && el->selectTest(pos, onlySelectable) >= 0)
                return  mParentPlot->selectionTolerance()*0.99;
    }

    return -1;
}

void BoardLayout::saveElementRectSettings(QCPLayoutElement *element, QSettings *settings)
{
    int index = elementIndex(element);
    if(index >= 0)
        saveElementRectSettings(index, settings);
}

void BoardLayout::saveElementRectSettings(int index, QSettings *settings)
{
    settings->setValue("column", elementColumnAt(index));
    settings->setValue("row", elementRowAt(index));
    settings->setValue("width", elementWidthAt(index));
    settings->setValue("height", elementHeightAt(index));
}

QRect BoardLayout::loadElementRectSettings(QSettings *settings)
{
    QRect rect;
    rect.setX(settings->value("column").toInt());
    rect.setY(settings->value("row").toInt());
    rect.setWidth(settings->value("width").toInt());
    rect.setHeight(settings->value("height").toInt());

    return rect;
}

//void BoardLayout::dragEvent(QDragMoveEvent *event)
//{
//    if(rect().contains(event->posF())) {

//        int indexCol = int((event->posF().x() - mMargins.left()) / mColumnWidth);
//        int indexRow = int((event->posF().y() - mMargins.top()) / mRowHeight);

//        if((indexCol != mDragMouseColumn) || (indexRow != mDragMouseRow)) {
//            mDragMouseColumn = indexCol;
//            mDragMouseRow = indexRow;

//            if (event->mimeData()->hasFormat(Board::elementNameMimeType())) {
//                if(selectTest(event->posF(), false) >= 0) {

//                    event->ignore();

//                    mTargetRect->hide();
//                    mMouseRect->hide();
//                    mSelectionRect->hide();
//                } else {
//                    event->accept();

//                    QRectF elementRect = QRectF(mMargins.left() + indexCol * mColumnWidth + double(mColumnSpacing / 2.0),
//                                                mMargins.top() + indexRow * mRowHeight + double(mRowSpacing / 2.0),
//                                                mColumnWidth - mColumnSpacing,
//                                                mRowHeight - mRowSpacing);

//                    mTargetRect->hide();
//                    mMouseRect->hide();
//                    mSelectionRect->hide();

//                    mTargetRect->resize(elementRect.size().toSize());
//                    mTargetRect->move((elementRect.topLeft().toPoint()));
//                    mTargetRect->update();
//                    mTargetRect->show();
//                }
//            } else if (event->mimeData()->hasFormat(Board::propertyNameMimeType()) ||
//                       event->mimeData()->hasFormat(Board::parameterNameMimeType()) ||
//                       event->mimeData()->hasFormat(Board::alarmConfigMimeType())) {
//                if(selectTest(event->posF(), false) >= 0) {
//                    int indexElement = -1;
//                    if(QCPLayoutElement *el = mElementsMap.at(indexCol).at(indexRow)) {
//                        indexElement = mElements.indexOf(el);
//                    }

//                    if(indexElement >= 0) {
//                        auto gridRect = mElementsGridRect.at(indexElement);
//                        QRectF elementRect = QRectF(mMargins.left() + gridRect.x() * mColumnWidth + double(mColumnSpacing / 2.0),
//                                                    mMargins.top() + gridRect.y() * mRowHeight + double(mRowSpacing / 2.0),
//                                                    gridRect.width() * mColumnWidth - mColumnSpacing,
//                                                    gridRect.height() * mRowHeight - mRowSpacing);


//                        mTargetRect->hide();
//                        mMouseRect->hide();
//                        mSelectionRect->hide();

//                        mTargetRect->resize(elementRect.size().toSize());
//                        mTargetRect->move((elementRect.topLeft().toPoint()));
//                        mTargetRect->update();
//                        mTargetRect->show();
//                    }
//                } else {
//                    event->accept();

//                    QRectF elementRect = QRectF(mMargins.left() + indexCol * mColumnWidth + double(mColumnSpacing / 2.0),
//                                                mMargins.top() + indexRow * mRowHeight + double(mRowSpacing / 2.0),
//                                                mColumnWidth - mColumnSpacing,
//                                                mRowHeight - mRowSpacing);

//                    mTargetRect->hide();
//                    mMouseRect->hide();
//                    mSelectionRect->hide();

//                    mTargetRect->resize(elementRect.size().toSize());
//                    mTargetRect->move((elementRect.topLeft().toPoint()));
//                    mTargetRect->update();
//                    mTargetRect->show();
//                }
//            } else {
//                event->ignore();
//                mTargetRect->hide();
//                mMouseRect->hide();
//                mSelectionRect->hide();
//            }
//        }
//    } else {
//        event->ignore();
//        mTargetRect->hide();
//        mMouseRect->hide();
//        mSelectionRect->hide();
//    }
//}

//void BoardLayout::dragLeaveEvent(QDragLeaveEvent *event)
//{
//    mTargetRect->hide();
//    event->ignore();
//}

QPen BoardLayout::backgroundPen() const
{
    return mBackgroundPen;
}

void BoardLayout::setBackgroundPen(const QPen &backgroundPen)
{
    mBackgroundPen = backgroundPen;
}

void BoardLayout::draw(QCPPainter *painter)
{
    drawBackground(painter);
}

void BoardLayout::drawBackground(QCPPainter *painter)
{
    // draw background fill:
    if (mBackgroundBrush != Qt::NoBrush) {
        painter->setBrush(mBackgroundBrush);
        painter->setPen(mBackgroundPen);
        painter->drawRect(mOuterRect);
    }

    // draw background pixmap (on top of fill, if brush specified):
    if (!mBackgroundPixmap.isNull()) {
        if (mBackgroundScaled) {
            // check whether mScaledBackground needs to be updated:
            QSizeF scaledSize(mBackgroundPixmap.size());
            scaledSize.scale(mRect.size(), mBackgroundScaledMode);
            if (mScaledBackgroundPixmap.size() != scaledSize)
                mScaledBackgroundPixmap = mBackgroundPixmap.scaled(mRect.size().toSize(), mBackgroundScaledMode, Qt::SmoothTransformation);
            painter->drawPixmap(mRect.topLeft()+QPointF(0, -1), mScaledBackgroundPixmap, QRectF(0, 0, mRect.width(), mRect.height()) & mScaledBackgroundPixmap.rect());
        } else {
            painter->drawPixmap(mRect.topLeft()+QPointF(0, -1), mBackgroundPixmap, QRectF(0, 0, mRect.width(), mRect.height()));
        }
    }
}

bool BoardLayout::addElement(QCPLayoutElement *element,
                             QPoint pos,
                             bool replot)
{
    int indexCol = int(mColumnCount *  pos.x() / mOuterRect.width()) ;
    int indexRow = int(mRowCount * pos.y() / mOuterRect.height()) ;

    bool status = addElement(element, replot, indexCol, indexRow);

    return status;
}

bool BoardLayout::setElementGeometry(int index, int col, int row, int width, int height)
{
    if(index >= 0 && index < mElementsGridRect.size()) {
        auto prevWidth = mElementsGridRect.at(index).width();
        auto prevHeight = mElementsGridRect.at(index).height();

        mElementsGridRect[index].setLeft(col);
        mElementsGridRect[index].setTop(row);

        if(width > 0)
            mElementsGridRect[index].setRight(col + width - 1);
        else
            mElementsGridRect[index].setRight(col + prevWidth - 1);

        if(height > 0)
            mElementsGridRect[index].setBottom(row + height - 1);
        else
            mElementsGridRect[index].setBottom(row + prevHeight - 1);

        updateElementMap();

        return true;
    }
    return false;
}

bool BoardLayout::addElement(QCPLayoutElement *element,
                             bool replot,
                             int col,
                             int row,
                             int width,
                             int height)
{
    if (element) {

        auto boardElement = qobject_cast<BoardElement *>(element);
        if(width < 0) {
            if(boardElement)
                width = boardElement->defaultWidth();
            else
                width = 1;
        }

        if(height < 0){
            if(boardElement)
                height = boardElement->defaultHeight();
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
            logger()->debug() << QString("Requested column/width (%1/%2) exceeds layout dimension (%3/%4)").arg(col).arg(width).arg(mColumnCount).arg(mRowCount);
            delete element;
            element = nullptr;
            return false;
        }

        if((row < 0) || (row + height > mRowCount)) {
            logger()->debug() << QString("Requested row/height (%1/%2) exceeds layout dimension (%3/%4)").arg(row).arg(height).arg(mColumnCount).arg(mRowCount);
            delete element;
            element = nullptr;
            return false;
        }

        if (element->layout()) // remove from old layout first
            element->layout()->take(element);

        mElements.append(element);
        mElementsGridRect.append(QRect(col,row,width, height));
        adoptElement(element);
        updateElementMap();

        if(replot) {
            mBoard->replot(QCustomPlot::rpQueuedReplot);
        }
        return true;
    }

    logger()->debug() << Q_FUNC_INFO << "Can't add null element";
    return false;
}

bool BoardLayout::addElement(QCPLayoutElement *element, QRect rect, bool replot)
{
    return addElement(element, replot, rect.x(), rect.y(), rect.width(), rect.height());
}

void BoardLayout::updateElementMap()
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
        auto gridRect = mElementsGridRect.at(i);
        for(int j=gridRect.x(); j<gridRect.x()+gridRect.width();j++)
            for(int k=gridRect.y(); k<gridRect.y()+gridRect.height();k++)
                mElementsMap[j][k] = mElements.at(i);
    }
}

int BoardLayout::rowCount() const
{
    return mRowCount;
}

void BoardLayout::setRowCount(int rowCount)
{
    if(rowCount > 0) {
        if(rowCount != mRowCount) {
            mRowCount = rowCount;
            simplify();
        }
    }
}

int BoardLayout::columnCount() const
{
    return mColumnCount;
}

void BoardLayout::setColumnCount(int columnCount)
{
    if(columnCount > 0) {
        if(columnCount != mColumnCount) {
            mColumnCount = columnCount;
            simplify();
        }
    }
}

int BoardLayout::columnSpacing() const
{
    return mColumnSpacing;
}

void BoardLayout::setColumnSpacing(int columnSpacing)
{
    if(columnSpacing >= 0)
        mColumnSpacing = columnSpacing;
}

int BoardLayout::rowSpacing() const
{
    return mRowSpacing;
}

void BoardLayout::setRowSpacing(int rowSpacing)
{
    if(rowSpacing >= 0)
        mRowSpacing = rowSpacing;
}

QSizeF BoardLayout::singleElementSize() const
{
    return mSingleElementSize;
}

void BoardLayout::updateSingleElementSize()
{
    mRowHeight = double(rect().height()) / double(mRowCount);
    mColumnWidth = double(rect().width()) / double(mColumnCount);
    mSingleElementSize.setWidth(mSingleElementColumnCount * mColumnWidth - mColumnSpacing);
    mSingleElementSize.setHeight(mSingleElementRowCount * mRowHeight - mRowSpacing);
}

double BoardLayout::columnWidth() const
{
    return mColumnWidth;
}

double BoardLayout::rowHeight() const
{
    return mRowHeight;
}

int BoardLayout::singleElementColumnCount() const
{
    return mSingleElementColumnCount;
}

void BoardLayout::setSingleElementColumnCount(int singleElementColumnCount)
{
    mSingleElementColumnCount = singleElementColumnCount;
}

int BoardLayout::singleElementRowCount() const
{
    return mSingleElementRowCount;
}

void BoardLayout::setSingleElementRowCount(int singleElementRowCount)
{
    mSingleElementRowCount = singleElementRowCount;
}
void BoardLayout::setBackground(const QPixmap &pm)
{
    mBackgroundPixmap = pm;
    mScaledBackgroundPixmap = QPixmap();
}

void BoardLayout::setBackground(const QBrush &brush)
{
    mBackgroundBrush = brush;
}

void BoardLayout::setBackground(const QPixmap &pm, bool scaled, Qt::AspectRatioMode mode)
{
    mBackgroundPixmap = pm;
    mScaledBackgroundPixmap = QPixmap();
    mBackgroundScaled = scaled;
    mBackgroundScaledMode = mode;
}

void BoardLayout::setBackgroundScaled(bool scaled)
{
    mBackgroundScaled = scaled;
}

void BoardLayout::setBackgroundScaledMode(Qt::AspectRatioMode mode)
{
    mBackgroundScaledMode = mode;
}
