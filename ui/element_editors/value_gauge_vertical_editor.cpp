#include "value_gauge_vertical_editor.h"
#include "ui_value_gauge_vertical_editor.h"

QTBValueGaugeVerticalEditor::QTBValueGaugeVerticalEditor(QTBValueGaugeVertical *display, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QTBValueGaugeVerticalEditor),
    mGauge(display)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->setCurrentRow(0);

    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &QTBValueGaugeVerticalEditor::updateElement);

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

    mPropertiesWidget = new PropertiesWidget(this);
    mPropertiesWidget->setVisible(false);
    connect(mPropertiesWidget, &PropertiesWidget::connectProperties,
            this, &QTBValueGaugeVerticalEditor::connectProperties);

    connect(ui->tabWidget, &QTabWidget::currentChanged,
            [this]() {
        if(ui->tabWidget->currentIndex() > 0)
            mRemoveParamButton->setVisible(true);
        else
            mRemoveParamButton->setVisible(false);
    });

    if(mGauge) {
        ui->headerCheckBox->setChecked(mGauge->headerVisible());
        ui->valueCheckBox->setChecked(mGauge->valueVisible());
        ui->transparentCheckBox->setChecked(mGauge->transparentBackground());
        ui->tankCheckBox->setChecked(mGauge->tankGauge());
        ui->ticksCheckBox->setChecked(mGauge->axisTicksVisible());
        ui->labelsCheckBox->setChecked(mGauge->axisLabelsVisible());
        ui->gridCheckBox->setChecked(mGauge->axisGridVisible());
        ui->rangeComboBox->setCurrentIndex(mGauge->axisScale());
        ui->rangeMinDoubleSpinBox->setValue(mGauge->axisMinCustom());
        ui->rangeMaxDoubleSpinBox->setValue(mGauge->axisMaxCustom());

        QSharedPointer<QTBDashboardParameter> dashParam = mGauge->dashParameter(0);
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
            mPropertiesWidget->setPropertiesMode(QTBParameterConfiguration::cmValue);
            mPropertiesWidget->setVisible(true);
            ui->tabWidget->insertTab(1, mPropertiesWidget, dashParam->getDisplayedLabel());
        }
    }
    setMinimumSize(600,400);
}

QTBValueGaugeVerticalEditor::~QTBValueGaugeVerticalEditor()
{
    delete ui;
}

void QTBValueGaugeVerticalEditor::accept()
{
    updateElement();
    QDialog::accept();
}

void QTBValueGaugeVerticalEditor::updateElement()
{
    if(mGauge) {
        mGauge->setHeaderVisible(ui->headerCheckBox->isChecked());
        mGauge->setValueVisible(ui->valueCheckBox->isChecked());
        mGauge->setTransparentBackground(ui->transparentCheckBox->isChecked());
        mGauge->setTankGauge(ui->tankCheckBox->isChecked());
        mGauge->setAxisGridVisible(ui->gridCheckBox->isChecked());
        mGauge->setAxisTicksVisible(ui->ticksCheckBox->isChecked());
        mGauge->setAxisLabelsVisible(ui->labelsCheckBox->isChecked());

        mGauge->setAxisScale(QTBValueGaugeVertical::AxisScale(ui->rangeComboBox->currentIndex()));
        mGauge->setAxisMinCustom(ui->rangeMinDoubleSpinBox->value());
        mGauge->setAxisMaxCustom(ui->rangeMaxDoubleSpinBox->value());

        QSharedPointer<QTBDashboardParameter> dashParam = mGauge->dashParameter(0);
        if(dashParam) {
            if(!mPropertiesWidget->isConnected()) {
                mPropertiesWidget->updateParameterSettings(dashParam->exclusiveParameterConfiguration());
                dashParam->disconnectProperties();
            } /*else {
                dashParam->setConnected(true);
            }*/
        }
    }
}

void QTBValueGaugeVerticalEditor::connectProperties(bool connected)
{
    QSharedPointer<QTBDashboardParameter> dashParam = mGauge->dashParameter(0);
    if(dashParam) {
        if(connected) {
            mPropertiesWidget->setEditionMode(PropertiesWidget::emElementConnected);
            mPropertiesWidget->updateUi(dashParam->sharedParameterConfiguration());
        } else {
            mPropertiesWidget->setEditionMode(PropertiesWidget::emElementDisconnected);
            mPropertiesWidget->updateUi(dashParam->exclusiveParameterConfiguration());
        }
        mPropertiesWidget->setPropertiesMode(QTBParameterConfiguration::cmValue);
    }
}

void QTBValueGaugeVerticalEditor::newParameter()
{
    QString paramLabel = QInputDialog::getText(this, "Parameter Label", "Parameter Label");
    if(!paramLabel.isEmpty()) {
        mGauge->addParameter(paramLabel);

        QSharedPointer<QTBDashboardParameter> dashParam = mGauge->dashParameter(0);
        if(dashParam) {
            mPropertiesWidget->setEditionMode(PropertiesWidget::emElementStandAlone);
            mPropertiesWidget->setPropertiesMode(QTBParameterConfiguration::cmValue);
            mPropertiesWidget->updateUi(dashParam->exclusiveParameterConfiguration());
            mPropertiesWidget->setVisible(true);
            ui->tabWidget->insertTab(1, mPropertiesWidget, dashParam->getDisplayedLabel());
        }
    }
}

void QTBValueGaugeVerticalEditor::removeParameter()
{
    mGauge->removeDashParameter(0);
    ui->tabWidget->removeTab(1);
}

void QTBValueGaugeVerticalEditor::on_rangeComboBox_currentIndexChanged(int index)
{
    if(index == 2)
        ui->scaleWidget->setEnabled(true);
    else
        ui->scaleWidget->setEnabled(false);
}


void QTBValueGaugeVerticalEditor::on_listWidget_currentRowChanged(int currentRow)
{
    ui->stackedWidget->setCurrentIndex(currentRow);
}

