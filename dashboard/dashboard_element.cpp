#include "dashboard_element.h"

#include <utility>

QTBDashboardElement::QTBDashboardElement(QTBoard *dashboard):
    QTBLayoutReactiveElement (dashboard),
    mBoard(nullptr),
    mParametersMaxCount(1),
    mConfigurationMode(QTBParameterConfiguration::cmFull)
{

}

QSharedPointer<QTBDashboardParameter> QTBDashboardElement::addParameter(QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings)
{
    QSharedPointer<QTBDashboardParameter> dashParam;
    if(!mParametersLabel.contains(parameterSettings->label())) {
        dashParam =  QSharedPointer<QTBDashboardParameter>(new QTBDashboardParameter(std::move(parameterSettings), mBoard));
        addParameter(dashParam);
    } else {
        int index = mParametersLabel.indexOf(parameterSettings->label());
        dashParam = mDashParameters.at(index);
    }
    return dashParam;
}

QSharedPointer<QTBDashboardParameter> QTBDashboardElement::addParameter(QString paramLabel)
{
    QSharedPointer<QTBDashboardParameter> dashParam;
    if(!mParametersLabel.contains(paramLabel)) {
        dashParam =  QSharedPointer<QTBDashboardParameter>(new QTBDashboardParameter(paramLabel, mBoard));
        addParameter(dashParam);
    } else {
        int index = mParametersLabel.indexOf(paramLabel);
        dashParam = mDashParameters.at(index);
    }
    return dashParam;
}

QSharedPointer<QTBDashboardParameter> QTBDashboardElement::addParameter(QSharedPointer<QTBParameter> dataParameter)
{
    QSharedPointer<QTBDashboardParameter> dashParam;
    if(!mParametersLabel.contains(dataParameter->label())) {
        dashParam =  QSharedPointer<QTBDashboardParameter>(new QTBDashboardParameter(dataParameter, mBoard));
        addParameter(dashParam);
    } else {
        int index = mParametersLabel.indexOf(dataParameter->label());
        dashParam = mDashParameters.at(index);
    }
    return dashParam;
}

void QTBDashboardElement::addParameter(QSharedPointer<QTBDashboardParameter> dashParameter)
{
    if(mDashParameters.contains(dashParameter))
        return;

    if(mDashParameters.count() < mParametersMaxCount) {
        mDashParameters.append(dashParameter);
        mParametersLabel.append(dashParameter->getLabel());
    } else if (mDashParameters.count() == mParametersMaxCount) {
        QSharedPointer<QTBDashboardParameter> toRemove = mDashParameters.takeLast();
        mParametersLabel.removeAll(toRemove->getLabel());
        mDashParameters.append(dashParameter);
        mParametersLabel.append(dashParameter->getLabel());
    }

    checkParameters();
    updateElement();
}

void QTBDashboardElement::updateDashboardParameters(QTBDashboardParameter::UpdateMode mode)
{
    for(int i= 0; i< mDashParameters.count();i++) {
        mDashParameters.at(i)->update(mode);
    }
}

void QTBDashboardElement::checkParameters()
{
    for(int i= 0; i< mDashParameters.count();i++) {
        QSharedPointer<QTBParameter> param;
        if(mBoard->dataManager())
            param = mBoard->dataManager()->parameter(mDashParameters.at(i)->getLabel());

        if(param){
            mDashParameters.at(i)->setUnit(param->unit());
            mDashParameters.at(i)->setParameterId(param->parameterId());
        } else {
            mDashParameters.at(i)->setUnit(QString());
            mDashParameters.at(i)->setParameterId(0);
        }
    }
}

void QTBDashboardElement::beforeReplot()
{
    for(int i= 0; i< mDashParameters.count();i++) {
        if(mDashParameters.at(i)->configurationHasChanged()) {
            updateElement();
            break;
        }
    }

    updateDashboardParameters();

    if(mBoard->liveDataRefreshEnabled())
        processNewSamples();
}

void QTBDashboardElement::afterReplot()
{
    for(int i= 0; i< mDashParameters.count();i++) {
        mDashParameters.at(i)->modificationsApplied();
    }
}

QTBDashboardElement::ElementType QTBDashboardElement::type() const
{
    return mType;
}

void QTBDashboardElement::setType(const ElementType &type)
{
    mType = type;
}

QTBParameterConfiguration::ConfigurationMode QTBDashboardElement::configurationMode() const
{
    return mConfigurationMode;
}

void QTBDashboardElement::setConfigurationMode(const QTBParameterConfiguration::ConfigurationMode &configurationMode)
{
    mConfigurationMode = configurationMode;
}

void QTBDashboardElement::initializeElement(QTBoard *dashboard)
{
    if(!mBoard)
        mBoard = dashboard;

    connect(mBoard, &QTBoard::beforeReplot, this, &QTBDashboardElement::beforeReplot);
    connect(mBoard, &QTBoard::afterReplot, this, &QTBDashboardElement::afterReplot);

    QTBLayoutReactiveElement::initializeElement(dashboard);
}

void QTBDashboardElement::droppedParameter(QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings)
{
    addParameter(std::move(parameterSettings));
}

void QTBDashboardElement::droppedParameter(QSharedPointer<QTBParameter> dataParameter)
{
    addParameter(std::move(dataParameter));
}

int QTBDashboardElement::parametersMaxCount() const
{
    return mParametersMaxCount;
}

void QTBDashboardElement::setParametersMaxCount(int parametersMaxCount)
{
    mParametersMaxCount = parametersMaxCount;
}

void QTBDashboardElement::saveConfigurations(QSettings *settings)
{
    settings->beginWriteArray("Parameters");

    for(int i= 0; i< mDashParameters.count();i++) {
        settings->setArrayIndex(i);
        mDashParameters.at(i)->saveParameterSettings(settings, mConfigurationMode);
    }
    settings->endArray();
}

void QTBDashboardElement::loadConfigurations(QSettings *settings)
{
    int paramCount = settings->beginReadArray("Parameters");

    for(int i= 0; i< paramCount;i++) {
        settings->setArrayIndex(i);
        QSharedPointer<QTBDashboardParameter> dashParam =  QSharedPointer<QTBDashboardParameter>(new QTBDashboardParameter(mBoard));
        mDashParameters.append(dashParam);
        dashParam->loadParameterSettings(settings, mConfigurationMode);
    }
    settings->endArray();

    checkParameters();
    updateElement();
}

QSharedPointer<QTBDashboardParameter> QTBDashboardElement::dashParameter(int index)
{
    if(mDashParameters.count() > 0 &&
            index < mDashParameters.count()) {
        return mDashParameters[index];
    }

    return nullptr;
}

void QTBDashboardElement::removeDashParameter(int index)
{
    QSharedPointer<QTBDashboardParameter> dashParam = mDashParameters.takeAt(index);
    mParametersLabel.removeAll(dashParam->getLabel());
    updateElement();
}

void QTBDashboardElement::removeAllDashParameter()
{
    mDashParameters.clear();
    updateElement();
}
