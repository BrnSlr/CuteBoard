#include "plot_time_editor.h"
#include "ui_plot_time_editor.h"
#include "ui/widgets/utils/icon_factory.h"
#include "ui/widgets/add_parameter_widget.h"

PlotTimeEditor::PlotTimeEditor(PlotTime *display, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotTimeEditor),
    mDisplay(display)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->setCurrentRow(0);

    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &PlotTimeEditor::updateElement);

    QWidget *cornerWidget = new QWidget(this);
    mNewParamButton = new QToolButton(cornerWidget);
    mNewParamButton->setCursor(Qt::ArrowCursor);
    mNewParamButton->setAutoRaise(true);
    mNewParamButton->setIconSize(QSize(20, 20));
    mNewParamButton->setMinimumSize(QSize(30, 30));
    mNewParamButton->setIcon(create_icon(":/icons8_plus_math_filled_50px.png"));
    QObject::connect(mNewParamButton, SIGNAL(clicked()), this, SLOT(newParameter()));
    mNewParamButton->setToolTip(tr("Add parameter"));

    mRemoveParamButton = new QToolButton(cornerWidget);
    mRemoveParamButton->setCursor(Qt::ArrowCursor);
    mRemoveParamButton->setAutoRaise(true);
    mRemoveParamButton->setIconSize(QSize(20, 20));
    mRemoveParamButton->setMinimumSize(QSize(30, 30));
    mRemoveParamButton->setIcon(create_icon(":/icons8_minus_math_filled_50px.png"));
    QObject::connect(mRemoveParamButton, SIGNAL(clicked()), this, SLOT(removeParameter()));
    mRemoveParamButton->setToolTip(tr("Remove parameter"));
    mRemoveParamButton->setVisible(false);

    mReplaceParamButton = new QToolButton(cornerWidget);
    mReplaceParamButton->setCursor(Qt::ArrowCursor);
    mReplaceParamButton->setAutoRaise(true);
    mReplaceParamButton->setIconSize(QSize(20, 20));
    mReplaceParamButton->setMinimumSize(QSize(30, 30));
    mReplaceParamButton->setIcon(create_icon(":/icons8_replace_filled_50px.png"));
    QObject::connect(mReplaceParamButton, SIGNAL(clicked()), this, SLOT(replaceParameter()));
    mReplaceParamButton->setToolTip(tr("Replace parameter"));
    mReplaceParamButton->setVisible(false);

    QHBoxLayout *lay = new QHBoxLayout(cornerWidget);
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget(mReplaceParamButton);
    lay->addWidget(mRemoveParamButton);
    lay->addWidget(mNewParamButton);

    cornerWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    ui->tabWidget->setCornerWidget(cornerWidget, Qt::TopRightCorner);

    connect(ui->tabWidget, &QTabWidget::currentChanged,
            [this]() {
        if(ui->tabWidget->currentIndex() > 0) {
            mRemoveParamButton->setVisible(true);
            mReplaceParamButton->setVisible(true);
        } else {
            mRemoveParamButton->setVisible(false);
            mReplaceParamButton->setVisible(false);
        }
    });

    if(mDisplay) {
        ui->backgroundComboBox->setCurrentIndex(mDisplay->backgroundMode());
        ui->legendPositionComboBox->setCurrentIndex(mDisplay->legendPosition());
        ui->yticksCheckBox->setChecked(mDisplay->yAxisTicksVisible());
        ui->ylineCheckBox->setChecked(mDisplay->yAxisLineVisible());
        ui->ylabelsCheckBox->setChecked(mDisplay->yAxisLabelsVisible());
        ui->ygridCheckBox->setChecked(mDisplay->yAxisGridVisible());
        ui->xticksCheckBox->setChecked(mDisplay->xAxisTicksVisible());
        ui->xlineCheckBox->setChecked(mDisplay->xAxisLineVisible());
        ui->xlabelsCheckBox->setChecked(mDisplay->xAxisLabelsVisible());
        ui->xgridCheckBox->setChecked(mDisplay->xAxisGridVisible());
        ui->xfixedtickerCheckBox->setChecked(mDisplay->xAxisFixedTicker());
        ui->rangeComboBox->setCurrentIndex(mDisplay->yAxisScale());
        ui->rangeMinDoubleSpinBox->setValue(mDisplay->yAxisMinCustom());
        ui->rangeMaxDoubleSpinBox->setValue(mDisplay->yAxisMaxCustom());
        ui->legendVisibleCheckBox->setChecked(mDisplay->legendVisible());
        ui->historyDoubleSpinBox->setValue(mDisplay->xAxisHistory());
        ui->directionComboBox->setCurrentIndex(mDisplay->xAxisDirection());
        ui->ythresholdsCheckBox->setChecked(mDisplay->thresholdsVisible());
        ui->statComboBox->setCurrentIndex(mDisplay->statMode());
        ui->verticalTicksCountSpinBox->setValue(mDisplay->yAxisTicksCount());
        ui->horizontalTicksCountSpinBox->setValue(mDisplay->xAxisTicksCount());
        ui->ticksStepMinValueCheckBox->setChecked(mDisplay->tickStepMinValueEnabled());
        ui->ticksStepMinValueDoubleSpinBox->setValue(mDisplay->tickStepMinValue());
        ui->titleLineEdit->setText(mDisplay->title());
        ui->visibleTitleCheckBox->setChecked(mDisplay->titleVisible());
        if(mDisplay->boardReferenceTimeActive())
            ui->timeSourceComboBox->setCurrentIndex(0);
        else
            ui->timeSourceComboBox->setCurrentIndex(1);
        updateTabs();
    }
    setMinimumSize(600,400);
}

