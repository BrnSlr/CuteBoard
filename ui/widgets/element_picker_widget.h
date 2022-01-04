#ifndef ELEMENTPICKERWIDGET_H
#define ELEMENTPICKERWIDGET_H

#include <QWidget>
#include "ui/board/factory/element_factory_helper.h"

namespace Ui {
class ElementPickerWidget;
}

class ElementPickerWidget : public QWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    explicit ElementPickerWidget( QWidget *parent = nullptr,
                                  BoardElement::ElementType type = BoardElement::etUnknown,
                                  BoardElement::TimeSeriesType tsType = BoardElement::tstUnknown,
                                  BoardElement::TimeSeriesSize tsSize = BoardElement::tssUnknown);
    ~ElementPickerWidget();

    QString selectedElement();

signals:
    void elementDoubleClicked();

private:
    Ui::ElementPickerWidget *ui;
};

#endif
