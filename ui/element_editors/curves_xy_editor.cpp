#include "curves_xy_editor.h"
#include "ui_curves_xy_editor.h"

QTBCurvesXYEditor::QTBCurvesXYEditor(QTBPlotXY *display, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QTBCurvesXYEditor),
    mDisplay(display),
    mXPropertiesWidget(nullptr)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->setCurrentRow(0);

    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &QTBCurvesXYEditor::updateElement);

    QWidget *cornerWidget = new QWidget(this);
    mNewParamButton = new QToolButton(cornerWidget);
    mNewParamButton->setCursor(Qt::ArrowCursor);
    mNewParamButton->setAutoRaise(true);
    mNewParamButton->setIconSize(QSize(25, 25));
    mNewParamButton->setIcon(QIcon(QPixmap(":/icons8_plus_math_32px_1.png")));
    QObject::connect(mNewParamButton, SIGNAL(clicked()), this, SLOT(newParameter()));
    mNewParamButton->setToolTip(tr("Add parameter"));

    mRemoveParamButton = new QToolButton(cornerWidget);
    mRemoveParamButton->setCursor(Qt::ArrowCursor);
    mRemoveParamButton->setAutoRaise(true);
    mRemoveParamButton->setIconSize(QSize(25, 25));
    mRemoveParamButton->setIcon(QIcon(QPixmap(":/icons8_minus_math_32px.png")));
    QObject::connect(mRemoveParamButton, SIGNAL(clicked()), this, SLOT(removeParameter()));
    mRemoveParamButton->setToolTip(tr("Remove parameter"));
    mRemoveParamButton->setVisible(false);

    QHBoxLayout *lay = new QHBoxLayout(cornerWidget);
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget(mRemoveParamButton);
    lay->addWidget(mNewParamButton);

    cornerWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    ui->tabWidget->setCornerWidget(cornerWidget, Qt::TopRightCorner);

    connect(ui->tabWidget, &QTabWidget::currentChanged,
            [this]() {
        if(ui->tabWidget->currentIndex() > 0)
            mRemoveParamButton->setVisible(true);
        else
            mRemoveParamButton->setVisible(false);
    });

    if(mDisplay) {
        ui->transparentCheckBox->setChecked(mDisplay->transparentBackground());
        ui->legendPositionComboBox->setCurrentIndex(mDisplay->yLegendPosition());
        ui->yticksCheckBox->setChecked(mDisplay->yAxisTicksVisible());
        ui->ylabelsCheckBox->setChecked(mDisplay->yAxisLabelsVisible());
        ui->ygridCheckBox->setChecked(mDisplay->yAxisGridVisible());
        ui->xticksCheckBox->setChecked(mDisplay->xAxisTicksVisible());
        ui->xlabelsCheckBox->setChecked(mDisplay->xAxisLabelsVisible());
        ui->xgridCheckBox->setChecked(mDisplay->xAxisGridVisible());

        ui->rangeComboBox->setCurrentIndex(mDisplay->yAxisScale());
        ui->rangeMinDoubleSpinBox->setValue(mDisplay->yAxisMinCustom());
        ui->rangeMaxDoubleSpinBox->setValue(mDisplay->yAxisMaxCustom());
        ui->legendVisibleCheckBox->setChecked(mDisplay->yLegendVisible());
        ui->xrangeComboBox->setCurrentIndex(mDisplay->xAxisScale());
        ui->xrangeMinDoubleSpinBox->setValue(mDisplay->xAxisMinCustom());
        ui->xrangeMaxDoubleSpinBox->setValue(mDisplay->xAxisMaxCustom());
        ui->xLegendVisibleCheckBox->setChecked(mDisplay->xLegendVisible());
        ui->xLegendPositionComboBox->setCurrentIndex(mDisplay->xLegendPosition());

        ui->historySpinBox->setValue(mDisplay->xAxisHistory());
        ui->ythresholdsCheckBox->setChecked(mDisplay->yThresholdsVisible());
        ui->xthresholdsCheckBox->setChecked(mDisplay->xThresholdsVisible());

        updateTabs();
    }
    setMinimumSize(600,400);
}

QTBCurvesXYEditor::~QTBCurvesXYEditor()
{
    delete ui;
}

void QTBCurvesXYEditor::accept()
{
    updateElement();
    QDialog::accept();
}