PlotTimeEditor::~PlotTimeEditor()
{
    delete ui;
}

void PlotTimeEditor::accept()
{
    updateElement();
    QDialog::accept();
}

void PlotTimeEditor::updateElement()
{
    if(mDisplay) {
        mDisplay->setTitle(ui->titleLineEdit->text());
        mDisplay->setTitleVisible(ui->visibleTitleCheckBox->isChecked());
        mDisplay->setBackgroundMode(BoardElement::BackgroundMode(ui->backgroundComboBox->currentIndex()));
        mDisplay->setLegendPosition(PlotTime::LegendPosition(ui->legendPositionComboBox->currentIndex()));
        mDisplay->setYAxisGridVisible(ui->ygridCheckBox->isChecked());
        mDisplay->setYAxisTicksVisible(ui->yticksCheckBox->isChecked());
        mDisplay->setYAxisLineVisible(ui->ylineCheckBox->isChecked());
        mDisplay->setYAxisLabelsVisible(ui->ylabelsCheckBox->isChecked());
        mDisplay->setXAxisGridVisible(ui->xgridCheckBox->isChecked());
        mDisplay->setXAxisTicksVisible(ui->xticksCheckBox->isChecked());
        mDisplay->setXAxisLineVisible(ui->xlineCheckBox->isChecked());
        mDisplay->setXAxisLabelsVisible(ui->xlabelsCheckBox->isChecked());
        mDisplay->setXAxisFixedTicker(ui->xfixedtickerCheckBox->isChecked());
        mDisplay->setLegendVisible(ui->legendVisibleCheckBox->isChecked());
        mDisplay->setXAxisHistory(ui->historyDoubleSpinBox->value());

        mDisplay->setYAxisScale(PlotTime::YAxisScale(ui->rangeComboBox->currentIndex()));
        mDisplay->setYAxisMinCustom(ui->rangeMinDoubleSpinBox->value());
        mDisplay->setYAxisMaxCustom(ui->rangeMaxDoubleSpinBox->value());
        mDisplay->setXAxisDirection(PlotTime::XAxisDirection(ui->directionComboBox->currentIndex()));
        mDisplay->setBoardReferenceTimeActive(!bool(ui->timeSourceComboBox->currentIndex()));
        mDisplay->setStatMode(PlotTime::StatMode(ui->statComboBox->currentIndex()));

        mDisplay->setThresholdsVisible(ui->ythresholdsCheckBox->isChecked());

        mDisplay->setYAxisTicksCount(ui->verticalTicksCountSpinBox->value());
        mDisplay->setXAxisTicksCount(ui->horizontalTicksCountSpinBox->value());

        mDisplay->setTickStepMinValue(ui->ticksStepMinValueDoubleSpinBox->value());
        mDisplay->setTickStepMinValueEnabled(ui->ticksStepMinValueCheckBox->isChecked());

        for(int i=0; i<mDisplay->parametersCount();i++) {
            QSharedPointer<BoardParameter> dashParam = mDisplay->boardParameter(i);
            if(dashParam) {
                if(!mPropertiesWidgets.at(i)->isConnected()) {
                    mPropertiesWidgets.at(i)->updateParameterSettings(dashParam->exclusiveParameterConfiguration());
                    dashParam->disconnectSharedConfiguration();
                } else {
                    dashParam->setSharedParameterConfiguration(mPropertiesWidgets.at(i)->currentSettings(), true);
                }
            }
        }
        mDisplay->rebuildElement(true);
    }
}

