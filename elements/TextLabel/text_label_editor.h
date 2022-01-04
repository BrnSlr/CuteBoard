#ifndef TEXTLABEL_EDITOR_H
#define TEXTLABEL_EDITOR_H

#include <QDialog>
#include "text_label.h"

namespace Ui {
class TextLabelEditor;
}

class TEXTLABEL_EXPORT TextLabelEditor : public QDialog
{
    Q_OBJECT

public:
    explicit TextLabelEditor(TextLabel *display, QWidget *parent = nullptr);
    ~TextLabelEditor() Q_DECL_OVERRIDE;

    void accept() Q_DECL_OVERRIDE;

private slots:
    void updateElement();

    void on_colorToolButton_clicked();

    void on_colorModeComboBox_currentIndexChanged(int index);

private:
    Ui::TextLabelEditor *ui;
    TextLabel *mElement;
};

#endif // TEXTLABEL_EDITOR_H
