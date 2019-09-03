#ifndef PARAMETERPICKERWIDGET_H
#define PARAMETERPICKERWIDGET_H

#include <QWidget>
#include "data/data_manager.h"

namespace Ui {
class ParameterPickerWidget;
}

class ParameterPickerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ParameterPickerWidget(QWidget *parent = nullptr);
    ~ParameterPickerWidget();

    void setDataManager(const QSharedPointer<QTBDataManager> &dataManager);

public slots:
    void updateParameterList();
private slots:
    void searchString(const QString& str);

private:
    Ui::ParameterPickerWidget *ui;
    QSharedPointer<QTBDataManager> mDataManager;
};

#endif // PARAMETERPICKERWIDGET_H