void QTBCurvesXYEditor::updateElement()
{
    if(mDisplay) {
        mDisplay->setTransparentBackground(ui->transparentCheckBox->isChecked());

        mDisplay->setYLegendPosition(QTBPlotXY::YLegendPosition(ui->legendPositionComboBox->currentIndex()));
        mDisplay->setXLegendPosition(QTBPlotXY::XLegendPosition(ui->xLegendPositionComboBox->currentIndex()));

        mDisplay->setYAxisGridVisible(ui->ygridCheckBox->isChecked());
        mDisplay->setYAxisTicksVisible(ui->yticksCheckBox->isChecked());
        mDisplay->setYAxisLabelsVisible(ui->ylabelsCheckBox->isChecked());
        mDisplay->setXAxisGridVisible(ui->xgridCheckBox->isChecked());
        mDisplay->setXAxisTicksVisible(ui->xticksCheckBox->isChecked());
        mDisplay->setXAxisLabelsVisible(ui->xlabelsCheckBox->isChecked());

        mDisplay->setYLegendVisible(ui->legendVisibleCheckBox->isChecked());
        mDisplay->setXLegendVisible(ui->xLegendVisibleCheckBox->isChecked());

        mDisplay->setXAxisHistory(ui->historySpinBox->value());

        mDisplay->setYAxisScale(QTBPlotXY::AxisScale(ui->rangeComboBox->currentIndex()));
        mDisplay->setYAxisMinCustom(ui->rangeMinDoubleSpinBox->value());
        mDisplay->setYAxisMaxCustom(ui->rangeMaxDoubleSpinBox->value());
        mDisplay->setXAxisScale(QTBPlotXY::AxisScale(ui->xrangeComboBox->currentIndex()));
        mDisplay->setXAxisMinCustom(ui->xrangeMinDoubleSpinBox->value());
        mDisplay->setXAxisMaxCustom(ui->xrangeMaxDoubleSpinBox->value());

        mDisplay->setYThresholdsVisible(ui->ythresholdsCheckBox->isChecked());
        mDisplay->setXThresholdsVisible(ui->xthresholdsCheckBox->isChecked());

        for(int i=0; i<mDisplay->parametersCount();i++) {
            QSharedPointer<QTBDashboardParameter> dashParam = mDisplay->dashParameter(i);
            if(dashParam) {
                if(!mPropertiesWidgets.at(i)->isConnected()) {
                    mPropertiesWidgets.at(i)->updateParameterSettings(dashParam->exclusiveParameterConfiguration());
                    dashParam->disconnectProperties();
                }/* else {
                    dashParam->setConnected(true);
                }*/
            }
        }

        if(mDisplay->xParameter()) {
            if(!mXPropertiesWidget->isConnected()) {
                mXPropertiesWidget->updateParameterSettings(mDisplay->xParameter()->exclusiveParameterConfiguration());
                mDisplay->xParameter()->disconnectProperties();
            } /*else {
                mDisplay->xParameter()->setConnected(true);
            }*/
        }
    }
}

