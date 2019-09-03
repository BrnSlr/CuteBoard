#include "state_display_editor.h"
#include "ui_state_display_editor.h"

QTBStateDisplayEditor::QTBStateDisplayEditor(QTBStateDisplay *display, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QTBStateDisplayEditor),
    mStateIndicator(display)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->setCurrentRow(0);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &QTBStateDisplayEditor::updateElement);

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
            this, &QTBStateDisplayEditor::connectProperties);

    connect(ui->tabWidget, &QTabWidget::currentChanged,
            [this]() {
        if(ui->tabWidget->currentIndex() > 0)
            mRemoveParamButton->setVisible(true);
        else
            mRemoveParamButton->setVisible(false);
    });

    if(mStateIndicator) {
        ui->orientationComboBox->setCurrentIndex(mStateIndicator->orientation());
        ui->headerCheckBox->setChecked(mStateIndicator->headerVisible());
        ui->transparentCheckBox->setChecked(mStateIndicator->transparentBackground());

        QSharedPointer<QTBDashboardParameter> dashParam = mStateIndicator->dashParameter(0);
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
            mPropertiesWidget->setPropertiesMode(QTBParameterConfiguration::cmState);
            mPropertiesWidget->setVisible(true);
            ui->tabWidget->insertTab(1, mPropertiesWidget, dashParam->getDisplayedLabel());
        }
    }
    setMinimumSize(600,400);
}

QTBStateDisplayEditor::~QTBStateDisplayEditor()
{
    delete ui;
}

void QTBStateDisplayEditor::accept()
{
    updateElement();
    QDialog::accept();
}

void QTBStateDisplayEditor::updateElement()
{
    if(mStateIndicator) {
        mStateIndicator->setOrientation(QTBStateDisplay::DisplayOrientation(ui->orientationComboBox->currentIndex()));
        mStateIndicator->setHeaderVisible(ui->headerCheckBox->isChecked());
        mStateIndicator->setTransparentBackground(ui->transparentCheckBox->isChecked());
        QSharedPointer<QTBDashboardParameter> dashParam = mStateIndicator->dashParameter(0);
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

void QTBStateDisplayEditor::connectProperties(bool connected)
{
    QSharedPointer<QTBDashboardParameter> dashParam = mStateIndicator->dashParameter(0);
    if(dashParam) {
        if(connected) {
            mPropertiesWidget->setEditionMode(PropertiesWidget::emElementConnected);
            mPropertiesWidget->updateUi(dashParam->sharedParameterConfiguration());
        } else {
            mPropertiesWidget->setEditionMode(PropertiesWidget::emElementDisconnected);
            mPropertiesWidget->updateUi(dashParam->exclusiveParameterConfiguration());
        }
        mPropertiesWidget->setPropertiesMode(QTBParameterConfiguration::cmState);
    }
}

void QTBStateDisplayEditor::newParameter()
{
    QString paramLabel = QInputDialog::getText(this, "Parameter Label", "Parameter Label");
    if(!paramLabel.isEmpty()) {
        mStateIndicator->addParameter(paramLabel);

        QSharedPointer<QTBDashboardParameter> dashParam = mStateIndicator->dashParameter(0);
        if(dashParam) {
            mPropertiesWidget->setEditionMode(PropertiesWidget::emElementStandAlone);
            mPropertiesWidget->setPropertiesMode(QTBParameterConfiguration::cmState);
            mPropertiesWidget->updateUi(dashParam->exclusiveParameterConfiguration());
            mPropertiesWidget->setVisible(true);
            ui->tabWidget->insertTab(1, mPropertiesWidget, dashParam->getDisplayedLabel());
        }
    }
}

void QTBStateDisplayEditor::removeParameter()
{
    mStateIndicator->removeDashParameter(0);
    ui->tabWidget->removeTab(1);
}
