#include "single_display.h"

QTBSingleDisplay::QTBSingleDisplay(QTBoard *dashboard) :
    QTBDashboardElement (dashboard),
    mTextLabel(nullptr),
    mTextUnit(nullptr),
    mTextValue(nullptr),
    mSpacing(1),
    mHeaderVisible(true),
    mValueVisible(true)
{
    setMargins(QMargins(2,2,2,2));
    setSizeConstraintRect(scrOuterRect);
    setConfigurationMode(QTBParameterConfiguration::cmValue);
}

void QTBSingleDisplay::clearElement()
{
    
    delete mTextValue;
    delete mTextUnit;
    delete mTextLabel;
    delete mEmptyElementBottom;
    delete mEmptyElementTop;
    delete mSubLayout;
    delete mMainLayout;
}

void QTBSingleDisplay::initializeElement(QTBoard *dashboard)
{
    if(dashboard) {
        QTBDashboardElement::initializeElement(dashboard);

        mTextLabel = new QTBAdjustTextElement(dashboard);
        QFont f = mTextLabel->font();
        f.setBold(true);
        mTextLabel->setFont(f);
        mTextLabel->setTextFlags(Qt::AlignCenter);
        mTextLabel->setMaxPointSize(11);
        mTextLabel->setMinPointSize(1);

        mTextUnit = new QTBAdjustTextElement(dashboard);
        mTextUnit->setTextFlags(Qt::AlignCenter);
        mTextUnit->setMaxPointSize(8);
        mTextUnit->setMinPointSize(1);
//        mTextUnit->setTextColor(dashboard->frontColor());

        mTextValue = new QTBAdjustTextElement(dashboard);
        mTextValue->setMaxPointSize(24);
        mTextValue->setMinPointSize(1);
        mTextValue->setMargins(QMargins(2,2,2,2));
        mTextValue->setTextFlags(Qt::AlignCenter);

        mEmptyElementBottom = new QCPLayoutElement(dashboard);
        mEmptyElementBottom->setMinimumSize(0.1,0.1);

        mEmptyElementTop = new QCPLayoutElement(dashboard);
        mEmptyElementTop->setMinimumSize(0.1,0.1);

        mSubLayout = new QTBLayoutGrid();
        mSubLayout->initializeParentPlot(dashboard);
        mSubLayout->setParentLayerable(this);

        mMainLayout = new QTBLayoutGrid();
        mMainLayout->initializeParentPlot(dashboard);
        mMainLayout->setParentLayerable(this);

        buildLayout();
    }
}

void QTBSingleDisplay::buildLayout()
{

}

void QTBSingleDisplay::edit()
{

}

void QTBSingleDisplay::update(QCPLayoutElement::UpdatePhase phase)
{
    QTBLayoutReactiveElement::update(phase);

    if(phase == upLayout) {
        updateSizeConstraints();
        mMainLayout->setOuterRect(rect());
    }

    mMainLayout->update(phase);

    if(phase == upLayout) {
        checkSizeAndVisibility();
        mSubLayout->updateLayout();
        mMainLayout->updateLayout();
    }
}

int QTBSingleDisplay::defaultWidth()
{
    return mBoard->dashboardLayout()->singleElementColumnCount();
}

int QTBSingleDisplay::defaultHeight()
{
    return mBoard->dashboardLayout()->singleElementRowCount();
}

void QTBSingleDisplay::loadSettings(QSettings *settings)
{
    settings->beginGroup("DisplaySettings");
    if(settings->contains("ValueVisible"))
        mValueVisible = settings->value("ValueVisible").toBool();
    if(settings->contains("HeaderVisible"))
        mHeaderVisible = settings->value("HeaderVisible").toBool();
    if(settings->contains("TransparentBackground"))
        mTransparentBackground = settings->value("TransparentBackground").toBool();
    settings->endGroup();
}

void QTBSingleDisplay::saveSettings(QSettings *settings)
{
    settings->beginGroup("DisplaySettings");
    settings->setValue("ValueVisible", mValueVisible);
    settings->setValue("HeaderVisible", mHeaderVisible);
    settings->setValue("TransparentBackground", mTransparentBackground);
    settings->endGroup();
}

void QTBSingleDisplay::processNewSamples()
{

}

void QTBSingleDisplay::updateElement()
{
    QSharedPointer<QTBDashboardParameter> dashParam = dashParameter(0);
    if(dashParam) {
        mTextLabel->setText(dashParam->getDisplayedLabel());
        mTextUnit->setText(dashParam->getDisplayedUnit());
    } else {
        mTextLabel->setText("-");
        mTextUnit->setText("-");
    }

    for(int i=0; i<mSubLayout->elementCount(); i++)
        mSubLayout->takeAt(i);
    mSubLayout->simplify();

    for(int i=0; i<mMainLayout->elementCount(); i++)
        mMainLayout->takeAt(i);
    mMainLayout->simplify();

    buildLayout();
}

void QTBSingleDisplay::updateSizeConstraints()
{

}

void QTBSingleDisplay::checkSizeAndVisibility()
{

}

bool QTBSingleDisplay::headerVisible() const
{
    return mHeaderVisible;
}

void QTBSingleDisplay::setHeaderVisible(bool headerVisible)
{
    if(mHeaderVisible != headerVisible) {
        mHeaderVisible = headerVisible;
        updateElement();
    }
}

bool QTBSingleDisplay::valueVisible() const
{
    return mValueVisible;
}

void QTBSingleDisplay::setValueVisible(bool valueVisible)
{
    if(mValueVisible != valueVisible) {
        mValueVisible = valueVisible;
        updateElement();
    }
}