void PlotTimeEditor::updateTabs()
{
    int tabCount = ui->tabWidget->count();
    for(int i = 1; i< tabCount;i++) {
        ui->tabWidget->removeTab(1);
    }

    qDeleteAll(mPropertiesWidgets);
    mPropertiesWidgets.clear();

    for(int i=0; i<mDisplay->parametersCount();i++) {

        QSharedPointer<BoardParameter> dashParam = mDisplay->boardParameter(i);
        if(dashParam) {

            PropertiesWidget *propertiesWidget = new PropertiesWidget(this);
            propertiesWidget->setVisible(false);
            propertiesWidget->setProject(mDisplay->board()->project());

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
            propertiesWidget->setPropertiesMode(ParameterConfiguration::cmGraph);
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

void PlotTimeEditor::newParameter()
{
    QString paramLabel = QInputDialog::getText(this, "Parameter Label", "Parameter Label");
    if(!paramLabel.isEmpty()) {
        mDisplay->addParameter(paramLabel);
        updateTabs();
        if(!mDisplay->board()->dataManager()->liveDataEnabled()) {
            mDisplay->board()->resetPlayback();
            mDisplay->board()->replot(QCustomPlot::rpQueuedReplot);
        }
    }
}

void PlotTimeEditor::removeParameter()
{
    if(ui->tabWidget->currentIndex() > 0 ) {
        mDisplay->removeBoardParameter(ui->tabWidget->currentIndex() - 1 );
    }
    updateTabs();
}

void PlotTimeEditor::replaceParameter()
{
    if(ui->tabWidget->currentIndex() > 0 ) {
        QString paramLabel = QInputDialog::getText(this, "Parameter Label", "Parameter Label");
        if(!paramLabel.isEmpty()) {
            mDisplay->replaceParameter(ui->tabWidget->currentIndex() - 1, paramLabel);
        }
        updateTabs();
        if(!mDisplay->board()->dataManager()->liveDataEnabled()) {
            mDisplay->board()->resetPlayback();
            mDisplay->board()->replot(QCustomPlot::rpQueuedReplot);
        }
    }
}

void PlotTimeEditor::on_listWidget_currentRowChanged(int currentRow)
{
    ui->stackedWidget->setCurrentIndex(currentRow);
}

void PlotTimeEditor::on_rangeComboBox_currentIndexChanged(int index)
{
    if(index == 3)
        ui->scaleWidget->setEnabled(true);
    else
        ui->scaleWidget->setEnabled(false);
}
