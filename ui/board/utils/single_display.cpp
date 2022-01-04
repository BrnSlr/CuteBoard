#include "single_display.h"

SingleDisplay::SingleDisplay(Board *dashboard) :
    BoardElement (dashboard),
    mTextLabel(nullptr),
    mTextUnit(nullptr),
    mTextValue(nullptr),
    mSpacing(1),
    mHeaderVisible(true),
    mValueVisible(true)
{
    setConfigurationMode(ParameterConfiguration::cmValue);
}

void SingleDisplay::clearElement()
{    
    delete mTextValue;
    delete mTextUnit;
    delete mTextLabel;
    delete mEmptyElementBottom;
    delete mEmptyElementTop;
    delete mSubLayout;
    delete mMainLayout;
}

void SingleDisplay::initialize(Board *dashboard)
{
    if(dashboard) {
        BoardElement::initialize(dashboard);

        mTextLabel = new AdaptiveTextElement(dashboard);
        QFont f = mTextLabel->font();
        f.setBold(true);
        mTextLabel->setFont(f);
        mTextLabel->setTextFlags(Qt::AlignCenter);
        mTextLabel->setMaxPointSize(10);
        mTextLabel->setMinPointSize(1);
        mTextLabel->setLayer(QLatin1String("main"));
        mTextLabel->setCachedPixmap(false);
        mTextLabel->setLayer(QLatin1String("elements_background"));

        mTextUnit = new AdaptiveTextElement(dashboard);
        mTextUnit->setTextFlags(Qt::AlignCenter);
        mTextUnit->setMaxPointSize(9);
        mTextUnit->setMinPointSize(1);
        mTextUnit->setLayer(QLatin1String("main"));
        mTextUnit->setCachedPixmap(false);
        mTextUnit->setLayer(QLatin1String("elements_background"));

        mTextValue = new AdaptiveTextElement(dashboard);
        mTextValue->setMaxPointSize(64);
        mTextValue->setMinPointSize(1);
        mTextValue->setTextFlags(Qt::AlignCenter);
        mTextValue->setLayer(QLatin1String("main"));

        mEmptyElementBottom = new QCPLayoutElement(dashboard);
        mEmptyElementTop = new QCPLayoutElement(dashboard);

        mSubLayout = new LayoutGrid();
        mSubLayout->initializeParentPlot(dashboard);
        mSubLayout->setParentLayerable(this);
        mSubLayout->setLayer(QLatin1String("main"));

        mMainLayout = new LayoutGrid();
        mMainLayout->initializeParentPlot(dashboard);
        mMainLayout->setParentLayerable(this);
        mMainLayout->setLayer(QLatin1String("main"));
        mMainLayout->setMargins(QMarginsF(4,4,4,4));
    }
}

void SingleDisplay::resetElement()
{
    for(int i=0; i<mSubLayout->elementCount(); i++)
        mSubLayout->takeAt(i);
    mSubLayout->simplify();

    for(int i=0; i<mMainLayout->elementCount(); i++)
        mMainLayout->takeAt(i);
    mMainLayout->simplify();
}

void SingleDisplay::buildElement()
{

}

void SingleDisplay::edit()
{

}

void SingleDisplay::checkParameters()
{
    BoardElement::checkParameters();

    QSharedPointer<BoardParameter> dashParam = boardParameter(0);
    if(dashParam) {
        mTextLabel->setText(dashParam->getDisplayedLabel());
        mTextUnit->setText(dashParam->getDisplayedUnit());
        if(dashParam->getTimeSeries().parameterId() == 0) {
            mTextValue->setText("X");
            mTextValue->setTextColor(mTextLabel->textColor());
            mTextValue->setBackgroundBrush(mTextLabel->backgroundBrush());
        }
    } else {
        mTextLabel->setText("-");
        mTextUnit->setText("-");
        mTextValue->setText("X");
        mTextValue->setTextColor(mTextLabel->textColor());
        mTextValue->setBackgroundBrush(mTextLabel->backgroundBrush());
    }
}

void SingleDisplay::update(QCPLayoutElement::UpdatePhase phase)
{
    BoardElement::update(phase);

    if(phase == upLayout) {
        mMainLayout->setOuterRect(rect());
        applySizeConstraints();
    }

    mMainLayout->update(phase);
}

int SingleDisplay::defaultWidth()
{
    return mBoard->dashboardLayout()->singleElementColumnCount();
}

int SingleDisplay::defaultHeight()
{
    return mBoard->dashboardLayout()->singleElementRowCount();
}

void SingleDisplay::loadSettings(QSettings *settings)
{
    settings->beginGroup("DisplaySettings");
    if(settings->contains("ValueVisible"))
        mValueVisible = settings->value("ValueVisible").toBool();
    if(settings->contains("HeaderVisible"))
        mHeaderVisible = settings->value("HeaderVisible").toBool();
    if(settings->contains("BackgroundMode")) {
        auto maxVal = QMetaEnum::fromType<BackgroundMode>().keyCount();
        auto val = settings->value("BackgroundMode").toInt();
        if(val <   maxVal)
            setBackgroundMode(BackgroundMode(val));
    }
    settings->endGroup();
}

void SingleDisplay::saveSettings(QSettings *settings)
{
    settings->beginGroup("DisplaySettings");
    settings->setValue("ValueVisible", mValueVisible);
    settings->setValue("HeaderVisible", mHeaderVisible);
    settings->setValue("BackgroundMode", mBackgroundMode);
    settings->endGroup();
}

void SingleDisplay::loadData()
{

}

void SingleDisplay::displayData()
{

}

void SingleDisplay::applySizeConstraints()
{

}

bool SingleDisplay::headerVisible() const
{
    return mHeaderVisible;
}

void SingleDisplay::setHeaderVisible(bool headerVisible)
{
    if(mHeaderVisible != headerVisible) {
        mHeaderVisible = headerVisible;
    }
}

bool SingleDisplay::valueVisible() const
{
    return mValueVisible;
}

void SingleDisplay::setValueVisible(bool valueVisible)
{
    if(mValueVisible != valueVisible) {
        mValueVisible = valueVisible;
    }
}
