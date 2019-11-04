#include "value_display_editor.h"
#include "ui_value_display_editor.h"

QTBValueDisplayEditor::QTBValueDisplayEditor(QTBValueDisplay *display, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QTBValueDisplayEditor),
    mValueDisplay(display)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->setCurrentRow(0);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &QTBValueDisplayEditor::updateElement);

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
            this, &QTBValueDisplayEditor::connectProperties);

    connect(ui->tabWidget, &QTabWidget::currentChanged,
            [this]() {
        if(ui->tabWidget->currentIndex() > 0)
            mRemoveParamButton->setVisible(true);
        else
            mRemoveParamButton->setVisible(false);
    });

    if(mValueDisplay) {
        ui->orientationComboBox->setCurrentIndex(mValueDisplay->orientation());
        ui->formatComboBox->setCurrentIndex(mValueDisplay->valueFormat());
        ui->headerCheckBox->setChecked(mValueDisplay->headerVisible());
        ui->transparentCheckBox->setChecked(mValueDisplay->transparentBackground());

        QSharedPointer<QTBDashboardParameter> dashParam = mValueDisplay->dashParameter(0);
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

QTBValueDisplayEditor::~QTBValueDisplayEditor()
{
    delete ui;
}

void QTBValueDisplayEditor::accept()
{
    updateElement();
    QDialog::accept();
}

void QTBValueDisplayEditor::updateElement()
{
    if(mValueDisplay) {
        mValueDisplay->setOrientation(QTBValueDisplay::DisplayOrientation(ui->orientationComboBox->currentIndex()));
        mValueDisplay->setValueFormat(ValueStringFormat(ui->formatComboBox->currentIndex()));
        mValueDisplay->setHeaderVisible(ui->headerCheckBox->isChecked());
        mValueDisplay->setTransparentBackground(ui->transparentCheckBox->isChecked());
        QSharedPointer<QTBDashboardParameter> dashParam = mValueDisplay->dashParameter(0);
        if(dashParam) {
            if(!mPropertiesWidget->isConnected()) {
                mPropertiesWidget->updateParameterSettings(dashParam->exclusiveParameterConfiguration());
                dashParam->disconnectSharedConfiguration();
            } /*else {
                dashParam->setConnected(true);
            }*/
        }
    }
}

void QTBValueDisplayEditor::connectProperties(bool connected)
{
    QSharedPointer<QTBDashboardParameter> dashParam = mValueDisplay->dashParameter(0);
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

void QTBValueDisplayEditor::newParameter()
{
    QString paramLabel = QInputDialog::getText(this, "Parameter Label", "Parameter Label");
    if(!paramLabel.isEmpty()) {
        mValueDisplay->addParameter(paramLabel);

        QSharedPointer<QTBDashboardParameter> dashParam = mValueDisplay->dashParameter(0);
        if(dashParam) {
            mPropertiesWidget->setEditionMode(PropertiesWidget::emElementStandAlone);
            mPropertiesWidget->setPropertiesMode(QTBParameterConfiguration::cmValue);
            mPropertiesWidget->updateUi(dashParam->exclusiveParameterConfiguration());
            mPropertiesWidget->setVisible(true);
            ui->tabWidget->insertTab(1, mPropertiesWidget, dashParam->getDisplayedLabel());
        }
    }
}

void QTBValueDisplayEditor::removeParameter()
{
    mValueDisplay->removeDashParameter(0);
    ui->tabWidget->removeTab(1);
}
