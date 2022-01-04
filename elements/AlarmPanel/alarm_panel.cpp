#include "alarm_panel.h"
#include "ui/board/management/project.h"

AlarmPanel::AlarmPanel(Board *dashboard) :
    BoardElement (dashboard),
    mAlarmConfigurationIsShared(false),
    mSharedAlarmConfiguration(new Alarms ()),
    mExclusiveAlarmConfiguration(new Alarms ()),
    mAlarmConfiguration(new Alarms ()),
    mUpdatingConfig(false),
    mAutoRowHeight(true),
    mRowHeight(20),
    mHeaderVisible(true),
    mAutoHeaderText(true),
    mHeaderText("Alarms")
{
    setType(BoardElement::etAlarm);
    disconnectAlarmConfig();
    setParametersMaxCount(2000);

    mListBorderPen = QPen(QApplication::palette().midlight().color(), 0, Qt::SolidLine, Qt::SquareCap);
}

int AlarmPanel::defaultWidth()
{
    return 3*mBoard->dashboardLayout()->singleElementColumnCount();
}

int AlarmPanel::defaultHeight()
{
    return 9*mBoard->dashboardLayout()->singleElementRowCount();
}

void AlarmPanel::initialize(Board *dashboard)
{
    if(dashboard) {

        BoardElement::initialize(dashboard);

        mHeader = new AdaptiveTextElement(dashboard);
        mHeader->setTextFlags(Qt::AlignCenter);
        mHeader->setMaxPointSize(10);
        mHeader->setMinPointSize(6);
        QFont f = mHeader->font();
        f.setUnderline(false);
        mHeader->setFont(f);
        auto textColor = QApplication::palette().toolTipText().color();
        mHeader->setTextColor(textColor);
        mHeader->setText(QString("Alarms"));
        mHeader->setAdjustStrategy(AdaptiveTextElement::asAdjustAndElide);

        mListLayout = new LayoutList();
        mListLayout->initializeParentPlot(dashboard);
        mListLayout->setParentLayerable(this);
        mListLayout->setRowSpacing(0);

        mMainLayout = new LayoutGrid();
        mMainLayout->initializeParentPlot(dashboard);
        mMainLayout->setParentLayerable(this);
        mMainLayout->setLayer(QLatin1String("main"));
        mMainLayout->setMargins(QMarginsF(4,4,4,4));

        for(int i=0;i<64;i++) {
            auto el = new AdaptiveTextElement(dashboard);
            el->setTextFlags(Qt::AlignVCenter | Qt::AlignLeft);
            el->setMaxPointSize(11);
            el->setMinPointSize(4);
            el->setVisible(false);
            el->setMargins(QMarginsF(4,0.5,4,0.5));
            el->setText(QString("Test Alarm %1").arg(i));
            el->setAdjustStrategy(AdaptiveTextElement::asAdjustAndElide);
            mTextElements.append(el);
        }
    }
}

void AlarmPanel::clearElement()
{
    delete mMainLayout;
}

void AlarmPanel::resetElement()
{
    mListLayout->takeAllElements();

    for(int i=0; i<mMainLayout->elementCount(); i++)
        mMainLayout->takeAt(i);
    mMainLayout->simplify();
}

void AlarmPanel::buildElement()
{
    QVector<bool> visibilites;
    for(int i=0;i<64;i++) {
        visibilites.append(mTextElements.at(i)->visible());
        mTextElements.at(i)->setVisible(false);
    }
    mHeader->setVisible(false);
    mMainLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);

    mHeader->setVisible(mHeaderVisible);
    if(mHeaderVisible) {
        if(mAutoHeaderText) {
            auto text = mAlarmConfiguration->name();
            if(text.isEmpty())
                text = mHeaderText;
            mHeader->setText(text);
        } else {
            mHeader->setText(mHeaderText);
        }
        mMainLayout->addElement(mHeader);
        mMainLayout->setRowStretchFactor(0,0.001);
    }

    for(int i=0;i<64;i++) {
        if(visibilites.at(i)) {
            mTextElements.at(i)->setVisible(true);
            mListLayout->addElement(mTextElements.at(i));
        }
    }
    mMainLayout->addElement(mListLayout);
    mMainLayout->setRowStretchFactor(mMainLayout->elementCount()-1,1);
    mMainLayout->setRowSpacing(5);
    mMainLayout->needUpdate(true);
}

