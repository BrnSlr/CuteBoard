#ifndef BITFIELDSTABLEEDITOR_H
#define BITFIELDSTABLEEDITOR_H

#include <QDialog>
#include "dashboard/elements/value_bitfields.h"
#include "ui/propertieswidget.h"

namespace Ui {
class QTBValueBitfieldsEditor;
}

class QTBValueBitfieldsEditor : public QDialog
{
    Q_OBJECT

public:
    explicit QTBValueBitfieldsEditor(QTBValueBitfields *display, QWidget *parent = nullptr);
    ~QTBValueBitfieldsEditor() Q_DECL_OVERRIDE;

    void accept() Q_DECL_OVERRIDE;

private slots:
    void updateElement();
    void connectProperties(bool connected);
    void newParameter();
    void removeParameter();

private:
    Ui::QTBValueBitfieldsEditor *ui;
    QTBValueBitfields *mBitfieldsTable;
    PropertiesWidget *mPropertiesWidget;
    QToolButton *mNewParamButton;
    QToolButton *mRemoveParamButton;
};

#endif // BITFIELDSTABLEEDITOR_H
