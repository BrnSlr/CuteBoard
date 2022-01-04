#include "gauge_radial_editor.h"
#include "ui_gauge_radial_editor.h"
#include "ui/widgets/utils/icon_factory.h"

GaugeRadialEditor::GaugeRadialEditor(GaugeRadial *display, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GaugeRadialEditor),
    mGauge(display)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->setCurrentRow(0);

    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &GaugeRadialEditor::updateElement);

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
        ui->ticksCheckBox->setChecked(mGauge->axisTicksVisible());
        ui->labelsCheckBox->setChecked(mGauge->axisLabelsVisible());
        ui->lineCheckBox->setChecked(mGauge->axisLineVisible());
        ui->rangeComboBox->setCurrentIndex(mGauge->axisScale());
        ui->rangeMinDoubleSpinBox->setValue(mGauge->axisMinCustom());
        ui->rangeMaxDoubleSpinBox->setValue(mGauge->axisMaxCustom());
        ui->positionComboBox->setCurrentIndex(mGauge->gaugePosition());
        ui->startAngleSpinBox->setValue(mGauge->angleStart());
        ui->spanAngleSpinBox->setValue(mGauge->angleSpan());

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

GaugeRadialEditor::~GaugeRadialEditor()
{
    delete ui;
}

void GaugeRadialEditor::accept()
{
    updateElement();
    QDialog::accept();
}

void GaugeRadialEditor::updateElement()
{
    if(mGauge) {
        mGauge->setHeaderVisible(ui->headerCheckBox->isChecked());
        mGauge->setValueVisible(ui->valueCheckBox->isChecked());
        mGauge->setBackgroundMode(BoardElement::BackgroundMode(ui->backgroundComboBox->currentIndex()));
        mGauge->setAxisTicksVisible(ui->ticksCheckBox->isChecked());
        mGauge->setAxisLabelsVisible(ui->labelsCheckBox->isChecked());
        mGauge->setAxisScale(GaugeRadial::AxisScale(ui->rangeComboBox->currentIndex()));
        mGauge->setAxisMinCustom(ui->rangeMinDoubleSpinBox->value());
        mGauge->setAxisMaxCustom(ui->rangeMaxDoubleSpinBox->value());
        mGauge->setAxisLineVisible(ui->lineCheckBox->isChecked());
        mGauge->setAngleSpan(ui->spanAngleSpinBox->value());
        mGauge->setAngleStart(ui->startAngleSpinBox->value());

        mGauge->setGaugePosition(GaugeRadial::GaugeRPosition(ui->positionComboBox->currentIndex()));
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

void GaugeRadialEditor::connectProperties(bool connected)
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

void GaugeRadialEditor::newParameter()
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

void GaugeRadialEditor::removeParameter()
{
    mGauge->removeBoardParameter(0);
    ui->tabWidget->removeTab(1);
}

void GaugeRadialEditor::replaceParameter()
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

void GaugeRadialEditor::on_rangeComboBox_currentIndexChanged(int index)
{
    if(index == 2)
        ui->scaleWidget->setEnabled(true);
    else
        ui->scaleWidget->setEnabled(false);
}

void GaugeRadialEditor::on_listWidget_currentRowChanged(int currentRow)
{
    ui->stackedWidget->setCurrentIndex(currentRow);
}
