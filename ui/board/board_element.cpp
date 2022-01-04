#include "board_element.h"
#include <utility>

BoardElement::BoardElement(Board *dashboard):
    QCPLayoutElement (dashboard),
    mBackgroundMode(bmBackground),
    mBoard(dashboard),
    mParametersMaxCount(1),
    mType(etUnknown),
    mTimeSeriesType(tstUnknown),
    mTimeSeriesSize(tssUnknown),
    mStatModeEnabled(false),
    mMouseInteractionsEnabled(false),
    mMainElement(true),
    mConfigurationMode(ParameterConfiguration::cmFull),
    mParameterColorIndex(1)
{    
    setMargins(QMargins(2,2,2,2));
    setSizeConstraintRect(scrOuterRect);
}

BoardElement::~BoardElement()
{
    removeBoardParameters();
    clearElement();
}

QSharedPointer<BoardParameter> BoardElement::addParameter(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings)
{
    QSharedPointer<BoardParameter> boardParam;
    if(!mParametersLabel.contains(parameterSettings->label())) {
        boardParam =  QSharedPointer<BoardParameter>(new BoardParameter(std::move(parameterSettings), mBoard));

        if(mBoard->dataManager()) {
            auto dataParam = mBoard->dataManager()->dataParameter(boardParam->getTimeSeries().name());
            if(dataParam) {
                boardParam->setTimeSeries(*dataParam.data());
            }
        }

        addParameter(boardParam);
    } else {
        int index = mParametersLabel.indexOf(parameterSettings->label());
        boardParam = mBoardParameters.at(index);
        boardParam->setSharedParameterConfiguration(parameterSettings, true);
    }
    return boardParam;
}

QSharedPointer<BoardParameter> BoardElement::addParameter(QString paramLabel)
{
    QSharedPointer<BoardParameter> boardParam;
    if(!mParametersLabel.contains(paramLabel)) {

        boardParam =  QSharedPointer<BoardParameter>(new BoardParameter(paramLabel, mBoard));
        boardParam->parameterConfiguration()->defaultColorSettingsRef().setColor(mBoard->randomColor(mParameterColorIndex));

        if(mBoard->dataManager()) {
            auto dataParam = mBoard->dataManager()->dataParameter(boardParam->getTimeSeries().name());
            if(dataParam) {
                boardParam->setTimeSeries(*dataParam.data());
            }
        }

        addParameter(boardParam);
    } else {
        int index = mParametersLabel.indexOf(paramLabel);
        boardParam = mBoardParameters.at(index);
    }
    return boardParam;
}

QSharedPointer<BoardParameter> BoardElement::addParameter(QSharedPointer<TimeSeries> dataParameter)
{
    QSharedPointer<BoardParameter> boardParam;
    if(!mParametersLabel.contains(dataParameter->name())) {
        boardParam =  QSharedPointer<BoardParameter>(new BoardParameter(dataParameter, mBoard));
        boardParam->parameterConfiguration()->defaultColorSettingsRef().setColor(mBoard->randomColor(mParameterColorIndex));

        addParameter(boardParam);
    } else {
        int index = mParametersLabel.indexOf(dataParameter->name());
        boardParam = mBoardParameters.at(index);
    }
    return boardParam;
}

void BoardElement::addParameter(QSharedPointer<BoardParameter> boardParameter)
{
    if(mBoardParameters.contains(boardParameter)) {
        logger()->info() << name() << " " << boardParameter->getTimeSeries().name() << " already added" ;
        return;
    } else {
        mParameterColorIndex++;
        if(mBoardParameters.count() < mParametersMaxCount) {
            mBoardParameters.append(boardParameter);
            mParametersLabel.append(boardParameter->getTimeSeries().name());
            mBoard->dataManager()->registerBoardParameterId(boardParameter->getTimeSeries().parameterId());
        } else if (mBoardParameters.count() == mParametersMaxCount) {
            auto toRemove = mBoardParameters.takeLast();

            mParametersLabel.removeAll(toRemove->getTimeSeries().name());
            mBoardParameters.append(boardParameter);
            mParametersLabel.append(boardParameter->getTimeSeries().name());
            mBoard->dataManager()->unregisterBoardParameterId(toRemove->getTimeSeries().parameterId());
            mBoard->dataManager()->registerBoardParameterId(boardParameter->getTimeSeries().parameterId());
        }
    }
}

void BoardElement::loadData()
{
    for(int i= 0; i< mBoardParameters.count();i++)
        mBoardParameters.at(i)->updateData();
}

void BoardElement::loadHistoricalData()
{
    for(int i= 0; i< mBoardParameters.count();i++)
        mBoardParameters.at(i)->updateData();
}

void BoardElement::displayData()
{

}

