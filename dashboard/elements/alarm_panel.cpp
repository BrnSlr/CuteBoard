#include "alarm_panel.h"
#include "project/project.h"
#include "ui/alarm_configuration_widget.h"

QTBAlarmPanel::QTBAlarmPanel(QTBoard *dashboard) :
    QTBDashboardElement (dashboard),
    mAlarmConfigurationIsShared(false),
    mSharedAlarmConfiguration(new QTBAlarmConfiguration ()),
    mExclusiveAlarmConfiguration(new QTBAlarmConfiguration ()),
    mAlarmConfiguration(new QTBAlarmConfiguration ())
{
    disconnectAlarmConfig();
    setParametersMaxCount(2000);
}

int QTBAlarmPanel::defaultWidth()
{
    return 3*mBoard->dashboardLayout()->singleElementColumnCount();
}

int QTBAlarmPanel::defaultHeight()
{
    return 9*mBoard->dashboardLayout()->singleElementRowCount();
}

void QTBAlarmPanel::initializeElement(QTBoard *dashboard)
{
    if(dashboard) {

        QTBDashboardElement::initializeElement(dashboard);

        mHeader = new QTBAdjustTextElement(dashboard);
        mHeader->setTextFlags(Qt::AlignCenter);
        mHeader->setMaxPointSize(10);
        mHeader->setMinPointSize(6);
        mHeader->setTextColor(dashboard->frontColor());
        mHeader->setText(QString("Alarms"));
        mHeader->setAdjustStrategy(QTBAdjustTextElement::asElide);

        mMainLayout = new QTBLayoutList();
        mMainLayout->initializeParentPlot(dashboard);
        mMainLayout->setParentLayerable(this);
        mMainLayout->setRowSpacing(2);

        mMainLayout->addElement(mHeader);

        for(int i=0;i<64;i++) {
            auto el = new QTBAdjustTextElement(dashboard);
            el->setTextFlags(Qt::AlignVCenter | Qt::AlignLeft);
            el->setMaxPointSize(11);
            el->setMinPointSize(6);
            el->setVisible(false);
            el->setText(QString("Test Alarm %1").arg(i));
            el->setAdjustStrategy(QTBAdjustTextElement::asElide);
            mTextElements.append(el);
            //            mMainLayout->addElement(el);
        }

        updateElement();
    }
}

void QTBAlarmPanel::clearElement()
{
    delete mMainLayout;
}

void QTBAlarmPanel::edit()
{    
    QDialog dial;
    QVBoxLayout dialLayout;

    QHBoxLayout rowHeightLayout;
    QLabel rowHeightLabel("List Row Height");
    QSpinBox rowHeightSpinBox(&dial);
    rowHeightSpinBox.setValue(int(mMainLayout->rowHeight()));
    rowHeightLayout.addWidget(&rowHeightLabel);
    rowHeightLayout.addWidget(&rowHeightSpinBox);
    dialLayout.addLayout(&rowHeightLayout);

    QTBAlarmConfigurationWidget w;
    if(mAlarmConfigurationIsShared) {
        w.setEditionMode(QTBAlarmConfigurationWidget::emElementConnected);
        w.setAlarmConfig(mSharedAlarmConfiguration);
    } else {
        w.setEditionMode(QTBAlarmConfigurationWidget::emElementStandAlone);
        w.setAlarmConfig(mExclusiveAlarmConfiguration);
    }
    w.updateUi();

    connect(&w, &QTBAlarmConfigurationWidget::stateChanged, [&w, this](int state){
        if(state == Qt::Checked) {
            w.setEditionMode(QTBAlarmConfigurationWidget::emElementConnected);
            w.setAlarmConfig(mSharedAlarmConfiguration);
            w.updateUi();
        } else {
            w.setEditionMode(QTBAlarmConfigurationWidget::emElementDisconnected);
            w.setAlarmConfig(mExclusiveAlarmConfiguration);
            w.updateUi();
        }
    });

    dialLayout.addWidget(&w);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok
                               | QDialogButtonBox::Cancel);

    connect(&buttonBox, SIGNAL(accepted()),&dial, SLOT(accept()));
    connect(&buttonBox, SIGNAL(rejected()),&dial, SLOT(reject()));
    dialLayout.addWidget(&buttonBox);

    dial.setLayout(&dialLayout);
    int res = dial.exec();
    if( res == QDialog::Accepted) {
        w.updateConfig();
        if(!w.isConnected())
            disconnectAlarmConfig();

        mMainLayout->setRowHeight(rowHeightSpinBox.value());
    }
}

