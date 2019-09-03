#include "value_bitfields_editor.h"
#include "ui_value_bitfields_editor.h"

QTBValueBitfieldsEditor::QTBValueBitfieldsEditor(QTBValueBitfields *display, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QTBValueBitfieldsEditor),
    mBitfieldsTable(display)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->setCurrentRow(0);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &QTBValueBitfieldsEditor::updateElement);

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
            this, &QTBValueBitfieldsEditor::connectProperties);

    connect(ui->tabWidget, &QTabWidget::currentChanged,
            [this]() {
        if(ui->tabWidget->currentIndex() > 0)
            mRemoveParamButton->setVisible(true);
        else
            mRemoveParamButton->setVisible(false);
    });

    if(mBitfieldsTable) {
        ui->headerCheckBox->setChecked(mBitfieldsTable->headerVisible());
        ui->valueCheckBox->setChecked(mBitfieldsTable->valueVisible());
        ui->formatComboBox->setCurrentIndex(mBitfieldsTable->valueFormat() - 1);
        ui->transparentCheckBox->setChecked(mBitfieldsTable->transparentBackground());

        QSharedPointer<QTBDashboardParameter> dashParam = mBitfieldsTable->dashParameter(0);
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
            mPropertiesWidget->setPropertiesMode(QTBParameterConfiguration::cmBitFields);
            mPropertiesWidget->setVisible(true);
            ui->tabWidget->insertTab(1, mPropertiesWidget, dashParam->getDisplayedLabel());
        }
    }
    setMinimumSize(600,400);
}

QTBValueBitfieldsEditor::~QTBValueBitfieldsEditor()
{
    delete ui;
}

void QTBValueBitfieldsEditor::accept()
{
    updateElement();
    QDialog::accept();
}

void QTBValueBitfieldsEditor::updateElement()
{
    if(mBitfieldsTable) {
        mBitfieldsTable->setHeaderVisible(ui->headerCheckBox->isChecked());
        mBitfieldsTable->setValueVisible(ui->valueCheckBox->isChecked());
        mBitfieldsTable->setValueFormat(ValueStringFormat(ui->formatComboBox->currentIndex() + 1));
        mBitfieldsTable->setTransparentBackground(ui->transparentCheckBox->isChecked());
        QSharedPointer<QTBDashboardParameter> dashParam = mBitfieldsTable->dashParameter(0);
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

void QTBValueBitfieldsEditor::connectProperties(bool connected)
{
    QSharedPointer<QTBDashboardParameter> dashParam = mBitfieldsTable->dashParameter(0);
    if(dashParam) {
        if(connected) {
            mPropertiesWidget->setEditionMode(PropertiesWidget::emElementConnected);
            mPropertiesWidget->updateUi(dashParam->sharedParameterConfiguration());
        } else {
            mPropertiesWidget->setEditionMode(PropertiesWidget::emElementDisconnected);
            mPropertiesWidget->updateUi(dashParam->exclusiveParameterConfiguration());
        }
        mPropertiesWidget->setPropertiesMode(QTBParameterConfiguration::cmBitFields);
    }
}

void QTBValueBitfieldsEditor::newParameter()
{
    QString paramLabel = QInputDialog::getText(this, "Parameter Label", "Parameter Label");
    if(!paramLabel.isEmpty()) {
        mBitfieldsTable->addParameter(paramLabel);

        QSharedPointer<QTBDashboardParameter> dashParam = mBitfieldsTable->dashParameter(0);
        if(dashParam) {
            mPropertiesWidget->setEditionMode(PropertiesWidget::emElementStandAlone);
            mPropertiesWidget->setPropertiesMode(QTBParameterConfiguration::cmBitFields);
            mPropertiesWidget->updateUi(dashParam->exclusiveParameterConfiguration());
            mPropertiesWidget->setVisible(true);
            ui->tabWidget->insertTab(1, mPropertiesWidget, dashParam->getDisplayedLabel());
        }
    }
}

void QTBValueBitfieldsEditor::removeParameter()
{
    mBitfieldsTable->removeDashParameter(0);
    ui->tabWidget->removeTab(1);
}