void AlarmPanel::edit()
{    
    QDialog dial;
    dial.setWindowTitle("Alarm Panel Edition");
    QVBoxLayout dialLayout;

    QHBoxLayout backgroundLayout;
    QLabel backgroundLabel("Background");
    QComboBox backgroundBox(&dial);
    backgroundBox.addItem("Background color");
    backgroundBox.addItem("Transparent");
    backgroundBox.setCurrentIndex(backgroundMode());
    backgroundLayout.addSpacing(40);
    backgroundLayout.addWidget(&backgroundLabel);
    backgroundLayout.addWidget(&backgroundBox);
    backgroundLayout.addSpacing(40);
    dialLayout.addLayout(&backgroundLayout);

    QHBoxLayout headerVisibleLayout;
    QLabel headerVisibleLabel("Header visible");
    QCheckBox headerVisibleCheckBox(&dial);
    headerVisibleCheckBox.setChecked(mHeaderVisible);
    headerVisibleLayout.addSpacing(40);
    headerVisibleLayout.addWidget(&headerVisibleLabel);
    headerVisibleLayout.addStretch();
    headerVisibleLayout.addWidget(&headerVisibleCheckBox);
    headerVisibleLayout.addSpacing(40);
    dialLayout.addLayout(&headerVisibleLayout);

    QHBoxLayout headerLayout;
    QLabel headerLabel("Header Text");
    QCheckBox autoHeaderTextCheckBox(&dial);
    autoHeaderTextCheckBox.setChecked(!mAutoHeaderText);
    autoHeaderTextCheckBox.setText(QString("Override"));
    QLineEdit headerTextLineEdit(&dial);
    headerTextLineEdit.setText(mHeaderText);
    headerLayout.addSpacing(40);
    headerLayout.addWidget(&headerLabel);
    headerLayout.addStretch();
    headerLayout.addWidget(&autoHeaderTextCheckBox);
    headerLayout.addWidget(&headerTextLineEdit);
    headerLayout.addSpacing(40);
    dialLayout.addLayout(&headerLayout);

    QHBoxLayout rowHeightLayout;
    QLabel rowHeightLabel("List Row Height");
    QSpinBox rowHeightSpinBox(&dial);
    QCheckBox autoRowHeightCheckBox(&dial);
    QLabel autoLabel(&dial);
    rowHeightSpinBox.setValue(mRowHeight);
    autoRowHeightCheckBox.setChecked(mAutoRowHeight);
    rowHeightSpinBox.setEnabled(!mAutoRowHeight);
    autoLabel.setText("Auto");
    rowHeightLayout.addSpacing(40);
    rowHeightLayout.addWidget(&rowHeightLabel);
    rowHeightLayout.addStretch();
    rowHeightLayout.addWidget(&autoRowHeightCheckBox);
    rowHeightLayout.addWidget(&autoLabel);
    rowHeightLayout.addWidget(&rowHeightSpinBox);
    rowHeightLayout.addSpacing(40);
    dialLayout.addLayout(&rowHeightLayout);

    QFrame line(&dial);
    line.setFrameShape(QFrame::HLine);
    line.setFrameShadow(QFrame::Sunken);
    dialLayout.addWidget(&line);

    AlarmConfigurationWidget w;
    if(mAlarmConfigurationIsShared) {
        w.setEditionMode(AlarmConfigurationWidget::emElementConnected);
        w.setAlarmConfig(mSharedAlarmConfiguration);
    } else {
        w.setEditionMode(AlarmConfigurationWidget::emElementStandAlone);
        w.setAlarmConfig(mExclusiveAlarmConfiguration);
    }
    w.updateUi();

    connect(&w, &AlarmConfigurationWidget::stateChanged, [&w, this](int state){
        if(state == Qt::Checked) {
            w.setEditionMode(AlarmConfigurationWidget::emElementConnected);
            w.setAlarmConfig(mSharedAlarmConfiguration);
            w.updateUi();
        } else {
            w.setEditionMode(AlarmConfigurationWidget::emElementDisconnected);
            w.setAlarmConfig(mExclusiveAlarmConfiguration);
            w.updateUi();
        }
    });

    connect(&autoRowHeightCheckBox, &QCheckBox::stateChanged, [&rowHeightSpinBox](int state) {
        if(state == Qt::Checked) {
            rowHeightSpinBox.setEnabled(false);
        } else {
            rowHeightSpinBox.setEnabled(true);
        }
    });

    dialLayout.addWidget(&w);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok
                               | QDialogButtonBox::Apply
                               | QDialogButtonBox::Cancel);

    connect(&buttonBox, SIGNAL(accepted()),&dial, SLOT(accept()));
    connect(&buttonBox, SIGNAL(rejected()),&dial, SLOT(reject()));

    connect(buttonBox.button(QDialogButtonBox::Apply), &QPushButton::clicked,
            [this, &w,
            &autoRowHeightCheckBox, &rowHeightSpinBox,
            &headerVisibleCheckBox, &autoHeaderTextCheckBox,
            &headerTextLineEdit, &backgroundBox](){
        mUpdatingConfig = true;
        w.updateConfig();
        if(!w.isConnected())
            disconnectAlarmConfig();
        updateAlarmConfiguration();

        mAutoRowHeight = autoRowHeightCheckBox.isChecked();
        mRowHeight = rowHeightSpinBox.value();

        mHeaderVisible = headerVisibleCheckBox.isChecked();
        mAutoHeaderText = !autoHeaderTextCheckBox.isChecked();
        mHeaderText = headerTextLineEdit.text();

        if(!mAutoRowHeight)
            mListLayout->setRowHeight(mRowHeight);
        setBackgroundMode(BoardElement::BackgroundMode(backgroundBox.currentIndex()));
        mUpdatingConfig = false;
        rebuildElement(true);
    });


    dialLayout.addWidget(&buttonBox);

    dial.setLayout(&dialLayout);
    int res = dial.exec();
    if( res == QDialog::Accepted) {
        mUpdatingConfig = true;
        w.updateConfig();
        if(!w.isConnected())
            disconnectAlarmConfig();
        updateAlarmConfiguration();

        mAutoRowHeight = autoRowHeightCheckBox.isChecked();
        mRowHeight = rowHeightSpinBox.value();

        mHeaderVisible = headerVisibleCheckBox.isChecked();
        mAutoHeaderText = autoHeaderTextCheckBox.isChecked();
        mHeaderText = headerTextLineEdit.text();

        if(!mAutoRowHeight)
            mListLayout->setRowHeight(mRowHeight);
        setBackgroundMode(BoardElement::BackgroundMode(backgroundBox.currentIndex()));
        mUpdatingConfig = false;
        rebuildElement(true);
    }
}

