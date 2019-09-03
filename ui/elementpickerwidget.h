#ifndef ELEMENTPICKERWIDGET_H
#define ELEMENTPICKERWIDGET_H

#include <QWidget>
#include "dashboard/elements_factory/elementfactory.h"

namespace Ui {
class ElementPickerWidget;
}

class ElementPickerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ElementPickerWidget(QWidget *parent = nullptr);
    ~ElementPickerWidget();

private:
    Ui::ElementPickerWidget *ui;
};

#endif
