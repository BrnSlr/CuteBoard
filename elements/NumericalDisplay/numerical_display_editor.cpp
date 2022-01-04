#include "numerical_display_editor.h"
#include "ui_numerical_display_editor.h"
#include "ui/widgets/utils/icon_factory.h"

NumericalDisplayEditor::NumericalDisplayEditor(NumericalDisplay *display, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NumericalDisplayEditor),
    mValueDisplay(display),
    mPropertiesWidget(nullptr)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->setCurrentRow(0);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &NumericalDisplayEditor::updateElement);

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

    if(mValueDisplay) {
        ui->modeComboBox->setCurrentIndex(mValueDisplay->statMode());
        ui->formatComboBox->setCurrentIndex(mValueDisplay->valueFormat());
        ui->headerCheckBox->setChecked(mValueDisplay->headerVisible());
        ui->backgroundComboBox->setCurrentIndex(mValueDisplay->backgroundMode());

        mPropertiesWidget = new PropertiesWidget(this);
        mPropertiesWidget->setVisible(false);
        mPropertiesWidget->setProject(display->board()->project());
        connect(mPropertiesWidget, SIGNAL(connectProperties(bool)),
                this, SLOT(connectProperties(bool)));

        QSharedPointer<BoardParameter> dashParam = mValueDisplay->boardParameter(0);
        if(dashParam) {

            if(dashParam->connected()) {
                mPropertiesWidget->setEditionMode(PropertiesWidget::emElementConnected);
                mPropertiesWidget->updateUi(dashParam->sharedParameterConfiguration());
            } else {
                if(dashParam->sharedParameterConfiguration()) {
                    mPropertiesWidget->setEditionMode(PropertiesWidget::emElementDisconnected);
                } else {
                    mPropertiesWidget->setEditionMode(PropertiesWidget::emElementStandAlone);
                }
                mPropertiesWidget->updateUi(dashParam->exclusiveParameterConfiguration());
            }
            mPropertiesWidget->setPropertiesMode(ParameterConfiguration::cmValue);
            mPropertiesWidget->setVisible(true);
            ui->tabWidget->insertTab(1, mPropertiesWidget,dashParam->getTimeSeries().name());
        }

    }
    setMinimumSize(600,400);
}

NumericalDisplayEditor::~NumericalDisplayEditor()
{
    disconnect(ui->tabWidget, &QTabWidget::currentChanged,this, nullptr);
    if(mPropertiesWidget)
        delete mPropertiesWidget;
    delete ui;
}

void NumericalDisplayEditor::accept()
{
    updateElement();
    QDialog::accept();
}

void NumericalDisplayEditor::updateElement()
{
    if(mValueDisplay) {
        mValueDisplay->setStatMode(NumericalDisplay::StatMode(ui->modeComboBox->currentIndex()));
        mValueDisplay->setValueFormat(ValueStringFormat(ui->formatComboBox->currentIndex()));
        mValueDisplay->setHeaderVisible(ui->headerCheckBox->isChecked());
        mValueDisplay->setBackgroundMode(BoardElement::BackgroundMode(ui->backgroundComboBox->currentIndex()));
        QSharedPointer<BoardParameter> dashParam = mValueDisplay->boardParameter(0);
        if(dashParam) {
            if(!mPropertiesWidget->isConnected()) {
                mPropertiesWidget->updateParameterSettings(dashParam->exclusiveParameterConfiguration());
                dashParam->disconnectSharedConfiguration();
            } else {
                dashParam->setSharedParameterConfiguration(mPropertiesWidget->currentSettings(), true);
            }
        }
        mValueDisplay->rebuildElement(true);
    }
}

void NumericalDisplayEditor::connectProperties(bool connected)
{
    QSharedPointer<BoardParameter> dashParam = mValueDisplay->boardParameter(0);
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

void NumericalDisplayEditor::newParameter()
{
    QString paramLabel = QInputDialog::getText(this, "Parameter Label", "Parameter Label");
    if(!paramLabel.isEmpty()) {
        mValueDisplay->addParameter(paramLabel);

        QSharedPointer<BoardParameter> dashParam = mValueDisplay->boardParameter(0);
        if(dashParam) {
            mPropertiesWidget->setEditionMode(PropertiesWidget::emElementStandAlone);
            mPropertiesWidget->setPropertiesMode(ParameterConfiguration::cmValue);
            mPropertiesWidget->updateUi(dashParam->exclusiveParameterConfiguration());
            mPropertiesWidget->setVisible(true);
            ui->tabWidget->insertTab(1, mPropertiesWidget, dashParam->getDisplayedLabel());
        }
        if(!mValueDisplay->board()->dataManager()->liveDataEnabled()) {
            mValueDisplay->board()->resetPlayback();
            mValueDisplay->board()->replot(QCustomPlot::rpQueuedReplot);
        }
    }
}

void NumericalDisplayEditor::removeParameter()
{
    mValueDisplay->removeBoardParameter(0);
    ui->tabWidget->removeTab(1);
}

void NumericalDisplayEditor::replaceParameter()
{
    if(ui->tabWidget->currentIndex() > 0 ) {
        QString paramLabel = QInputDialog::getText(this, "Parameter Label", "Parameter Label");
        if(!paramLabel.isEmpty()) {
            QSharedPointer<BoardParameter> dashParam = mValueDisplay->replaceParameter(ui->tabWidget->currentIndex() - 1, paramLabel);
            if(dashParam) {
                mPropertiesWidget->setEditionMode(PropertiesWidget::emElementStandAlone);
                mPropertiesWidget->setPropertiesMode(ParameterConfiguration::cmValue);
                mPropertiesWidget->updateUi(dashParam->exclusiveParameterConfiguration());
                mPropertiesWidget->setVisible(true);
                ui->tabWidget->insertTab(1, mPropertiesWidget, dashParam->getDisplayedLabel());
            }
            if(!mValueDisplay->board()->dataManager()->liveDataEnabled()) {
                mValueDisplay->board()->resetPlayback();
                mValueDisplay->board()->replot(QCustomPlot::rpQueuedReplot);
            }
        }
    }
}
