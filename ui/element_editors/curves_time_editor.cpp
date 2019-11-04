#include "curves_time_editor.h"
#include "ui_curves_time_editor.h"

QTBCurvesTimeEditor::QTBCurvesTimeEditor(QTBPlotTime *display, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QTBCurvesTimeEditor),
    mDisplay(display)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->setCurrentRow(0);

    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &QTBCurvesTimeEditor::updateElement);

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
        ui->legendPositionComboBox->setCurrentIndex(mDisplay->legendPosition());
        ui->yticksCheckBox->setChecked(mDisplay->yAxisTicksVisible());
        ui->ylabelsCheckBox->setChecked(mDisplay->yAxisLabelsVisible());
        ui->ygridCheckBox->setChecked(mDisplay->yAxisGridVisible());
        ui->xticksCheckBox->setChecked(mDisplay->xAxisTicksVisible());
        ui->xlabelsCheckBox->setChecked(mDisplay->xAxisLabelsVisible());
        ui->xgridCheckBox->setChecked(mDisplay->xAxisGridVisible());
        ui->rangeComboBox->setCurrentIndex(mDisplay->yAxisScale());
        ui->rangeMinDoubleSpinBox->setValue(mDisplay->yAxisMinCustom());
        ui->rangeMaxDoubleSpinBox->setValue(mDisplay->yAxisMaxCustom());
        ui->legendVisibleCheckBox->setChecked(mDisplay->legendVisible());
        ui->historySpinBox->setValue(mDisplay->xAxisHistory());
        ui->directionComboBox->setCurrentIndex(mDisplay->xAxisDirection());
        ui->ythresholdsCheckBox->setChecked(mDisplay->thresholdsVisible());
        updateTabs();
    }
    setMinimumSize(600,400);
}

QTBCurvesTimeEditor::~QTBCurvesTimeEditor()
{
    delete ui;
}

void QTBCurvesTimeEditor::accept()
{
    updateElement();
    QDialog::accept();
}

void QTBCurvesTimeEditor::updateElement()
{
    if(mDisplay) {
        mDisplay->setTransparentBackground(ui->transparentCheckBox->isChecked());
        mDisplay->setLegendPosition(QTBPlotTime::LegendPosition(ui->legendPositionComboBox->currentIndex()));
        mDisplay->setYAxisGridVisible(ui->ygridCheckBox->isChecked());
        mDisplay->setYAxisTicksVisible(ui->yticksCheckBox->isChecked());
        mDisplay->setYAxisLabelsVisible(ui->ylabelsCheckBox->isChecked());
        mDisplay->setXAxisGridVisible(ui->xgridCheckBox->isChecked());
        mDisplay->setXAxisTicksVisible(ui->xticksCheckBox->isChecked());
        mDisplay->setXAxisLabelsVisible(ui->xlabelsCheckBox->isChecked());
        mDisplay->setLegendVisible(ui->legendVisibleCheckBox->isChecked());
        mDisplay->setXAxisHistory(ui->historySpinBox->value());

        mDisplay->setYAxisScale(QTBPlotTime::YAxisScale(ui->rangeComboBox->currentIndex()));
        mDisplay->setYAxisMinCustom(ui->rangeMinDoubleSpinBox->value());
        mDisplay->setYAxisMaxCustom(ui->rangeMaxDoubleSpinBox->value());
        mDisplay->setXAxisDirection(QTBPlotTime::XAxisDirection(ui->directionComboBox->currentIndex()));

        mDisplay->setThresholdsVisible(ui->ythresholdsCheckBox->isChecked());

        for(int i=0; i<mDisplay->parametersCount();i++) {
            QSharedPointer<QTBDashboardParameter> dashParam = mDisplay->dashParameter(i);
            if(dashParam) {
                if(!mPropertiesWidgets.at(i)->isConnected()) {
                    mPropertiesWidgets.at(i)->updateParameterSettings(dashParam->exclusiveParameterConfiguration());
                    dashParam->disconnectSharedConfiguration();
                } /*else {
                    dashParam->setConnected(true);
                }*/
            }
        }
    }
}

void QTBCurvesTimeEditor::updateTabs()
{
    int tabCount = ui->tabWidget->count();
    for(int i = 1; i< tabCount;i++) {
        ui->tabWidget->removeTab(1);
    }

    qDeleteAll(mPropertiesWidgets);
    mPropertiesWidgets.clear();

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
            propertiesWidget->setPropertiesMode(QTBParameterConfiguration::cmGraph);
            propertiesWidget->setVisible(true);
            ui->tabWidget->addTab(propertiesWidget, dashParam->getDisplayedLabel());

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

void QTBCurvesTimeEditor::newParameter()
{
    QString paramLabel = QInputDialog::getText(this, "Parameter Label", "Parameter Label");
    if(!paramLabel.isEmpty()) {
        mDisplay->addParameter(paramLabel);
        updateTabs();
    }
}

void QTBCurvesTimeEditor::removeParameter()
{
    if(ui->tabWidget->currentIndex() > 0 ) {
        mDisplay->removeDashParameter(ui->tabWidget->currentIndex() - 1 );
    }
    updateTabs();
}

void QTBCurvesTimeEditor::on_listWidget_currentRowChanged(int currentRow)
{
    ui->stackedWidget->setCurrentIndex(currentRow);
}

void QTBCurvesTimeEditor::on_rangeComboBox_currentIndexChanged(int index)
{
    if(index == 2)
        ui->scaleWidget->setEnabled(true);
    else
        ui->scaleWidget->setEnabled(false);
}