void AlarmPanel::update(QCPLayoutElement::UpdatePhase phase)
{
    BoardElement::update(phase);

    if(phase == upLayout) {
        mMainLayout->setOuterRect(rect());       

        QSizeF defaultSize = mBoard->dashboardLayout()->singleElementSize();
        defaultSize.rwidth() -= mMargins.left() + mMargins.right() + mMainLayout->margins().left() + mMainLayout->margins().right();
        defaultSize.rheight() -= mMargins.top() + mMargins.bottom() + mMainLayout->margins().top() + mMainLayout->margins().bottom();

        QSizeF currentSize = mRect.size();
        currentSize.rwidth() -= mMainLayout->margins().left() + mMainLayout->margins().right();
        currentSize.rheight() -= mMainLayout->margins().top() + mMainLayout->margins().bottom();

        QSizeF labelSize = defaultSize;

        if(currentSize.height() < defaultSize.height())
            labelSize = currentSize;

        mHeader->setMinimumSize(labelSize.width(), 0.3 * labelSize.height());

        if(mAutoRowHeight) {
            int height = int(0.3 * labelSize.height());
            if(height > 20)
                height = 20;
            mListLayout->setRowHeight(height);
        }
    }

    mMainLayout->update(phase);
}

void AlarmPanel::loadData()
{

}

