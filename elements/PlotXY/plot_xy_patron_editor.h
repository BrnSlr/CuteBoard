#ifndef CURVES_XY_PATRON_EDITOR_H
#define CURVES_XY_PATRON_EDITOR_H

#include <QWidget>
#include <QColorDialog>
#include "ui/board/management/curve_patron.h"
#include "plot_xy_shared.h"

namespace Ui {
class PLOT_XY_EXPORT CurvesXYPatronEditor;
}

class PLOT_XY_EXPORT CurvesXYPatronEditor : public QWidget
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER

public:
    explicit CurvesXYPatronEditor(QWidget *parent = nullptr);
    ~CurvesXYPatronEditor();

    bool checkName();
    void loadPatron(QSharedPointer<CurvePatron> patron);
    QSharedPointer<CurvePatron> createPatron();
    void updatePatron();

private slots:
    void on_importPushButton_clicked();
    void on_exportPushButton_clicked();

    void on_colorPushButton_clicked();

private:
    Ui::CurvesXYPatronEditor *ui;
    QSharedPointer<CurvePatron> mPatron;
};

#endif // CURVES_XY_PATRON_EDITOR_H
