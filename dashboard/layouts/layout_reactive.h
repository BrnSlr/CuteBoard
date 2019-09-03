#ifndef GRIDLAYOUT_H
#define GRIDLAYOUT_H

#include "dashboard/dashboard.h"
#include "dashboard/elements_base/elementhighlightedrect.h"
#include "dashboard/elements_base/elementresizehandle.h"

class QTBLayoutReactiveElement;
class QTBLayoutReactive : public QCPLayout
{
    Q_OBJECT
    Q_PROPERTY(int rowCount READ rowCount)
    Q_PROPERTY(int columnCount READ columnCount)
    Q_PROPERTY(int columnSpacing READ columnSpacing WRITE setColumnSpacing)
    Q_PROPERTY(int rowSpacing READ rowSpacing WRITE setRowSpacing)
public:
    enum DragAction {
        daMoveElement,
        daResizeElement};
    Q_ENUMS(DragAction)

    QTBLayoutReactive(QTBoard *dashboard, int nbRows = 24, int nbCols = 24);
    ~QTBLayoutReactive() Q_DECL_OVERRIDE;

    void clearLayout();

    // reimplemented virtual methods:
    void simplify() Q_DECL_OVERRIDE;
    void updateLayout() Q_DECL_OVERRIDE;
    int elementCount() const Q_DECL_OVERRIDE;
    QCPLayoutElement *elementAt(int index) const Q_DECL_OVERRIDE;
    QCPLayoutElement *elementAt(const QPointF &pos) const;
    QCPLayoutElement *elementAt( int col, int row) const;
    QCPLayoutElement *elementAt( int col, int row, int width, int height) const;
    int elementRowAt(int index) const;
    int elementColumnAt(int index) const;
    int elementWidthAt(int index) const;
    int elementHeightAt(int index) const;
    QCPLayoutElement *takeAt(int index) Q_DECL_OVERRIDE;
    bool take(QCPLayoutElement* element) Q_DECL_OVERRIDE;
    QList<QCPLayoutElement *> elements(bool recursive) const Q_DECL_OVERRIDE;
    double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=nullptr) const Q_DECL_OVERRIDE;

    void saveElementRectSettings(int index, QSettings *settings);
    QRect loadElementRectSettings(QSettings *settings);

    void draggedElement(QDragMoveEvent *event);
    void droppedElement(QDropEvent *event);

    // non-virtual methods:
    void addElement(QCPLayoutElement *element, int col, int row, int width = -1, int height = -1);
    void addElement(QCPLayoutElement *element, QRect rect);
    void updateElementMap();

    // getters:
    int rowCount() const;
    int columnCount() const;
    int singleElementRowCount() const;
    int singleElementColumnCount() const;
    int columnSpacing() const;
    int rowSpacing() const;
    bool locked() const;
    double rowHeight() const;
    double columnWidth() const;
    QSizeF singleElementSize() const;

    // setters:
    void setRowCount(int rowCount);
    void setColumnCount(int columnCount);
    void setColumnSpacing(int columnSpacing);
    void setRowSpacing(int rowSpacing);
    void setLocked(bool locked);
    void setSingleElementRowCount(int singleElementRowCount);
    void setSingleElementColumnCount(int singleElementColumnCount);
    void setElementResizable(int index, bool resizable);
    void setElementDraggable(int index, bool draggable);
    void setElementEditable(int index, bool editable);
    void setElementClosable(int index, bool draggable);

public slots:
    void mousePress(QMouseEvent *event);
    void mouseMove(QMouseEvent *event);
    void mouseRelease(QMouseEvent *event);
    void mouseDoubleClick(QMouseEvent *event);

signals:
    void pageModified();

protected:
    int mRowCount;
    int mColumnCount;
    double mRowHeight;
    double mColumnWidth;
    int mRowSpacing;
    int mColumnSpacing;

    int mSingleElementRowCount;
    int mSingleElementColumnCount;
    QSizeF mSingleElementSize;

    bool mLocked;
    bool mDragging;
    DragAction mDragAction;
    int mDraggedElementIndex;
    int mDraggedElementInitialColumn;
    int mDraggedElementInitialRow;
    int mDraggedElementInitialWidth;
    int mDraggedElementInitialHeight;
    QRectF mDraggedElementInitalRect;
    int mDraggedElementNewColumn;
    int mDraggedElementNewRow;
    int mDraggedElementNewWidth;
    int mDraggedElementNewHeight;
    QPoint mDragStartPoint;
    QTBElementHighlightedRect *mHighlightRect;

    QMenu *mRightClickMenu;
    QAction *mEditAction;
    QAction *mCloseAction;

    QList<QCPLayoutElement *> mElements;
    QList<QList<QCPLayoutElement*>> mElementsMap;
    QList<QTBElementResizeHandle *> mResizeHandles;
    QList<int> mElementsColumn;
    QList<int> mElementsRow;
    QList<int> mElementsWidth;
    QList<int> mElementsHeight;
    QList<bool> mElementsResizable;
    QList<bool> mElementsDraggable;
    QList<bool> mElementsEditable;
    QList<bool> mElementsClosable;

    QTBoard *mBoard;

private:
    Q_DISABLE_COPY(QTBLayoutReactive)
};
Q_DECLARE_METATYPE(QTBLayoutReactive::DragAction)

#endif // GRIDLAYOUT_H
