#include "table_values.h"
#include "table_values_editor.h"

TableValues::TableValues(Board *dashboard):
    BoardElement (dashboard),
    mAlternatingRowColors(true),
    mUnitVisible(true),
    mTitleVisible(true),
    mTitle("Values"),
    mValueFormat(vsfDecimal),
    mPrecision(2)
{
    mTableMargins = QMargins(20,20,20,20);

    mTableWidget = nullptr;

    setType(BoardElement::etTimeSeries);
    setTimeSeriesType(BoardElement::tstScalar);
    setTimeSeriesSize(BoardElement::tssMulti);
    setParametersMaxCount(64);
}

TableValues::~TableValues()
{
    delete mTextLabel;
    delete mEmptyElementBottom;
    delete mMainLayout;
    if (mTableWidget)
        delete mTableWidget;
}

void TableValues::update(QCPLayoutElement::UpdatePhase phase)
{
    BoardElement::update(phase);

    if(phase == upLayout) {
        mMainLayout->setOuterRect(rect());
        applySizeConstraints();
    }

    mMainLayout->update(phase);

    if(phase == upLayout) {
        auto rect = mEmptyElementBottom->outerRect().adjusted(mTableMargins.left(),
                                                              mTableMargins.top(),
                                                              -mTableMargins.right(),
                                                              -mTableMargins.bottom());
        mTableWidget->resize(rect.size().toSize());
        mTableWidget->move(rect.topLeft().toPoint());
    }
}

void TableValues::initialize(Board *dashboard)
{
    if(dashboard) {
        BoardElement::initialize(dashboard);

        mTextLabel = new AdaptiveTextElement(dashboard);
        mTextLabel->setTextFlags(Qt::AlignCenter);
        mTextLabel->setMaxPointSize(10);
        mTextLabel->setMinPointSize(6);
        mTextLabel->setAdjustStrategy(AdaptiveTextElement::asAdjustAndElide);

        QFont f = mTextLabel->font();
        f.setUnderline(false);
        mTextLabel->setFont(f);
        auto textColor = QApplication::palette().toolTipText().color();
        mTextLabel->setTextColor(textColor);
        mTextLabel->setLayer(QLatin1String("main"));
        mTextLabel->setCachedPixmap(true);
        mTextLabel->setText("Values");

        mEmptyElementBottom = new QCPLayoutElement(dashboard);

        mMainLayout = new LayoutGrid();
        mMainLayout->initializeParentPlot(dashboard);
        mMainLayout->setParentLayerable(this);
        mMainLayout->setLayer(QLatin1String("main"));
        mMainLayout->setMargins(QMarginsF(4,4,4,4));

        mTableWidget = new QTableWidget(dashboard);
        mTableWidget->setColumnCount(3);
        QStringList header;
        header << "Parameter" << "Unit" << "Value";
        mTableWidget->setHorizontalHeaderLabels(header);
        mTableWidget->verticalHeader()->setVisible(false);
        mTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        mTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        mTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        mTableWidget->horizontalHeader()->setHighlightSections(false);
        mTableWidget->setEditTriggers(QTableWidget::NoEditTriggers);
        mTableWidget->setSelectionMode(QTableWidget::NoSelection);
        mTableWidget->setSelectionBehavior(QTableWidget::SelectRows);

        mTableWidget->show();
    }
}

int TableValues::defaultWidth()
{
    return 4*mBoard->dashboardLayout()->singleElementColumnCount();
}

int TableValues::defaultHeight()
{
    return 6*mBoard->dashboardLayout()->singleElementRowCount();
}

void TableValues::resetElement()
{
    for(int i=0; i<mMainLayout->elementCount(); i++)
        mMainLayout->takeAt(i);
    mMainLayout->simplify();
}

void TableValues::clearElement()
{
    mTableWidget->setRowCount(0);
}