void QTBAlarmPanel::updateElement()
{

}

void QTBAlarmPanel::update(QCPLayoutElement::UpdatePhase phase)
{
    QTBLayoutReactiveElement::update(phase);

    if(phase == upLayout) {
        mMainLayout->setOuterRect(rect());
    }

    mMainLayout->update(phase);
}

void QTBAlarmPanel::updateDashboardParameters(QTBDashboardParameter::UpdateMode mode)
{
    Q_UNUSED(mode)
    for(int i= 0; i< mDashParameters.count();i++) {
        mDashParameters.at(i)->update(QTBDashboardParameter::umValueOnly);

        if(mDashParametersSecondary.at(i))
            mDashParametersSecondary.at(i)->update(QTBDashboardParameter::umValueOnly);
    }
}

void QTBAlarmPanel::processNewSamples()
{
    mMainLayout->takeAllElements();
    mMainLayout->addElement(mHeader);

    for(int i=0;i<64;i++) {
        mTextElements.at(i)->setVisible(false);
    }

    int elementIndex = 0;
    for(int i = 0; i< mAlarmConfiguration->alarmsCount();i++) {
        if(mAlarmConfiguration->active().at(i)) {
            if(mDashParametersPrimary.at(i)->getParameterId() > 0) {
                bool alarmSet = false;
                switch (mAlarmConfiguration->functions().at(i)) {
                case QTBAlarmConfiguration::afGreaterThan:
                    if(mDashParametersPrimary.at(i)->getValueDouble() > mAlarmConfiguration->functionArgs().at(i))
                        alarmSet = true;
                    break;
                case QTBAlarmConfiguration::afLowerThan:
                    if(mDashParametersPrimary.at(i)->getValueDouble() < mAlarmConfiguration->functionArgs().at(i))
                        alarmSet = true;
                    break;
                case QTBAlarmConfiguration::afBitSet:
                    alarmSet = (1 == ((mDashParametersPrimary.at(i)->getValueBinaryWeight32() >> int(mAlarmConfiguration->functionArgs().at(i)) & 1)));
                    break;
                }

                if(alarmSet) {
                    mTextElements.at(elementIndex)->setText(mAlarmConfiguration->messages().at(i));
                    mTextElements.at(elementIndex)->setTextColor(mAlarmConfiguration->colors().at(i));
                    mTextElements.at(elementIndex)->setTextFlags(Qt::AlignVCenter | Qt::AlignLeft);
                    mTextElements.at(elementIndex)->setVisible(true);
                    mTextElements.at(elementIndex)->setBoldText(true);
                    mMainLayout->addElement(mTextElements.at(elementIndex));
                    elementIndex++;
                    if(elementIndex == mTextElements.count())
                        return;

                    if(mDashParametersSecondary.at(i)) {
                        if(mDashParametersSecondary.at(i)->getParameterId() > 0) {
                            mTextElements.at(elementIndex)->setText(QString("     %1 : %2").arg(mDashParametersSecondary.at(i)->getLabel()).arg(mDashParametersSecondary.at(i)->getValueString()));
                            mTextElements.at(elementIndex)->setTextColor(mAlarmConfiguration->colors().at(i));
                            mTextElements.at(elementIndex)->setVisible(true);
                            mTextElements.at(elementIndex)->setBoldText(false);
                            mMainLayout->addElement(mTextElements.at(elementIndex));
                            elementIndex++;
                            if(elementIndex == mTextElements.count())
                                return;
                        }
                    }
                }
            }
        }
    }
}

void QTBAlarmPanel::checkParameters()
{
    if(mBoard->dataManager()) {
        for(int i= 0; i< mDashParameters.count();i++) {
            if(QSharedPointer<QTBParameter> param = mBoard->dataManager()->parameter(mDashParameters.at(i)->getLabel())) {
                mDashParameters.at(i)->setParameterId(param->parameterId());
            } else {
                mDashParameters.at(i)->setParameterId(0);
            }
        }

        for(int i= 0; i< mDashParametersSecondary.count();i++) {
            if(mDashParametersSecondary.at(i)) {
                if(QSharedPointer<QTBParameter> param = mBoard->dataManager()->parameter(mDashParametersSecondary.at(i)->getLabel())) {
                    mDashParametersSecondary.at(i)->setParameterId(param->parameterId());
                } else {
                    mDashParametersSecondary.at(i)->setParameterId(0);
                }
            }
        }
    }
}

