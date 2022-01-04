#ifndef BOARDLAYOUT_H
#define BOARDLAYOUT_H

#include "board.h"
#include "utils/selection_rect.h"
#include "utils/resize_handle.h"

class BOARD_UI_EXPORT BoardLayout : public QCPLayout
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
    Q_PROPERTY(int rowCount READ rowCount)
    Q_PROPERTY(int columnCount READ columnCount)
    Q_PROPERTY(int columnSpacing READ columnSpacing WRITE setColumnSpacing)
    Q_PROPERTY(int rowSpacing READ rowSpacing WRITE setRowSpacing)
public:

    BoardLayout(Board *dashboard, int nbRows = 24, int nbCols = 24);
    ~BoardLayout() Q_DECL_OVERRIDE;

    void clearLayout();

    // reimplemented virtual methods
    void update(UpdatePhase phase) Q_DECL_OVERRIDE;
    void simplify() Q_DECL_OVERRIDE;
    void updateLayout() Q_DECL_OVERRIDE;
    int elementCount() const Q_DECL_OVERRIDE;
    int elementIndex(int col, int row) const;
    QCPLayoutElement *elementAt(int index) const Q_DECL_OVERRIDE;
    QCPLayoutElement *elementAt(const QPointF &pos) const;
    QCPLayoutElement *elementAt( int col, int row) const;
    QCPLayoutElement *elementAt( int col, int row, int width, int height) const;
    int colIndex(qreal x);
    int rowIndex(qreal y);
    QRect elementGridRectAt(int index) const;
    int elementRowAt(int index) const;
    int elementColumnAt(int index) const;
    int elementWidthAt(int index) const;
    int elementHeightAt(int index) const;
    int elementIndex(QCPLayoutElement* element) const;
    QCPLayoutElement *takeAt(int index) Q_DECL_OVERRIDE;
    bool take(QCPLayoutElement* element) Q_DECL_OVERRIDE;
    QList<QCPLayoutElement *> elements(bool recursive) const Q_DECL_OVERRIDE;
    double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=nullptr) const Q_DECL_OVERRIDE;

    void saveElementRectSettings(int index, QSettings *settings);
    void saveElementRectSettings(QCPLayoutElement *element, QSettings *settings);
    QRect loadElementRectSettings(QSettings *settings);

    // non-virtual methods:
    bool addElement(QCPLayoutElement *element, bool replot, int col, int row, int width = -1, int height = -1);
    bool addElement(QCPLayoutElement *element, QRect rect, bool replot = false);
    bool addElement(QCPLayoutElement *element, QPoint pos, bool replot = false);

    bool setElementGeometry(int index, int col, int row, int width = -1, int height = -1);

    void updateElementMap();

    // getters:
    int rowCount() const;
    int columnCount() const;
    int singleElementRowCount() const;
    int singleElementColumnCount() const;
    int columnSpacing() const;
    int rowSpacing() const;
    double rowHeight() const;
    double columnWidth() const;
    QSizeF singleElementSize() const;
    void updateSingleElementSize();

    // setters:
    void setRowCount(int rowCount);
    void setColumnCount(int columnCount);
    void setColumnSpacing(int columnSpacing);
    void setRowSpacing(int rowSpacing);
    void setSingleElementRowCount(int singleElementRowCount);
    void setSingleElementColumnCount(int singleElementColumnCount);

    QPixmap background() const { return mBackgroundPixmap; }
    QBrush backgroundBrush() const { return mBackgroundBrush; }
    QPen backgroundPen() const;
    bool backgroundScaled() const { return mBackgroundScaled; }
    Qt::AspectRatioMode backgroundScaledMode() const { return mBackgroundScaledMode; }

    void setBackground(const QPixmap &pm);
    void setBackground(const QPixmap &pm, bool scaled, Qt::AspectRatioMode mode=Qt::KeepAspectRatioByExpanding);
    void setBackground(const QBrush &brush);
    void setBackgroundScaled(bool scaled);
    void setBackgroundScaledMode(Qt::AspectRatioMode mode);
    void setBackgroundPen(const QPen &backgroundPen);

public slots:

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

    QList<QCPLayoutElement *> mElements;
    QList<QList<QCPLayoutElement*>> mElementsMap;
    QList<QRect> mElementsGridRect;

    QBrush mBackgroundBrush;
    QPen mBackgroundPen;
    QPixmap mBackgroundPixmap;
    QPixmap mScaledBackgroundPixmap;
    bool mBackgroundScaled;
    Qt::AspectRatioMode mBackgroundScaledMode;

    Board *mBoard;

    virtual void draw(QCPPainter *painter) Q_DECL_OVERRIDE;
    void drawBackground(QCPPainter *painter);

private:
    Q_DISABLE_COPY(BoardLayout)
};

#endif // BOARDLAYOUT_H
