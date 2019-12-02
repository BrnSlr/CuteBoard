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
    void changeListDisplay();
    void searchString(const QString& str);
    void searchAndHide(const QString& str, QTreeWidgetItem *item);

private:
    Ui::ParameterPickerWidget *ui;
    QSharedPointer<QTBDataManager> mDataManager;
    bool mTreeMode;
};

#endif // PARAMETERPICKERWIDGET_H