void QTBCurvesXYEditor::updateTabs()
{
    int tabCount = ui->tabWidget->count();
    for(int i = 1; i< tabCount;i++) {
        ui->tabWidget->removeTab(1);
    }

    qDeleteAll(mPropertiesWidgets);
    mPropertiesWidgets.clear();

    if(mXPropertiesWidget) {
        delete mXPropertiesWidget;
        mXPropertiesWidget = nullptr;
    }

    if(mDisplay->xParameter()) {
        mXPropertiesWidget = new PropertiesWidget(this);
        mXPropertiesWidget->setVisible(false);

        if(mDisplay->xParameter()->connected()) {
            mXPropertiesWidget->setEditionMode(PropertiesWidget::emElementConnected);
            mXPropertiesWidget->updateUi(mDisplay->xParameter()->sharedParameterConfiguration());
        } else {
            if(mDisplay->xParameter()->sharedParameterConfiguration())
                mXPropertiesWidget->setEditionMode(PropertiesWidget::emElementDisconnected);
            else
                mXPropertiesWidget->setEditionMode(PropertiesWidget::emElementStandAlone);
            mXPropertiesWidget->updateUi(mDisplay->xParameter()->exclusiveParameterConfiguration());
        }
        mXPropertiesWidget->setPropertiesMode(QTBParameterConfiguration::cmCurve);
        mXPropertiesWidget->setVisible(true);
        ui->tabWidget->addTab(mXPropertiesWidget, QString("X : %1").arg(mDisplay->xParameter()->getDisplayedLabel()));

        connect(mXPropertiesWidget, &PropertiesWidget::connectProperties,
                [=](bool connected){
            if(connected) {
                mXPropertiesWidget->setEditionMode(PropertiesWidget::emElementConnected);
                mXPropertiesWidget->updateUi(mDisplay->xParameter()->sharedParameterConfiguration());
            } else {
                mXPropertiesWidget->setEditionMode(PropertiesWidget::emElementDisconnected);
                mXPropertiesWidget->updateUi(mDisplay->xParameter()->exclusiveParameterConfiguration());
            }
        });
    } else {
        ui->tabWidget->addTab(new QWidget(), QString("X : "));
    }

    for(int i=0; i<mDisplay->parametersCount();i++) {

        QSharedPointer<QTBDashboardParameter> dashParam = mDisplay->dashParameter(i);
        if(dashParam) {

            PropertiesWidget *propertiesWidget = new PropertiesWidget(this);
            propertiesWidget->setVisible(false);

            if(dashParam->connected()) {
                propertiesWidget->setEditionMode(PropertiesWidget::emElementConnected);
                propertiesWidget->updateUi(dashParam->sharedParameterConfiguration());
            } else {
                if(dashParam->sharedParameterConfiguration())
                    propertiesWidget->setEditionMode(PropertiesWidget::emElementDisconnected);
                else
                    propertiesWidget->setEditionMode(PropertiesWidget::emElementStandAlone);
                propertiesWidget->updateUi(dashParam->exclusiveParameterConfiguration());
            }
            propertiesWidget->setPropertiesMode(QTBParameterConfiguration::cmCurve);
            propertiesWidget->setVisible(true);
            ui->tabWidget->addTab(propertiesWidget, QString("Y : %1").arg(dashParam->getDisplayedLabel()));

            connect(propertiesWidget, &PropertiesWidget::connectProperties,
                    [propertiesWidget, dashParam](bool connected){
                if(connected) {
                    propertiesWidget->setEditionMode(PropertiesWidget::emElementConnected);
                    propertiesWidget->updateUi(dashParam->sharedParameterConfiguration());
                } else {
                    propertiesWidget->setEditionMode(PropertiesWidget::emElementDisconnected);
                    propertiesWidget->updateUi(dashParam->exclusiveParameterConfiguration());
                }
            });

            mPropertiesWidgets.append(propertiesWidget);
        }
    }
}

void QTBCurvesXYEditor::newParameter()
{
    if(ui->tabWidget->currentIndex() == 1) {
        QString paramLabel = QInputDialog::getText(this, "X Axis : Parameter Label", "X Axis : Parameter Label");
        if(!paramLabel.isEmpty()) {
            mDisplay->addXParameter(paramLabel);
            updateTabs();
        }
    } else {
        QString paramLabel = QInputDialog::getText(this, "Y Axis : Parameter Label", "Y Axis : Parameter Label");
        if(!paramLabel.isEmpty()) {
            mDisplay->addYParameter(paramLabel);
            updateTabs();
        }
    }
}

void QTBCurvesXYEditor::removeParameter()
{
    if(ui->tabWidget->currentIndex() == 1 ) {
        mDisplay->removeXParameter();
    } else if(ui->tabWidget->currentIndex() > 1 ) {
        mDisplay->removeYParameter(ui->tabWidget->currentIndex() - 2 );
    }
    updateTabs();
}

void QTBCurvesXYEditor::on_listWidget_currentRowChanged(int currentRow)
{
    ui->stackedWidget->setCurrentIndex(currentRow);
}

void QTBCurvesXYEditor::on_rangeComboBox_currentIndexChanged(int index)
{
    if(index == 2)
        ui->scaleWidget->setEnabled(true);
    else
        ui->scaleWidget->setEnabled(false);
}

void QTBCurvesXYEditor::on_xrangeComboBox_currentIndexChanged(int index)
{
    if(index == 2)
        ui->scaleWidget_2->setEnabled(true);
    else
        ui->scaleWidget_2->setEnabled(false);
}