void TableValues::buildElement()
{
    mTableWidget->setColumnHidden(1, !mUnitVisible);
    mTableWidget->setAlternatingRowColors(mAlternatingRowColors);

    mMainLayout->setFillOrder(QCPLayoutGrid::foRowsFirst);
    mTextLabel->setTextFlags(Qt::AlignCenter);
    mTextLabel->setText(mTitle);
    mTextLabel->setVisible(mTitleVisible);
    mTextLabel->needUpdate(true);

    if(mTitleVisible) {
        mMainLayout->addElement(mTextLabel);
        mMainLayout->setRowStretchFactor(0,0.001);
        mTableMargins = QMargins(20,5,20,20);
    } else {
        mTableMargins = QMargins(20,20,20,20);
    }

    mMainLayout->addElement(mEmptyElementBottom);
    mMainLayout->setRowStretchFactor(mMainLayout->elementCount()-1,1);
    mMainLayout->setRowSpacing(0);
    mMainLayout->needUpdate(true);
}

void TableValues::edit()
{
    TableValuesEditor editor(this);
    editor.exec();
}

void TableValues::applySizeConstraints()
{
    QSizeF defaultSize = mBoard->dashboardLayout()->singleElementSize();
    defaultSize.rwidth() -= mMargins.left() + mMargins.right() + mMainLayout->margins().left() + mMainLayout->margins().right();
    defaultSize.rheight() -= mMargins.top() + mMargins.bottom() + mMainLayout->margins().top() + mMainLayout->margins().bottom();

    QSizeF currentSize = mRect.size();
    currentSize.rwidth() -= mMainLayout->margins().left() + mMainLayout->margins().right();
    currentSize.rheight() -= mMainLayout->margins().top() + mMainLayout->margins().bottom();

    QSizeF labelSize = defaultSize;

    if(currentSize.height() < defaultSize.height())
        labelSize = currentSize;

    mTextLabel->setMinimumSize(labelSize.width(), 0.3 * labelSize.height());
}

void TableValues::loadSettings(QSettings *settings)
{
    settings->beginGroup("TableValuesSettings");
    if(settings->contains("BackgroundMode")) {
        auto maxVal = QMetaEnum::fromType<BackgroundMode>().keyCount();
        auto val = settings->value("BackgroundMode").toInt();
        if(val <   maxVal)
            setBackgroundMode(BackgroundMode(val));
    }
    if(settings->contains("Format"))
        mValueFormat = ValueStringFormat(settings->value("Format").toInt());
    mUnitVisible = settings->value("UnitVisible", true).toBool();
    mTitleVisible = settings->value("TitleVisible", false).toBool();
    mTitle = settings->value("Title", "Values").toString();
    mAlternatingRowColors = settings->value("AlternatingRowColors", true).toBool();
    mPrecision = settings->value("Precision", 2).toInt();
    settings->endGroup();
}

void TableValues::saveSettings(QSettings *settings)
{
    settings->beginGroup("TableValuesSettings");
    settings->setValue("BackgroundMode", mBackgroundMode);
    settings->setValue("UnitVisible", mUnitVisible);
    settings->setValue("TitleVisible", mTitleVisible);
    settings->setValue("Title", mTitle);
    settings->setValue("AlternatingRowColors", mAlternatingRowColors);
    settings->setValue("Format", mValueFormat);
    settings->setValue("Precision", mPrecision);
    settings->endGroup();
}

void TableValues::loadConfigurations(QSettings *settings)
{
    BoardElement::loadConfigurations(settings);
    for(int i=0; i< parametersCount(); i++) {
        QSharedPointer<BoardParameter> dashParam = boardParameter(i);
        if(dashParam) {
            mTableWidget->insertRow(mTableWidget->rowCount());
            mTableWidget->setItem(mTableWidget->rowCount() - 1,
                                  0,
                                  new QTableWidgetItem(dashParam->getDisplayedLabel()));

            auto unitItem = new QTableWidgetItem(dashParam->getDisplayedUnit());
            unitItem->setTextAlignment(Qt::AlignCenter);
            mTableWidget->setItem(mTableWidget->rowCount() - 1,
                                  1,
                                  unitItem);

            auto valueItem = new QTableWidgetItem("-");
            valueItem->setTextAlignment(Qt::AlignCenter);
            mTableWidget->setItem(mTableWidget->rowCount() - 1,
                                  2,
                                  valueItem);
        }
    }
}