void BoardElement::update(QCPLayoutElement::UpdatePhase phase)
{
    QCPLayoutElement::update(phase);
}

void BoardElement::checkParameters()
{
    for(auto boardParam : mBoardParameters) {
        quint32 id = boardParam->getTimeSeries().parameterId();
        if(id > 0) {
            if(mBoard->dataManager()) {
                auto dataParam = mBoard->dataManager()->dataParameter(boardParam->getTimeSeries().name());
                if(dataParam) {
                    auto dataId = dataParam->parameterId();
                    if(dataId != id) {
                        mBoard->dataManager()->registerBoardParameterId(dataId);
                        mBoard->dataManager()->unregisterBoardParameterId(id);

                        boardParam->setTimeSeries(*dataParam.data());
                    }
                } else {
                    mBoard->dataManager()->unregisterBoardParameterId(id);
                    TimeSeriesParameter param;
                    param.setName(boardParam->getTimeSeries().name());
                    boardParam->setTimeSeries(param);
                }
            }
        } else {
            if(mBoard->dataManager()) {
                auto dataParam = mBoard->dataManager()->dataParameter(boardParam->getTimeSeries().name());
                if(dataParam) {
                    auto dataId = dataParam->parameterId();
                    mBoard->dataManager()->registerBoardParameterId(dataId);
                    boardParam->setTimeSeries(*dataParam.data());
                }
            }
        }
    }
}

void BoardElement::resetModifications()
{
    for(int i= 0; i< mBoardParameters.count();i++)
        mBoardParameters.at(i)->modificationsApplied();
}

bool BoardElement::statModeEnabled() const
{
    return mStatModeEnabled;
}

void BoardElement::setStatModeEnabled(bool statModeEnabled)
{
    if(statModeEnabled != mStatModeEnabled) {
        mStatModeEnabled = statModeEnabled;
        rebuildElement();
    }
}

bool BoardElement::mouseInteractionsEnabled() const
{
    return mMouseInteractionsEnabled;
}

void BoardElement::setMouseInteractionsEnabled(bool mouseInteractionsEnabled)
{
    if(mouseInteractionsEnabled != mMouseInteractionsEnabled) {
        mMouseInteractionsEnabled = mouseInteractionsEnabled;
        rebuildElement();
    }
}

void BoardElement::setBoardLiveState(bool isLive)
{
    Q_UNUSED(isLive)
}

void BoardElement::setMainElement(bool mainElement)
{
    mMainElement = mainElement;
}


void BoardElement::draw(QCPPainter *painter)
{

    switch(mBackgroundMode) {
    case bmBackground:
        //                painter->setBrush(mDefaultBrush);
        //                painter->setPen(mBorderPen);
        //                painter->drawRoundedRect(mRect, 4 , 4);
        painter->setBrush(mHGradientBrush);
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(mRect, 4 , 4);
        painter->setBrush(mVGradientBrush);
        painter->setPen(mBorderPen);
        painter->drawRoundedRect(mRect, 4 , 4);
        break;
    case bmTransparent:
        break;
    }
}

Board *BoardElement::board() const
{
    return mBoard;
}

QString BoardElement::name() const
{
    return mName;
}

void BoardElement::setName(const QString &name)
{
    mName = name;
}

BoardElement::TimeSeriesSize BoardElement::timeSeriesSize() const
{
    return mTimeSeriesSize;
}

void BoardElement::setTimeSeriesSize(const TimeSeriesSize &timeSeriesSize)
{
    mTimeSeriesSize = timeSeriesSize;
}

BoardElement::TimeSeriesType BoardElement::timeSeriesType() const
{
    return mTimeSeriesType;
}

void BoardElement::setTimeSeriesType(const TimeSeriesType &timeSeriesType)
{
    mTimeSeriesType = timeSeriesType;
}

BoardElement::BackgroundMode BoardElement::backgroundMode() const
{
    return mBackgroundMode;
}

void BoardElement::setBackgroundMode(const BackgroundMode &backgroundMode)
{
    mBackgroundMode = backgroundMode;
}

BoardElement::ElementType BoardElement::type() const
{
    return mType;
}

void BoardElement::setType(const ElementType &type)
{
    mType = type;
}

void BoardElement::beforeDataUpdate(bool refresh)
{
    for(int i= 0; i< mBoardParameters.count();i++) {
        if(mBoardParameters.at(i)->configurationHasChanged() || refresh) {
            rebuildElement();
            break;
        }
    }
}

ParameterConfiguration::ConfigurationMode BoardElement::configurationMode() const
{
    return mConfigurationMode;
}

void BoardElement::setConfigurationMode(const ParameterConfiguration::ConfigurationMode &configurationMode)
{
    mConfigurationMode = configurationMode;
}

