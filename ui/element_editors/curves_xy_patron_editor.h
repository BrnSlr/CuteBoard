#ifndef CURVES_XY_PATRON_EDITOR_H
#define CURVES_XY_PATRON_EDITOR_H

#include <QWidget>
#include <QColorDialog>
#include "project/curve_patron_configuration.h"

namespace Ui {
class CurvesXYPatronEditor;
}

class CurvesXYPatronEditor : public QWidget
{
    Q_OBJECT

public:
    explicit CurvesXYPatronEditor(QWidget *parent = nullptr);
    ~CurvesXYPatronEditor();

    bool checkName();
    void loadPatron(QSharedPointer<QTBCurvePatronConfiguration> patron);
    QSharedPointer<QTBCurvePatronConfiguration> createPatron();
    void updatePatron();

private slots:
    void on_importPushButton_clicked();
    void on_exportPushButton_clicked();

    void on_colorPushButton_clicked();

private:
    Ui::CurvesXYPatronEditor *ui;
    QSharedPointer<QTBCurvePatronConfiguration> mPatron;
};

#endif // CURVES_XY_PATRON_EDITOR_H