void AlarmPanel::displayData()
{
    if(mUpdatingConfig)
        return;

    for(int i=0;i<64;i++) {
        mTextElements.at(i)->setVisible(false);
    }

    mListLayout->takeAllElements();

    int elementIndex = 0;
    for(int i = 0; i< mAlarmConfiguration->alarmsCount();i++) {
        if(i < mDashParametersPrimary.count()) {
            if(mAlarmConfiguration->active().at(i)) {
                bool alarmSet = false;
                bool aliveParam = false;

                if(mDashParametersPrimary.at(i)->getTimeSeries().parameterId() > 0) {
                    mDashParametersPrimary.at(i)->updateData();
                    aliveParam = true;
                }
                switch (mAlarmConfiguration->functions().at(i)) {
                case Alarms::afGreaterThan:
                    if(aliveParam && mDashParametersPrimary.at(i)->getValueDouble() > mAlarmConfiguration->functionArgs().at(i))
                        alarmSet = true;
                    break;
                case Alarms::afLowerThan:
                    if(aliveParam && mDashParametersPrimary.at(i)->getValueDouble() < mAlarmConfiguration->functionArgs().at(i))
                        alarmSet = true;
                    break;
                case Alarms::afBitSet:
                    if(aliveParam)
                        alarmSet = (1 == ((mDashParametersPrimary.at(i)->getValueBinaryWeight32() >> int(mAlarmConfiguration->functionArgs().at(i)) & 1)));
                    break;
                case Alarms::afParamDead:
                    alarmSet = !aliveParam;
                    break;
                }

                if(alarmSet) {
                    mTextElements.at(elementIndex)->setText(mAlarmConfiguration->messages().at(i));
                    mTextElements.at(elementIndex)->setTextColor(mAlarmConfiguration->colors().at(i));
                    mTextElements.at(elementIndex)->setTextFlags(Qt::AlignVCenter | Qt::AlignLeft);
                    mTextElements.at(elementIndex)->setVisible(true);
                    mTextElements.at(elementIndex)->setBoldText(true);
                    mListLayout->addElement(mTextElements.at(elementIndex));
                    elementIndex++;
                    if(elementIndex == mTextElements.count())
                        return;

                    if(mDashParametersSecondary.at(i)) {

                        if(mDashParametersSecondary.at(i)->getTimeSeries().parameterId() > 0) {
                            mDashParametersSecondary.at(i)->updateData();
                            mTextElements.at(elementIndex)->setText(QString("   %1 : %2").arg(mDashParametersSecondary.at(i)->getTimeSeries().name()).arg(mDashParametersSecondary.at(i)->getValueString()));
                            mTextElements.at(elementIndex)->setTextColor(mAlarmConfiguration->colors().at(i));
                            mTextElements.at(elementIndex)->setVisible(true);
                            mTextElements.at(elementIndex)->setBoldText(true);
                            mListLayout->addElement(mTextElements.at(elementIndex));
                            elementIndex++;
                            if(elementIndex == mTextElements.count())
                                return;
                        } else {
                            mTextElements.at(elementIndex)->setText(QString("   %1 not avail.").arg(mDashParametersSecondary.at(i)->getTimeSeries().name()));
                            mTextElements.at(elementIndex)->setTextColor(mAlarmConfiguration->colors().at(i));
                            mTextElements.at(elementIndex)->setVisible(true);
                            mTextElements.at(elementIndex)->setBoldText(true);
                            mListLayout->addElement(mTextElements.at(elementIndex));
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

void AlarmPanel::checkParameters()
{
    auto listParam = mBoardParameters;
    listParam.append(mDashParametersSecondary);

    for(auto boardParam : listParam) {
        quint32 id = 0;
        if(boardParam) {
            id = boardParam->getTimeSeries().parameterId();

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
}

void AlarmPanel::saveSettings(QSettings *settings)
{
    settings->beginGroup("List");
    settings->setValue("RowHeight", mRowHeight);
    settings->setValue("AutoRowHeight", mAutoRowHeight);
    settings->endGroup();
    settings->beginGroup("DisplaySettings");
    settings->setValue("BackgroundMode", mBackgroundMode);
    settings->setValue("HeaderVisible", mHeaderVisible);
    settings->setValue("AutoHeaderText", mAutoHeaderText);
    settings->setValue("HeaderText", mHeaderText);
    settings->endGroup();

}

void AlarmPanel::loadSettings(QSettings *settings)
{
    settings->beginGroup("List");
    mRowHeight = settings->value("RowHeight", mRowHeight).toInt();
    mListLayout->setRowHeight(mRowHeight);
    mAutoRowHeight = settings->value("AutoRowHeight", mAutoRowHeight).toBool();
    settings->endGroup();
    settings->beginGroup("DisplaySettings");
    if(settings->contains("BackgroundMode")) {
        auto maxVal = QMetaEnum::fromType<BackgroundMode>().keyCount();
        auto val = settings->value("BackgroundMode").toInt();
        if(val <   maxVal)
            setBackgroundMode(BackgroundMode(val));
    }
    if(settings->contains("HeaderVisible"))
        setHeaderVisible(settings->value("HeaderVisible").toBool());
    if(settings->contains("AutoHeaderText"))
        setAutoHeaderText(settings->value("AutoHeaderText").toBool());
    if(settings->contains("HeaderText"))
        setHeaderText(settings->value("HeaderText").toString());
    settings->endGroup();
}

void AlarmPanel::saveConfigurations(QSettings *settings)
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

void AlarmPanel::loadConfigurations(QSettings *settings)
{
    settings->beginGroup("Alarms");
    mAlarmConfigurationIsShared = settings->value("Connected").toBool();
    if(mAlarmConfigurationIsShared) {
        QString alarmName = settings->value("AlarmName").toString();
        QExplicitlySharedDataPointer<Alarms> conf = mBoard->project()->alarmConfiguration(alarmName);
        if(conf) {
            addAlarm(conf);
        } else {
            mAlarmConfigurationIsShared = false;
            mExclusiveAlarmConfiguration->load(settings);
            mAlarmConfiguration = mExclusiveAlarmConfiguration;
            updateAlarmConfiguration();
        }
    } else {
        mExclusiveAlarmConfiguration->load(settings);
        mAlarmConfiguration = mExclusiveAlarmConfiguration;
        updateAlarmConfiguration();
    }
    settings->endGroup();
}

QSharedPointer<BoardParameter> AlarmPanel::addParameter(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings)
{
    Q_UNUSED(parameterSettings)
    return nullptr;
}

QSharedPointer<BoardParameter> AlarmPanel::addParameter(QString paramLabel)
{
    Q_UNUSED(paramLabel)
    return nullptr;
}

QSharedPointer<BoardParameter> AlarmPanel::addParameter(QSharedPointer<TimeSeries> dataParameter)
{
    Q_UNUSED(dataParameter)
    return nullptr;
}

void AlarmPanel::removeBoardParameters()
{
    mExclusiveAlarmConfiguration->clearAlarms();
    mAlarmConfigurationIsShared = false;
    mSharedAlarmConfiguration.reset();
    mAlarmConfiguration = mExclusiveAlarmConfiguration;
    BoardElement::removeBoardParameters();

    mDashParametersPrimary.clear();
    mDashParametersSecondary.clear();
}

void AlarmPanel::addAlarm(QExplicitlySharedDataPointer<Alarms> alarmConfig)
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

void AlarmPanel::updateAlarmConfiguration()
{
    BoardElement::removeBoardParameters();
    mDashParametersPrimary.clear();
    mDashParametersSecondary.clear();

    for(int i = 0; i< mAlarmConfiguration->alarmsCount();i++) {
        QSharedPointer<BoardParameter> dashParamPrimary = BoardElement::addParameter(mAlarmConfiguration->watchedParameters().at(i));
        mDashParametersPrimary.append(dashParamPrimary);

        QSharedPointer<BoardParameter> dashParamSecondary;
        if(!mAlarmConfiguration->outputParameters().at(i).isEmpty()) {
            dashParamSecondary = BoardElement::addParameter(mAlarmConfiguration->outputParameters().at(i));
        }
        mDashParametersSecondary.append(dashParamSecondary);
    }
}

bool AlarmPanel::connected() const
{
    return mAlarmConfigurationIsShared;
}

void AlarmPanel::disconnectAlarmConfig()
{
    mAlarmConfigurationIsShared = false;
    mSharedAlarmConfiguration.reset();
    mAlarmConfiguration = mExclusiveAlarmConfiguration;
}

void AlarmPanel::resetModifications()
{
    mAlarmConfiguration->setModified(false);
}

void AlarmPanel::beforeDataUpdate(bool /*refresh*/)
{
    if(mAlarmConfiguration->modified())
        updateAlarmConfiguration();
}

bool AlarmPanel::autoRowHeight() const
{
    return mAutoRowHeight;
}

void AlarmPanel::setAutoRowHeight(bool autoRowHeight)
{
    mAutoRowHeight = autoRowHeight;
}

int AlarmPanel::rowHeight() const
{
    return mRowHeight;
}

void AlarmPanel::setRowHeight(int rowHeight)
{
    mRowHeight = rowHeight;
}

bool AlarmPanel::headerVisible() const
{
    return mHeaderVisible;
}

void AlarmPanel::setHeaderVisible(bool headerVisible)
{
    mHeaderVisible = headerVisible;
}

bool AlarmPanel::autoHeaderText() const
{
    return mAutoHeaderText;
}

void AlarmPanel::setAutoHeaderText(bool autoHeaderText)
{
    mAutoHeaderText = autoHeaderText;
}

QString AlarmPanel::headerText() const
{
    return mHeaderText;
}

void AlarmPanel::setHeaderText(const QString &headerText)
{
    mHeaderText = headerText;
}

void AlarmPanel::draw(QCPPainter *painter)
{
    BoardElement::draw(painter);

    painter->setBrush(Qt::NoBrush);
    painter->setPen(mListBorderPen);
    painter->drawRoundedRect(mListLayout->outerRect(), 4, 4);
}
