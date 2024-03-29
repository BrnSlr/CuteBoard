#ifndef LAYOUT_LIST_H
#define LAYOUT_LIST_H

#include "ui/board/board.h"

class BOARD_UI_EXPORT LayoutList : public QCPLayout
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    LayoutList();
    ~LayoutList() Q_DECL_OVERRIDE;

    void clearLayout();

    // reimplemented virtual methods:
    void simplify() Q_DECL_OVERRIDE;
    QCPLayoutElement *takeAt(int index) Q_DECL_OVERRIDE;
    bool take(QCPLayoutElement* element) Q_DECL_OVERRIDE;
    void updateLayout() Q_DECL_OVERRIDE;
    int elementCount() const Q_DECL_OVERRIDE;
    QCPLayoutElement *elementAt(int index) const Q_DECL_OVERRIDE;

    void addElement(QCPLayoutElement *element);
    void takeAllElements();

    double rowHeight() const;
    int rowSpacing() const;

    void setRowHeight(double rowHeight);
    void setRowSpacing(int rowSpacing);

protected:
    double mRowHeight;
    int mRowSpacing;

    QList<QCPLayoutElement *> mElements;

    friend class AlarmPanel;
};

#endif // LAYOUT_LIST_H
