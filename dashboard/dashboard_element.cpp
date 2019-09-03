#include "dashboard_element.h"

#include <utility>

QTBDashboardElement::QTBDashboardElement(QTBoard *dashboard):
    QTBLayoutReactiveElement (dashboard),
    mBoard(nullptr),
    mParametersMaxCount(1)
{

}

QSharedPointer<QTBDashboardParameter> QTBDashboardElement::addParameter(QExplicitlySharedDataPointer<QTBParameterConfiguration> parameterSettings)
{
    QSharedPointer<QTBDashboardParameter> dashParam =  QSharedPointer<QTBDashboardParameter>(new QTBDashboardParameter(std::move(parameterSettings), mBoard));
    addDashboardParameter(dashParam);
    return dashParam;
}

QSharedPointer<QTBDashboardParameter> QTBDashboardElement::addParameter(QString paramLabel)
{
    QSharedPointer<QTBDashboardParameter> dashParam =  QSharedPointer<QTBDashboardParameter>(new QTBDashboardParameter(paramLabel, mBoard));
    addDashboardParameter(dashParam);
    return dashParam;
}

QSharedPointer<QTBDashboardParameter> QTBDashboardElement::addParameter(QSharedPointer<QTBParameter> dataParameter)
{
    QSharedPointer<QTBDashboardParameter> dashParam =  QSharedPointer<QTBDashboardParameter>(new QTBDashboardParameter(dataParameter, mBoard));
    addDashboardParameter(dashParam);
    return dashParam;
}

void QTBDashboardElement::addDashboardParameter(QSharedPointer<QTBDashboardParameter> dashParameter)
{
    if(mDashParameters.contains(dashParameter))
        return;

    if(mDashParameters.count() < mParametersMaxCount) {
        mDashParameters.append(dashParameter);
    } else if (mDashParameters.count() == mParametersMaxCount) {
        mDashParameters.removeLast();
        mDashParameters.append(dashParameter);
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
        if(mDashParameters.at(i)->configurationChanged()) {
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
        mDashParameters.at(i)->propertiesChecked();
    }
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

void QTBDashboardElement::saveParametersSettings(QSettings *settings, QTBParameterConfiguration::ConfigurationModule mode)
{
    settings->beginWriteArray("Parameters");

    for(int i= 0; i< mDashParameters.count();i++) {
        settings->setArrayIndex(i);
        mDashParameters.at(i)->saveParameterSettings(settings, mode);
    }
    settings->endArray();
}

void QTBDashboardElement::loadParametersSettings(QSettings *settings, QTBParameterConfiguration::ConfigurationModule mode)
{
    int paramCount = settings->beginReadArray("Parameters");

    for(int i= 0; i< paramCount;i++) {
        settings->setArrayIndex(i);
        QSharedPointer<QTBDashboardParameter> dashParam =  QSharedPointer<QTBDashboardParameter>(new QTBDashboardParameter(mBoard));
        mDashParameters.append(dashParam);
        dashParam->loadParameterSettings(settings, mode);
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
    mDashParameters.removeAt(index);
    updateElement();
}

void QTBDashboardElement::removeAllDashParameter()
{
    mDashParameters.clear();
    updateElement();
}
