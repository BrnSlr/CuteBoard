#include "gauge_horizontal_editor.h"
#include "ui_gauge_horizontal_editor.h"
#include "ui/widgets/utils/icon_factory.h"

GaugeHorizontalEditor::GaugeHorizontalEditor(GaugeHorizontal *display, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GaugeHorizontalEditor),
    mGauge(display)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->setCurrentRow(0);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &GaugeHorizontalEditor::updateElement);

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

    mPropertiesWidget = new PropertiesWidget(this);
    mPropertiesWidget->setVisible(false);
    mPropertiesWidget->setProject(display->board()->project());

    connect(mPropertiesWidget, SIGNAL(connectProperties(bool)),
            this, SLOT(connectProperties(bool)));

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

    if(mGauge) {

        ui->headerCheckBox->setChecked(mGauge->headerVisible());
        ui->valueCheckBox->setChecked(mGauge->valueVisible());
        ui->backgroundComboBox->setCurrentIndex(mGauge->backgroundMode());
        ui->tankCheckBox->setChecked(mGauge->tankGauge());
        ui->ticksCheckBox->setChecked(mGauge->axisTicksVisible());
        ui->labelsCheckBox->setChecked(mGauge->axisLabelsVisible());
        ui->lineCheckBox->setChecked(mGauge->axisLineVisible());
        ui->gridCheckBox->setChecked(mGauge->axisGridVisible());
        ui->rangeComboBox->setCurrentIndex(mGauge->axisScale());
        ui->rangeMinDoubleSpinBox->setValue(mGauge->axisMinCustom());
        ui->rangeMaxDoubleSpinBox->setValue(mGauge->axisMaxCustom());
        ui->positionComboBox->setCurrentIndex(mGauge->gaugeHPosition());

        QSharedPointer<BoardParameter> dashParam = mGauge->boardParameter(0);
        if(dashParam) {
            if(dashParam->connected()) {
                mPropertiesWidget->setEditionMode(PropertiesWidget::emElementConnected);
                mPropertiesWidget->updateUi(dashParam->sharedParameterConfiguration());
            } else {
                if(dashParam->sharedParameterConfiguration())
                    mPropertiesWidget->setEditionMode(PropertiesWidget::emElementDisconnected);
                else
                    mPropertiesWidget->setEditionMode(PropertiesWidget::emElementStandAlone);
                mPropertiesWidget->updateUi(dashParam->exclusiveParameterConfiguration());
            }
            mPropertiesWidget->setPropertiesMode(ParameterConfiguration::cmValue);
            mPropertiesWidget->setVisible(true);
            ui->tabWidget->insertTab(1, mPropertiesWidget, dashParam->getDisplayedLabel());
        }
    }
    setMinimumSize(600,400);
}

GaugeHorizontalEditor::~GaugeHorizontalEditor()
{
    delete ui;
}

void GaugeHorizontalEditor::accept()
{
    updateElement();
    QDialog::accept();
}

void GaugeHorizontalEditor::updateElement()
{
    if(mGauge) {
        mGauge->setHeaderVisible(ui->headerCheckBox->isChecked());
        mGauge->setValueVisible(ui->valueCheckBox->isChecked());
        mGauge->setBackgroundMode(BoardElement::BackgroundMode(ui->backgroundComboBox->currentIndex()));
        mGauge->setTankGauge(ui->tankCheckBox->isChecked());
        mGauge->setAxisGridVisible(ui->gridCheckBox->isChecked());
        mGauge->setAxisTicksVisible(ui->ticksCheckBox->isChecked());
        mGauge->setAxisLineVisible(ui->lineCheckBox->isChecked());
        mGauge->setAxisLabelsVisible(ui->labelsCheckBox->isChecked());
        mGauge->setGaugeHPosition(GaugeHorizontal::GaugeHPosition(ui->positionComboBox->currentIndex()));

        mGauge->setAxisScale(GaugeHorizontal::AxisScale(ui->rangeComboBox->currentIndex()));
        mGauge->setAxisMinCustom(ui->rangeMinDoubleSpinBox->value());
        mGauge->setAxisMaxCustom(ui->rangeMaxDoubleSpinBox->value());

        QSharedPointer<BoardParameter> dashParam = mGauge->boardParameter(0);
        if(dashParam) {
            if(!mPropertiesWidget->isConnected()) {
                mPropertiesWidget->updateParameterSettings(dashParam->exclusiveParameterConfiguration());
                dashParam->disconnectSharedConfiguration();
            } else {
                dashParam->setSharedParameterConfiguration(mPropertiesWidget->currentSettings(), true);
            }
        }

        mGauge->rebuildElement(true);
    }
}

