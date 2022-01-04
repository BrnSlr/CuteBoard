#include "state_display_editor.h"
#include "ui_state_display_editor.h"
#include "ui/widgets/utils/icon_factory.h"

StateDisplayEditor::StateDisplayEditor(StateDisplay *display, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StateDisplayEditor),
    mStateIndicator(display)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->setCurrentRow(0);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
            this, &StateDisplayEditor::updateElement);

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

    if(mStateIndicator) {
        ui->orientationComboBox->setCurrentIndex(mStateIndicator->orientation());
        ui->headerCheckBox->setChecked(mStateIndicator->headerVisible());
        ui->backgroundComboBox->setCurrentIndex(mStateIndicator->backgroundMode());

        QSharedPointer<BoardParameter> dashParam = mStateIndicator->boardParameter(0);
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
            mPropertiesWidget->setPropertiesMode(ParameterConfiguration::cmState);
            mPropertiesWidget->setVisible(true);
            ui->tabWidget->insertTab(1, mPropertiesWidget, dashParam->getDisplayedLabel());
        }
    }
    setMinimumSize(600,400);
}

StateDisplayEditor::~StateDisplayEditor()
{
    delete ui;
}

void StateDisplayEditor::accept()
{
    updateElement();
    QDialog::accept();
}

void StateDisplayEditor::updateElement()
{
    if(mStateIndicator) {
        mStateIndicator->setOrientation(StateDisplay::DisplayOrientation(ui->orientationComboBox->currentIndex()));
        mStateIndicator->setHeaderVisible(ui->headerCheckBox->isChecked());

        mStateIndicator->setBackgroundMode(BoardElement::BackgroundMode(ui->backgroundComboBox->currentIndex()));
        QSharedPointer<BoardParameter> dashParam = mStateIndicator->boardParameter(0);
        if(dashParam) {
            if(!mPropertiesWidget->isConnected()) {
                mPropertiesWidget->updateParameterSettings(dashParam->exclusiveParameterConfiguration());
                dashParam->disconnectSharedConfiguration();
            } else {
                dashParam->setSharedParameterConfiguration(mPropertiesWidget->currentSettings(), true);
            }
        }
        mStateIndicator->rebuildElement(true);
    }
}

void StateDisplayEditor::connectProperties(bool connected)
{
    QSharedPointer<BoardParameter> dashParam = mStateIndicator->boardParameter(0);
    if(dashParam) {
        if(connected) {
            mPropertiesWidget->setEditionMode(PropertiesWidget::emElementConnected);
            mPropertiesWidget->updateUi(dashParam->sharedParameterConfiguration());
        } else {
            mPropertiesWidget->setEditionMode(PropertiesWidget::emElementDisconnected);
            mPropertiesWidget->updateUi(dashParam->exclusiveParameterConfiguration());
        }
        mPropertiesWidget->setPropertiesMode(ParameterConfiguration::cmState);
    }
}

void StateDisplayEditor::newParameter()
{
    QString paramLabel = QInputDialog::getText(this, "Parameter Label", "Parameter Label");
    if(!paramLabel.isEmpty()) {
        mStateIndicator->addParameter(paramLabel);

        QSharedPointer<BoardParameter> dashParam = mStateIndicator->boardParameter(0);
        if(dashParam) {
            mPropertiesWidget->setEditionMode(PropertiesWidget::emElementStandAlone);
            mPropertiesWidget->setPropertiesMode(ParameterConfiguration::cmState);
            mPropertiesWidget->updateUi(dashParam->exclusiveParameterConfiguration());
            mPropertiesWidget->setVisible(true);
            ui->tabWidget->insertTab(1, mPropertiesWidget, dashParam->getDisplayedLabel());
        }
        if(!mStateIndicator->board()->dataManager()->liveDataEnabled()) {
            mStateIndicator->board()->resetPlayback();
            mStateIndicator->board()->replot(QCustomPlot::rpQueuedReplot);
        }
    }
}

void StateDisplayEditor::removeParameter()
{
    mStateIndicator->removeBoardParameter(0);
    ui->tabWidget->removeTab(1);
}

void StateDisplayEditor::replaceParameter()
{
    if(ui->tabWidget->currentIndex() > 0 ) {
        QString paramLabel = QInputDialog::getText(this, "Parameter Label", "Parameter Label");
        if(!paramLabel.isEmpty()) {
            QSharedPointer<BoardParameter> dashParam = mStateIndicator->replaceParameter(ui->tabWidget->currentIndex() - 1, paramLabel);
            if(dashParam) {
                mPropertiesWidget->setEditionMode(PropertiesWidget::emElementStandAlone);
                mPropertiesWidget->setPropertiesMode(ParameterConfiguration::cmState);
                mPropertiesWidget->updateUi(dashParam->exclusiveParameterConfiguration());
                mPropertiesWidget->setVisible(true);
                ui->tabWidget->insertTab(1, mPropertiesWidget, dashParam->getDisplayedLabel());
            }
            if(!mStateIndicator->board()->dataManager()->liveDataEnabled()) {
                mStateIndicator->board()->resetPlayback();
                mStateIndicator->board()->replot(QCustomPlot::rpQueuedReplot);
            }
        }
    }
}
