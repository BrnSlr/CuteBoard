#ifndef LAYOUT_LIST_H
#define LAYOUT_LIST_H

#include "dashboard/dashboard.h"

class QTBLayoutList : public QCPLayout
{
    Q_OBJECT
public:
    QTBLayoutList();
    ~QTBLayoutList() Q_DECL_OVERRIDE;

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

    friend class QTBAlarmPanel;
};

#endif // LAYOUT_LIST_H