void GaugeHorizontalEditor::connectProperties(bool connected)
{
    QSharedPointer<BoardParameter> dashParam = mGauge->boardParameter(0);
    if(dashParam) {
        if(connected) {
            mPropertiesWidget->setEditionMode(PropertiesWidget::emElementConnected);
            mPropertiesWidget->updateUi(dashParam->sharedParameterConfiguration());
        } else {
            mPropertiesWidget->setEditionMode(PropertiesWidget::emElementDisconnected);
            mPropertiesWidget->updateUi(dashParam->exclusiveParameterConfiguration());
        }
        mPropertiesWidget->setPropertiesMode(ParameterConfiguration::cmValue);
    }
}

void GaugeHorizontalEditor::newParameter()
{
    QString paramLabel = QInputDialog::getText(this, "Parameter Label", "Parameter Label");
    if(!paramLabel.isEmpty()) {
        mGauge->addParameter(paramLabel);

        QSharedPointer<BoardParameter> dashParam = mGauge->boardParameter(0);
        if(dashParam) {
            mPropertiesWidget->setEditionMode(PropertiesWidget::emElementStandAlone);
            mPropertiesWidget->setPropertiesMode(ParameterConfiguration::cmValue);
            mPropertiesWidget->updateUi(dashParam->exclusiveParameterConfiguration());
            mPropertiesWidget->setVisible(true);
            ui->tabWidget->insertTab(1, mPropertiesWidget, dashParam->getDisplayedLabel());
        }

        if(!mGauge->board()->dataManager()->liveDataEnabled()) {
            mGauge->board()->resetPlayback();
            mGauge->board()->replot(QCustomPlot::rpQueuedReplot);
        }
    }
}

void GaugeHorizontalEditor::removeParameter()
{
    mGauge->removeBoardParameter(0);
    ui->tabWidget->removeTab(1);
}

void GaugeHorizontalEditor::replaceParameter()
{
    if(ui->tabWidget->currentIndex() > 0 ) {
        QString paramLabel = QInputDialog::getText(this, "Parameter Label", "Parameter Label");
        if(!paramLabel.isEmpty()) {
            QSharedPointer<BoardParameter> dashParam = mGauge->replaceParameter(ui->tabWidget->currentIndex() - 1, paramLabel);
            if(dashParam) {
                mPropertiesWidget->setEditionMode(PropertiesWidget::emElementStandAlone);
                mPropertiesWidget->setPropertiesMode(ParameterConfiguration::cmValue);
                mPropertiesWidget->updateUi(dashParam->exclusiveParameterConfiguration());
                mPropertiesWidget->setVisible(true);
                ui->tabWidget->insertTab(1, mPropertiesWidget, dashParam->getDisplayedLabel());
            }
            if(!mGauge->board()->dataManager()->liveDataEnabled()) {
                mGauge->board()->resetPlayback();
                mGauge->board()->replot(QCustomPlot::rpQueuedReplot);
            }
        }
    }
}

void GaugeHorizontalEditor::on_rangeComboBox_currentIndexChanged(int index)
{
    if(index == 2)
        ui->scaleWidget->setEnabled(true);
    else
        ui->scaleWidget->setEnabled(false);
}

void GaugeHorizontalEditor::on_listWidget_currentRowChanged(int currentRow)
{
    ui->stackedWidget->setCurrentIndex(currentRow);
}