void BoardElement::initialize(Board *dashboard)
{
    if(!parentPlot())
        initializeParentPlot(dashboard);
    mBoard = dashboard;
    setLayer(QLatin1String("elements_background"));

    QColor color;
    QColor borderColor;
    color = QApplication::palette().mid().color();
    borderColor = QApplication::palette().midlight().color();

    mDefaultBrush = QBrush(color);
    mBorderPen = QPen(borderColor, 0, Qt::SolidLine, Qt::SquareCap);


    QLinearGradient h_gradient(0,0,1,0);
    h_gradient.setColorAt(0, QApplication::palette().dark().color());
    h_gradient.setColorAt(1, QApplication::palette().mid().color());
    h_gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    mHGradientBrush = QBrush(h_gradient);

    QLinearGradient v_gradient(0,0,0,1);
    v_gradient.setColorAt(0, Qt::transparent);
    v_gradient.setColorAt(1, QApplication::palette().mid().color());
    v_gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    mVGradientBrush = QBrush(v_gradient);
}

void BoardElement::rebuildElement(bool replot)
{
    resetElement();
    buildElement();
    checkParameters();
    displayData();
    if(replot)
        mBoard->replot(QCustomPlot::rpQueuedReplot);
}

void BoardElement::droppedParameter(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings)
{
    addParameter(std::move(parameterSettings));
    rebuildElement(false);
}

void BoardElement::droppedParameter(QSharedPointer<TimeSeries> dataParameter, bool replace)
{
    if(replace)
        replaceParameter(0, std::move(dataParameter));
    else
        addParameter(std::move(dataParameter));
    rebuildElement(false);
}

int BoardElement::parametersMaxCount() const
{
    return mParametersMaxCount;
}

void BoardElement::setParametersMaxCount(int parametersMaxCount)
{
    mParametersMaxCount = parametersMaxCount;
}

void BoardElement::saveConfigurations(QSettings *settings)
{
    settings->beginWriteArray("Parameters");
    for(int i= 0; i< mBoardParameters.count();i++) {
        settings->setArrayIndex(i);
        mBoardParameters.at(i)->saveParameterSettings(settings, mConfigurationMode);
    }
    settings->endArray();
}

void BoardElement::loadConfigurations(QSettings *settings)
{
    int paramCount = settings->beginReadArray("Parameters");
    for(int i= 0; i< paramCount;i++) {
        settings->setArrayIndex(i);
        auto boardParam =  QSharedPointer<BoardParameter>(new BoardParameter(mBoard));
        boardParam->loadParameterSettings(settings, mConfigurationMode);
        if(mBoard->dataManager()) {
            auto dataParam = mBoard->dataManager()->dataParameter(boardParam->getTimeSeries().name());
            if(dataParam) {
                boardParam->setTimeSeries(*dataParam.data());
            }
        }
        BoardElement::addParameter(boardParam);
    }
    settings->endArray();
    rebuildElement();
}

QSharedPointer<BoardParameter> BoardElement::boardParameter(int index)
{
    if(mBoardParameters.count() > 0 &&
            index < mBoardParameters.count()) {
        return mBoardParameters[index];
    }
    return nullptr;
}

QSharedPointer<BoardParameter> BoardElement::boardParameter(QString label)
{
    if(mBoardParameters.count()) {
        auto index = mParametersLabel.indexOf(label);
        if(index >= 0 && index < mBoardParameters.count())
            return mBoardParameters[index];
    }
    return nullptr;
}

void BoardElement::removeBoardParameter(int index, bool update)
{
    auto boardParam = mBoardParameters.takeAt(index);
    if(mBoard->dataManager())
        mBoard->dataManager()->unregisterBoardParameterId(boardParam->getTimeSeries().parameterId());
    mParametersLabel.removeAll(boardParam->getTimeSeries().name());
    if(update)
        rebuildElement();
}

void BoardElement::removeBoardParameter(QString label, bool update)
{
    qDebug() << Q_FUNC_INFO << label << update;
    auto index = mParametersLabel.indexOf(label);
    qDebug() << Q_FUNC_INFO << index << mBoardParameters.count();
    if(index >= 0 && index < mBoardParameters.count()) {
        auto boardParam = mBoardParameters.takeAt(index);
        if(boardParam) {
            if(mBoard->dataManager())
                mBoard->dataManager()->unregisterBoardParameterId(boardParam->getTimeSeries().parameterId());
            mParametersLabel.removeAll(boardParam->getTimeSeries().name());
            if(update)
                rebuildElement();
        } else {
            qDebug() << Q_FUNC_INFO << "No parameter for label" << label;
        }
    }
}