void QTBAlarmPanel::saveSettings(QSettings *settings)
{
    settings->beginGroup("List");
    settings->setValue("RowHeight", mMainLayout->rowHeight());
    settings->endGroup();
}

void QTBAlarmPanel::loadSettings(QSettings *settings)
{
    settings->beginGroup("List");
    mMainLayout->setRowHeight(settings->value("RowHeight", mMainLayout->rowHeight()).toInt());
    settings->endGroup();
}

void QTBAlarmPanel::saveConfigurations(QSettings *settings)
{
    settings->beginGroup("Alarms");
    settings->setValue("Connected", mAlarmConfigurationIsShared);
    if(mAlarmConfigurationIsShared) {
        settings->setValue("AlarmName", mSharedAlarmConfiguration->name());
    } else {
        mExclusiveAlarmConfiguration->save(settings);
    }
    settings->endGroup();
}

void QTBAlarmPanel::loadConfigurations(QSettings *settings)
{
    settings->beginGroup("Alarms");
    mAlarmConfigurationIsShared = settings->value("Connected").toBool();
    if(mAlarmConfigurationIsShared) {
        QString alarmName = settings->value("AlarmName").toString();
        QExplicitlySharedDataPointer<QTBAlarmConfiguration> conf = mBoard->project()->alarmConfiguration(alarmName);
        if(conf) {
            addAlarm(conf);
        }
    } else {
        mExclusiveAlarmConfiguration->load(settings);
        mAlarmConfiguration = mExclusiveAlarmConfiguration;
    }
    settings->endGroup();
}

QSharedPointer<QTBDashboardParameter> QTBAlarmPanel::addParameter(QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings)
{
    Q_UNUSED(parameterSettings)
    return nullptr;
}

QSharedPointer<QTBDashboardParameter> QTBAlarmPanel::addParameter(QString paramLabel)
{
    Q_UNUSED(paramLabel)
    return nullptr;
}

QSharedPointer<QTBDashboardParameter> QTBAlarmPanel::addParameter(QSharedPointer<QTBParameter> dataParameter)
{
    Q_UNUSED(dataParameter)
    return nullptr;
}

void QTBAlarmPanel::addAlarm(QExplicitlySharedDataPointer<QTBAlarmConfiguration> alarmConfig)
{
    if(alarmConfig) {
        mAlarmConfigurationIsShared = true;
        mSharedAlarmConfiguration = alarmConfig;
        mExclusiveAlarmConfiguration = alarmConfig;
        mExclusiveAlarmConfiguration.detach();
        mAlarmConfiguration = mSharedAlarmConfiguration;

        updateAlarmConfiguration();
    }
}

void QTBAlarmPanel::updateAlarmConfiguration()
{
    removeAllDashParameter();
    mDashParametersSecondary.clear();

    for(int i = 0; i< mAlarmConfiguration->alarmsCount();i++) {
        QSharedPointer<QTBDashboardParameter> dashParamPrimary = QTBDashboardElement::addParameter(mAlarmConfiguration->watchedParameters().at(i));
        mDashParametersPrimary.append(dashParamPrimary);

        QSharedPointer<QTBDashboardParameter> dashParam;
        if(!mAlarmConfiguration->outputParameters().at(i).isEmpty()) {
            dashParam  =  QSharedPointer<QTBDashboardParameter>(new QTBDashboardParameter(mAlarmConfiguration->outputParameters().at(i), mBoard));
        }
        mDashParametersSecondary.append(dashParam);
    }
    checkParameters();
}

bool QTBAlarmPanel::connected() const
{
    return mAlarmConfigurationIsShared;
}

void QTBAlarmPanel::disconnectAlarmConfig()
{
    mAlarmConfigurationIsShared = false;
    mSharedAlarmConfiguration.reset();
    mAlarmConfiguration = mExclusiveAlarmConfiguration;
}

void QTBAlarmPanel::beforeReplot()
{
    if(mAlarmConfiguration->modified())
        updateAlarmConfiguration();

    updateDashboardParameters();

    if(mBoard->liveDataRefreshEnabled())
        processNewSamples();
}

void QTBAlarmPanel::afterReplot()
{
    mAlarmConfiguration->setModified(false);
}
