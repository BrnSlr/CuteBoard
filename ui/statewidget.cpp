#include "statewidget.h"
#include "ui_statewidget.h"

StateWidget::StateWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StateWidget)
{
    ui->setupUi(this);
}

StateWidget::~StateWidget()
{
    delete ui;
}