void TableValues::checkParameters()
{
    BoardElement::checkParameters();
    for(auto boardParam : mBoardParameters)
        boardParam->parameterConfiguration()->setPrecision(mPrecision);
}

QSharedPointer<BoardParameter> TableValues::addParameter(QExplicitlySharedDataPointer<ParameterConfiguration> parameterSettings)
{
    QSharedPointer<BoardParameter> dashParam = BoardElement::addParameter(parameterSettings);
    dashParam->parameterConfiguration()->setPrecision(mPrecision);
    return dashParam;
}

QSharedPointer<BoardParameter> TableValues::addParameter(QString paramLabel)
{
    QSharedPointer<BoardParameter> dashParam = BoardElement::addParameter(paramLabel);
    dashParam->parameterConfiguration()->setPrecision(mPrecision);
    return dashParam;
}

void TableValues::addParameter(QSharedPointer<BoardParameter> boardParameter)
{
    BoardElement::addParameter(boardParameter);
    boardParameter->parameterConfiguration()->setPrecision(mPrecision);

    if(mTableWidget->rowCount() == parametersMaxCount()) {
        mTableWidget->item(mTableWidget->rowCount() - 1,0)->setText(boardParameter->getDisplayedLabel());
        mTableWidget->item(mTableWidget->rowCount() - 1,1)->setText(boardParameter->getDisplayedUnit());
        mTableWidget->item(mTableWidget->rowCount() - 1,2)->setText("-");
    } else {
        mTableWidget->insertRow(mTableWidget->rowCount());
        mTableWidget->setItem(mTableWidget->rowCount() - 1,
                              0,
                              new QTableWidgetItem(boardParameter->getDisplayedLabel()));


        auto unitItem = new QTableWidgetItem(boardParameter->getDisplayedUnit());
        unitItem->setTextAlignment(Qt::AlignCenter);
        mTableWidget->setItem(mTableWidget->rowCount() - 1,
                              1,
                              unitItem);

        auto valueItem = new QTableWidgetItem("-");
        valueItem->setTextAlignment(Qt::AlignCenter);
        mTableWidget->setItem(mTableWidget->rowCount() - 1,
                              2,
                              valueItem);
    }
}

void TableValues::removeBoardParameter(int index, bool update)
{
    mTableWidget->removeRow(index);
    BoardElement::removeBoardParameter(index, update);
}

void TableValues::displayData()
{
    for(int i=0; i< parametersCount(); i++) {
        if(i < mTableWidget->rowCount()) {
            QSharedPointer<BoardParameter> dashParam = boardParameter(i);
            if(dashParam && dashParam->getTimeSeries().parameterId() > 0) {
                mTableWidget->item(i,2)->setText(dashParam->getValueString(mValueFormat));
            } else {
                mTableWidget->item(i,2)->setText("X");
            }
        }
    }
}

bool TableValues::alternatingRowColors() const
{
    return mAlternatingRowColors;
}

void TableValues::setAlternatingRowColors(bool alternatingRowColors)
{
    mAlternatingRowColors = alternatingRowColors;
}

bool TableValues::unitVisible() const
{
    return mUnitVisible;
}

void TableValues::setUnitVisible(bool unitVisible)
{
    mUnitVisible = unitVisible;
}

ValueStringFormat TableValues::valueFormat() const
{
    return mValueFormat;
}

void TableValues::setValueFormat(const ValueStringFormat &valueFormat)
{
    mValueFormat = valueFormat;
}

int TableValues::precision() const
{
    return mPrecision;
}

void TableValues::setPrecision(int precision)
{
    mPrecision = precision;
}

QString TableValues::title() const
{
    return mTitle;
}

void TableValues::setTitle(const QString &title)
{
    mTitle = title;
}

bool TableValues::titleVisible() const
{
    return mTitleVisible;
}

void TableValues::setTitleVisible(bool titleVisible)
{
    mTitleVisible = titleVisible;
}
