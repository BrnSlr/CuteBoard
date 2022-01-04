#ifndef TABLEVALUESEDITOR_H
#define TABLEVALUESEDITOR_H

#include <QDialog>
#include "table_values.h"

namespace Ui {
class TableValuesEditor;
}

class TABLEVALUES_EXPORT TableValuesEditor : public QDialog
{
    Q_OBJECT

public:
    explicit TableValuesEditor(TableValues *display, QWidget *parent = nullptr);
    ~TableValuesEditor() Q_DECL_OVERRIDE;

    void accept() Q_DECL_OVERRIDE;

private slots:
    void updateElement();

private:
    Ui::TableValuesEditor *ui;
    TableValues *mElement;
};

#endif // TABLEVALUESEDITOR_H