void BoardElement::removeBoardParameters()
{
    for(int i= mBoardParameters.count() -1; i>=0;i--)
        removeBoardParameter(i, false);
    mParametersLabel.clear();
    mBoardParameters.clear();
    rebuildElement();
}

QSharedPointer<BoardParameter> BoardElement::replaceParameter(int index, QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings)
{
    QSharedPointer<BoardParameter> boardParam;
    if(!mParametersLabel.contains(parameterSettings->label())) {
        if (index < mBoardParameters.count() && index >= 0) {
            boardParam =  QSharedPointer<BoardParameter>(new BoardParameter(parameterSettings, mBoard));

            if(mBoard->dataManager()) {
                auto dataParam = mBoard->dataManager()->dataParameter(boardParam->getTimeSeries().name());
                if(dataParam) {
                    boardParam->setTimeSeries(*dataParam.data());
                }
            }

            replaceParameter(index, boardParam);
        }

    } else {
        boardParam = mBoardParameters.at(index);
    }
    return boardParam;
}

QSharedPointer<BoardParameter> BoardElement::replaceParameter(int index, QString paramLabel)
{
    QSharedPointer<BoardParameter> boardParam;
    if(!mParametersLabel.contains(paramLabel)) {
        if (index < mBoardParameters.count() && index >= 0) {
            boardParam =  QSharedPointer<BoardParameter>(new BoardParameter(paramLabel, mBoard));

            if(mBoard->dataManager()) {
                auto dataParam = mBoard->dataManager()->dataParameter(boardParam->getTimeSeries().name());
                if(dataParam) {
                    boardParam->setTimeSeries(*dataParam.data());
                }
            }

            auto conf = mBoardParameters.at(index)->parameterConfiguration();
            conf->setLabel(paramLabel);
            conf->setUserUnitEnabled(false);
            conf->setUserLabelEnabled(false);
            conf->setUserDefinedUnit("");
            conf->setUserDefinedLabel("");
            conf.detach();
            boardParam->setExclusiveParameterConfiguration(conf);
            boardParam->disconnectSharedConfiguration(true);

            replaceParameter(index, boardParam);
        }

    } else {
        boardParam = mBoardParameters.at(index);
    }
    return boardParam;
}

QSharedPointer<BoardParameter> BoardElement::replaceParameter(int index, QSharedPointer<TimeSeries> dataParameter)
{
    QSharedPointer<BoardParameter> boardParam;
    if(!mParametersLabel.contains(dataParameter->name())) {
        if (index < mBoardParameters.count() && index >= 0) {
            boardParam =  QSharedPointer<BoardParameter>(new BoardParameter(dataParameter, mBoard));

            auto conf = mBoardParameters.at(index)->parameterConfiguration();
            conf->setLabel(dataParameter->name());
            conf->setUserUnitEnabled(false);
            conf->setUserLabelEnabled(false);
            conf->setUserDefinedUnit("");
            conf->setUserDefinedLabel("");
            conf.detach();
            boardParam->setExclusiveParameterConfiguration(conf);
            boardParam->disconnectSharedConfiguration(true);

            replaceParameter(index, boardParam);
        }

    } else {
        boardParam = mBoardParameters.at(index);
    }
    return boardParam;
}

void BoardElement::replaceParameter(int index, QSharedPointer<BoardParameter> boardParameter)
{
    if(mBoardParameters.contains(boardParameter)) {
        logger()->info() << name() << " " << boardParameter->getTimeSeries().name() << " already added" ;
        return;
    } else {
        if (index < mBoardParameters.count() && index >= 0) {
            auto id = mBoardParameters.at(index)->getTimeSeries().parameterId();

            mBoardParameters.replace(index, boardParameter);
            mParametersLabel.replace(index, boardParameter->getTimeSeries().name());
            mBoard->dataManager()->registerBoardParameterId(boardParameter->getTimeSeries().parameterId());
            mBoard->dataManager()->unregisterBoardParameterId(id);
        }
    }
}

double BoardElement::selectTest(const QPointF &pos, bool onlySelectable, QVariant *details) const
{
    Q_UNUSED(details)

    if (onlySelectable)
        return -1;

    int left = int(mOuterRect.x());
    int right = int(std::ceil(mOuterRect.x() + mOuterRect.width()));
    int bottom = int(mOuterRect.y());
    int top = int(std::ceil(mOuterRect.y() + mOuterRect.height()));
    if(pos.x() >= left && pos.x() <= right &&
            pos.y() >= bottom && pos.y() <= top) {
        if (mParentPlot) {
            return mParentPlot->selectionTolerance()*0.99;
        } else {
            logger()->debug() << Q_FUNC_INFO << "parent plot not defined";
            return -1;
        }
    } else {
        return -1;
    }
}
